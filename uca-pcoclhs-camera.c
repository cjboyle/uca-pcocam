#include <glib.h>
#include <gmodule.h>
#include <gio/gio.h>
#include <string.h>
#include <math.h>
#include <signal.h>

#include "pcoclhs.h"
#include "uca-pcoclhs-camera.h"
#include "uca-pcoclhs-enums.h"

#define UCA_PCO_CLHS_CAMERA_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), UCA_TYPE_PCO_CLHS_CAMERA, UcaPcoClhsCameraPrivate))

#define CHECK_AND_RETURN_VOID_ON_PCO_ERROR(err)               \
    if ((err) != 0)                                           \
    {                                                         \
        g_set_error(error, UCA_PCO_CLHS_CAMERA_ERROR,         \
                    UCA_PCO_CLHS_CAMERA_ERROR_PCOSDK_GENERAL, \
                    "libpcoclhs error %x", err);              \
        return;                                               \
    }

#define CHECK_AND_RETURN_VAL_ON_PCO_ERROR(err, val)           \
    if ((err) != 0)                                           \
    {                                                         \
        g_set_error(error, UCA_PCO_CLHS_CAMERA_ERROR,         \
                    UCA_PCO_CLHS_CAMERA_ERROR_PCOSDK_GENERAL, \
                    "libpcoclhs error %x", err);              \
        return val;                                           \
    }

static void uca_pco_clhs_camera_initable_iface_init(GInitableIface *iface);

G_DEFINE_TYPE_WITH_CODE(UcaPcoClhsCamera, uca_pco_clhs_camera, UCA_TYPE_CAMERA,
                        G_IMPLEMENT_INTERFACE(G_TYPE_INITABLE, uca_pco_clhs_camera_initable_iface_init))

GQuark uca_pco_clhs_camera_error_quark()
{
    return g_quark_from_static_string("uca-pco-clhs-camera-error-quark");
}

static GMutex signal_mutex;
static GCond signal_cond;

static void
handle_sigusr1(int signum)
{
    g_mutex_lock(&signal_mutex);
    g_cond_signal(&signal_cond);
    g_mutex_unlock(&signal_mutex);
}

enum
{
    PROP_SENSOR_EXTENDED = N_BASE_PROPERTIES,
    PROP_SENSOR_WIDTH_EXTENDED,
    PROP_SENSOR_HEIGHT_EXTENDED,
    PROP_SENSOR_TEMPERATURE,
    PROP_SENSOR_PIXELRATES,
    PROP_SENSOR_PIXELRATE,
    PROP_HAS_DOUBLE_IMAGE_MODE,
    PROP_DOUBLE_IMAGE_MODE,
    PROP_ACQUIRE_MODE,
    PROP_FAST_SCAN,
    PROP_NOISE_FILTER,
    PROP_TIMESTAMP_MODE,
    PROP_VERSION,
    PROP_EDGE_GLOBAL_SHUTTER,
    PROP_FRAME_GRABBER_TIMEOUT,
    PROP_DELAY_TIME,
    PROP_GRAB_N_BUFFERS,
    N_PROPERTIES
};

static gint base_overrideables[] = {
    PROP_NAME,
    PROP_SENSOR_WIDTH,
    PROP_SENSOR_HEIGHT,
    PROP_SENSOR_PIXEL_WIDTH,
    PROP_SENSOR_PIXEL_HEIGHT,
    PROP_SENSOR_BITDEPTH,
    PROP_SENSOR_HORIZONTAL_BINNING,
    PROP_SENSOR_VERTICAL_BINNING,
    PROP_EXPOSURE_TIME,
    PROP_FRAMES_PER_SECOND,
    PROP_TRIGGER_SOURCE,
    PROP_ROI_X,
    PROP_ROI_Y,
    PROP_ROI_WIDTH,
    PROP_ROI_HEIGHT,
    PROP_ROI_WIDTH_MULTIPLIER,
    PROP_ROI_HEIGHT_MULTIPLIER,
    PROP_HAS_STREAMING,
    PROP_HAS_CAMRAM_RECORDING,
    PROP_IS_RECORDING,
    0,
};

static GParamSpec *pco_clhs_properties[N_PROPERTIES] = {
    NULL,
};

static const gchar *DEFAULT_VERSION = "0, 0.0, 0.0";

typedef struct
{
    int type;
    int subtype;
    int max_width;
    int max_height;
    long fast_pixel_rate;
    long slow_pixel_rate;
} pco_clhs_map_entry;

static pco_clhs_map_entry pco_clhs_map[] = {
    {CAMERATYPE_PCO_EDGE_HS, CAMERASUBTYPE_PCO_EDGE_42, 2048, 2048, 548000000, 200000000},
    {CAMERATYPE_PCO_EDGE_HS, CAMERASUBTYPE_PCO_EDGE_55, 2560, 2160, 572000000, 190700000},
    {0, 0, 0, 0.0f, FALSE},
};

static pco_clhs_map_entry *get_pco_clhs_map_entry(int camera_type, int camera_subtype)
{
    pco_clhs_map_entry *entry = pco_clhs_map;

    while (entry->type != 0)
    {
        if (entry->type == camera_type && entry->subtype == camera_subtype)
            return entry;
        entry++;
    }

    return NULL;
}

struct _UcaPcoClhsCameraPrivate
{
    GError *construct_error;
    pco_handle *pco;
    pco_clhs_map_entry *description;

    guint16 board;
    guint16 port;

    gsize buffer_size;
    guint *grab_buffer;
    guint num_buffers;

    GValueArray *pixelrates;

    gint64 last_frame;
    guint num_recorded_images;
    guint current_image;

    gchar *version;

    UcaCameraTriggerSource trigger_source;

    /* threading */
    gboolean thread_running;
    GThread *grab_thread;
    GAsyncQueue *trigger_queue;
};

static void fill_pixelrates(UcaPcoClhsCameraPrivate *priv, guint32 rates[4], gint num_rates)
{
    GValue val = {0};
    g_value_init(&val, G_TYPE_UINT);
    priv->pixelrates = g_value_array_new(num_rates);

    for (gint i = 0; i < num_rates; i++)
    {
        g_value_set_uint(&val, (guint)rates[i]);
        g_value_array_append(priv->pixelrates, &val);
    }
}

static void property_override_default_guint_value(GObjectClass *oclass, const gchar *property_name, guint new_default)
{
    GParamSpecUInt *pspec = G_PARAM_SPEC_UINT(g_object_class_find_property(oclass, property_name));
    if (pspec != NULL)
        pspec->default_value = new_default;
    else
        g_warning("pspec for %s not found\n", property_name);
}

static void check_pco_clhs_property_error(guint err, guint property_id)
{
    if (err != PCO_NOERROR)
    {
        g_warning("Call to PCO SDK failed with error code %x for property '%s'",
                  err, pco_clhs_properties[property_id]->name);
    }
}

static gpointer grab_func(gpointer rawptr)
{
    UcaCamera *camera = UCA_CAMERA(rawptr);
    g_return_val_if_fail(UCA_IS_PCO_CLHS_CAMERA(camera), NULL);

    UcaPcoClhsCameraPrivate *priv = UCA_PCO_CLHS_CAMERA_GET_PRIVATE(camera);
    gpointer frame = NULL;
    guint err;

    err = pco_await_next_image(priv->pco, frame);
    // CHECK_AND_RETURN_VAL_ON_PCO_ERROR(err, NULL);
    if (err != 0)
    {
        g_error("Error acquiring next image at <%s:%i>", __FILE__, __LINE__);
        return NULL;
    }

    guint32 width, height;
    err += pco_grabber_get_actual_size(priv->pco, &width, &height);

    if (err == PCO_NOERROR && priv->thread_running)
    {
        pco_reorder_image(priv->pco, priv->grab_buffer, frame, width, height);
        camera->grab_func(frame, camera->user_data);
    }

    return NULL;
}

static void uca_pco_clhs_camera_start_recording(UcaCamera *camera, GError **error)
{
    UcaPcoClhsCameraPrivate *priv;
    guint16 binned_width, width, width_ex, bh;
    guint16 binned_height, height, height_ex, bv;
    guint16 roi[4];
    guint16 use_extended;
    gboolean transfer_async;
    guint err;

    g_return_if_fail(UCA_IS_PCO_CLHS_CAMERA(camera));
    priv = UCA_PCO_CLHS_CAMERA_GET_PRIVATE(camera);

    signal(SIGUSR1, handle_sigusr1);

    g_object_get(camera,
                 "trigger-source", &priv->trigger_source,
                 "transfer-asynchronously", &transfer_async,
                 "num-buffers", &priv->num_buffers,
                 NULL);

    err = pco_get_resolution(priv->pco, &width, &height, &width_ex, &height_ex);
    CHECK_AND_RETURN_VOID_ON_PCO_ERROR(err);

    err = pco_get_binning(priv->pco, &bh, &bv);
    CHECK_AND_RETURN_VOID_ON_PCO_ERROR(err);

    err = pco_get_roi(priv->pco, roi);
    CHECK_AND_RETURN_VOID_ON_PCO_ERROR(err);

    err = pco_get_sensor_format(priv->pco, &use_extended);
    CHECK_AND_RETURN_VOID_ON_PCO_ERROR(err);

    if (use_extended)
    {
        binned_width = width_ex;
        binned_height = height_ex;
    }
    else
    {
        binned_width = width;
        binned_height = height;
    }

    binned_width /= bh;
    binned_height /= bv;

    // check if the ROI dimensions exceed the available binned dimensions
    if ((roi[2] - roi[0] > binned_width) || (roi[3] - roi[1] > binned_height))
    {
        g_set_error(error, UCA_PCO_CLHS_CAMERA_ERROR, UCA_PCO_CLHS_CAMERA_ERROR_UNSUPPORTED,
                    "ROI of size %ix%i @ (%i, %i) is outside of (binned) sensor size %ix%i\n",
                    roi[2] - roi[0], roi[3] - roi[1], roi[0], roi[1], binned_width, binned_height);
    }

    if (priv->grab_buffer)
        g_free(priv->grab_buffer);

    priv->buffer_size = binned_width * binned_height * sizeof(guint16);

    priv->grab_buffer = g_malloc0(priv->buffer_size);
    memset(priv->grab_buffer, 0, priv->buffer_size);

    if (transfer_async)
    {
        GError *th_err = NULL;
        priv->thread_running = TRUE;
#if GLIB_CHECK_VERSION(2, 32, 0)
        priv->grab_thread = g_thread_new(NULL, grab_func, camera);
#else
        priv->grab_thread = g_thread_create(grab_func, camera, TRUE, &th_err);
#endif
        if (th_err != NULL)
        {
            priv->thread_running = FALSE;
            g_propagate_error(error, th_err);
        }
    }

    err = pco_start_recording(priv->pco);
    CHECK_AND_RETURN_VOID_ON_PCO_ERROR(err);
}

static void uca_pco_clhs_camera_stop_recording(UcaCamera *camera, GError **error)
{
    UcaPcoClhsCameraPrivate *priv;
    guint err;

    g_return_if_fail(UCA_IS_PCO_CLHS_CAMERA(camera));

    priv = UCA_PCO_CLHS_CAMERA_GET_PRIVATE(camera);

    err = pco_stop_recording(priv->pco);
    CHECK_AND_RETURN_VOID_ON_PCO_ERROR(err);

    gboolean transfer_async = FALSE;
    g_object_get(G_OBJECT(camera), "transfer-asynchronously", &transfer_async, NULL);
    if (transfer_async)
    {
        priv->thread_running = FALSE;
        g_thread_join(priv->grab_thread);
    }
}

static void uca_pco_clhs_camera_trigger(UcaCamera *camera, GError **error)
{
    UcaPcoClhsCameraPrivate *priv;
    guint16 success;
    guint err;

    g_return_if_fail(UCA_IS_PCO_CLHS_CAMERA(camera));

    priv = UCA_PCO_CLHS_CAMERA_GET_PRIVATE(camera);

    err = pco_force_trigger(priv->pco, &success);

    if (!success || err != 0)
    {
        g_set_error(error, UCA_PCO_CLHS_CAMERA_ERROR, UCA_PCO_CLHS_CAMERA_ERROR_PCOSDK_GENERAL,
                    "Could not trigger frame acquisition");
    }
}

static gboolean uca_pco_clhs_camera_grab(UcaCamera *camera, gpointer data, GError **error)
{
    g_return_val_if_fail(UCA_IS_PCO_CLHS_CAMERA(camera), FALSE);
    UcaPcoClhsCameraPrivate *priv = UCA_PCO_CLHS_CAMERA_GET_PRIVATE(camera);

    guint err;
    guint w, h;
    // guint counter;

    err = pco_grabber_get_actual_size(priv->pco, &w, &h);
    CHECK_AND_RETURN_VAL_ON_PCO_ERROR(err, FALSE);

    gsize size = w * h * sizeof(guint16);
    gpointer frame = g_malloc0(size);

    if (frame == NULL)
    {
        g_set_error(error, UCA_PCO_CLHS_CAMERA_ERROR,
                    UCA_PCO_CLHS_CAMERA_ERROR_FG_GENERAL,
                    "Frame data is NULL");
        return FALSE;
    }

    data = g_realloc(data, size);

    if (data == NULL)
    {
        g_set_error(error, UCA_PCO_CLHS_CAMERA_ERROR,
                    UCA_PCO_CLHS_CAMERA_ERROR_FG_GENERAL,
                    "Output buffer is NULL");
        return FALSE;
    }

    err = pco_acquire_image(priv->pco, frame);
    CHECK_AND_RETURN_VAL_ON_PCO_ERROR(err, FALSE);

    memcpy(data, frame, size);

    g_free(frame);
    frame = NULL;

    return TRUE;
}

static void uca_pco_clhs_camera_set_property(GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
    g_return_if_fail(UCA_IS_PCO_CLHS_CAMERA(object));
    UcaPcoClhsCameraPrivate *priv = UCA_PCO_CLHS_CAMERA_GET_PRIVATE(object);
    guint err = PCO_NOERROR;

    if (uca_camera_is_recording(UCA_CAMERA(object)) && !uca_camera_is_writable_during_acquisition(UCA_CAMERA(object), pspec->name))
    {
        g_warning("Property '%s' cant be changed during acquisition", pspec->name);
        return;
    }

    switch (property_id)
    {
    case PROP_SENSOR_EXTENDED:
    {
        guint16 format = g_value_get_boolean(value) ? SENSORFORMAT_EXTENDED : SENSORFORMAT_STANDARD;
        err = pco_set_sensor_format(priv->pco, format);
    }
    break;

    case PROP_ROI_X:
    {
        guint16 x = g_value_get_uint(value);
        guint16 window[4];
        pco_get_roi(priv->pco, window);
        window[0] = x;
        pco_set_roi(priv->pco, window);
    }
    break;

    case PROP_ROI_Y:
    {
        guint16 y = g_value_get_uint(value);
        guint16 window[4];
        pco_get_roi(priv->pco, window);
        window[1] = y;
        pco_set_roi(priv->pco, window);
    }
    break;

    case PROP_ROI_WIDTH:
    {
        guint16 width = g_value_get_uint(value);
        guint16 hs, vs;
        pco_get_roi_steps(priv->pco, &hs, &vs);

        if (width % hs != 0)
            g_warning("ROI width %i is not a multiple of %i", width, hs);
        else
        {
            guint16 window[4];
            pco_get_roi(priv->pco, window);
            window[2] = window[0] + width;
            pco_set_roi(priv->pco, window);
        }
    }
    break;

    case PROP_ROI_HEIGHT:
    {
        guint16 height = g_value_get_uint(value);
        guint16 hs, vs;
        pco_get_roi_steps(priv->pco, &hs, &vs);

        if (height % vs != 0)
            g_warning("ROI height %i is not a multiple of %i", height, vs);
        else
        {
            guint16 window[4];
            pco_get_roi(priv->pco, window);
            window[3] = window[1] + height;
            pco_set_roi(priv->pco, window);
        }
    }
    break;

    case PROP_SENSOR_HORIZONTAL_BINNING:
    {
        guint16 h, v;
        pco_get_binning(priv->pco, &h, &v);
        pco_set_binning(priv->pco, g_value_get_uint(value), v);
    }
    break;

    case PROP_SENSOR_VERTICAL_BINNING:
    {
        guint16 h, v;
        pco_get_binning(priv->pco, &h, &v);
        pco_set_binning(priv->pco, h, g_value_get_uint(value));
    }
    break;

    case PROP_DELAY_TIME:
    {
        uint32_t min_ns, max_ms, steps_ns;
        pco_get_delay_range(priv->pco, &min_ns, &max_ms, &steps_ns);
        double delay_sec = g_value_get_double(value);
        double delay_ms = CNV_UNIT_TO_MILLI(delay_sec);
        double delay_ns = CNV_UNIT_TO_NANO(delay_sec);
        double min_sec = CNV_NANO_TO_UNIT(min_ns);
        double max_sec = CNV_MILLI_TO_UNIT(max_ms);
        if (delay_ms > max_ms)
            err = pco_set_delay_time(priv->pco, max_sec);
        else if (delay_ns < min_ns)
            err = pco_set_delay_time(priv->pco, min_sec);
        else
            err = pco_set_delay_time(priv->pco, delay_sec);
    }
    break;

    case PROP_EXPOSURE_TIME:
    {
        uint32_t min_ns, max_ms, steps_ns;
        pco_get_exposure_range(priv->pco, &min_ns, &max_ms, &steps_ns);
        double exposure_sec = g_value_get_double(value);
        double exposure_ms = CNV_UNIT_TO_MILLI(exposure_sec);
        double exposure_ns = CNV_UNIT_TO_NANO(exposure_sec);
        double min_sec = CNV_NANO_TO_UNIT(min_ns);
        double max_sec = CNV_MILLI_TO_UNIT(max_ms);
        if (exposure_ms > max_ms)
            err = pco_set_exposure_time(priv->pco, max_sec);
        else if (exposure_ns < min_ns)
            err = pco_set_exposure_time(priv->pco, min_sec);
        else
            err = pco_set_exposure_time(priv->pco, exposure_sec);
    }
    break;

    case PROP_SENSOR_PIXELRATE:
    {
        guint desired_pixel_rate = g_value_get_uint(value);
        guint32 pixel_rate = 0;

        for (guint i = 0; i < priv->pixelrates->n_values; i++)
        {
            if (g_value_get_uint(g_value_array_get_nth(priv->pixelrates, i)) == desired_pixel_rate)
            {
                pixel_rate = desired_pixel_rate;
                break;
            }
        }

        if (pixel_rate != 0)
        {
            err = pco_set_pixelrate(priv->pco, pixel_rate);

            if (err != PCO_NOERROR)
                err = pco_reset(priv->pco);
        }
        else
            g_warning("%i Hz is not possible. Please check the \"sensor-pixelrates\" property", desired_pixel_rate);
    }
    break;

    case PROP_DOUBLE_IMAGE_MODE:
        if (pco_is_double_image_mode_available(priv->pco))
            err = pco_set_double_image_mode(priv->pco, g_value_get_boolean(value));
        break;

    case PROP_ACQUIRE_MODE:
    {
        UcaPcoClhsCameraAcquireMode mode = (UcaPcoClhsCameraAcquireMode)g_value_get_enum(value);

        if (mode == UCA_PCO_CLHS_CAMERA_ACQUIRE_MODE_AUTO)
            err = pco_set_acquire_mode(priv->pco, ACQUIRE_MODE_AUTO);
        else if (mode == UCA_PCO_CLHS_CAMERA_ACQUIRE_MODE_EXTERNAL)
            err = pco_set_acquire_mode(priv->pco, ACQUIRE_MODE_EXTERNAL);
        else
            g_warning("Unknown acquire mode");
    }
    break;

    case PROP_FAST_SCAN:
    {
        guint32 mode = g_value_get_boolean(value)
                           ? PCO_SCANMODE_FAST
                           : PCO_SCANMODE_SLOW;
        err = pco_set_scan_mode(priv->pco, mode);
    }
    break;

    case PROP_TRIGGER_SOURCE:
    {
        UcaCameraTriggerSource trigger_source = g_value_get_enum(value);

        switch (trigger_source)
        {
        case UCA_CAMERA_TRIGGER_SOURCE_AUTO:
            err = pco_set_trigger_mode(priv->pco, TRIGGER_MODE_AUTOTRIGGER);
            break;
        case UCA_CAMERA_TRIGGER_SOURCE_SOFTWARE:
            err = pco_set_trigger_mode(priv->pco, TRIGGER_MODE_SOFTWARETRIGGER);
            break;
        case UCA_CAMERA_TRIGGER_SOURCE_EXTERNAL:
            err = pco_set_trigger_mode(priv->pco, TRIGGER_MODE_EXTERNALTRIGGER);
            break;
        }
    }
    break;

    case PROP_NOISE_FILTER:
    {
        guint16 filter_mode = g_value_get_boolean(value) ? NOISE_FILTER_MODE_ON : NOISE_FILTER_MODE_OFF;
        err = pco_set_noise_filter_mode(priv->pco, filter_mode);
    }
    break;

    case PROP_TIMESTAMP_MODE:
    {
        guint16 table[] = {
            TIMESTAMP_MODE_OFF,
            TIMESTAMP_MODE_BINARY,
            TIMESTAMP_MODE_BINARYANDASCII,
            TIMESTAMP_MODE_ASCII,
        };

        err = pco_set_timestamp_mode(priv->pco, table[g_value_get_enum(value)]);
    }
    break;

    case PROP_EDGE_GLOBAL_SHUTTER:
    {
        //     pco_edge_shutter shutter;

        //     shutter = g_value_get_boolean(value) ? PCO_EDGE_GLOBAL_SHUTTER : PCO_EDGE_ROLLING_SHUTTER;
        //     err = pcoclhs_edge_set_shutter(priv->pco, shutter);
        //     pcoclhs_destroy(priv->pco);
        //     g_warning("Camera rebooting... Create a new camera instance to continue.");
    }
    break;

    case PROP_FRAMES_PER_SECOND:
    {
        gdouble rate = g_value_get_double(value);
        err = pco_set_fps(priv->pco, rate);
    }

    case PROP_FRAME_GRABBER_TIMEOUT:
    {
        gint timeout = g_value_get_uint(value);
        if (timeout < 0)
            timeout = INT32_MAX;
        err = pco_grabber_set_timeout(priv->pco, timeout);
    }
    break;

    case PROP_GRAB_N_BUFFERS:
    {
        priv->num_buffers = g_value_get_uint(value);
    }
    break;

    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        return;
    }

    check_pco_clhs_property_error(err, property_id);
}

static void uca_pco_clhs_camera_get_property(GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
    UcaPcoClhsCameraPrivate *priv;
    guint err = PCO_NOERROR;

    priv = UCA_PCO_CLHS_CAMERA_GET_PRIVATE(object);

    /* https://github.com/ufo-kit/libuca/issues/20 - Avoid property access while recording */
    if (uca_camera_is_recording(UCA_CAMERA(object)))
    {
        g_warning("Property '%s' cannot be accessed during acquisition", pspec->name);
        return;
    }

    switch (property_id)
    {
    case PROP_SENSOR_EXTENDED:
    {
        guint16 format;
        err = pco_get_sensor_format(priv->pco, &format);
        g_value_set_boolean(value, format == SENSORFORMAT_EXTENDED);
    }
    break;

    case PROP_SENSOR_WIDTH:
    {
        guint16 w, h, wx, hx;
        pco_get_resolution(priv->pco, &w, &h, &wx, &hx);
        g_value_set_uint(value, w);
    }
    break;

    case PROP_SENSOR_HEIGHT:
    {
        guint16 w, h, wx, hx;
        pco_get_resolution(priv->pco, &w, &h, &wx, &hx);
        g_value_set_uint(value, h);
    }
    break;

    case PROP_SENSOR_PIXEL_WIDTH:
        switch (priv->description->type)
        {
        case CAMERATYPE_PCO_EDGE_HS:
            g_value_set_double(value, 0.0000065);
            break;
        }
        break;

    case PROP_SENSOR_PIXEL_HEIGHT:
        switch (priv->description->type)
        {
        case CAMERATYPE_PCO_EDGE_HS:
            g_value_set_double(value, 0.0000065);
            break;
        }
        break;

    case PROP_SENSOR_WIDTH_EXTENDED:
    {
        guint16 w, h, wx, hx;
        pco_get_resolution(priv->pco, &w, &h, &wx, &hx);
        g_value_set_uint(value, wx < w ? w : wx);
    }
    break;

    case PROP_SENSOR_HEIGHT_EXTENDED:
    {
        guint16 w, h, wx, hx;
        pco_get_resolution(priv->pco, &w, &h, &wx, &hx);
        g_value_set_uint(value, hx < h ? h : hx);
    }
    break;

    case PROP_SENSOR_HORIZONTAL_BINNING:
    {
        guint16 h, v;
        pco_get_binning(priv->pco, &h, &v);
        g_value_set_uint(value, h);
    }
    break;

    case PROP_SENSOR_VERTICAL_BINNING:
    {
        guint16 h, v;
        pco_get_binning(priv->pco, &h, &v);
        g_value_set_uint(value, v);
    }
    break;

    case PROP_SENSOR_BITDEPTH:
        switch (priv->description->type)
        {
        case CAMERATYPE_PCO_EDGE_HS:
        {
            g_value_set_uint(value, 16);
            break;
        }
        default:
        {
            g_value_set_uint(value, 16);
            break;
        }
        }
        break;

    case PROP_SENSOR_TEMPERATURE:
    {
        gint16 ccd, camera, power;
        err = pco_get_temperature(priv->pco, &ccd, &camera, &power);
        g_value_set_double(value, ccd / 10.0);
    }
    break;

    case PROP_SENSOR_PIXELRATES:
        g_value_set_boxed(value, priv->pixelrates);
        break;

    case PROP_SENSOR_PIXELRATE:
    {
        guint32 pixelrate;
        err = pco_get_pixelrate(priv->pco, &pixelrate);
        g_value_set_uint(value, pixelrate);
    }
    break;

    case PROP_DELAY_TIME:
    {
        double delay;
        err = pco_get_delay_time(priv->pco, &delay);
        g_value_set_double(value, delay);
    }
    break;

    case PROP_EXPOSURE_TIME:
    {
        double exposure;
        err = pco_get_exposure_time(priv->pco, &exposure);
        g_value_set_double(value, exposure);
    }
    break;

    case PROP_HAS_DOUBLE_IMAGE_MODE:
        g_value_set_boolean(value, pco_is_double_image_mode_available(priv->pco));
        break;

    case PROP_DOUBLE_IMAGE_MODE:
        if (pco_is_double_image_mode_available(priv->pco))
        {
            bool on;
            err = pco_get_double_image_mode(priv->pco, &on);
            g_value_set_boolean(value, on);
        }
        break;

    case PROP_HAS_STREAMING:
        g_value_set_boolean(value, TRUE);
        break;

    case PROP_HAS_CAMRAM_RECORDING:
        g_value_set_boolean(value, FALSE); /* Edge cameras don't have onboard RAM */
        break;

    case PROP_ACQUIRE_MODE:
    {
        guint16 mode;
        err = pco_get_acquire_mode(priv->pco, &mode);

        if (mode == ACQUIRE_MODE_AUTO)
            g_value_set_enum(value, UCA_PCO_CLHS_CAMERA_ACQUIRE_MODE_AUTO);
        else if (mode == ACQUIRE_MODE_EXTERNAL)
            g_value_set_enum(value, UCA_PCO_CLHS_CAMERA_ACQUIRE_MODE_EXTERNAL);
        else
            g_warning("pco acquire mode not handled");
    }
    break;

    case PROP_FAST_SCAN:
    {
        guint32 mode;
        err = pco_get_scan_mode(priv->pco, &mode);
        g_value_set_boolean(value, mode == PCO_SCANMODE_FAST);
    }
    break;

    case PROP_TRIGGER_SOURCE:
    {
        guint16 mode;
        err = pco_get_trigger_mode(priv->pco, &mode);

        switch (mode)
        {
        case TRIGGER_MODE_AUTOTRIGGER:
            g_value_set_enum(value, UCA_CAMERA_TRIGGER_SOURCE_AUTO);
            break;
        case TRIGGER_MODE_SOFTWARETRIGGER:
            g_value_set_enum(value, UCA_CAMERA_TRIGGER_SOURCE_SOFTWARE);
            break;
        case TRIGGER_MODE_EXTERNALTRIGGER:
            g_value_set_enum(value, UCA_CAMERA_TRIGGER_SOURCE_EXTERNAL);
            break;
        default:
            g_warning("pco trigger mode not handled");
        }
    }
    break;

    case PROP_ROI_X:
    {
        guint16 window[4];
        pco_get_roi(priv->pco, window);
        g_value_set_uint(value, window[0]);
    }
    break;

    case PROP_ROI_Y:
    {
        guint16 window[4];
        pco_get_roi(priv->pco, window);
        g_value_set_uint(value, window[1]);
    }
    break;

    case PROP_ROI_WIDTH:
    {
        guint16 window[4];
        pco_get_roi(priv->pco, window);
        g_value_set_uint(value, window[2] - window[0]);
    }
    break;

    case PROP_ROI_HEIGHT:
    {
        guint16 window[4];
        pco_get_roi(priv->pco, window);
        g_value_set_uint(value, window[3] - window[1]);
    }
    break;

    case PROP_ROI_WIDTH_MULTIPLIER:
    {
        guint16 h, v;
        pco_get_roi_steps(priv->pco, &h, &v);
        g_value_set_uint(value, h);
    }
    break;

    case PROP_ROI_HEIGHT_MULTIPLIER:
    {
        guint16 h, v;
        pco_get_roi_steps(priv->pco, &h, &v);
        g_value_set_uint(value, v);
    }
    break;

    case PROP_NAME:
    {
        char *name;
        pco_get_name(priv->pco, &name);
        g_value_set_string(value, name);
    }
    break;

    case PROP_NOISE_FILTER:
    {
        guint16 mode;
        err = pco_get_noise_filter_mode(priv->pco, &mode);
        g_value_set_boolean(value, mode != NOISE_FILTER_MODE_OFF);
    }
    break;

    case PROP_TIMESTAMP_MODE:
    {
        guint16 mode;
        UcaPcoClhsCameraTimestamp table[] = {
            UCA_PCO_CLHS_CAMERA_TIMESTAMP_NONE,
            UCA_PCO_CLHS_CAMERA_TIMESTAMP_BINARY,
            UCA_PCO_CLHS_CAMERA_TIMESTAMP_BOTH,
            UCA_PCO_CLHS_CAMERA_TIMESTAMP_ASCII};

        err = pco_get_timestamp_mode(priv->pco, &mode);
        g_value_set_enum(value, table[mode]);
    }
    break;

    case PROP_VERSION:
        g_value_set_string(value, priv->version);
        break;

    case PROP_EDGE_GLOBAL_SHUTTER:
    {
        pco_edge_shutter shutter;
        err = pco_edge_get_shutter(priv->pco, &shutter);
        g_value_set_boolean(value, shutter == PCO_EDGE_GLOBAL_SHUTTER);
    }
    break;

    case PROP_IS_RECORDING:
    {
        bool is_recording = pco_is_recording(priv->pco);
        g_value_set_boolean(value, (gboolean)is_recording);
    }
    break;

    case PROP_FRAMES_PER_SECOND:
    {
        gdouble rate;
        err = pco_get_fps(priv->pco, &rate);
        g_value_set_double(value, rate);
        // guint16 w, h;
        // pco_get_pixelrate(priv->pco, &rate);
        // pco_grabber_get_actual_size(priv->pco, &w, &h);
        // g_value_set_float(value, ((float)(w * h)) / (float)rate);
    }
    break;

    case PROP_FRAME_GRABBER_TIMEOUT:
    {
        int timeout;
        pco_grabber_get_timeout(priv->pco, &timeout);
        g_value_set_uint(value, (guint)timeout);
    }
    break;

    case PROP_GRAB_N_BUFFERS:
    {
        g_value_set_uint(value, priv->num_buffers);
    }
    break;

    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        return;
    }

    check_pco_clhs_property_error(err, property_id);
}

static void uca_pco_clhs_camera_finalize(GObject *object)
{
    UcaPcoClhsCameraPrivate *priv = UCA_PCO_CLHS_CAMERA_GET_PRIVATE(object);

    if (priv->thread_running)
    {
        priv->thread_running = FALSE;
        g_thread_join(priv->grab_thread);
    }

    if (priv->pixelrates)
        g_value_array_free(priv->pixelrates);

    pco_grabber_free_memory(priv->pco);

    if (priv->version)
    {
        g_free(priv->version);
        priv->version = NULL;
    }

    if (priv->pco)
        pco_destroy(priv->pco);

    g_free(priv->grab_buffer);
    g_clear_error(&priv->construct_error);

    G_OBJECT_CLASS(uca_pco_clhs_camera_parent_class)->finalize(object);
}

static gboolean uca_pco_clhs_camera_initable_init(GInitable *initable, GCancellable *cancellable, GError **error)
{
    UcaPcoClhsCamera *camera;
    UcaPcoClhsCameraPrivate *priv;

    g_return_val_if_fail(UCA_IS_PCO_CLHS_CAMERA(initable), FALSE);

    if (cancellable != NULL)
    {
        g_set_error_literal(error, G_IO_ERROR, G_IO_ERROR_NOT_SUPPORTED,
                            "Cancellable initialization not supported");
        return FALSE;
    }

    camera = UCA_PCO_CLHS_CAMERA(initable);
    priv = camera->priv;

    if (priv->construct_error != NULL)
    {
        if (error)
            *error = g_error_copy(priv->construct_error);

        return FALSE;
    }

    return TRUE;
}

static void uca_pco_clhs_camera_initable_iface_init(GInitableIface *iface)
{
    iface->init = uca_pco_clhs_camera_initable_init;
}

static void uca_pco_clhs_camera_class_init(UcaPcoClhsCameraClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
    gobject_class->set_property = uca_pco_clhs_camera_set_property;
    gobject_class->get_property = uca_pco_clhs_camera_get_property;
    gobject_class->finalize = uca_pco_clhs_camera_finalize;

    UcaCameraClass *camera_class = UCA_CAMERA_CLASS(klass);
    camera_class->start_recording = uca_pco_clhs_camera_start_recording;
    camera_class->stop_recording = uca_pco_clhs_camera_stop_recording;
    camera_class->trigger = uca_pco_clhs_camera_trigger;
    camera_class->grab = uca_pco_clhs_camera_grab;

    for (guint i = 0; base_overrideables[i] != 0; i++)
        g_object_class_override_property(gobject_class, base_overrideables[i], uca_camera_props[base_overrideables[i]]);

    /**
     * UcaPcoClhsCamera:sensor-extended:
     *
     * Activate larger sensor area that contains surrounding pixels for dark
     * references and dummies. Use #UcaPcoClhsCamera:sensor-width-extended and
     * #UcaPcoClhsCamera:sensor-height-extended to query the resolution of the
     * larger area.
     */
    pco_clhs_properties[PROP_SENSOR_EXTENDED] =
        g_param_spec_boolean("sensor-extended",
                             "Use extended sensor format",
                             "Use extended sensor format",
                             FALSE, G_PARAM_READWRITE);

    pco_clhs_properties[PROP_SENSOR_WIDTH_EXTENDED] =
        g_param_spec_uint("sensor-width-extended",
                          "Width of extended sensor",
                          "Width of the extended sensor in pixels",
                          1, G_MAXUINT, 1,
                          G_PARAM_READABLE);

    pco_clhs_properties[PROP_SENSOR_HEIGHT_EXTENDED] =
        g_param_spec_uint("sensor-height-extended",
                          "Height of extended sensor",
                          "Height of the extended sensor in pixels",
                          1, G_MAXUINT, 1,
                          G_PARAM_READABLE);

    /**
     * UcaPcoClhsCamera:sensor-pixelrate:
     *
     * Read and write the pixel rate or clock of the sensor in terms of Hertz.
     * Make sure to query the possible pixel rates through the
     * #UcaPcoClhsCamera:sensor-pixelrates property. Any other value will be
     * rejected by the camera.
     */
    pco_clhs_properties[PROP_SENSOR_PIXELRATE] =
        g_param_spec_uint("sensor-pixelrate",
                          "Pixel rate",
                          "Pixel rate",
                          1, G_MAXUINT, 1,
                          G_PARAM_READWRITE);

    pco_clhs_properties[PROP_SENSOR_PIXELRATES] =
        g_param_spec_value_array("sensor-pixelrates",
                                 "Array of possible sensor pixel rates",
                                 "Array of possible sensor pixel rates",
                                 pco_clhs_properties[PROP_SENSOR_PIXELRATE],
                                 G_PARAM_READABLE);

    pco_clhs_properties[PROP_NAME] =
        g_param_spec_string("name",
                            "Name of the camera",
                            "Name of the camera",
                            "", G_PARAM_READABLE);

    pco_clhs_properties[PROP_SENSOR_TEMPERATURE] =
        g_param_spec_double("sensor-temperature",
                            "Temperature of the sensor",
                            "Temperature of the sensor in degrees Celsius",
                            -G_MAXDOUBLE, G_MAXDOUBLE, 0.0,
                            G_PARAM_READABLE);

    pco_clhs_properties[PROP_HAS_DOUBLE_IMAGE_MODE] =
        g_param_spec_boolean("has-double-image-mode",
                             "Is double image mode supported by this model",
                             "Is double image mode supported by this model",
                             FALSE, G_PARAM_READABLE);

    pco_clhs_properties[PROP_DOUBLE_IMAGE_MODE] =
        g_param_spec_boolean("double-image-mode",
                             "Use double image mode",
                             "Use double image mode",
                             FALSE, G_PARAM_READWRITE);

    pco_clhs_properties[PROP_ACQUIRE_MODE] =
        g_param_spec_enum("acquire-mode",
                          "Acquire mode",
                          "Acquire mode",
                          UCA_TYPE_PCO_CLHS_CAMERA_ACQUIRE_MODE, UCA_PCO_CLHS_CAMERA_ACQUIRE_MODE_AUTO,
                          G_PARAM_READWRITE);

    pco_clhs_properties[PROP_FAST_SCAN] =
        g_param_spec_boolean("fast-scan",
                             "Use fast scan mode",
                             "Use fast scan mode with less dynamic range",
                             FALSE, G_PARAM_READWRITE);

    pco_clhs_properties[PROP_NOISE_FILTER] =
        g_param_spec_boolean("noise-filter",
                             "Noise filter",
                             "Noise filter",
                             FALSE, G_PARAM_READWRITE);

    pco_clhs_properties[PROP_TIMESTAMP_MODE] =
        g_param_spec_enum("timestamp-mode",
                          "Timestamp mode",
                          "Timestamp mode",
                          UCA_TYPE_PCO_CLHS_CAMERA_TIMESTAMP, UCA_PCO_CLHS_CAMERA_TIMESTAMP_NONE,
                          G_PARAM_READWRITE);

    pco_clhs_properties[PROP_VERSION] =
        g_param_spec_string("version",
                            "Camera version",
                            "Camera version given as `serial number, hardware major.minor, firmware major.minor'",
                            DEFAULT_VERSION,
                            G_PARAM_READABLE);

    pco_clhs_properties[PROP_EDGE_GLOBAL_SHUTTER] =
        g_param_spec_boolean("global-shutter",
                             "Global shutter enabled",
                             "Global shutter enabled",
                             FALSE, G_PARAM_READABLE);

    pco_clhs_properties[PROP_FRAME_GRABBER_TIMEOUT] =
        g_param_spec_uint("frame-grabber-timeout",
                          "Frame grabber timeout in seconds",
                          "Frame grabber timeout in seconds",
                          0, G_MAXUINT, 5,
                          G_PARAM_READWRITE);

    pco_clhs_properties[PROP_DELAY_TIME] =
        g_param_spec_double("delay-time",
                            "Capture delay time",
                            "Capture delay time in seconds",
                            0., 1., 0.,
                            G_PARAM_READWRITE);

    for (guint id = N_BASE_PROPERTIES; id < N_PROPERTIES; id++)
        g_object_class_install_property(gobject_class, id, pco_clhs_properties[id]);

    g_type_class_add_private(klass, sizeof(UcaPcoClhsCameraPrivate));
}

static gboolean setup_pco_clhs_camera(UcaPcoClhsCameraPrivate *priv)
{
    pco_clhs_map_entry *map_entry;
    guint16 roi[4];
    guint16 camera_type;
    guint16 camera_subtype;
    guint32 serial;
    guint16 version[4];
    guint32 err;
    GError **error = &priv->construct_error;

    priv->pco = pco_init(0, 0);

    if (priv->pco == NULL)
    {
        g_set_error(error,
                    UCA_PCO_CLHS_CAMERA_ERROR, UCA_PCO_CLHS_CAMERA_ERROR_PCOSDK_INIT,
                    "Initializing pco wrapper failed");
        return FALSE;
    }

    pco_get_camera_type(priv->pco, &camera_type, &camera_subtype);
    map_entry = get_pco_clhs_map_entry(camera_type, camera_subtype);

    if (map_entry == NULL)
    {
        g_set_error(error,
                    UCA_PCO_CLHS_CAMERA_ERROR, UCA_PCO_CLHS_CAMERA_ERROR_UNSUPPORTED,
                    "Camera type is not supported: 0x%x::0x%x", camera_type, camera_subtype);
        return FALSE;
    }

    priv->description = map_entry;

    priv->num_recorded_images = 0;

    err = pco_get_camera_version(priv->pco, &serial, &version[0], &version[1], &version[2], &version[3]);
    CHECK_AND_RETURN_VAL_ON_PCO_ERROR(err, FALSE);

    g_free(priv->version);
    priv->version = g_strdup_printf("%u, %u.%u, %u.%u", serial, version[0], version[1], version[2], version[3]);

    return TRUE;
}

static gboolean setup_frame_grabber(UcaPcoClhsCameraPrivate *priv)
{
    // Handled by wrapper library
    return TRUE;
}

static void override_property_ranges(UcaPcoClhsCamera *camera)
{
    UcaPcoClhsCameraPrivate *priv;
    GObjectClass *oclass;

    priv = UCA_PCO_CLHS_CAMERA_GET_PRIVATE(camera);
    oclass = G_OBJECT_CLASS(UCA_PCO_CLHS_CAMERA_GET_CLASS(camera));

    // guint16 w, h;
    // pco_grabber_get_actual_size(priv->pco, &w, &h);
    // property_override_default_guint_value(oclass, "roi-width", w);
    // property_override_default_guint_value(oclass, "roi-height", h);

    guint32 rates[4] = {0};
    gint num_rates = 0;

    if (pco_get_available_pixelrates(priv->pco, rates, &num_rates) == PCO_NOERROR)
    {
        fill_pixelrates(priv, rates, num_rates);
        // property_override_default_guint_value(oclass, "sensor-pixelrate", rates[0]);
    }
}

static void
uca_pco_clhs_camera_init(UcaPcoClhsCamera *self)
{
    UcaCamera *camera;
    UcaPcoClhsCameraPrivate *priv;

    self->priv = priv = UCA_PCO_CLHS_CAMERA_GET_PRIVATE(self);

    priv->description = NULL;
    priv->last_frame = 0;
    priv->grab_buffer = NULL;
    priv->construct_error = NULL;
    priv->version = g_strdup(DEFAULT_VERSION);

    if (!setup_pco_clhs_camera(priv))
        return;

    if (!setup_frame_grabber(priv))
        return;

    override_property_ranges(self);

    camera = UCA_CAMERA(self);
    uca_camera_register_unit(camera, "sensor-width-extended", UCA_UNIT_PIXEL);
    uca_camera_register_unit(camera, "sensor-height-extended", UCA_UNIT_PIXEL);
    uca_camera_register_unit(camera, "sensor-temperature", UCA_UNIT_DEGREE_CELSIUS);
    uca_camera_register_unit(camera, "delay-time", UCA_UNIT_SECOND);
    uca_camera_set_writable(camera, "frames-per-second", TRUE);
}

G_MODULE_EXPORT GType camera_plugin_get_type(void)
{
    return UCA_TYPE_PCO_CLHS_CAMERA;
}

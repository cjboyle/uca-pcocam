#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "pcoclhs.h"

#include "pco/Cpco_com_clhs.h"
#include "pco/Cpco_grab_clhs.h"

#define PCO_ERRT_H_CREATE_OBJECT
#define sprintf_s snprintf
#include "pco/PCO_errt.h"
/** 
 * This module serves as a wrapper to access the C++ implementations of
 * the PCO.Camera methods. Otherwise, it is essentially a port of
 * UFO-KIT's libpco library partial implementation.
 */

#define CHECK_ERROR(code)                                                      \
    if ((code) != 0)                                                           \
    {                                                                          \
        fprintf(stderr, "Error: %x at <%s:%i>\n", (code), __FILE__, __LINE__); \
        fprintf(stderr, "  %s\n", _get_error_text((code)));                    \
    }

#define CHECK_ERROR_AND_RETURN(code) \
    {                                \
        CHECK_ERROR(code);           \
        if ((code) != 0)             \
            return (code);           \
    }

#define CHECK_ERROR_THEN_RETURN(code) \
    {                                 \
        CHECK_ERROR_AND_RETURN(code); \
        return 0;                     \
    }

/* Static helper functions */

static char *_get_error_text(DWORD code)
{
    char *s = (char *)malloc(255);
    PCO_GetErrorText(code, s, 255);
    return (char *)s;
}

static void _decode_line(int width, void *bufout, void *bufin)
{
    uint32_t *lineadr_in = (uint32_t *)bufin;
    uint32_t *lineadr_out = (uint32_t *)bufout;
    uint32_t a;

    for (int x = 0; x < (width * 12) / 32; x += 3)
    {
        a = (*lineadr_in & 0x0000FFF0) >> 4;
        a |= (*lineadr_in & 0x0000000F) << 24;
        a |= (*lineadr_in & 0xFF000000) >> 8;
        *lineadr_out = a;
        lineadr_out++;

        a = (*lineadr_in & 0x00FF0000) >> 12;
        lineadr_in++;
        a |= (*lineadr_in & 0x0000F000) >> 12;
        a |= (*lineadr_in & 0x00000FFF) << 16;
        *lineadr_out = a;
        lineadr_out++;

        a = (*lineadr_in & 0xFFF00000) >> 20;
        a |= (*lineadr_in & 0x000F0000) << 8;
        lineadr_in++;
        a |= (*lineadr_in & 0x0000FF00) << 8;
        *lineadr_out = a;
        lineadr_out++;

        a = (*lineadr_in & 0x000000FF) << 4;
        a |= (*lineadr_in & 0xF0000000) >> 28;
        a |= (*lineadr_in & 0x0FFF0000);
        *lineadr_out = a;
        lineadr_out++;
        lineadr_in++;
    }
}

static void func_reorder_image_5x12(uint16_t *bufout, uint16_t *bufin, int width, int height)
{
    uint16_t *line_top = bufout;
    uint16_t *line_bottom = bufout + (height - 1) * width;
    uint16_t *line_in = bufin;
    int off = (width * 12) / 16;

    for (int y = 0; y < height / 2; y++)
    {
        _decode_line(width, line_top, line_in);
        line_in += off;
        _decode_line(width, line_bottom, line_in);
        line_in += off;
        line_top += width;
        line_bottom -= width;
    }
}

static void func_reorder_image_5x16(uint16_t *bufout, uint16_t *bufin, int width, int height)
{
    uint16_t *line_top = bufout;
    uint16_t *line_bottom = bufout + (height - 1) * width;
    uint16_t *line_in = bufin;

    for (int y = 0; y < height / 2; y++)
    {
        memcpy(line_top, line_in, width * sizeof(uint16_t));
        line_in += width;
        memcpy(line_bottom, line_in, width * sizeof(uint16_t));
        line_in += width;
        line_top += width;
        line_bottom -= width;
    }
}

static void _fill_binning_array(uint16_t *a, unsigned int n, int is_linear)
{
    if (is_linear)
    {
        for (int i = 0; i < n; i++)
            a[i] = i + 1;
    }
    else
    {
        for (int i = 0, j = 1; i < n; i++, j *= 2)
            a[i] = j;
    }
}

static uint16_t _msb_position(uint16_t x)
{
    uint16_t val = 0;
    while (x >>= 1)
        ++val;
    return val;
}

static uint16_t _get_num_binnings(uint16_t max_binning, int is_linear)
{
    return is_linear ? max_binning : _msb_position(max_binning) + 1;
}

/*************************/

struct _pcoclhs_handle
{
    CPco_com_clhs *com;      /* Comm. interface to a camera */
    CPco_grab_clhs *grabber; /* Frame-grabber interface */

    uint32_t cachedDelay, cachedExposure;
    pcoclhs_reorder_image_t reorder_func;

    void *imgadr;
};

static unsigned int _pcoclhs_init(pcoclhs_handle *pco, int board, int port)
{
    DWORD err;
    pco = (pcoclhs_handle *)malloc(sizeof(pcoclhs_handle));

    CPco_com_clhs *com;
    com = new CPco_com_clhs();
    pco->com = com;

    CPco_grab_clhs *grab;
    grab = new CPco_grab_clhs(com);
    pco->grabber = grab;

    for (unsigned int i = 0; i <= 8; i++)
    {
        err = pco->com->Open_Cam(i);
        if (err == PCO_NOERROR)
            break;
    }
    CHECK_ERROR_AND_RETURN(err);

    err = pco->grabber->Open_Grabber(0);
    CHECK_ERROR_AND_RETURN(err);

    pco->reorder_func = &func_reorder_image_5x16;

    err = pco->grabber->Set_Grabber_Timeout(10000);
    CHECK_ERROR_AND_RETURN(err);

    err = pco->com->PCO_SetCameraToCurrentTime();
    CHECK_ERROR_AND_RETURN(err);

    err = pco->com->PCO_ResetSettingsToDefault();
    CHECK_ERROR_AND_RETURN(err);

    err = pco->com->PCO_SetBitAlignment(BIT_ALIGNMENT_LSB);
    CHECK_ERROR_AND_RETURN(err);

    err = pcoclhs_set_recording_state(pco, 0);
    CHECK_ERROR_AND_RETURN(err);

    err = pcoclhs_arm_camera(pco);
    CHECK_ERROR_AND_RETURN(err);

    return PCO_NOERROR;
}

void pcoclhs_init(pcoclhs_handle *pco, int board, int port)
{
    if (_pcoclhs_init(pco, 0, 0) != PCO_NOERROR)
        pcoclhs_destroy(pco);
}

void pcoclhs_destroy(pcoclhs_handle *pco)
{
    if (pco == NULL)
        return;

    pco->grabber->Close_Grabber();
    pco->com->Close_Cam();

    delete pco->com;
    delete pco->grabber;
}

unsigned int pcoclhs_control_command(pcoclhs_handle *pco, void *buffer_in, uint32_t size_in, void *buffer_out, uint32_t size_out)
{
    DWORD err = pco->com->Control_Command(buffer_in, size_in, buffer_out, size_out);
    CHECK_ERROR_THEN_RETURN(err);
}

unsigned int pcoclhs_grabber_set_size(pcoclhs_handle *pco, uint32_t width, uint32_t height)
{
    DWORD err = pco->grabber->Set_Grabber_Size(width, height);
    CHECK_ERROR_THEN_RETURN(err);
}

unsigned int pcoclhs_grabber_allocate_memory(pcoclhs_handle *pco, int size)
{
    // not actually implemented, just returns PCO_NOERROR
    return pco->grabber->Allocate_Framebuffer(size);
}

unsigned int pcoclhs_grabber_free_memory(pcoclhs_handle *pco)
{
    // not actually implemented, just returns PCO_NOERROR
    return pco->grabber->Free_Framebuffer();
}

unsigned int pcoclhs_prepare_recording(pcoclhs_handle *pco)
{
    DWORD err;

    uint32_t cameraW, cameraH;
    err = pco->com->PCO_GetActualSize(&cameraW, &cameraH);
    CHECK_ERROR_AND_RETURN(err);

    uint32_t grabberW, grabberH, depth;
    err = pco->grabber->Get_actual_size(&grabberW, &grabberH, &depth);
    CHECK_ERROR_AND_RETURN(err);

    if (cameraW != grabberW || cameraH != grabberH)
    {
        err = pco->grabber->Set_Grabber_Size(cameraW, cameraH);
        CHECK_ERROR_AND_RETURN(err);
    }

    err = pco->grabber->PostArm();
    CHECK_ERROR_AND_RETURN(err);

    return pcoclhs_set_recording_state(pco, 1);
}

unsigned int pcoclhs_start_recording(pcoclhs_handle *pco)
{
    DWORD err = pcoclhs_prepare_recording(pco);
    CHECK_ERROR_AND_RETURN(err);
    err = pco->grabber->Start_Acquire();
    CHECK_ERROR_THEN_RETURN(err);
}

unsigned int pcoclhs_stop_recording(pcoclhs_handle *pco)
{
    DWORD err = pcoclhs_set_recording_state(pco, 0);
    CHECK_ERROR_AND_RETURN(err);
    err = pco->grabber->Stop_Acquire();
    CHECK_ERROR_THEN_RETURN(err);
}

unsigned int pcoclhs_is_recording(pcoclhs_handle *pco, bool *is_recording)
{
    *is_recording = pco->grabber->started() == TRUE;
    return 0;
}

unsigned int pcoclhs_is_active(pcoclhs_handle *pco)
{
    uint16_t discard1, discard2;
    return pcoclhs_get_camera_type(pco, &discard1, &discard2) == 0;
}

static unsigned int __pcoclhs_get_camera_type(pcoclhs_handle *pco, SC2_Camera_Type_Response *resp)
{
    SC2_Simple_Telegram req = {.wCode = GET_CAMERA_TYPE, .wSize = sizeof(req)};
    DWORD err = pcoclhs_control_command(pco, &req, sizeof(req), &resp, sizeof(resp));
    CHECK_ERROR_AND_RETURN(err);
}

unsigned int pcoclhs_get_camera_type(pcoclhs_handle *pco, uint16_t *type, uint16_t *subtype)
{
    SC2_Camera_Type_Response resp;
    DWORD err = __pcoclhs_get_camera_type(pco, &resp);
    if (err == 0)
    {
        *type = resp.wCamType;
        *subtype = resp.wCamSubType;
    }
    return err;
}

unsigned int pcoclhs_get_camera_version(pcoclhs_handle *pco, uint32_t *serial_number, uint16_t *hw_major, uint16_t *hw_minor, uint16_t *fw_major, uint16_t *fw_minor)
{
    SC2_Camera_Type_Response resp;
    DWORD err = __pcoclhs_get_camera_type(pco, &resp);
    if (err == 0)
    {
        *serial_number = resp.dwSerialNumber;
        *hw_major = resp.dwHWVersion >> 16;
        *hw_minor = resp.dwHWVersion & 0xFFFF;
        *fw_major = resp.dwFWVersion >> 16;
        *fw_minor = resp.dwFWVersion & 0xFFFF;
    }
    return err;
}

unsigned int pcoclhs_get_health_state(pcoclhs_handle *pco, uint32_t *warnings, uint32_t *errors, uint32_t *status)
{
    DWORD err = pco->com->PCO_GetHealthStatus(warnings, errors, status);
    CHECK_ERROR_THEN_RETURN(err);
}

unsigned int pcoclhs_reset(pcoclhs_handle *pco)
{
    pcoclhs_destroy(pco);
    return _pcoclhs_init(pco, 0, 0);
}

unsigned int pcoclhs_get_temperature(pcoclhs_handle *pco, int16_t *ccd, int16_t *camera, int16_t *power)
{
    DWORD err = pco->com->PCO_GetTemperature(ccd, camera, power);
    CHECK_ERROR_THEN_RETURN(err);
}

unsigned int pcoclhs_get_name(pcoclhs_handle *pco, char **name)
{
    char *str = (char *)malloc(40);
    DWORD err = pco->com->PCO_GetCameraName(str, sizeof(str));
    if (err != 0)
    {
        *name = "Unknown Camera Type";
    }
    else
    {
        char *s;
        strncpy(s, str, 40);
        *name = s;
    }
    return err;
}

unsigned int pcoclhs_get_sensor_format(pcoclhs_handle *pco, uint16_t *format)
{
    DWORD err = pco->com->PCO_GetSensorFormat(format);
    CHECK_ERROR_THEN_RETURN(err);
}

unsigned int pcoclhs_set_sensor_format(pcoclhs_handle *pco, uint16_t format)
{
    DWORD err = pco->com->PCO_SetSensorFormat(format);
    CHECK_ERROR_AND_RETURN(err);
    return pcoclhs_arm_camera(pco);
}

unsigned int pcoclhs_get_resolution(pcoclhs_handle *pco, uint16_t *width_std, uint16_t *height_std, uint16_t *width_ex, uint16_t *height_ex)
{
    SC2_Camera_Description_Response desc;
    DWORD err = pco->com->PCO_GetCameraDescriptor(&desc);
    CHECK_ERROR(err);
    if (err == 0)
    {
        *width_std = desc.wMaxHorzResStdDESC;
        *height_std = desc.wMaxVertResStdDESC;
        *width_ex = desc.wMaxHorzResExtDESC;
        *height_ex = desc.wMaxVertResExtDESC;
    }
    return err;
}

unsigned int pcoclhs_get_available_pixelrates(pcoclhs_handle *pco, uint32_t rates[4], int *num_rates)
{
    SC2_Camera_Description_Response desc;
    DWORD err = pco->com->PCO_GetCameraDescriptor(&desc);
    CHECK_ERROR(err);
    if (err == 0)
    {
        int n = 0;
        for (int i = 0; i < 4; i++)
        {
            if (desc.dwPixelRateDESC[i] > 0)
                rates[n++] = desc.dwPixelRateDESC[i];
        }
        *num_rates = n;
    }
    return err;
}

unsigned int pcoclhs_get_pixelrate(pcoclhs_handle *pco, uint32_t *rate)
{
    DWORD err = pco->com->PCO_GetPixelRate(rate);
    CHECK_ERROR_THEN_RETURN(err);
}

unsigned int pcoclhs_set_pixelrate(pcoclhs_handle *pco, uint32_t rate)
{
    DWORD err = pco->com->PCO_SetPixelRate(rate);
    CHECK_ERROR_AND_RETURN(err);
    return pcoclhs_arm_camera(pco);
}

unsigned int pcoclhs_get_available_conversion_factors(pcoclhs_handle *pco, uint16_t factors[4], int *num_factors)
{
    SC2_Camera_Description_Response desc;
    DWORD err = pco->com->PCO_GetCameraDescriptor(&desc);
    CHECK_ERROR(err);
    if (err == 0)
    {
        int n = 0;
        for (int i = 0; i < 4; i++)
        {
            if (desc.wConvFactDESC[i] > 0)
                factors[n++] = desc.wConvFactDESC[i];
        }
        *num_factors = n;
    }
    return err;
}

/**
 * mode = 0 (PCO_SCANMODE_SLOW) or 1 (PCO_SCANMODE_FAST)
 */
unsigned int pcoclhs_set_scan_mode(pcoclhs_handle *pco, uint32_t mode)
{
    uint32_t pixelrates[4];
    int n;
    DWORD err = pcoclhs_get_available_pixelrates(pco, pixelrates, &n);

    CHECK_ERROR_AND_RETURN(err);

    uint32_t pixelrate = pixelrates[mode];

    if (pixelrate == 0)
        return 0x80000000;

    if (mode == PCO_SCANMODE_SLOW)
    {
        pco->reorder_func = &func_reorder_image_5x16;
        pco->grabber->Set_DataFormat(SCCMOS_FORMAT_TOP_CENTER_BOTTOM_CENTER | PCO_CL_DATAFORMAT_5x16);
    }
    else if (mode == PCO_SCANMODE_FAST)
    {
        pco->reorder_func = &func_reorder_image_5x12;
        pco->grabber->Set_DataFormat(SCCMOS_FORMAT_TOP_CENTER_BOTTOM_CENTER | PCO_CL_DATAFORMAT_5x12);
    }

    return pcoclhs_set_pixelrate(pco, pixelrate);
}

unsigned int pcoclhs_get_scan_mode(pcoclhs_handle *pco, uint32_t *mode)
{
    uint32_t pixelrates[4];
    int n;
    DWORD err = pcoclhs_get_available_pixelrates(pco, pixelrates, &n);
    CHECK_ERROR_AND_RETURN(err);

    uint32_t curr_pixelrate;
    err = pcoclhs_get_pixelrate(pco, &curr_pixelrate);
    CHECK_ERROR_AND_RETURN(err);

    for (int i = 0; i < 4; i++)
    {
        if (curr_pixelrate == pixelrates[i])
        {
            *mode = i;
            return 0;
        }
    }

    *mode = 0xFFFF;
    return 0x80000000;
}

unsigned int pcoclhs_set_lut(pcoclhs_handle *pco, uint16_t key, uint16_t val)
{
    DWORD err = pco->com->PCO_SetLut(key, val);
    CHECK_ERROR_AND_RETURN(err);
    return pcoclhs_arm_camera(pco);
}

unsigned int pcoclhs_get_lut(pcoclhs_handle *pco, uint16_t *key, uint16_t *val)
{
    DWORD err = pco->com->PCO_GetLut(key, val);
    CHECK_ERROR_THEN_RETURN(err);
}

unsigned int pcoclhs_set_roi(pcoclhs_handle *pco, uint16_t *window)
{
    DWORD err = pco->com->PCO_SetROI(window[0], window[1], window[2], window[3]);
    CHECK_ERROR_AND_RETURN(err);
    return pcoclhs_arm_camera(pco);
}

unsigned int pcoclhs_get_roi(pcoclhs_handle *pco, uint16_t *window)
{
    uint16_t x1, y1, x2, y2;
    DWORD err = pco->com->PCO_GetROI(&x1, &y1, &x2, &y2);
    CHECK_ERROR(err);
    if (err == 0)
    {
        window[0] = x1;
        window[1] = y1;
        window[2] = x2;
        window[3] = y2;
    }
    return err;
}

unsigned int pcoclhs_get_roi_steps(pcoclhs_handle *pco, uint16_t *horizontal, uint16_t *vertical)
{
    SC2_Camera_Description_Response desc;
    DWORD err = pco->com->PCO_GetCameraDescriptor(&desc);
    CHECK_ERROR_AND_RETURN(err);
    *horizontal = desc.wRoiHorStepsDESC;
    *vertical = desc.wRoiVertStepsDESC;
    return 0;
}

bool pcoclhs_is_double_image_mode_available(pcoclhs_handle *pco)
{
    SC2_Camera_Description_Response desc;
    DWORD err = pco->com->PCO_GetCameraDescriptor(&desc);
    CHECK_ERROR(err);
    return err == 0 && desc.wDoubleImageDESC == 1;
}

unsigned int pcoclhs_set_double_image_mode(pcoclhs_handle *pco, bool on)
{
    DWORD err = pco->com->PCO_SetDoubleImageMode(on ? 1 : 0);
    CHECK_ERROR_AND_RETURN(err);
    return pcoclhs_arm_camera(pco);
}

unsigned int pcoclhs_get_double_image_mode(pcoclhs_handle *pco, bool *on)
{
    uint16_t tmp;
    DWORD err = pco->com->PCO_GetDoubleImageMode(&tmp);
    CHECK_ERROR(err);
    if (err == 0)
        *on = tmp == 1;
    return err;
}

unsigned int pcoclhs_set_offset_mode(pcoclhs_handle *pco, bool on)
{
    DWORD err = pco->com->PCO_SetOffsetMode(on ? 0 : 1); // 0=AUTO, 1=OFF
    CHECK_ERROR_AND_RETURN(err);
    return pcoclhs_arm_camera(pco);
}

unsigned int pcoclhs_get_offset_mode(pcoclhs_handle *pco, bool *on)
{
    uint16_t tmp;
    DWORD err = pco->com->PCO_GetOffsetMode(&tmp);
    CHECK_ERROR(err);
    if (err == 0)
        *on = tmp == 0;
    return err;
}

unsigned int pcoclhs_get_segment_sizes(pcoclhs_handle *pco, uint32_t sizes[4])
{
    DWORD err = pco->com->PCO_GetCameraRamSegmentSize(sizes);
    CHECK_ERROR_THEN_RETURN(err);
}

unsigned int pcoclhs_get_active_segment(pcoclhs_handle *pco, uint16_t *segment)
{
    DWORD err = pco->com->PCO_GetActiveRamSegment(segment);
    CHECK_ERROR_THEN_RETURN(err);
}

unsigned int pcoclhs_clear_active_segment(pcoclhs_handle *pco)
{
    DWORD err = pco->com->PCO_ClearRamSegment();
    CHECK_ERROR_THEN_RETURN(err);
}

unsigned int pcoclhs_get_bit_alignment(pcoclhs_handle *pco, bool *msb_aligned)
{
    uint16_t alignment;
    DWORD err = pco->com->PCO_GetBitAlignment(&alignment);
    CHECK_ERROR(err);
    if (err == 0)
        *msb_aligned = alignment == 0;
    return err;
}

unsigned int pcoclhs_set_bit_alignment(pcoclhs_handle *pco, bool msb_aligned)
{
    DWORD err = pco->com->PCO_SetBitAlignment(msb_aligned ? 0 : 1);
    CHECK_ERROR_AND_RETURN(err);
    return pcoclhs_arm_camera(pco);
}

unsigned int pcoclhs_get_num_images(pcoclhs_handle *pco, uint16_t segment, uint32_t *num_images)
{
    uint32_t discard = -1;
    DWORD err = pco->com->PCO_GetNumberOfImagesInSegment(segment, num_images, &discard);
    CHECK_ERROR_THEN_RETURN(err);
}

unsigned int pcoclhs_force_trigger(pcoclhs_handle *pco, uint16_t *success)
{
    DWORD err = pco->com->PCO_ForceTrigger(success);
    CHECK_ERROR_AND_RETURN(err);
    return pcoclhs_arm_camera(pco);
}

unsigned int pcoclhs_set_timestamp_mode(pcoclhs_handle *pco, uint16_t mode)
{
    DWORD err = pco->com->PCO_SetTimestampMode(mode);
    CHECK_ERROR_AND_RETURN(err);
    return pcoclhs_arm_camera(pco);
}

unsigned int pcoclhs_get_timestamp_mode(pcoclhs_handle *pco, uint16_t *mode)
{
    DWORD err = pco->com->PCO_GetTimestampMode(mode);
    CHECK_ERROR_THEN_RETURN(err);
}

unsigned int pcoclhs_set_timebase(pcoclhs_handle *pco, uint16_t delay, uint16_t expos)
{
    DWORD err = pco->com->PCO_SetTimebase(delay, expos);
    CHECK_ERROR_AND_RETURN(err);
    return pcoclhs_arm_camera(pco); //! check that this is called after SETs
}

unsigned int pcoclhs_get_timebase(pcoclhs_handle *pco, uint16_t *delay, uint16_t *expos)
{
    DWORD err = pco->com->PCO_GetTimebase(delay, expos);
    CHECK_ERROR_THEN_RETURN(err);
}

unsigned int pcoclhs_get_delay_time(pcoclhs_handle *pco, uint32_t *delay)
{
    uint32_t discard;
    return pcoclhs_get_delay_exposure(pco, delay, &discard);
}

unsigned int pcoclhs_set_delay_time(pcoclhs_handle *pco, uint32_t delay)
{
    return pcoclhs_set_delay_exposure(pco, delay, pco->cachedExposure);
}

unsigned int pcoclhs_get_delay_range(pcoclhs_handle *pco, uint32_t *min_ns, uint32_t *max_ms, uint32_t *step_ns)
{
    SC2_Camera_Description_Response desc;
    DWORD err = pco->com->PCO_GetCameraDescriptor(&desc);
    CHECK_ERROR(err);
    if (err == 0)
    {
        *min_ns = desc.dwMinDelayDESC;
        *max_ms = desc.dwMaxDelayDESC;
        *step_ns = desc.dwMinDelayStepDESC;
    }
    return err;
}

unsigned int pcoclhs_get_exposure_time(pcoclhs_handle *pco, uint32_t *exposure)
{
    uint32_t discard;
    return pcoclhs_get_delay_exposure(pco, &discard, exposure);
}

unsigned int pcoclhs_set_exposure_time(pcoclhs_handle *pco, uint32_t exposure)
{
    return pcoclhs_set_delay_exposure(pco, pco->cachedDelay, exposure);
}

unsigned int pcoclhs_get_exposure_range(pcoclhs_handle *pco, uint32_t *min_ns, uint32_t *max_ms, uint32_t *step_ns)
{
    SC2_Camera_Description_Response desc;
    DWORD err = pco->com->PCO_GetCameraDescriptor(&desc);
    CHECK_ERROR(err);
    if (err == 0)
    {
        *min_ns = desc.dwMinExposureDESC;
        *max_ms = desc.dwMaxExposureDESC;
        *step_ns = desc.dwMinExposureStepDESC;
    }
    return err;
}

unsigned int pcoclhs_get_delay_exposure(pcoclhs_handle *pco, uint32_t *delay, uint32_t *exposure)
{
    DWORD err = pco->com->PCO_GetDelayExposure(delay, exposure);
    CHECK_ERROR(err);
    if (err == 0)
    {
        pco->cachedDelay = *delay;
        pco->cachedExposure = *exposure;
    }
    return err;
}

unsigned int pcoclhs_set_delay_exposure(pcoclhs_handle *pco, uint32_t delay, uint32_t exposure)
{
    DWORD err = pco->com->PCO_SetDelayExposure(delay, exposure);
    CHECK_ERROR_AND_RETURN(err);
    pco->cachedDelay = delay;
    pco->cachedExposure = exposure;
    return pcoclhs_arm_camera(pco);
}

unsigned int pcoclhs_get_trigger_mode(pcoclhs_handle *pco, uint16_t *mode)
{
    DWORD err = pco->com->PCO_GetTriggerMode(mode);
    CHECK_ERROR_THEN_RETURN(err);
}

unsigned int pcoclhs_set_trigger_mode(pcoclhs_handle *pco, uint16_t mode)
{
    DWORD err = pco->com->PCO_SetTriggerMode(mode);
    CHECK_ERROR_AND_RETURN(err);
    return pcoclhs_arm_camera(pco);
}

unsigned int pcoclhs_set_framerate(pcoclhs_handle *pco, uint32_t framerate_mhz, uint32_t exposure_ns, bool framerate_priority)
{
    uint16_t mode = framerate_priority
                        ? SET_FRAMERATE_MODE_FRAMERATE_HAS_PRIORITY
                        : SET_FRAMERATE_MODE_EXPTIME_HAS_PRIORITY;
    uint16_t discard;
    DWORD err = pco->com->PCO_SetFrameRate(&discard, mode, &framerate_mhz, &exposure_ns);
    CHECK_ERROR_AND_RETURN(err);
    return pcoclhs_arm_camera(pco);
}

unsigned int pcoclhs_get_framerate(pcoclhs_handle *pco, uint32_t *framerate_mhz, uint32_t *exposure_ns)
{
    uint16_t discard;
    DWORD err = pco->com->PCO_GetFrameRate(&discard, framerate_mhz, exposure_ns);
    CHECK_ERROR_THEN_RETURN(err);
}

unsigned int pcoclhs_get_storage_mode(pcoclhs_handle *pco, uint16_t *mode)
{
    DWORD err = pco->com->PCO_GetStorageMode(mode);
    CHECK_ERROR_THEN_RETURN(err);
}

unsigned int pcoclhs_set_storage_mode(pcoclhs_handle *pco, uint16_t mode)
{
    DWORD err = pco->com->PCO_SetStorageMode(mode);
    CHECK_ERROR_AND_RETURN(err);
    return pcoclhs_arm_camera(pco);
}

unsigned int pcoclhs_get_record_mode(pcoclhs_handle *pco, uint16_t *mode)
{
    DWORD err = pco->com->PCO_GetRecorderSubmode(mode);
    CHECK_ERROR_THEN_RETURN(err);
}

unsigned int pcoclhs_set_record_mode(pcoclhs_handle *pco, uint16_t mode)
{
    DWORD err = pco->com->PCO_SetRecorderSubmode(mode);
    CHECK_ERROR_AND_RETURN(err);
    return pcoclhs_arm_camera(pco);
}

unsigned int pcoclhs_arm_camera(pcoclhs_handle *pco)
{
    int tout;
    pco->grabber->Get_Grabber_Timeout(&tout);
    pco->grabber->Set_Grabber_Timeout(tout + 5000);
    int ret = pco->com->PCO_ArmCamera();
    pco->grabber->Set_Grabber_Timeout(tout);
    CHECK_ERROR_THEN_RETURN(ret);
}

unsigned int pcoclhs_get_recording_state(pcoclhs_handle *pco, uint16_t *state)
{
    DWORD err = pco->com->PCO_GetRecordingState(state);
    CHECK_ERROR_THEN_RETURN(err);
}

unsigned int pcoclhs_set_recording_state(pcoclhs_handle *pco, uint16_t state)
{
    DWORD err = pco->com->PCO_SetRecordingState(state);
    CHECK_ERROR_THEN_RETURN(err);
}

unsigned int pcoclhs_get_acquire_mode(pcoclhs_handle *pco, uint16_t *mode)
{
    DWORD err = pco->com->PCO_GetAcquireMode(mode);
    CHECK_ERROR_THEN_RETURN(err);
}

unsigned int pcoclhs_set_acquire_mode(pcoclhs_handle *pco, uint16_t mode)
{
    DWORD err = pco->com->PCO_SetAcquireMode(mode);
    CHECK_ERROR_AND_RETURN(err);
    return pcoclhs_arm_camera(pco);
}

unsigned int pcoclhs_request_image(pcoclhs_handle *pco)
{
    DWORD err = pco->com->PCO_RequestImage();
    CHECK_ERROR_THEN_RETURN(err);
}

unsigned int pcoclhs_get_next_image(pcoclhs_handle *pco, void *adr)
{
    DWORD err;
    uint16_t mode, triggered;

    err = pcoclhs_get_trigger_mode(pco, &mode);
    CHECK_ERROR_AND_RETURN(err);

    if (mode == 0x0001)
    {
        err = pcoclhs_force_trigger(pco, &triggered);
        CHECK_ERROR_AND_RETURN(err);
    }

    DWORD err = pco->grabber->Wait_For_Next_Image(adr, 10000);
    CHECK_ERROR_THEN_RETURN(err);
}

unsigned int pcoclhs_get_image(pcoclhs_handle *pco, uint16_t segment, uint32_t number, void *adr)
{
    DWORD err = pco->grabber->Get_Image(segment, number, adr);
    CHECK_ERROR_THEN_RETURN(err);
}

unsigned int pcoclhs_read_images(pcoclhs_handle *pco, uint16_t segment, uint32_t start, uint32_t end)
{
    DWORD err = pco->com->PCO_ReadImagesFromSegment(segment, start, end);
    CHECK_ERROR_THEN_RETURN(err);
}

unsigned int pcoclhs_get_actual_size(pcoclhs_handle *pco, uint32_t *width, uint32_t *height)
{
    DWORD err = pco->com->PCO_GetActualSize(width, height);
    CHECK_ERROR_THEN_RETURN(err);
}

unsigned int pcoclhs_get_binning(pcoclhs_handle *pco, uint16_t *horizontal, uint16_t *vertical)
{
    DWORD err = pco->com->PCO_GetBinning(horizontal, vertical);
    CHECK_ERROR_THEN_RETURN(err);
}

unsigned int pcoclhs_set_binning(pcoclhs_handle *pco, uint16_t horizontal, uint16_t vertical)
{
    DWORD err = pco->com->PCO_SetBinning(horizontal, vertical);
    CHECK_ERROR_AND_RETURN(err);
    return pcoclhs_arm_camera(pco);
}

unsigned int pcoclhs_get_possible_binnings(pcoclhs_handle *pco, uint16_t **horizontal, unsigned int *num_horizontal, uint16_t **vertical, unsigned int *num_vertical)
{
    /* uint16_t maxBinHorz, stepBinHorz, maxBinVert, stepBinVert; */
    SC2_Camera_Description_Response desc;
    DWORD err = pco->com->PCO_GetCameraDescriptor(&desc);
    CHECK_ERROR_AND_RETURN(err);

    unsigned int num_h = _get_num_binnings(desc.wMaxBinHorzDESC, desc.wBinHorzSteppingDESC);
    uint16_t *r_horizontal = (uint16_t *)malloc(num_h * sizeof(uint16_t));
    _fill_binning_array(r_horizontal, num_h, desc.wBinHorzSteppingDESC);

    unsigned int num_v = _get_num_binnings(desc.wMaxBinVertDESC, desc.wBinVertSteppingDESC);
    uint16_t *r_vertical = (uint16_t *)malloc(num_v * sizeof(uint16_t));
    _fill_binning_array(r_vertical, num_v, desc.wBinVertSteppingDESC);

    *horizontal = r_horizontal;
    *vertical = r_vertical;
    *num_horizontal = num_h;
    *num_vertical = num_v;

    return 0;
}

unsigned int pcoclhs_set_hotpixel_correction(pcoclhs_handle *pco, uint32_t mode)
{
    DWORD err = pco->com->PCO_SetHotPixelCorrectionMode(mode);
    CHECK_ERROR_AND_RETURN(err);
    return pcoclhs_arm_camera(pco);
}

unsigned int pcoclhs_get_noise_filter_mode(pcoclhs_handle *pco, uint16_t *mode)
{
    DWORD err = pco->com->PCO_GetNoiseFilterMode(mode);
    CHECK_ERROR_THEN_RETURN(err);
}

unsigned int pcoclhs_set_noise_filter_mode(pcoclhs_handle *pco, uint16_t mode)
{
    DWORD err = pco->com->PCO_SetNoiseFilterMode(mode);
    CHECK_ERROR_AND_RETURN(err);
    return pcoclhs_arm_camera(pco);
}

unsigned int pcoclhs_edge_get_shutter(pcoclhs_handle *pco, pco_edge_shutter *shutter)
{
    SC2_Get_Camera_Setup_Response setup;
    SC2_Simple_Telegram req = {.wCode = GET_CAMERA_SETUP, .wSize = sizeof(req)};
    DWORD err = pcoclhs_control_command(pco, &req, sizeof(req), &setup, sizeof(setup));
    CHECK_ERROR(err);
    if (err == 0)
        *shutter = (pco_edge_shutter)setup.dwSetupFlags[0];
    return err;
}

unsigned int pcoclhs_edge_set_shutter(pcoclhs_handle *pco, pco_edge_shutter shutter)
{
    SC2_Set_Camera_Setup req = {.wCode = SET_CAMERA_SETUP, .wSize = sizeof(req), .wType = 0};
    SC2_Set_Camera_Setup_Response resp;
    for (int i = 0; i < NUMSETUPFLAGS; i++)
        req.dwSetupFlags[i] = shutter;
    DWORD err = pcoclhs_control_command(pco, &req, sizeof(req), &resp, sizeof(resp));
    CHECK_ERROR_AND_RETURN(err);
    return pcoclhs_arm_camera(pco);
}

unsigned int pcoclhs_set_date_time(pcoclhs_handle *pco)
{
    DWORD err = pco->com->PCO_SetCameraToCurrentTime();
    CHECK_ERROR_THEN_RETURN(err);
}

pcoclhs_reorder_image_t pcoclhs_get_reorder_func(pcoclhs_handle *pco)
{
    return pco->reorder_func;
}

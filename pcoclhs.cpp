#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "pcoclhs.h"

#include "pco/include/clhs/Cpco_com_clhs.h"
#include "pco/include/clhs/Cpco_grab_clhs.h"
#include "pco/include/clhs/Cpco_log.h"
#include "pco/include/clhs/reorderfunc.h"

// PCO_errt.h is a header file w/ hard-coded function implementations.
// It also contains non-standard function calls that must be escaped.
#ifndef sprintf_s
#define sprintf_s snprintf
#endif

#ifndef PCO_ERRT_H_CREATE_OBJECT
#define PCO_ERRT_H_CREATE_OBJECT
#endif
#include "pco/include/clhs/PCO_errt.h"

/** 
 * This module serves as a wrapper to access the C++ implementations of
 * the PCO.Camera methods. Otherwise, it is essentially a port of
 * UFO-KIT's libpco library partial implementation.
 */

#define CHECK_ERROR(code)                                                        \
    if ((code) != 0)                                                             \
    {                                                                            \
        fprintf(stderr, "Error: 0x%x at <%s:%i>\n", (code), __FILE__, __LINE__); \
        fprintf(stderr, "  %s\n", _get_error_text((code)));                      \
    }

#define CHECK_ERROR_AND_RETURN(code) \
    {                                \
        CHECK_ERROR(code);           \
        if ((code) != 0)             \
            return (code);           \
    }

#define CHECK_ERROR_THEN_RETURN(code) \
    {                                 \
        CHECK_ERROR(code);            \
        return (code);                \
    }

#define USR_TIMEBASE TIMEBASE_MS
#define CAM_TIMEBASE TIMEBASE_NS
#define CONVERT_CAM2SEC_TIMEBASE(val) (double)((val)*1e-9)
#define CONVERT_CAM2USR_TIMEBASE(val) (double)((val)*1e-6)
#define CONVERT_USR2SEC_TIMEBASE(val) (double)((val)*1e-3)
#define CONVERT_USR2CAM_TIMEBASE(val) (uint32_t)((val)*1e6)

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
    CPco_Log *logger;

    double cachedDelay, cachedExposure;
    pcoclhs_reorder_image_t reorder_func;

    uint16_t cameraType, cameraSubType;
    SC2_Camera_Description_Response description;

    int board, port;
};

static unsigned int _pcoclhs_init(pcoclhs_handle *pco, int board, int port)
{
    DWORD err;

    pco->board = board;
    pco->port = port;

    CPco_com_clhs *com;
    com = new CPco_com_clhs();
    pco->com = com;

    CPco_grab_clhs *grab;
    grab = new CPco_grab_clhs(com);
    pco->grabber = grab;

    CPco_Log *logger;
    logger = new CPco_Log("pcoclhs.log");
    logger->set_logbits(0x000FF0FF);
    pco->logger = logger;

    pco->grabber->SetLog(pco->logger);
    pco->com->SetLog(pco->logger);

    err = pcoclhs_open_camera(pco, port);
    CHECK_ERROR_AND_RETURN(err);

    err = pco->grabber->Open_Grabber(board);
    CHECK_ERROR_AND_RETURN(err);

    pco->reorder_func = &func_reorder_image_5x16;

    err = pco->com->PCO_GetCameraDescriptor(&pco->description);
    CHECK_ERROR_AND_RETURN(err);

    err = pcoclhs_get_camera_type(pco, &pco->cameraType, &pco->cameraSubType);
    CHECK_ERROR_AND_RETURN(err);

    err = pco->grabber->Set_Grabber_Timeout(10000);
    CHECK_ERROR_AND_RETURN(err);

    err = pco->com->PCO_SetCameraToCurrentTime();
    CHECK_ERROR_AND_RETURN(err);

    err = pcoclhs_set_recording_state(pco, 0);
    CHECK_ERROR_AND_RETURN(err);

    err = pco->com->PCO_ResetSettingsToDefault();
    CHECK_ERROR_AND_RETURN(err);

    err = pcoclhs_set_timestamp_mode(pco, TIMESTAMP_MODE_BINARYANDASCII);
    CHECK_ERROR_AND_RETURN(err);

    err = pcoclhs_set_timebase(pco, TIMEBASE_MS, TIMEBASE_MS);
    CHECK_ERROR_AND_RETURN(err);

    err = pcoclhs_set_delay_exposure(pco, 0.0, 10.0);
    CHECK_ERROR_AND_RETURN(err);

    if (pco->description.wNumADCsDESC > 1)
    {
        err = pco->com->PCO_SetADCOperation(2);
        CHECK_ERROR_AND_RETURN(err);
    }

    err = pco->com->PCO_SetBitAlignment(BIT_ALIGNMENT_LSB);
    CHECK_ERROR(err);
    if (err != PCO_NOERROR)
    {
    }

    // err = pcoclhs_arm_camera(pco);
    // CHECK_ERROR_AND_RETURN(err);

    // DWORD times[3] = {2000, 10000, 10000};
    // pco->com->Set_Timeouts(times, 3);

    return PCO_NOERROR;
}

pcoclhs_handle *pcoclhs_init(int board, int port)
{
    pcoclhs_handle *pco = (pcoclhs_handle *)malloc(sizeof(*pco));
    DWORD err = _pcoclhs_init(pco, 0, 0);
    if (err != PCO_NOERROR)
    {
        pcoclhs_destroy(pco);
        CHECK_ERROR(err);
    }
    return pco;
}

void pcoclhs_destroy(pcoclhs_handle *pco)
{
    if (pco == NULL)
        return;

    if (pco->grabber->IsOpen())
        pco->grabber->Close_Grabber();

    if (pco->com->IsOpen())
        pco->com->Close_Cam();

    delete pco->grabber;
    pco->grabber = NULL;

    delete pco->com;

    free(pco);
    pco = NULL;
}

unsigned int pcoclhs_open_camera(pcoclhs_handle *pco, int port)
{
    DWORD err;
    if (pco->com->IsOpen())
    {
        err = pco->com->Close_Cam();
        CHECK_ERROR(err);
    }
    for (unsigned int i = 0; i <= MAXNUM_DEVICES; i++)
    {
        err = pco->com->Open_Cam(i);
        if (err == PCO_NOERROR)
        {
            if (port == 0)
                break;
            else
            {
                pco->com->Close_Cam();
                port--;
            }
        }
    }
    CHECK_ERROR_THEN_RETURN(err);
}

unsigned int pcoclhs_control_command(pcoclhs_handle *pco, void *buffer_in, uint32_t size_in, void *buffer_out, uint32_t size_out)
{
    DWORD err = pco->com->Control_Command(buffer_in, size_in, buffer_out, size_out);
    CHECK_ERROR_THEN_RETURN(err);
}

unsigned int pcoclhs_grabber_set_size(pcoclhs_handle *pco, uint32_t width, uint32_t height)
{
    // DWORD err = pco->grabber->Set_Grabber_Size(2 * cameraW, cameraH);  // CL v CLHS
    DWORD err = pco->grabber->Set_Grabber_Size(width, height);
    CHECK_ERROR_THEN_RETURN(err);
}

unsigned int pcoclhs_grabber_allocate_memory(pcoclhs_handle *pco, int size)
{
    // not actually implemented, just returns PCO_NOERROR
    //return pco->grabber->Allocate_Framebuffer(size);  // CL v CLHS
    return 0;
}

unsigned int pcoclhs_grabber_free_memory(pcoclhs_handle *pco)
{
    // not actually implemented, just returns PCO_NOERROR
    // return pco->grabber->Free_Framebuffer();  // CL v CLHS
    return 0;
}

unsigned int pcoclhs_grabber_set_timeout(pcoclhs_handle *pco, int milliseconds)
{
    return pco->grabber->Set_Grabber_Timeout(milliseconds);
}

unsigned int pcoclhs_grabber_get_timeout(pcoclhs_handle *pco, int *milliseconds)
{
    return pco->grabber->Get_Grabber_Timeout(milliseconds);
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
        err = pcoclhs_grabber_set_size(pco, cameraW, cameraH);
        CHECK_ERROR_AND_RETURN(err);

        pcoclhs_grabber_allocate_memory(pco, 20);
    }

    err = pcoclhs_arm_camera(pco);
    CHECK_ERROR_THEN_RETURN(err);
}

unsigned int pcoclhs_start_recording(pcoclhs_handle *pco)
{
    DWORD err = pcoclhs_prepare_recording(pco);
    CHECK_ERROR_AND_RETURN(err);

    err = pcoclhs_set_recording_state(pco, 1);
    CHECK_ERROR_AND_RETURN(err);

    err = pco->grabber->Start_Acquire();
    CHECK_ERROR_THEN_RETURN(err);
}

unsigned int pcoclhs_stop_recording(pcoclhs_handle *pco)
{
    DWORD err = pcoclhs_set_recording_state(pco, 0);
    CHECK_ERROR_AND_RETURN(err);
    err = pco->com->PCO_CancelImage();
    // ignore error
    err = pco->grabber->Stop_Acquire();
    CHECK_ERROR_THEN_RETURN(err);
}

bool pcoclhs_is_recording(pcoclhs_handle *pco)
{
    return pco->grabber->started();
}

bool pcoclhs_is_active(pcoclhs_handle *pco)
{
    uint16_t discard1, discard2;
    DWORD err = pcoclhs_get_camera_type(pco, &discard1, &discard2) == 0;
    return err == PCO_NOERROR;
}

static unsigned int __pcoclhs_get_camera_type(pcoclhs_handle *pco, SC2_Camera_Type_Response *resp)
{
    SC2_Simple_Telegram req = {.wCode = GET_CAMERA_TYPE, .wSize = sizeof(req)};
    DWORD err = pcoclhs_control_command(pco, &req, sizeof(req), resp, sizeof(*resp));
    CHECK_ERROR_THEN_RETURN(err);
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
    if (pco->grabber->IsOpen())
    {
        pco->grabber->Close_Grabber();
        pco->grabber = NULL;
    }

    if (pco->com->IsOpen())
        pco->com->Close_Cam();

    return _pcoclhs_init(pco, pco->board, pco->port);
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
    CHECK_ERROR_THEN_RETURN(err);
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
    CHECK_ERROR_THEN_RETURN(err);
}

static void pcoclhs_post_update_pixelrate(pcoclhs_handle *pco)
{
    return;
    ///////////////////////////////
    // CL v CLHS

    if (pco->cameraType == CAMERATYPE_PCO_EDGE)
    {
        uint32_t pixelrate;
        uint16_t w, h, wx, hx, lut = 0;
        pcoclhs_get_pixelrate(pco, &pixelrate);
        pcoclhs_get_resolution(pco, &w, &h, &wx, &hx);

        PCO_SC2_CL_TRANSFER_PARAM tparam;
        pco->com->PCO_GetTransferParameter(&tparam, sizeof(tparam));

        if ((w > 1920) && (pixelrate >= 286000000))
        {
            pco->reorder_func = &func_reorder_image_5x12;
            tparam.DataFormat = SCCMOS_FORMAT_TOP_CENTER_BOTTOM_CENTER | PCO_CL_DATAFORMAT_5x12;
            lut = 0x1612;
        }
        else
        {
            pco->reorder_func = &func_reorder_image_5x16;
            tparam.DataFormat = SCCMOS_FORMAT_TOP_CENTER_BOTTOM_CENTER | PCO_CL_DATAFORMAT_5x16;
        }
        pco->grabber->Set_DataFormat(tparam.DataFormat);
        pco->com->PCO_SetTransferParameter(&tparam, sizeof(tparam));
        pco->com->PCO_SetLut(lut, 0);
    }
}

unsigned int pcoclhs_set_fps(pcoclhs_handle *pco, double fps)
{
    WORD status;
    uint32_t rate = (uint32_t)(fps * 1e3); // Hz (FPS) to milli-Hz (mFPS)
    uint32_t expo_ns = CONVERT_USR2CAM_TIMEBASE(pco->cachedExposure);
    DWORD err = pco->com->PCO_SetFrameRate(&status, SET_FRAMERATE_MODE_FRAMERATE_HAS_PRIORITY, &rate, &expo_ns);
    CHECK_ERROR(err);
    if (err != PCO_NOERROR)
    {
        fprintf(stderr, "Setting the FPS is not supported by the camera.\n");
        fprintf(stderr, "Please manually set the FPS through `exposure-time' and `delay-time'\n");
    }
    else if (status == SET_FRAMERATE_STATUS_NOT_YET_VALIDATED)
        return err;
}

unsigned int pcoclhs_get_fps(pcoclhs_handle *pco, double *fps)
{
    WORD status;
    uint32_t rate, discard;
    DWORD err = pco->com->PCO_GetFrameRate(&status, &rate, &discard);
    if (err == PCO_NOERROR)
    {
        *fps = rate * 1e-3; // mHz (sub-FPS) to Hz (FPS)
    }
    else
    {
        // try calculate from delay and exposure time.
        // may get here if PCO_GetFrameRate is not supported.
        double expo, delay;
        err = pcoclhs_get_delay_exposure(pco, &delay, &expo);
        CHECK_ERROR_AND_RETURN(err);
        double msecs = expo + delay;
        double secs = CONVERT_USR2SEC_TIMEBASE(msecs);
        *fps = 1.0 / secs;
    }
    return err;
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
        return PCO_ERROR_IS_ERROR;

    err = pcoclhs_set_pixelrate(pco, pixelrate);
    CHECK_ERROR_THEN_RETURN(err);

    return 0;
    //////////////////////////

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

    err = pcoclhs_set_pixelrate(pco, pixelrate);
    CHECK_ERROR_THEN_RETURN(err);
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
    // TODO
}

unsigned int pcoclhs_set_lut(pcoclhs_handle *pco, uint16_t key, uint16_t val)
{
    DWORD err = pco->com->PCO_SetLut(key, val);
    CHECK_ERROR_THEN_RETURN(err);
}

unsigned int pcoclhs_get_lut(pcoclhs_handle *pco, uint16_t *key, uint16_t *val)
{
    DWORD err = pco->com->PCO_GetLut(key, val);
    CHECK_ERROR_THEN_RETURN(err);
}

unsigned int pcoclhs_set_roi(pcoclhs_handle *pco, uint16_t *window)
{
    DWORD err = pco->com->PCO_SetROI(
        window[0] > 0 ? window[0] : 1,
        window[1] > 0 ? window[1] : 1,
        window[2], window[3]);
    CHECK_ERROR_THEN_RETURN(err);
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
    if (pcoclhs_is_double_image_mode_available(pco))
    {
        DWORD err = pco->com->PCO_SetDoubleImageMode(on ? 1 : 0);
        CHECK_ERROR_THEN_RETURN(err);
    }
    return 0;
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
    CHECK_ERROR_THEN_RETURN(err);
}

unsigned int pcoclhs_force_trigger(pcoclhs_handle *pco, uint16_t *success)
{
    DWORD err = pco->com->PCO_ForceTrigger(success);
    CHECK_ERROR_THEN_RETURN(err);
}

unsigned int pcoclhs_set_timestamp_mode(pcoclhs_handle *pco, uint16_t mode)
{
    DWORD err = pco->com->PCO_SetTimestampMode(mode);
    CHECK_ERROR_THEN_RETURN(err);
}

unsigned int pcoclhs_get_timestamp_mode(pcoclhs_handle *pco, uint16_t *mode)
{
    DWORD err = pco->com->PCO_GetTimestampMode(mode);
    CHECK_ERROR_THEN_RETURN(err);
}

unsigned int pcoclhs_set_timebase(pcoclhs_handle *pco, uint16_t delay, uint16_t expos)
{
    DWORD err = pco->com->PCO_SetTimebase(delay, expos);
    CHECK_ERROR_THEN_RETURN(err);
}

unsigned int pcoclhs_get_timebase(pcoclhs_handle *pco, uint16_t *delay, uint16_t *expos)
{
    DWORD err = pco->com->PCO_GetTimebase(delay, expos);
    CHECK_ERROR_THEN_RETURN(err);
}

unsigned int pcoclhs_get_delay_time(pcoclhs_handle *pco, double *delay)
{
    double discard;
    DWORD err = pcoclhs_get_delay_exposure(pco, delay, &discard);
    CHECK_ERROR_THEN_RETURN(err);
}

unsigned int pcoclhs_set_delay_time(pcoclhs_handle *pco, double delay)
{
    DWORD err = pcoclhs_set_delay_exposure(pco, delay, pco->cachedExposure);
    CHECK_ERROR_THEN_RETURN(err);
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

unsigned int pcoclhs_get_exposure_time(pcoclhs_handle *pco, double *exposure)
{
    double discard;
    DWORD err = pcoclhs_get_delay_exposure(pco, &discard, exposure);
    CHECK_ERROR_THEN_RETURN(err);
}

unsigned int pcoclhs_set_exposure_time(pcoclhs_handle *pco, double exposure)
{
    DWORD err = pcoclhs_set_delay_exposure(pco, pco->cachedDelay, exposure);
    CHECK_ERROR_THEN_RETURN(err);
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

static unsigned int __pcoclhs_get_delay_exposure_ns(pcoclhs_handle *pco, uint32_t *delay_ns, uint32_t *expos_ns)
{
    DWORD err = pco->com->PCO_GetDelayExposure(delay_ns, expos_ns);
    CHECK_ERROR_THEN_RETURN(err);
}

unsigned int pcoclhs_get_delay_exposure(pcoclhs_handle *pco, double *delay, double *exposure)
{
    uint32_t delay_ns, expos_ns;
    DWORD err = __pcoclhs_get_delay_exposure_ns(pco, &delay_ns, &expos_ns);
    CHECK_ERROR(err);
    if (err == 0)
    {
        *delay = CONVERT_CAM2USR_TIMEBASE(delay_ns);
        *exposure = CONVERT_CAM2USR_TIMEBASE(expos_ns);
        pco->cachedDelay = *delay;
        pco->cachedExposure = *exposure;
    }
    return err;
}

static unsigned int __pcoclhs_set_delay_exposure_ns(pcoclhs_handle *pco, uint32_t delay_ns, uint32_t expos_ns)
{
    DWORD err = pco->com->PCO_SetDelayExposureTime(delay_ns, expos_ns, CAM_TIMEBASE, CAM_TIMEBASE);
    CHECK_ERROR_THEN_RETURN(err);
}

unsigned int pcoclhs_set_delay_exposure(pcoclhs_handle *pco, double delay, double exposure)
{
    uint32_t delay_ns = CONVERT_USR2CAM_TIMEBASE(delay);
    uint32_t expo_ns = CONVERT_USR2CAM_TIMEBASE(exposure);
    DWORD err = __pcoclhs_set_delay_exposure_ns(pco, delay_ns, expo_ns);
    CHECK_ERROR(err);
    if (err == 0)
    {
        pco->cachedDelay = delay;
        pco->cachedExposure = exposure;
    }
    return err;
}

unsigned int pcoclhs_get_trigger_mode(pcoclhs_handle *pco, uint16_t *mode)
{
    DWORD err = pco->com->PCO_GetTriggerMode(mode);
    CHECK_ERROR_THEN_RETURN(err);
}

unsigned int pcoclhs_set_trigger_mode(pcoclhs_handle *pco, uint16_t mode)
{
    DWORD err = pco->com->PCO_SetTriggerMode(mode);
    CHECK_ERROR_THEN_RETURN(err);
}

unsigned int pcoclhs_arm_camera(pcoclhs_handle *pco)
{
    // int tout;
    // pco->grabber->Get_Grabber_Timeout(&tout);
    // pco->grabber->Set_Grabber_Timeout(tout + 5000);
    DWORD err = pco->com->PCO_ArmCamera();
    CHECK_ERROR_AND_RETURN(err);
    err = pco->grabber->PostArm();
    // pco->grabber->Set_Grabber_Timeout(tout);
    CHECK_ERROR_THEN_RETURN(err);
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
    CHECK_ERROR_THEN_RETURN(err);
}

unsigned int pcoclhs_await_next_image_ex(pcoclhs_handle *pco, void *adr, int timeout)
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

    err = pco->grabber->Wait_For_Next_Image(adr, timeout);
    CHECK_ERROR_THEN_RETURN(err);
}

unsigned int pcoclhs_await_next_image(pcoclhs_handle *pco, void *adr)
{
    DWORD err = pcoclhs_await_next_image_ex(pco, adr, 10000);
    CHECK_ERROR_THEN_RETURN(err);
}

unsigned int pcoclhs_acquire_image(pcoclhs_handle *pco, void *adr)
{
    DWORD err = pco->grabber->Acquire_Image(adr);
    CHECK_ERROR_THEN_RETURN(err);
}

unsigned int pcoclhs_acquire_image_ex(pcoclhs_handle *pco, void *adr, int timeout)
{
    DWORD err = pco->grabber->Acquire_Image(adr, timeout);
    CHECK_ERROR_THEN_RETURN(err);
}

unsigned int pcoclhs_acquire_n_images(pcoclhs_handle *pco, WORD **adr, int count)
{
    WORD img_size = sizeof(adr) / count, err = 0;
    // WORD *picbuf[4];

    // memset(picbuf, 0, sizeof(WORD *) * 4);
    // for (int i = 0; i < 4; i++)
    // {
    //     adr[i] = (WORD *)malloc(img_size * sizeof(WORD));
    //     if (adr[i] == NULL)
    //     {
    //         CHECK_ERROR_AND_RETURN(PCO_ERROR_NOMEMORY);
    //     }
    // }

    for (DWORD i = 0; i < count; i++)
    {
        DWORD buf_nr = i * img_size;
        err = pcoclhs_acquire_image(pco, adr[buf_nr]);
        CHECK_ERROR_THEN_RETURN(err);

        if (i == 0)
            pco->logger->start_time_mess();
    }

    pco->logger->stop_time_mess();
    return err;
}

unsigned int pcoclhs_get_segment_image(pcoclhs_handle *pco, void *adr, int seg, int nr)
{
    DWORD err;

    if (!pcoclhs_is_recording(pco))
    {
        if (pco->description.dwGeneralCaps1 & GENERALCAPS1_NO_RECORDER)
        {
            fprintf(stderr, "Camera does not support image readout from segments\n");
            return -1;
        }

        DWORD valid, max;
        err = pco->com->PCO_GetNumberOfImagesInSegment(seg, &valid, &max);
        CHECK_ERROR_AND_RETURN(err);

        if (valid == 0)
        {
            fprintf(stderr, "No images available in segment %d\n", seg);
            return -1;
        }
        if (nr > (int)valid)
        {
            fprintf(stderr, "Selected image number is out of range");
            return -1;
        }
    }

    unsigned int w, h, l;
    WORD *buf;
    pcoclhs_get_actual_size(pco, &w, &h);

    err = pco->grabber->Get_Image(seg, nr, adr);
    CHECK_ERROR_AND_RETURN(err);
}

unsigned int pcoclhs_get_actual_size(pcoclhs_handle *pco, uint32_t *width, uint32_t *height)
{
    // DWORD err = pco->com->PCO_GetActualSize(width, height);
    DWORD err = pco->grabber->Get_actual_size(width, height, NULL);
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
    CHECK_ERROR_THEN_RETURN(err);
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

unsigned int pcoclhs_get_noise_filter_mode(pcoclhs_handle *pco, uint16_t *mode)
{
    DWORD err = pco->com->PCO_GetNoiseFilterMode(mode);
    CHECK_ERROR_THEN_RETURN(err);
}

unsigned int pcoclhs_set_noise_filter_mode(pcoclhs_handle *pco, uint16_t mode)
{
    DWORD err = pco->com->PCO_SetNoiseFilterMode(mode);
    CHECK_ERROR_THEN_RETURN(err);
}

unsigned int pcoclhs_edge_get_shutter(pcoclhs_handle *pco, pco_edge_shutter *shutter)
{
    DWORD *flags = (DWORD *)malloc(4 * sizeof(DWORD));
    WORD numflags = 4;
    DWORD err = pco->com->PCO_GetCameraSetup((WORD)0, flags, &numflags);
    CHECK_ERROR_AND_RETURN(err);
    *shutter = (pco_edge_shutter)flags[0];
    return 0;
}

unsigned int pcoclhs_update_camera_datetime(pcoclhs_handle *pco)
{
    DWORD err = pco->com->PCO_SetCameraToCurrentTime();
    CHECK_ERROR_THEN_RETURN(err);
}

pcoclhs_reorder_image_t pcoclhs_get_reorder_func(pcoclhs_handle *pco)
{
    return pco->reorder_func;
}

void pcoclhs_reorder_image(pcoclhs_handle *pco, uint16_t *bufout, uint16_t *bufin, int width, int height)
{
    uint32_t format = pco->grabber->Get_DataFormat();
    reorder_image(bufout, bufin, width, height, format);
    //pcoclhs_get_reorder_func(pco)(bufout, bufin, width, height);
}
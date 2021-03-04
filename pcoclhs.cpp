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

#define RETURN_IF_ERROR(code) \
    {                         \
        CHECK_ERROR(code);    \
        if ((code) != 0)      \
            return (code);    \
    }

#define RETURN_ANY_CODE(code) \
    {                         \
        CHECK_ERROR(code);    \
        return (code);        \
    }

/* Static helper functions */

static char *_get_error_text(DWORD code)
{
    char *s = (char *)malloc(255);
    PCO_GetErrorText(code, s, 255);
    return (char *)s;
}

/*************************/

struct _pco_handle
{
    CPco_com_clhs *com;      /* Comm. interface to a camera */
    CPco_grab_clhs *grabber; /* Frame-grabber interface */
    CPco_Log *logger;

    pco_reorder_image_t reorder_func;

    uint16_t cameraType, cameraSubType;
    SC2_Camera_Description_Response description;

    int board, port;
};

void func_reorder_image_5x12(uint16_t *bufout, uint16_t *bufin, int width, int height);
void func_reorder_image_5x16(uint16_t *bufout, uint16_t *bufin, int width, int height);

static unsigned int _pco_init(pco_handle *pco, int board, int port)
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

    err = pco_open_camera(pco, port);
    RETURN_IF_ERROR(err);

    err = pco->grabber->Open_Grabber(board);
    RETURN_IF_ERROR(err);

    pco->reorder_func = &func_reorder_image_5x16;

    err = pco->com->PCO_GetCameraDescriptor(&pco->description);
    RETURN_IF_ERROR(err);

    err = pco_get_camera_type(pco, &pco->cameraType, &pco->cameraSubType);
    RETURN_IF_ERROR(err);

    err = pco->grabber->Set_Grabber_Timeout(10000);
    RETURN_IF_ERROR(err);

    err = pco->com->PCO_SetCameraToCurrentTime();
    RETURN_IF_ERROR(err);

    err = pco_set_recording_state(pco, 0);
    RETURN_IF_ERROR(err);

    err = pco->com->PCO_ResetSettingsToDefault();
    RETURN_IF_ERROR(err);

    err = pco_set_timestamp_mode(pco, TIMESTAMP_MODE_BINARYANDASCII);
    RETURN_IF_ERROR(err);

    err = pco_set_timebase(pco, TIMEBASE_MS, TIMEBASE_MS);
    RETURN_IF_ERROR(err);

    err = pco_set_delay_exposure(pco, 0.0, 0.01);
    RETURN_IF_ERROR(err);

    if (pco->description.wNumADCsDESC > 1)
    {
        err = pco->com->PCO_SetADCOperation(2);
        RETURN_IF_ERROR(err);
    }

    err = pco->com->PCO_SetBitAlignment(BIT_ALIGNMENT_LSB);
    CHECK_ERROR(err);
    if (err != PCO_NOERROR)
    {
    }

    // err = pco_arm_camera(pco);
    // RETURN_IF_ERROR(err);

    // DWORD times[3] = {2000, 10000, 10000};
    // pco->com->Set_Timeouts(times, 3);

    return PCO_NOERROR;
}

pco_handle *pco_init(int board, int port)
{
    setenv("SISODIR5", SISODIR5, 0);

    pco_handle *pco = (pco_handle *)malloc(sizeof(*pco));
    DWORD err = _pco_init(pco, 0, 0);
    if (err != PCO_NOERROR)
    {
        pco_destroy(pco);
        CHECK_ERROR(err);
    }
    return pco;
}

void pco_destroy(pco_handle *pco)
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

unsigned int pco_open_camera(pco_handle *pco, int port)
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
    RETURN_ANY_CODE(err);
}

unsigned int pco_control_command(pco_handle *pco, void *buffer_in, uint32_t size_in, void *buffer_out, uint32_t size_out)
{
    DWORD err = pco->com->Control_Command(buffer_in, size_in, buffer_out, size_out);
    RETURN_ANY_CODE(err);
}

unsigned int pco_grabber_set_size(pco_handle *pco, uint32_t width, uint32_t height)
{
    DWORD err = pco->grabber->Set_Grabber_Size(width, height);
    RETURN_ANY_CODE(err);
}

unsigned int pco_grabber_allocate_memory(pco_handle *pco, int size)
{
    // not actually implemented, just returns PCO_NOERROR
    // return pco->grabber->Allocate_Framebuffer(size);
    return 0;
}

unsigned int pco_grabber_free_memory(pco_handle *pco)
{
    // not actually implemented, just returns PCO_NOERROR
    // return pco->grabber->Free_Framebuffer();
    return 0;
}

unsigned int pco_grabber_set_timeout(pco_handle *pco, int milliseconds)
{
    return pco->grabber->Set_Grabber_Timeout(milliseconds);
}

unsigned int pco_grabber_get_timeout(pco_handle *pco, int *milliseconds)
{
    return pco->grabber->Get_Grabber_Timeout(milliseconds);
}

unsigned int pco_prepare_recording(pco_handle *pco)
{
    DWORD err;

    uint32_t cameraW, cameraH;
    err = pco->com->PCO_GetActualSize(&cameraW, &cameraH);
    RETURN_IF_ERROR(err);

    uint32_t grabberW, grabberH, depth;
    err = pco->grabber->Get_actual_size(&grabberW, &grabberH, &depth);
    RETURN_IF_ERROR(err);

    if (cameraW != grabberW || cameraH != grabberH)
    {
        err = pco->grabber->Set_Grabber_Size(cameraW, cameraH);
        RETURN_IF_ERROR(err);

        pco_grabber_allocate_memory(pco, 20);
    }

    err = pco_arm_camera(pco);
    RETURN_ANY_CODE(err);
}

unsigned int pco_start_recording(pco_handle *pco)
{
    DWORD err = pco_prepare_recording(pco);
    RETURN_IF_ERROR(err);

    err = pco_set_recording_state(pco, 1);
    RETURN_IF_ERROR(err);

    err = pco->grabber->Start_Acquire();
    RETURN_ANY_CODE(err);
}

unsigned int pco_stop_recording(pco_handle *pco)
{
    DWORD err = pco_set_recording_state(pco, 0);
    RETURN_IF_ERROR(err);
    // err = pco->com->PCO_CancelImage();
    // ignore error
    err = pco->grabber->Stop_Acquire();
    RETURN_ANY_CODE(err);
}

bool pco_is_recording(pco_handle *pco)
{
    WORD state;
    DWORD err = pco->com->PCO_GetRecordingState(&state);
    CHECK_ERROR(err);
    return state == 1 && pco->grabber->started();
}

bool pco_is_active(pco_handle *pco)
{
    uint16_t discard1, discard2;
    DWORD err = pco_get_camera_type(pco, &discard1, &discard2);
    return err == PCO_NOERROR;
}

static unsigned int __pco_get_camera_type(pco_handle *pco, SC2_Camera_Type_Response *resp)
{
    SC2_Simple_Telegram req = {.wCode = GET_CAMERA_TYPE, .wSize = sizeof(req)};
    DWORD err = pco_control_command(pco, &req, sizeof(req), resp, sizeof(*resp));
    RETURN_ANY_CODE(err);
}

unsigned int pco_get_camera_type(pco_handle *pco, uint16_t *type, uint16_t *subtype)
{
    SC2_Camera_Type_Response resp;
    DWORD err = __pco_get_camera_type(pco, &resp);
    if (err == 0)
    {
        *type = resp.wCamType;
        *subtype = resp.wCamSubType;
    }
    return err;
}

unsigned int pco_get_camera_version(pco_handle *pco, uint32_t *serial_number, uint16_t *hw_major, uint16_t *hw_minor, uint16_t *fw_major, uint16_t *fw_minor)
{
    SC2_Camera_Type_Response resp;
    DWORD err = __pco_get_camera_type(pco, &resp);
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

unsigned int pco_get_health_state(pco_handle *pco, uint32_t *warnings, uint32_t *errors, uint32_t *status)
{
    DWORD err = pco->com->PCO_GetHealthStatus(warnings, errors, status);
    RETURN_ANY_CODE(err);
}

unsigned int pco_reset(pco_handle *pco)
{
    if (pco->grabber->IsOpen())
    {
        pco->grabber->Close_Grabber();
        pco->grabber = NULL;
    }

    if (pco->com->IsOpen())
        pco->com->Close_Cam();

    return _pco_init(pco, pco->board, pco->port);
}

unsigned int pco_get_temperature(pco_handle *pco, int16_t *ccd, int16_t *camera, int16_t *power)
{
    DWORD err = pco->com->PCO_GetTemperature(ccd, camera, power);
    RETURN_ANY_CODE(err);
}

unsigned int pco_get_name(pco_handle *pco, char **name)
{
    char *str = (char *)malloc(40 * sizeof(char));
    DWORD err = pco->com->PCO_GetCameraName(str, 40);
    if (err != 0)
    {
        *name = "Unknown Camera Type";
    }
    else
    {
        char *sout = (char *)malloc((strlen(str) + 1) * sizeof(char));
        strncpy(sout, str, strlen(str) + 1);
        *name = sout;
    }
    free(str);
    str = NULL;
    return err;
}

unsigned int pco_get_sensor_format(pco_handle *pco, uint16_t *format)
{
    DWORD err = pco->com->PCO_GetSensorFormat(format);
    RETURN_ANY_CODE(err);
}

unsigned int pco_set_sensor_format(pco_handle *pco, uint16_t format)
{
    DWORD err = pco->com->PCO_SetSensorFormat(format);
    RETURN_ANY_CODE(err);
}

uint32_t pco_get_cooling_setpoint(pco_handle *pco, short *temperature)
{
    DWORD err = pco->com->PCO_GetCoolingSetpointTemperature(temperature);
    RETURN_ANY_CODE(err);
}

uint32_t pco_set_cooling_setpoint(pco_handle *pco, short temperature)
{
    DWORD err = pco->com->PCO_SetCoolingSetpointTemperature(temperature);
    RETURN_ANY_CODE(err);
}

unsigned int pco_get_resolution(pco_handle *pco, uint16_t *width_std, uint16_t *height_std, uint16_t *width_ex, uint16_t *height_ex)
{
    *width_std = pco->description.wMaxHorzResStdDESC;
    *height_std = pco->description.wMaxVertResStdDESC;
    *width_ex = pco->description.wMaxHorzResExtDESC;
    *height_ex = pco->description.wMaxVertResExtDESC;
    return 0;
}

unsigned int pco_get_actual_size(pco_handle *pco, uint32_t *width, uint32_t *height)
{
    DWORD err = pco->com->PCO_GetActualSize(width, height);
}

unsigned int pco_get_available_pixelrates(pco_handle *pco, uint32_t rates[4], int *num_rates)
{
    int n = 0;
    for (int i = 0; i < 4; i++)
    {
        if (pco->description.dwPixelRateDESC[i] > 0)
            rates[n++] = pco->description.dwPixelRateDESC[i];
    }
    *num_rates = n;
    return 0;
}

unsigned int pco_get_pixelrate(pco_handle *pco, uint32_t *rate)
{
    DWORD err = pco->com->PCO_GetPixelRate(rate);
    RETURN_ANY_CODE(err);
}

unsigned int pco_set_pixelrate(pco_handle *pco, uint32_t rate)
{
    DWORD err = pco->com->PCO_SetPixelRate(rate);
    RETURN_ANY_CODE(err);
}

static void pco_post_update_pixelrate(pco_handle *pco)
{
    return;
    // CL v CLHS

    // if (pco->cameraType == CAMERATYPE_PCO_EDGE)
    // {
    //     uint32_t pixelrate;
    //     uint16_t w, h, wx, hx, lut = 0;
    //     pco_get_pixelrate(pco, &pixelrate);
    //     pco_get_resolution(pco, &w, &h, &wx, &hx);

    //     PCO_SC2_CL_TRANSFER_PARAM tparam;
    //     pco->com->PCO_GetTransferParameter(&tparam, sizeof(tparam));

    //     if ((w > 1920) && (pixelrate >= 286000000))
    //     {
    //         pco->reorder_func = &func_reorder_image_5x12;
    //         tparam.DataFormat = SCCMOS_FORMAT_TOP_CENTER_BOTTOM_CENTER | PCO_CL_DATAFORMAT_5x12;
    //         lut = 0x1612;
    //     }
    //     else
    //     {
    //         pco->reorder_func = &func_reorder_image_5x16;
    //         tparam.DataFormat = SCCMOS_FORMAT_TOP_CENTER_BOTTOM_CENTER | PCO_CL_DATAFORMAT_5x16;
    //     }
    //     pco->grabber->Set_DataFormat(tparam.DataFormat);
    //     pco->com->PCO_SetTransferParameter(&tparam, sizeof(tparam));
    //     pco->com->PCO_SetLut(lut, 0);
    // }
}

unsigned int pco_set_fps(pco_handle *pco, double fps)
{
    WORD status;
    uint32_t rate = (uint32_t)(fps * 1e3); // Hz (FPS) to milli-Hz (mFPS)
    double expo;
    DWORD err = pco_get_exposure_time(pco, &expo);
    RETURN_IF_ERROR(err);
    uint32_t expo_ns = (uint32_t)CNV_UNIT_TO_NANO(expo);
    err = pco->com->PCO_SetFrameRate(&status, SET_FRAMERATE_MODE_FRAMERATE_HAS_PRIORITY, &rate, &expo_ns);
    CHECK_ERROR(err);
    if (err != PCO_NOERROR)
    {
        fprintf(stderr, "Setting the FPS is not supported by the camera.\n");
        fprintf(stderr, "Please manually set the FPS through `exposure-time' and `delay-time'\n");
    }
    else if (status == SET_FRAMERATE_STATUS_NOT_YET_VALIDATED)
        return err;
    return err;
}

unsigned int pco_get_fps(pco_handle *pco, double *fps)
{
    WORD status;
    uint32_t rate, discard;
    DWORD err = pco->com->PCO_GetFrameRate(&status, &rate, &discard);
    if (err == PCO_NOERROR)
    {
        *fps = rate * 1e-3; // mHz to Hz
    }
    else
    {
        // try calculate from delay and exposure time.
        // may get here if PCO_GetFrameRate is not supported.
        double expo, delay;
        err = pco_get_delay_exposure(pco, &delay, &expo);
        RETURN_IF_ERROR(err);
        double secs = expo + delay;
        *fps = 1.0 / secs;
    }
    RETURN_ANY_CODE(err);
}

unsigned int pco_get_available_conversion_factors(pco_handle *pco, uint16_t factors[4], int *num_factors)
{
    int n = 0;
    for (int i = 0; i < 4; i++)
    {
        if (pco->description.wConvFactDESC[i] > 0)
            factors[n++] = pco->description.wConvFactDESC[i];
    }
    *num_factors = n;
    return 0;
}

/**
 * mode = 0 (PCO_SCANMODE_SLOW) or 1 (PCO_SCANMODE_FAST)
 */
unsigned int pco_set_scan_mode(pco_handle *pco, uint32_t mode)
{
    uint32_t pixelrates[4];
    int n;
    DWORD err = pco_get_available_pixelrates(pco, pixelrates, &n);

    RETURN_IF_ERROR(err);

    uint32_t pixelrate = pixelrates[mode];

    if (pixelrate == 0)
        return PCO_ERROR_IS_ERROR;

    err = pco_set_pixelrate(pco, pixelrate);
    RETURN_ANY_CODE(err);
}

unsigned int pco_get_scan_mode(pco_handle *pco, uint32_t *mode)
{
    uint32_t pixelrates[4];
    int n;
    DWORD err = pco_get_available_pixelrates(pco, pixelrates, &n);
    RETURN_IF_ERROR(err);

    uint32_t curr_pixelrate;
    err = pco_get_pixelrate(pco, &curr_pixelrate);
    RETURN_IF_ERROR(err);

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

unsigned int pco_set_lut(pco_handle *pco, uint16_t key, uint16_t val)
{
    DWORD err = pco->com->PCO_SetLut(key, val);
    RETURN_ANY_CODE(err);
}

unsigned int pco_get_lut(pco_handle *pco, uint16_t *key, uint16_t *val)
{
    DWORD err = pco->com->PCO_GetLut(key, val);
    RETURN_ANY_CODE(err);
}

unsigned int pco_set_roi(pco_handle *pco, uint16_t *window)
{
    // DWORD err = pco->com->PCO_SetROI(
    //     window[0] > 0 ? window[0] : 1,
    //     window[1] > 0 ? window[1] : 1,
    //     window[2], window[3]);
    DWORD err = pco->com->PCO_SetROI(window[0] + 1, window[1] + 1, window[2], window[3]);
    RETURN_ANY_CODE(err);
}

unsigned int pco_get_roi(pco_handle *pco, uint16_t *window)
{
    uint16_t x1, y1, x2, y2;
    DWORD err = pco->com->PCO_GetROI(&x1, &y1, &x2, &y2);
    CHECK_ERROR(err);
    if (err == 0)
    {
        window[0] = x1 - 1;
        window[1] = y1 - 1;
        window[2] = x2;
        window[3] = y2;
    }
    return err;
}

unsigned int pco_get_roi_steps(pco_handle *pco, uint16_t *horizontal, uint16_t *vertical)
{
    *horizontal = pco->description.wRoiHorStepsDESC;
    *vertical = pco->description.wRoiVertStepsDESC;
    return 0;
}

bool pco_is_double_image_mode_available(pco_handle *pco)
{
    return pco->description.wDoubleImageDESC == 1;
}

unsigned int pco_set_double_image_mode(pco_handle *pco, bool on)
{
    if (pco_is_double_image_mode_available(pco))
    {
        DWORD err = pco->com->PCO_SetDoubleImageMode(on ? 1 : 0);
        RETURN_ANY_CODE(err);
    }
    return 0;
}

unsigned int pco_get_double_image_mode(pco_handle *pco, bool *on)
{
    uint16_t tmp;
    DWORD err = pco->com->PCO_GetDoubleImageMode(&tmp);
    CHECK_ERROR(err);
    if (err == 0)
        *on = tmp == 1;
    return err;
}

unsigned int pco_get_bit_alignment(pco_handle *pco, bool *msb_aligned)
{
    uint16_t alignment;
    DWORD err = pco->com->PCO_GetBitAlignment(&alignment);
    CHECK_ERROR(err);
    if (err == 0)
        *msb_aligned = alignment == 0;
    return err;
}

unsigned int pco_set_bit_alignment(pco_handle *pco, bool msb_aligned)
{
    DWORD err = pco->com->PCO_SetBitAlignment(msb_aligned ? 0 : 1);
    RETURN_ANY_CODE(err);
}

unsigned int pco_force_trigger(pco_handle *pco, uint16_t *success)
{
    DWORD err = pco->com->PCO_ForceTrigger(success);
    RETURN_ANY_CODE(err);
}

unsigned int pco_set_timestamp_mode(pco_handle *pco, uint16_t mode)
{
    DWORD err = pco->com->PCO_SetTimestampMode(mode);
    RETURN_ANY_CODE(err);
}

unsigned int pco_get_timestamp_mode(pco_handle *pco, uint16_t *mode)
{
    DWORD err = pco->com->PCO_GetTimestampMode(mode);
    RETURN_ANY_CODE(err);
}

unsigned int pco_set_timebase(pco_handle *pco, uint16_t delay, uint16_t expos)
{
    DWORD err = pco->com->PCO_SetTimebase(delay, expos);
    RETURN_ANY_CODE(err);
}

unsigned int pco_get_timebase(pco_handle *pco, uint16_t *delay, uint16_t *expos)
{
    DWORD err = pco->com->PCO_GetTimebase(delay, expos);
    RETURN_ANY_CODE(err);
}

unsigned int pco_get_delay_time(pco_handle *pco, double *delay)
{
    double discard;
    DWORD err = pco_get_delay_exposure(pco, delay, &discard);
    RETURN_ANY_CODE(err);
}

unsigned int pco_set_delay_time(pco_handle *pco, double delay)
{
    DWORD err = pco_set_delay_exposure(pco, delay, -1);
    RETURN_ANY_CODE(err);
}

unsigned int pco_get_delay_range(pco_handle *pco, uint32_t *min_ns, uint32_t *max_ms, uint32_t *step_ns)
{
    *min_ns = pco->description.dwMinDelayDESC;
    *max_ms = pco->description.dwMaxDelayDESC;
    *step_ns = pco->description.dwMinDelayStepDESC;
    return 0;
}

unsigned int pco_get_exposure_time(pco_handle *pco, double *exposure)
{
    double discard;
    DWORD err = pco_get_delay_exposure(pco, &discard, exposure);
    RETURN_ANY_CODE(err);
}

unsigned int pco_set_exposure_time(pco_handle *pco, double exposure)
{
    DWORD err = pco_set_delay_exposure(pco, -1, exposure);
    RETURN_ANY_CODE(err);
}

unsigned int pco_get_exposure_range(pco_handle *pco, uint32_t *min_ns, uint32_t *max_ms, uint32_t *step_ns)
{
    *min_ns = pco->description.dwMinExposureDESC;
    *max_ms = pco->description.dwMaxExposureDESC;
    *step_ns = pco->description.dwMinExposureStepDESC;
    return 0;
}

unsigned int pco_get_delay_exposure(pco_handle *pco, double *delay, double *exposure)
{
    uint32_t curr_delay, curr_expos;
    uint16_t tb_delay, tb_expos;
    DWORD err = pco->com->PCO_GetDelayExposureTime(&curr_delay, &curr_expos, &tb_delay, &tb_expos);
    if (err == 0)
    {
        if (tb_delay == TIMEBASE_NS)
            *delay = CNV_NANO_TO_UNIT(curr_delay);
        else if (tb_delay == TIMEBASE_US)
            *delay = CNV_MICRO_TO_UNIT(curr_delay);
        else if (tb_delay == TIMEBASE_MS)
            *delay = CNV_MILLI_TO_UNIT(curr_delay);
        else
            err = PCO_ERROR_FIRMWARE_COC_TIMEBASE_INVALID;
    }
    RETURN_IF_ERROR(err);
    if (err == 0)
    {
        if (tb_expos == TIMEBASE_NS)
            *exposure = CNV_NANO_TO_UNIT(curr_expos);
        else if (tb_expos == TIMEBASE_US)
            *exposure = CNV_MICRO_TO_UNIT(curr_expos);
        else if (tb_expos == TIMEBASE_MS)
            *exposure = CNV_MILLI_TO_UNIT(curr_expos);
        else
            err = PCO_ERROR_FIRMWARE_COC_TIMEBASE_INVALID;
    }
    RETURN_ANY_CODE(err);
}

unsigned int pco_set_delay_exposure(pco_handle *pco, double delay, double exposure)
{
    uint32_t curr_delay, curr_expos;
    uint16_t tb_delay, tb_expos;
    DWORD err = pco->com->PCO_GetDelayExposureTime(&curr_delay, &curr_expos, &tb_delay, &tb_expos);
    RETURN_IF_ERROR(err);
    if (delay >= 0) {
        if (delay > 1e-9) {
            tb_delay = TIMEBASE_NS;
            curr_delay = (uint32_t)CNV_UNIT_TO_NANO(delay);
        }
        if (delay > 1e-6) {
            tb_delay = TIMEBASE_US;
            curr_delay = (uint32_t)CNV_UNIT_TO_MICRO(delay);
        }
        if (delay > 1e-3) {
            tb_delay = TIMEBASE_MS;
            curr_delay = (uint32_t)CNV_UNIT_TO_MILLI(delay);
        }
        else
            curr_delay = 0;
    }
    if (exposure >= 0) {
        if (exposure > 1e-9) {
            tb_expos = TIMEBASE_NS;
            curr_expos = (uint32_t)CNV_UNIT_TO_NANO(exposure);
        }
        if (exposure > 1e-6) {
            tb_expos = TIMEBASE_US;
            curr_expos = (uint32_t)CNV_UNIT_TO_MICRO(exposure);
        }
        if (exposure > 1e-3) {
            tb_expos = TIMEBASE_MS;
            curr_expos = (uint32_t)CNV_UNIT_TO_MILLI(exposure);
        }
    }
    err = pco->com->PCO_SetDelayExposureTime(curr_delay, curr_expos, tb_delay, tb_expos);
    RETURN_ANY_CODE(err);
}

unsigned int pco_get_trigger_mode(pco_handle *pco, uint16_t *mode)
{
    DWORD err = pco->com->PCO_GetTriggerMode(mode);
    RETURN_ANY_CODE(err);
}

unsigned int pco_set_trigger_mode(pco_handle *pco, uint16_t mode)
{
    DWORD err = pco->com->PCO_SetTriggerMode(mode);
    RETURN_ANY_CODE(err);
}

unsigned int pco_arm_camera(pco_handle *pco)
{
    DWORD err = pco->com->PCO_ArmCamera();
    RETURN_IF_ERROR(err);
    err = pco->grabber->PostArm();
    RETURN_ANY_CODE(err);
}

unsigned int pco_get_recording_state(pco_handle *pco, uint16_t *state)
{
    DWORD err = pco->com->PCO_GetRecordingState(state);
    RETURN_ANY_CODE(err);
}

unsigned int pco_set_recording_state(pco_handle *pco, uint16_t state)
{
    DWORD err = pco->com->PCO_SetRecordingState(state);
    RETURN_ANY_CODE(err);
}

unsigned int pco_get_acquire_mode(pco_handle *pco, uint16_t *mode)
{
    DWORD err = pco->com->PCO_GetAcquireMode(mode);
    RETURN_ANY_CODE(err);
}

unsigned int pco_set_acquire_mode(pco_handle *pco, uint16_t mode)
{
    DWORD err = pco->com->PCO_SetAcquireMode(mode);
    RETURN_ANY_CODE(err);
}

unsigned int pco_read_segment_images(pco_handle *pco, uint16_t segment, uint32_t start, uint32_t end)
{
    if (pco->description.dwGeneralCaps1 & GENERALCAPS1_NO_RECORDER)
    {
        fprintf(stderr, "Camera does not support image readout from segments\n");
        return -1;
    }
    DWORD err = pco->com->PCO_ReadImagesFromSegment(segment, start, end);
    RETURN_ANY_CODE(err);
}

unsigned int pco_request_image(pco_handle *pco)
{
    DWORD err = pco->com->PCO_RequestImage();
    RETURN_ANY_CODE(err);
}

unsigned int pco_force_acquire_ex(pco_handle *pco, void *adr, int timeout)
{
    DWORD err;
    uint16_t mode, triggered;

    err = pco_get_trigger_mode(pco, &mode);
    RETURN_IF_ERROR(err);

    if (mode == 0x0001 || mode == 0x0002)
    {
        err = pco_force_trigger(pco, &triggered);
        RETURN_IF_ERROR(err);
    }

    err = pco_acquire_image_ex_await(pco, adr, timeout);
    RETURN_ANY_CODE(err);
}

unsigned int pco_force_acquire(pco_handle *pco, void *adr)
{
    DWORD err = pco_force_acquire_ex(pco, adr, 10000);
    RETURN_ANY_CODE(err);
}

unsigned int pco_acquire_image(pco_handle *pco, void *adr)
{
    DWORD err = pco->grabber->Acquire_Image(adr);
    RETURN_ANY_CODE(err);
}

unsigned int pco_acquire_image_ex(pco_handle *pco, void *adr, int timeout)
{
    DWORD err = pco->grabber->Acquire_Image(adr, timeout);
    RETURN_ANY_CODE(err);
}

unsigned int pco_acquire_image_async(pco_handle *pco, void *adr)
{
    RETURN_ANY_CODE(PCO_ERROR_APPLICATION_FUNCTIONNOTSUPPORTED);
}

unsigned int pco_acquire_image_ex_async(pco_handle *pco, void *adr, int timeout)
{
    RETURN_ANY_CODE(PCO_ERROR_APPLICATION_FUNCTIONNOTSUPPORTED);
}

unsigned int pco_acquire_image_await(pco_handle *pco, void *adr)
{
    DWORD err = pco->grabber->Wait_For_Next_Image(adr, 10000);
    RETURN_ANY_CODE(err);
}

unsigned int pco_acquire_image_ex_await(pco_handle *pco, void *adr, int timeout)
{
    DWORD err = pco->grabber->Wait_For_Next_Image(adr, timeout);
    RETURN_ANY_CODE(err);
}

unsigned int pco_get_segment_image(pco_handle *pco, void *adr, int seg, int nr)
{
    DWORD err;

    if (!pco_is_recording(pco))
    {
        if (pco->description.dwGeneralCaps1 & GENERALCAPS1_NO_RECORDER)
        {
            fprintf(stderr, "Camera does not support image readout from segments\n");
            return -1;
        }

        DWORD valid, max;
        err = pco->com->PCO_GetNumberOfImagesInSegment(seg, &valid, &max);
        RETURN_IF_ERROR(err);

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

    unsigned int w, h;
    pco_grabber_get_actual_size(pco, &w, &h);

    err = pco->grabber->Get_Image(seg, nr, adr);
    RETURN_ANY_CODE(err);
}

unsigned int pco_grabber_get_actual_size(pco_handle *pco, uint32_t *width, uint32_t *height)
{
    DWORD err = pco->grabber->Get_actual_size(width, height, NULL);
    RETURN_ANY_CODE(err);
}

unsigned int pco_grabber_get_actual_size_ex(pco_handle *pco, uint32_t *width, uint32_t *height, uint32_t *depth)
{
    DWORD err = pco->grabber->Get_actual_size(width, height, depth);
    RETURN_ANY_CODE(err);
}

unsigned int pco_get_binning(pco_handle *pco, uint16_t *horizontal, uint16_t *vertical)
{
    DWORD err = pco->com->PCO_GetBinning(horizontal, vertical);
    RETURN_ANY_CODE(err);
}

unsigned int pco_set_binning(pco_handle *pco, uint16_t horizontal, uint16_t vertical)
{
    DWORD err = pco->com->PCO_SetBinning(horizontal, vertical);
    RETURN_ANY_CODE(err);
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

void func_reorder_image_5x12(uint16_t *bufout, uint16_t *bufin, int width, int height)
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

void func_reorder_image_5x16(uint16_t *bufout, uint16_t *bufin, int width, int height)
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

static uint16_t _msb_position(uint16_t x)
{
    uint16_t val = 0;
    while (x >>= 1)
        ++val;
    return val;
}

static void _fill_binning_array(uint16_t *a, unsigned int n, int is_linear)
{
    if (is_linear)
    {
        for (unsigned int i = 0; i < n; i++)
            a[i] = i + 1;
    }
    else
    {
        for (unsigned int i = 0, j = 1; i < n; i++, j *= 2)
            a[i] = j;
    }
}

static uint16_t _get_num_binnings(uint16_t max_binning, int is_linear)
{
    return is_linear ? max_binning : _msb_position(max_binning) + 1;
}

unsigned int pco_get_possible_binnings(pco_handle *pco, uint16_t **horizontal, unsigned int *num_horizontal, uint16_t **vertical, unsigned int *num_vertical)
{
    /* uint16_t maxBinHorz, stepBinHorz, maxBinVert, stepBinVert; */
    SC2_Camera_Description_Response desc = pco->description;

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

unsigned int pco_get_noise_filter_mode(pco_handle *pco, uint16_t *mode)
{
    DWORD err = pco->com->PCO_GetNoiseFilterMode(mode);
    RETURN_ANY_CODE(err);
}

unsigned int pco_set_noise_filter_mode(pco_handle *pco, uint16_t mode)
{
    DWORD err = pco->com->PCO_SetNoiseFilterMode(mode);
    RETURN_ANY_CODE(err);
}

unsigned int pco_edge_get_shutter(pco_handle *pco, pco_edge_shutter *shutter)
{
    DWORD *flags = (DWORD *)malloc(4 * sizeof(DWORD));
    WORD numflags = 4;
    DWORD err = pco->com->PCO_GetCameraSetup((WORD)0, flags, &numflags);
    RETURN_IF_ERROR(err);
    *shutter = (pco_edge_shutter)flags[0];
    return 0;
}

unsigned int pco_update_camera_datetime(pco_handle *pco)
{
    DWORD err = pco->com->PCO_SetCameraToCurrentTime();
    RETURN_ANY_CODE(err);
}

void pco_reorder_image(pco_handle *pco, uint16_t *bufout, uint16_t *bufin, int width, int height)
{
    uint32_t format = pco->grabber->Get_DataFormat();
    reorder_image(bufout, bufin, width, height, format);
    // memcpy(bufout, bufin, sizeof(uint16_t) * width * height);
}
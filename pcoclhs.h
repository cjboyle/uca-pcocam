#ifndef _PCOCLHS_H_
#define _PCOCLHS_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <stdbool.h>

#include "pco/defs.h"
#include "pco/sc2_defs.h"
#include "pco/PCO_err.h"

#define PCO_SCANMODE_SLOW 0
#define PCO_SCANMODE_FAST 1

    typedef void (*pcoclhs_reorder_image_t)(uint16_t *bufout, uint16_t *bufin, int width, int height);

    typedef enum
    {
        PCO_EDGE_ROLLING_SHUTTER = PCO_EDGE_SETUP_ROLLING_SHUTTER,
        PCO_EDGE_GLOBAL_SHUTTER = PCO_EDGE_SETUP_GLOBAL_SHUTTER,
        PCO_EDGE_GLOBAL_RESET = PCO_EDGE_SETUP_GLOBAL_RESET,
    } pco_edge_shutter;

    struct _pcoclhs_handle;
    typedef struct _pcoclhs_handle pcoclhs_handle; /* Handle to a PCO CameraLink HS wrapper containing references to a camera and grabber. */

    pcoclhs_handle* pcoclhs_init(int board, int port);

    void pcoclhs_destroy(pcoclhs_handle *pco);

    unsigned int pcoclhs_control_command(pcoclhs_handle *pco, void *buffer_in, uint32_t size_in, void *buffer_out, uint32_t size_out);

    unsigned int pcoclhs_grabber_set_size(pcoclhs_handle *pco, uint32_t width, uint32_t height);
    unsigned int pcoclhs_grabber_allocate_memory(pcoclhs_handle *pco, int size);
    unsigned int pcoclhs_grabber_free_memory(pcoclhs_handle *pco);

    unsigned int pcoclhs_prepare_recording(pcoclhs_handle *pco);

    unsigned int pcoclhs_start_recording(pcoclhs_handle *pco);

    unsigned int pcoclhs_stop_recording(pcoclhs_handle *pco);

    unsigned int pcoclhs_is_recording(pcoclhs_handle *pco, bool *is_recording);

    unsigned int pcoclhs_is_active(pcoclhs_handle *pco);

    unsigned int pcoclhs_get_camera_type(pcoclhs_handle *pco, uint16_t *type, uint16_t *subtype);

    unsigned int pcoclhs_get_camera_version(pcoclhs_handle *pco, uint32_t *serial_number, uint16_t *hw_major, uint16_t *hw_minor, uint16_t *fw_major, uint16_t *fw_minor);

    unsigned int pcoclhs_get_health_state(pcoclhs_handle *pco, uint32_t *warnings, uint32_t *errors, uint32_t *status);

    unsigned int pcoclhs_reset(pcoclhs_handle *pco);

    unsigned int pcoclhs_get_temperature(pcoclhs_handle *pco, int16_t *ccd, int16_t *camera, int16_t *power);

    unsigned int pcoclhs_get_name(pcoclhs_handle *pco, char **name);

    unsigned int pcoclhs_get_sensor_format(pcoclhs_handle *pco, uint16_t *format);

    unsigned int pcoclhs_set_sensor_format(pcoclhs_handle *pco, uint16_t format);

    unsigned int pcoclhs_get_resolution(pcoclhs_handle *pco, uint16_t *width_std, uint16_t *height_std, uint16_t *width_ex, uint16_t *height_ex);

    unsigned int pcoclhs_get_available_pixelrates(pcoclhs_handle *pco, uint32_t rates[4], int *num_rates);

    unsigned int pcoclhs_get_pixelrate(pcoclhs_handle *pco, uint32_t *rate);

    unsigned int pcoclhs_set_pixelrate(pcoclhs_handle *pco, uint32_t rate);

    unsigned int pcoclhs_get_available_conversion_factors(pcoclhs_handle *pco, uint16_t factors[4], int *num_factors);

    unsigned int pcoclhs_set_scan_mode(pcoclhs_handle *pco, uint32_t mode);

    unsigned int pcoclhs_get_scan_mode(pcoclhs_handle *pco, uint32_t *mode);

    unsigned int pcoclhs_set_lut(pcoclhs_handle *pco, uint16_t key, uint16_t val);

    unsigned int pcoclhs_get_lut(pcoclhs_handle *pco, uint16_t *key, uint16_t *val);

    unsigned int pcoclhs_set_roi(pcoclhs_handle *pco, uint16_t *window);

    unsigned int pcoclhs_get_roi(pcoclhs_handle *pco, uint16_t *window);

    unsigned int pcoclhs_get_roi_steps(pcoclhs_handle *pco, uint16_t *horizontal, uint16_t *vertical);

    bool pcoclhs_is_double_image_mode_available(pcoclhs_handle *pco);

    unsigned int pcoclhs_set_double_image_mode(pcoclhs_handle *pco, bool on);

    unsigned int pcoclhs_get_double_image_mode(pcoclhs_handle *pco, bool *on);

    // unsigned int pcoclhs_set_offset_mode(pcoclhs_handle *pco, bool on);

    // unsigned int pcoclhs_get_offset_mode(pcoclhs_handle *pco, bool *on);

    unsigned int pcoclhs_get_segment_sizes(pcoclhs_handle *pco, uint32_t sizes[4]);

    unsigned int pcoclhs_get_active_segment(pcoclhs_handle *pco, uint16_t *segment);

    unsigned int pcoclhs_clear_active_segment(pcoclhs_handle *pco);

    unsigned int pcoclhs_get_bit_alignment(pcoclhs_handle *pco, bool *msb_aligned);

    unsigned int pcoclhs_set_bit_alignment(pcoclhs_handle *pco, bool msb_aligned);

    unsigned int pcoclhs_get_num_images(pcoclhs_handle *pco, uint16_t segment, uint32_t *num_images);

    unsigned int pcoclhs_force_trigger(pcoclhs_handle *pco, uint16_t *success);

    unsigned int pcoclhs_set_timestamp_mode(pcoclhs_handle *pco, uint16_t mode);

    unsigned int pcoclhs_get_timestamp_mode(pcoclhs_handle *pco, uint16_t *mode);

    unsigned int pcoclhs_set_timebase(pcoclhs_handle *pco, uint16_t delay, uint16_t expos);

    unsigned int pcoclhs_get_timebase(pcoclhs_handle *pco, uint16_t *delay, uint16_t *expos);

    unsigned int pcoclhs_get_delay_time(pcoclhs_handle *pco, uint32_t *delay);

    unsigned int pcoclhs_set_delay_time(pcoclhs_handle *pco, uint32_t delay);

    unsigned int pcoclhs_get_delay_range(pcoclhs_handle *pco, uint32_t *min_ns, uint32_t *max_ms, uint32_t *step_ns);

    unsigned int pcoclhs_get_exposure_time(pcoclhs_handle *pco, uint32_t *exposure);

    unsigned int pcoclhs_set_exposure_time(pcoclhs_handle *pco, uint32_t exposure);

    unsigned int pcoclhs_get_exposure_range(pcoclhs_handle *pco, uint32_t *min_ns, uint32_t *max_ms, uint32_t *step_ns);

    unsigned int pcoclhs_get_delay_exposure(pcoclhs_handle *pco, uint32_t *delay, uint32_t *exposure);

    unsigned int pcoclhs_set_delay_exposure(pcoclhs_handle *pco, uint32_t delay, uint32_t exposure);

    unsigned int pcoclhs_get_trigger_mode(pcoclhs_handle *pco, uint16_t *mode);

    unsigned int pcoclhs_set_trigger_mode(pcoclhs_handle *pco, uint16_t mode);

    unsigned int pcoclhs_set_framerate(pcoclhs_handle *pco, uint32_t framerate_mhz, uint32_t exposure_ns, bool framerate_priority);

    // unsigned int pcoclhs_get_framerate(pcoclhs_handle *pco, uint32_t *framerate_mhz, uint32_t *exposure_ns);

    unsigned int pcoclhs_get_storage_mode(pcoclhs_handle *pco, uint16_t *mode);

    unsigned int pcoclhs_set_storage_mode(pcoclhs_handle *pco, uint16_t mode);

    unsigned int pcoclhs_get_record_mode(pcoclhs_handle *pco, uint16_t *mode);

    unsigned int pcoclhs_set_record_mode(pcoclhs_handle *pco, uint16_t mode);

    unsigned int pcoclhs_arm_camera(pcoclhs_handle *pco);

    unsigned int pcoclhs_get_recording_state(pcoclhs_handle *pco, uint16_t *state);

    unsigned int pcoclhs_set_recording_state(pcoclhs_handle *pco, uint16_t state);

    unsigned int pcoclhs_get_acquire_mode(pcoclhs_handle *pco, uint16_t *mode);

    unsigned int pcoclhs_set_acquire_mode(pcoclhs_handle *pco, uint16_t mode);

    unsigned int pcoclhs_request_image(pcoclhs_handle *pco);

    unsigned int pcoclhs_get_next_image(pcoclhs_handle *pco, void *adr);

    unsigned int pcoclhs_get_image(pcoclhs_handle *pco, uint16_t segment, uint32_t number, void *adr);

    unsigned int pcoclhs_read_images(pcoclhs_handle *pco, uint16_t segment, uint32_t start, uint32_t end);

    unsigned int pcoclhs_get_actual_size(pcoclhs_handle *pco, uint32_t *width, uint32_t *height);

    unsigned int pcoclhs_get_binning(pcoclhs_handle *pco, uint16_t *horizontal, uint16_t *vertical);

    unsigned int pcoclhs_set_binning(pcoclhs_handle *pco, uint16_t horizontal, uint16_t vertical);

    unsigned int pcoclhs_get_possible_binnings(pcoclhs_handle *pco, uint16_t **horizontal, unsigned int *num_horizontal, uint16_t **vertical, unsigned int *num_vertical);

    unsigned int pcoclhs_set_hotpixel_correction(pcoclhs_handle *pco, uint32_t mode);

    unsigned int pcoclhs_get_noise_filter_mode(pcoclhs_handle *pco, uint16_t *mode);

    unsigned int pcoclhs_set_noise_filter_mode(pcoclhs_handle *pco, uint16_t mode);

    unsigned int pcoclhs_edge_get_shutter(pcoclhs_handle *pco, pco_edge_shutter *shutter);

    unsigned int pcoclhs_edge_set_shutter(pcoclhs_handle *pco, pco_edge_shutter shutter);

    unsigned int pcoclhs_set_date_time(pcoclhs_handle *pco);

    pcoclhs_reorder_image_t pcoclhs_get_reorder_func(pcoclhs_handle *pco);

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /*_PCOCLHS_H_*/
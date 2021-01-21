#ifndef _PCOCLHS_H_
#define _PCOCLHS_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <stdbool.h>

#include "pco.h"

#include "pco/include/clhs/defs.h"
#include "pco/include/clhs/sc2_defs.h"
#include "pco/include/clhs/PCO_err.h"

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
    typedef struct _pcoclhs_handle pcoclhs_handle; /* Handle to a PCO CameraLinkHS wrapper containing references to a camera and grabber. */

    /**
     * Constructor to get a wrapper to the PCO CLHS SDK.
     * @param board the index of the frame-grabber board
     * @param port the index of the camera port to connect
     * @return A handle to a PCO CLHS camera on success, else nullptr
     */
    pcoclhs_handle *pcoclhs_init(int board, int port);

    /**
     * Set the current camera port connection.
     * @param pco handle
     * @param port the index of the camera port to connect
     * @return 0 on success, otherwise less than 0
     */
    unsigned int pcoclhs_open_camera(pcoclhs_handle *pco, int port);

    /**
     * Closes the connections to the frame-grabber and camera.
     */
    void pcoclhs_destroy(pcoclhs_handle *pco);

    /**
     * (INTERNAL) camera IO control function
     */
    unsigned int pcoclhs_control_command(pcoclhs_handle *pco, void *buffer_in, uint32_t size_in, void *buffer_out, uint32_t size_out);

    /**
     * Set the frame grabber capture dimensions.
     * @param pco handle
     * @param width the frame width in pixels
     * @param height the frame height in pixels
     * @return 0 on success, otherwise less than 0
     */
    unsigned int pcoclhs_grabber_set_size(pcoclhs_handle *pco, uint32_t width, uint32_t height);

    /**
     * Allocate memory in the frame-grabber buffer.
     * @param pco handle
     * @param size the amount of memory to allocate
     * @return 0 on success, otherwise less than 0
     */
    unsigned int pcoclhs_grabber_allocate_memory(pcoclhs_handle *pco, int size);

    /**
     * (NOT SUPPORTED) Free memory in the frame-grabber buffer.
     * @param pco handle
     * @return 0
     */
    unsigned int pcoclhs_grabber_free_memory(pcoclhs_handle *pco);

    /**
     * Set the general frame grabber timeout.
     * @param pco handle
     * @param milliseconds the timeout in milliseconds
     * @return 0 on success, otherwise less than 0
     */
    unsigned int pcoclhs_grabber_set_timeout(pcoclhs_handle *pco, int milliseconds);

    /**
     * Get the general frame grabber timeout.
     * @param pco handle
     * @param milliseconds output the timeout in milliseconds
     * @return 0 on success, otherwise less than 0
     */
    unsigned int pcoclhs_grabber_get_timeout(pcoclhs_handle *pco, int *milliseconds);

    /**
     * (INTERNAL) Prepare the camera and grabber for acquisition. This function is called 
     * by `pcoclhs_start_recording(pcoclhs_handle *)`
     * @param pco handle
     * @return 0 on success, otherwise less than 0
     */
    unsigned int pcoclhs_prepare_recording(pcoclhs_handle *pco);

    /**
     * Start camera acquisition.
     * @param pco handle
     * @return 0 on success, otherwise less than 0
     */
    unsigned int pcoclhs_start_recording(pcoclhs_handle *pco);

    /**
     * Stop camera acquisition.
     * @param pco handle
     * @return 0 on success, otherwise less than 0
     */
    unsigned int pcoclhs_stop_recording(pcoclhs_handle *pco);

    /**
     * Check the camera acquisition state.
     * @param pco handle
     * @return TRUE if recording, else FALSE
     */
    bool pcoclhs_is_recording(pcoclhs_handle *pco);

    /**
     * Check the camera connection state.
     * @param pco handle
     * @return TRUE if active, else FALSE
     */
    bool pcoclhs_is_active(pcoclhs_handle *pco);

    /**
     * Get the type and subtype of the PCO camera hardware.
     * @param pco handle
     * @param type output the primary type (e.g. CAMERATYPE_PCO_EDGE_HS)
     * @param subtype output the secondary type (e.g. CAMERASUBTYPE_PCO_EDGE_55)
     * @return 0 on success, otherwise less than 0
     */
    unsigned int pcoclhs_get_camera_type(pcoclhs_handle *pco, uint16_t *type, uint16_t *subtype);

    /**
     * Get the versions and serial number of the PCO camera hardware.
     * @param pco handle
     * @param serial_number output the camera serial number
     * @param hw_major output the hardware major version number
     * @param hw_minor output the hardware minor version number
     * @param fw_major output the firmware major version number
     * @param fw_minor output the firmware minor version number
     * @return 0 on success, otherwise less than 0
     */
    unsigned int pcoclhs_get_camera_version(pcoclhs_handle *pco, uint32_t *serial_number, uint16_t *hw_major, uint16_t *hw_minor, uint16_t *fw_major, uint16_t *fw_minor);

    /**
     * Get the health of the PCO camera hardware.
     * @param pco handle
     * @param warnings output warning code
     * @param errors output error code
     * @param status output status code
     * @return 0 on success, otherwise less than 0
     */
    unsigned int pcoclhs_get_health_state(pcoclhs_handle *pco, uint32_t *warnings, uint32_t *errors, uint32_t *status);

    /**
     * Reset the camera connection.
     * @param pco handle
     * @return 0 on success, otherwise less than 0
     */
    unsigned int pcoclhs_reset(pcoclhs_handle *pco);

    /**
     * Get the temperature of the camera hardware, in degree Celsius.
     * @param pco handle
     * @param ccd output the camera sensor temperature
     * @param camera output the camera mainboard temperature
     * @param power output the camera power supply temperature
     * @return 0 on success, otherwise less than 0
     */
    unsigned int pcoclhs_get_temperature(pcoclhs_handle *pco, int16_t *ccd, int16_t *camera, int16_t *power);

    /**
     * Get the name of the camera.
     * @param pco handle
     * @param name output string name
     * @return 0 on success, otherwise less than 0
     */
    unsigned int pcoclhs_get_name(pcoclhs_handle *pco, char **name);

    /**
     * Get the camera sensor format.
     * @param pco handle
     * @param format output the camera sensor format [0x0000=STANDARD, 0x0001=EXTENDED]
     * @return 0 on success, otherwise less than 0
     */
    unsigned int pcoclhs_get_sensor_format(pcoclhs_handle *pco, uint16_t *format);

    /**
     * Set the camera sensor format.
     * @param pco handle
     * @param format the camera sensor format [0x0000=STANDARD, 0x0001=EXTENDED]
     * @return 0 on success, otherwise less than 0
     */
    unsigned int pcoclhs_set_sensor_format(pcoclhs_handle *pco, uint16_t format);

    /**
     * Get the camera sensor resolution, in pixels.
     * @param pco handle
     * @param width_std output the maximum width in STANDARD format
     * @param height_std output the maximum height in STANDARD format
     * @param width_ex output the maximum width in EXTENDED format
     * @param height_ex output the maximum height in EXTENDED format
     * @return 0 on success, otherwise less than 0
     */
    unsigned int pcoclhs_get_resolution(pcoclhs_handle *pco, uint16_t *width_std, uint16_t *height_std, uint16_t *width_ex, uint16_t *height_ex);

    /**
     * Get list of available sensor pixel rates, in Hz.
     * @param pco handle
     * @param rates output the array of pixel rates (up to 4)
     * @param num_rates output the actual number of pixel rates in the array
     * @return 0 on success, otherwise less than 0
     */
    unsigned int pcoclhs_get_available_pixelrates(pcoclhs_handle *pco, uint32_t rates[4], int *num_rates);

    /**
     * Get the current pixel rate of the camera, in Hz.
     * @param pco handle
     * @param rate output the pixel rate
     * @return 0 on success, otherwise less than 0
     */
    unsigned int pcoclhs_get_pixelrate(pcoclhs_handle *pco, uint32_t *rate);

    /**
     * Set the current pixel rate of the camera, in Hz.
     * @param pco handle
     * @param rate the pixel rate
     * @return 0 on success, otherwise less than 0
     */
    unsigned int pcoclhs_set_pixelrate(pcoclhs_handle *pco, uint32_t rate);
    
    /**
     * Get the framerate of the camera, in Hz.
     * @param pco handle
     * @param rate output the frames per second rate
     * @return 0 on success, otherwise less than 0
     */
    unsigned int pcoclhs_get_fps(pcoclhs_handle *pco, double *fps);
    
    /**
     * Set the framerate of the camera, in Hz.
     * @param pco handle
     * @param rate the frames per second rate
     * @return 0 on success, otherwise less than 0
     */
    unsigned int pcoclhs_set_fps(pcoclhs_handle *pco, double fps);

    /**
     * Get list of available conversion factors, in e/cnt.
     * @param pco handle
     * @param rates output the array of conversion factors (up to 4)
     * @param num_rates output the actual number of conversion factors in the array
     * @return 0 on success, otherwise less than 0
     */
    unsigned int pcoclhs_get_available_conversion_factors(pcoclhs_handle *pco, uint16_t factors[4], int *num_factors);

    /**
     * Set the scan mode of the camera. This also sets the data transfer format.
     * @param pco handle
     * @param mode the scan mode [0=PCO_SCANMODE_SLOW, 1=PCO_SCANMODE_FAST]
     * @return 0 on success, otherwise less than 0
     */
    unsigned int pcoclhs_set_scan_mode(pcoclhs_handle *pco, uint32_t mode);

    /**
     * Get the scan mode of the camera.
     * @param pco handle
     * @param mode output the scan mode [0=PCO_SCANMODE_SLOW, 1=PCO_SCANMODE_FAST]
     * @return 0 on success, otherwise less than 0
     */
    unsigned int pcoclhs_get_scan_mode(pcoclhs_handle *pco, uint32_t *mode);

    /**
     * Activate a lookup table entry in the camera.
     * @param pco handle
     * @param key the LUT identifier (0x0000 for none)
     * @param val the LUT parameter
     * @return 0 on success, otherwise less than 0
     */
    unsigned int pcoclhs_set_lut(pcoclhs_handle *pco, uint16_t key, uint16_t val);

    /**
     * Get a lookup table entry from the camera.
     * @param pco handle
     * @param key output the active LUT identifier (0x0000 for none)
     * @param val output the active LUT parameter
     * @return 0 on success, otherwise less than 0
     */
    unsigned int pcoclhs_get_lut(pcoclhs_handle *pco, uint16_t *key, uint16_t *val);

    /**
     * Set the Region of Interest window frame.
     * @param pco handle
     * @param window the array of ROI coordinates {x0, y0, x1, y1}
     * @return 0 on success, otherwise less than 0
     */
    unsigned int pcoclhs_set_roi(pcoclhs_handle *pco, uint16_t *window);

    /**
     * Get the Region of Interest window frame.
     * @param pco handle
     * @param window output the array of ROI coordinates {x0, y0, x1, y1}
     * @return 0 on success, otherwise less than 0
     */
    unsigned int pcoclhs_get_roi(pcoclhs_handle *pco, uint16_t *window);

    /**
     * Get the Region of Interest step values.
     * @param pco handle
     * @param horizontal output the step value in the horizontal dimension
     * @param vertical output the step value in the vertical dimension
     * @return 0 on success, otherwise less than 0
     */
    unsigned int pcoclhs_get_roi_steps(pcoclhs_handle *pco, uint16_t *horizontal, uint16_t *vertical);

    /**
     * Check if double image mode is available on the camera.
     * @param pco handle
     * @return TRUE if mode is available, else FALSE
     */
    bool pcoclhs_is_double_image_mode_available(pcoclhs_handle *pco);

    /**
     * Set the double image mode on the camera.
     * @param pco handle
     * @param on TRUE=on, FALSE=off
     * @return 0 on success, otherwise less than 0
     */
    unsigned int pcoclhs_set_double_image_mode(pcoclhs_handle *pco, bool on);

    /**
     * Get the double image mode on the camera.
     * @param pco handle
     * @param on output TRUE=on, FALSE=off
     * @return 0 on success, otherwise less than 0
     */
    unsigned int pcoclhs_get_double_image_mode(pcoclhs_handle *pco, bool *on);

    /**
     * Get the bit alignment for image data.
     * @param pco handle
     * @param on output the bit alignment [0x0000=LSB, 0x0001=MSB]
     * @return 0 on success, otherwise less than 0
     */
    unsigned int pcoclhs_get_bit_alignment(pcoclhs_handle *pco, bool *msb_aligned);

    /**
     * Set the bit alignment for image data.
     * @param pco handle
     * @param on the bit alignment [0x0000=LSB, 0x0001=MSB]
     * @return 0 on success, otherwise less than 0
     */
    unsigned int pcoclhs_set_bit_alignment(pcoclhs_handle *pco, bool msb_aligned);

    /**
     * Force a software trigger to the camera.
     * @param pco handle
     * @param success output 0x0001 if a trigger was successful, else 0x0000
     * @return 0 on success, otherwise less than 0
     */
    unsigned int pcoclhs_force_trigger(pcoclhs_handle *pco, uint16_t *success);

    /**
     * Set the timestamp format on images.
     * @param pco handle
     * @param mode the timestamp mode [0=OFF, 1=BINARY, 2=BINARY+ASCII, 3=ASCII]
     * @return 0 on success, otherwise less than 0
     */
    unsigned int pcoclhs_set_timestamp_mode(pcoclhs_handle *pco, uint16_t mode);

    /**
     * Get the timestamp format on images.
     * @param pco handle
     * @param mode output the timestamp mode [0=OFF, 1=BINARY, 2=BINARY+ASCII, 3=ASCII]
     * @return 0 on success, otherwise less than 0
     */
    unsigned int pcoclhs_get_timestamp_mode(pcoclhs_handle *pco, uint16_t *mode);

    /**
     * Set the delay and exposure timebases.
     * @param pco handle
     * @param delay the delay timebase [0=nanoseconds, 1=microseconds, 2=milliseconds]
     * @param exposure the exposure timebase [0=nanoseconds, 1=microseconds, 2=milliseconds]
     * @return 0 on success, otherwise less than 0
     */
    unsigned int pcoclhs_set_timebase(pcoclhs_handle *pco, uint16_t delay, uint16_t expos);

    /**
     * Get the delay and exposure timebases.
     * @param pco handle
     * @param delay output the delay timebase [0=nanoseconds, 1=microseconds, 2=milliseconds]
     * @param exposure output the exposure timebase [0=nanoseconds, 1=microseconds, 2=milliseconds]
     * @return 0 on success, otherwise less than 0
     */
    unsigned int pcoclhs_get_timebase(pcoclhs_handle *pco, uint16_t *delay, uint16_t *expos);

    /**
     * Get the capture delay time-length, in milliseconds. 
     * @param pco handle
     * @param delay output the delay time (ms)
     * @return 0 on success, otherwise less than 0
     */
    unsigned int pcoclhs_get_delay_time(pcoclhs_handle *pco, double *delay);

    /**
     * Set the capture delay time-length, in milliseconds. 
     * @param pco handle
     * @param delay the delay time (ms)
     * @return 0 on success, otherwise less than 0
     */
    unsigned int pcoclhs_set_delay_time(pcoclhs_handle *pco, double delay);

    /**
     * Get the available time range for capture delay.
     * @param pco handle
     * @param min_ns output the minimum delay in nanoseconds (usually 0ns)
     * @param max_ms output the maximum delay in milliseconds
     * @param step_ns output the step value in nanoseconds
     * @return 0 on success, otherwise less than 0
     */
    unsigned int pcoclhs_get_delay_range(pcoclhs_handle *pco, uint32_t *min_ns, uint32_t *max_ms, uint32_t *step_ns);

    /**
     * Get the capture exposure time-length, in milliseconds
     * @param pco handle
     * @param exposure output the exposure time (ms)
     * @return 0 on success, otherwise less than 0
     */
    unsigned int pcoclhs_get_exposure_time(pcoclhs_handle *pco, double *exposure);

    /**
     * Set the capture exposure time-length, in milliseconds
     * @param pco handle
     * @param exposure the exposure time (ms)
     * @return 0 on success, otherwise less than 0
     */
    unsigned int pcoclhs_set_exposure_time(pcoclhs_handle *pco, double exposure);

    /**
     * Get the available time range for capture exposure.
     * @param pco handle
     * @param min_ns output the minimum exposure in nanoseconds
     * @param max_ms output the maximum exposure in milliseconds
     * @param step_ns output the step value in nanoseconds
     * @return 0 on success, otherwise less than 0
     */
    unsigned int pcoclhs_get_exposure_range(pcoclhs_handle *pco, uint32_t *min_ns, uint32_t *max_ms, uint32_t *step_ns);

    /**
     * Get the capture delay and exposure time-lengths, both in milliseconds
     * @param pco handle
     * @param delay the delay time (ms)
     * @param exposure the exposure time (ms)
     * @return 0 on success, otherwise less than 0
     */
    unsigned int pcoclhs_get_delay_exposure(pcoclhs_handle *pco, double *delay, double *exposure);

    /**
     * Set the capture delay and exposure time-lengths, both in milliseconds
     * @param pco handle
     * @param delay the delay time (ms)
     * @param exposure the exposure time (ms)
     * @return 0 on success, otherwise less than 0
     */
    unsigned int pcoclhs_set_delay_exposure(pcoclhs_handle *pco, double delay, double exposure);

    /**
     * Get the camera trigger mode. Mode availability depends on the camera model.
     * @param pco handle
     * @param mode output the trigger mode [0=AUTO, 1=SOFTWARE, 2=SOFTWARE+EXTERNAL, 3=EXTERNAL]
     * @return 0 on success, otherwise less than 0
     */
    unsigned int pcoclhs_get_trigger_mode(pcoclhs_handle *pco, uint16_t *mode);

    /**
     * Set the camera trigger mode. Mode availability depends on the camera model.
     * @param pco handle
     * @param mode the trigger mode [0=AUTO, 1=SOFTWARE, 2=SOFTWARE+EXTERNAL, 3=EXTERNAL]
     * @return 0 on success, otherwise less than 0
     */
    unsigned int pcoclhs_set_trigger_mode(pcoclhs_handle *pco, uint16_t mode);

    /**
     * Arm the camera and validate settings.
     * @param pco handle
     * @return 0 on success, otherwise less than 0
     */
    unsigned int pcoclhs_arm_camera(pcoclhs_handle *pco);

    /**
     * Get the acquisition recording state.
     * @param pco handle
     * @param state output the recording state [0x0000=STOPPED, 0x0001=RUNNING]
     * @return 0 on success, otherwise less than 0
     */
    unsigned int pcoclhs_get_recording_state(pcoclhs_handle *pco, uint16_t *state);

    /**
     * Set the acquisition recording state.
     * @param pco handle
     * @param state the recording state [0x0000=STOP, 0x0001=RUN]
     * @return 0 on success, otherwise less than 0
     */
    unsigned int pcoclhs_set_recording_state(pcoclhs_handle *pco, uint16_t state);

    /**
     * Get the camera acquisition mode.
     * @param pco handle
     * @param state output the acquisition mode [0x0000=AUTO, 0x0001=EXTERN-STATIC, 0x0002=EXTERN-DYNAMIC]
     * @return 0 on success, otherwise less than 0
     */
    unsigned int pcoclhs_get_acquire_mode(pcoclhs_handle *pco, uint16_t *mode);

    /**
     * Set the camera acquisition mode.
     * @param pco handle
     * @param state the acquisition mode [0x0000=AUTO, 0x0001=EXTERN-STATIC, 0x0002=EXTERN-DYNAMIC]
     * @return 0 on success, otherwise less than 0
     */
    unsigned int pcoclhs_set_acquire_mode(pcoclhs_handle *pco, uint16_t mode);

    /**
     * Trigger an image capture and transfer with a 10 second timeout
     * @param pco handle
     * @param adr external buffer to write image data
     * @return 0 on success, otherwise less than 0
     */
    unsigned int pcoclhs_await_next_image(pcoclhs_handle *pco, void *adr);

    /**
     * Trigger an image capture and transfer.
     * @param pco handle
     * @param adr external buffer to write image data
     * @param timeout the number of milliseconds to wait before an error is produced.
     * @return 0 on success, otherwise less than 0
     */
    unsigned int pcoclhs_await_next_image_ex(pcoclhs_handle *pco, void *adr, int timeout);

    /**
     * Simple image acquisition call to the pco.camera SDK with grabber-configured timeout.
     * @param pco handle
     * @param adr external buffer to write image data
     * @return 0 on success, otherwise less than 0
     */
    unsigned int pcoclhs_acquire_image(pcoclhs_handle *pco, void *adr);

    /**
     * Simple image acquisition call to the pco.camera SDK.
     * @param pco handle
     * @param adr external buffer to write image data
     * @param timeout the number of milliseconds to wait before an error is produced.
     * @return 0 on success, otherwise less than 0
     */
    unsigned int pcoclhs_acquire_image_ex(pcoclhs_handle *pco, void *adr, int timeout);

    /**
     * Get the actual size of the image capture area, accounting for binning and ROI.
     * @param pco handle
     * @param width output the area width
     * @param height output the area height
     * @return 0 on success, otherwise less than 0
     */
    unsigned int pcoclhs_get_actual_size(pcoclhs_handle *pco, uint32_t *width, uint32_t *height);

    /**
     * Get the binning values of the camera.
     * @param pco handle
     * @param horizontal output the horizontal binning value (usually 1, 2, or 4)
     * @param vertical output the vertical binning value (usually 1, 2, or 4)
     * @return 0 on success, otherwise less than 0
     */
    unsigned int pcoclhs_get_binning(pcoclhs_handle *pco, uint16_t *horizontal, uint16_t *vertical);

    /**
     * Set the binning values of the camera.
     * @param pco handle
     * @param horizontal the horizontal binning value (usually 1, 2, or 4)
     * @param vertical the vertical binning value (usually 1, 2, or 4)
     * @return 0 on success, otherwise less than 0
     */
    unsigned int pcoclhs_set_binning(pcoclhs_handle *pco, uint16_t horizontal, uint16_t vertical);

    /**
     * Get the possible binning values available to the camera.
     * @param pco handle
     * @param horizontal output array of possible horizontal binning values
     * @param num_horizontal output the number of horizontal binning values in the array
     * @param vertical output array of possible vertical binning values
     * @param num_vertical output the number of vertical binning values in the array
     * @return 0 on success, otherwise less than 0
     */
    unsigned int pcoclhs_get_possible_binnings(pcoclhs_handle *pco, uint16_t **horizontal, unsigned int *num_horizontal, uint16_t **vertical, unsigned int *num_vertical);

    /**
     * Get the camera noise filter mode.
     * @param pco handle
     * @param mode output the moise filter mode [0x0000=OFF, 0x0001=ON, 0x0101=ON+HOTPIXELCORRECTION]
     * @return 0 on success, otherwise less than 0
     */
    unsigned int pcoclhs_get_noise_filter_mode(pcoclhs_handle *pco, uint16_t *mode);

    /**
     * Get the camera noise filter mode.
     * @param pco handle
     * @param mode output the moise filter mode [0x0000=OFF, 0x0001=ON, 0x0101=ON+HOTPIXELCORRECTION]
     * @return 0 on success, otherwise less than 0
     */
    unsigned int pcoclhs_set_noise_filter_mode(pcoclhs_handle *pco, uint16_t mode);

    /**
     * Get the camera shutter mode.
     * @param pco handle
     * @param shutter output the configured shutter mode [1=ROLLING, 2=GLOBAL, 4=RESET]
     * @return 0 on success, otherwise less than 0
     */
    unsigned int pcoclhs_edge_get_shutter(pcoclhs_handle *pco, pco_edge_shutter *shutter);

    /**
     * Set the camera shutter mode. A reboot of the camera is necessary.
     * @param pco handle
     * @param shutter the shutter mode [1=ROLLING, 2=GLOBAL, 4=RESET]
     * @return 0 on success, otherwise less than 0
     */
    // unsigned int pcoclhs_edge_set_shutter(pcoclhs_handle *pco, pco_edge_shutter shutter);

    /**
     * Ensure the camera time is in sync with the current system time.
     * @param pco handle
     * @param shutter the shutter mode [1=ROLLING, 2=GLOBAL, 4=RESET]
     * @return 0 on success, otherwise less than 0
     */
    unsigned int pcoclhs_update_camera_datetime(pcoclhs_handle *pco);

    /**
     * Get a pointer to the configured function for reordering image bytes. 
     *    (uint16_t *bufout, uint16_t *bufin, int width, int height) -> void
     * @note signature
     * @param pco handle
     * @return function pointer
     */
    pcoclhs_reorder_image_t pcoclhs_get_reorder_func(pcoclhs_handle *pco);

    /**
     * Reorder image data based on camera and format.
     * @param pco handle
     * @param bufout the output buffer
     * @param bufin the input buffer
     * @param width the image width
     * @param height the image height
     * @return void
     */
    void pcoclhs_reorder_image(pcoclhs_handle *pco, uint16_t *bufout, uint16_t *bufin, int width, int height);


#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /*_PCOCLHS_H_*/
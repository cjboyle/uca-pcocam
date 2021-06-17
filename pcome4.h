#ifndef _PCOME4_H_
#define _PCOME4_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "pco.h"

#include "pco/include/me4/defs.h"
#include "pco/include/me4/sc2_defs.h"
#include "pco/include/me4/sc2_cl_defs.h"
#include "pco/include/me4/PCO_err.h"

#ifndef SISODIR5
#define SISODIR5 "/opt/SiliconSoftware/Runtime5.7.0"
#endif

    /**
     * Start camera acquisition for a given number of images.
     * @param pco handle
     * @param nr_images the number of images to acquire, or infinite if <= 0
     * @return 0 on success, otherwise less than 0
     */
    unsigned int pco_start_recording_ex(pco_handle *pco, int nr_images, bool block);

    /**
     * Start camera acquisition from memory.
     * @param pco handle
     * @return 0 on success, otherwise less than 0
     */
    unsigned int pco_start_readout(pco_handle *pco);

    /**
     * Start camera acquisition from memory for a given number of images.
     * @param pco handle
     * @param nr_images the number of images to acquire, or infinite if <= 0
     * @param block if TRUE, written buffers will not be overwritten until read; if FALSE, 
     * @return 0 on success, otherwise less than 0
     */
    unsigned int pco_start_readout_ex(pco_handle *pco, int nr_images, bool block);

    /**
     * Stop camera acquisition from memory.
     * @param pco handle
     * @return 0 on success, otherwise less than 0
     */
    unsigned int pco_stop_readout(pco_handle *pco);

    /**
     * Get the current internal memory segment.
     * @param pco handle
     * @param segment output the current segment
     * @return 0 on success, otherwise less than 0
     */
    unsigned int pco_get_active_segment(pco_handle *pco, uint16_t *segment);

    /**
     * Set the current internal memory segment.
     * @param pco handle
     * @param segment the memory segment to use
     * @return 0 on success, otherwise less than 0
     */
    unsigned int pco_set_active_segment(pco_handle *pco, uint16_t segment);

    /**
     * Clear the active memory segment.
     * @param pco handle
     * @return 0 on success, otherwise less than 0
     */
    unsigned int pco_clear_active_segment(pco_handle *pco);

    /**
     * Get the number of recorded images in the given segment.
     * @param pco handle
     * @param segment the memory segment to use
     * @param nr_images output the number of recorded images
     * @param max_images output the maximum image capacity
     * @return 0 on success, otherwise less than 0
     */
    unsigned int pco_get_nr_recorded_images(pco_handle *pco, uint16_t segment, uint32_t *nr_images, uint32_t *max_images);
    
    /**
     * Get the number of the last transferred image.
     * @param pco handle
     * @param image_nr output the last image number
     * @return 0 on success, otherwise less than 0
     */
    unsigned int pco_last_image_index(pco_handle *pco, int *image_nr);

    /**
     * Wait until the next image is fully transferred and return its number.
     * @param pco handle
     * @param image_nr output the last image number
     * @return 0 on success, otherwise less than 0
     */
    unsigned int pco_next_image_index(pco_handle *pco, int *image_nr);

    /**
     * Wait until the next image is fully transferred and return its number.
     * @param pco handle
     * @param image_nr output the last image number
     * @param timeout the number of milliseconds to wait before an error is produced
     * @return 0 on success, otherwise less than 0
     */
    unsigned int pco_next_image_index_ex(pco_handle *pco, int *image_nr, int timeout);

    /**
     * Read the images stored in the active memory segment.
     * @param pco handle
     * @param segment the memory segment to use
     * @param first_image the index of the first image (1-based)
     * @param last_image the index of the last image (1-based)
     * @return 0 on success, otherwise less than 0
     */
    unsigned int pco_read_segment_images(pco_handle *pco, uint16_t segment, uint32_t first_image, uint32_t last_image);

    /**
     * Get a pointer to the image specified by the given image number.
     * @param pco handle
     * @param adr output the pointer to a framebuffer
     * @param image_nr the number of the image to transfer
     * @return 0 on success, otherwise less than 0
     */
    unsigned int pco_get_image_ptr(pco_handle *pco, void **adr, int image_nr);

    /**
     * Read an image from the internal CamRAM module.
     * @param pco handle
     * @param adr external buffer to write image data
     * @param segment the memory segment to use
     * @param image_nr the number of the image to transfer
     * @return 0 on success, otherwise less than 0
     */
    unsigned int pco_readout_image(pco_handle *pco, void *adr, uint16_t segment, int image_nr);

    /**
     * Get the number of ADCs in the camera sensor.
     * @param pco handle
     * @param nr_adcs output the number of ADCs
     * @return 0 on success, otherwise less than 0
     */
    unsigned int pco_get_nr_adcs(pco_handle *pco, uint16_t *nr_adcs);

    /**
     * Get the number of ADCs in use.
     * @param pco handle
     * @param mode output the number of ADCs
     * @return 0 on success, otherwise less than 0
     */
    unsigned int pco_get_adc_mode(pco_handle *pco, uint16_t *mode);
    
    /**
     * Set the number of ADCs to use.
     * @param pco handle
     * @param mode the number of ADCs
     * @return 0 on success, otherwise less than 0
     */
    unsigned int pco_set_adc_mode(pco_handle *pco, uint16_t mode);

    /**
     * Get the camera recorder mode.
     * @param pco handle
     * @param mode output the recorder mode (sequence or ring buffer)
     * @return 0 on success, otherwise less than 0
     */
    unsigned int pco_get_recorder_mode(pco_handle *pco, uint16_t *mode);
    
    /**
     * Set the camera recorder mode.
     * @param pco handle
     * @param mode the recorder mode (sequence or ring buffer)
     * @return 0 on success, otherwise less than 0
     */
    unsigned int pco_set_recorder_mode(pco_handle *pco, uint16_t mode);

    /**
     * Get the camera storage mode.
     * @param pco handle
     * @param mode output the storage mode (recorder or FIFO)
     * @return 0 on success, otherwise less than 0
     */
    unsigned int pco_get_storage_mode(pco_handle *pco, uint16_t *mode);
    
    /**
     * Set the camera storage mode.
     * @param pco handle
     * @param mode the storage mode (recorder or FIFO)
     * @return 0 on success, otherwise less than 0
     */
    unsigned int pco_set_storage_mode(pco_handle *pco, uint16_t mode);

    /**
     * Reorder image data based on camera and format.
     * @param pco handle
     * @param bufout the output buffer
     * @param bufin the input buffer
     * @param width the image width
     * @param height the image height
     */
    void pco_extract_image(pco_handle *pco, uint16_t *bufout, uint16_t *bufin, int width, int height);

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /*_PCOME4_H_*/
#ifndef _PCOUSB_H_
#define _PCOUSB_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "pco.h"

#include "pco/include/usb/defs.h"
#include "pco/include/usb/sc2_defs.h"
#include "pco/include/usb/PCO_err.h"

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
     * Get the number of triggered frames. This function may not be reliably fast for some applications.
     * @param pco handle
     * @param count output the number of triggered frames
     * @return 0 on success, otherwise less than 0
     */
    unsigned int pco_get_trigger_count(pco_handle *pco, uint32_t *count);

    /**
     * Non-blocking image acquisition call to the pco.camera SDK with grabber-configured timeout.
     * @param pco handle
     * @param adr external buffer to write image data
     * @return 0 on success, otherwise less than 0
     */
    uint32_t pco_acquire_image_async(pco_handle *pco, void *adr);

    /**
     * Non-blocking image acquisition call to the pco.camera SDK.
     * @param pco handle
     * @param adr external buffer to write image data
     * @param timeout the number of milliseconds to wait before an error is produced
     * @return 0 on success, otherwise less than 0
     */
    uint32_t pco_acquire_image_async_ex(pco_handle *pco, void *adr, int timeout);

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /* _PCOUSB_H_ */
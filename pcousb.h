#ifndef _PCOUSB_H_
#define _PCOUSB_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <stdbool.h>

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

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /* _PCOUSB_H_ */
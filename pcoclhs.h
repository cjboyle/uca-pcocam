#ifndef _PCOCLHS_H_
#define _PCOCLHS_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "pco.h"

#include "pco/include/clhs/defs.h"
#include "pco/include/clhs/sc2_defs.h"
#include "pco/include/clhs/PCO_err.h"

#ifndef SISODIR5
#define SISODIR5 "/opt/SiliconSoftware/Runtime5.7.0"
#endif

    /**
     * Get the number of triggered frames. This function may not be reliably fast for some applications.
     * @param pco handle
     * @param count output the number of triggered frames
     * @return 0 on success, otherwise less than 0
     */
    unsigned int pco_get_trigger_count(pco_handle *pco, uint32_t *count);

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /*_PCOCLHS_H_*/
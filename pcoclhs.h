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

    enum _pco_edge_shutter
    {
        PCO_EDGE_ROLLING_SHUTTER = PCO_EDGE_SETUP_ROLLING_SHUTTER,
        PCO_EDGE_GLOBAL_SHUTTER = PCO_EDGE_SETUP_GLOBAL_SHUTTER,
        PCO_EDGE_GLOBAL_RESET = PCO_EDGE_SETUP_GLOBAL_RESET,
    };

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /*_PCOCLHS_H_*/
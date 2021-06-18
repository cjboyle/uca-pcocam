#include "pco/include/me4/defs.h"
#include "pco/include/me4/Cpco_grab_cl_me4.h"

#ifndef DWORD
typedef unsigned int DWORD;
#endif

class CPco_grab_cl_me4
{
protected:
    Fg_Struct *fg;
    int port;
    dma_mem *pMemInt;
    int aquire_status, aquire_flag;

public:
    DWORD Start_Acquire_NonBlock(int nr_of_ima);
};

DWORD CPco_grab_cl_me4::Start_Acquire_NonBlock(int nr_of_ima)
{
    if (nr_of_ima < 0)
        nr_of_ima = GRAB_INFINITE;
    aquire_status = ACQ_STANDARD;
    if (Fg_AcquireEx(fg, port, nr_of_ima, aquire_status, pMemInt) != FG_OK)
        return PCO_ERROR_DRIVER_SYSERR | PCO_ERROR_DRIVER_CAMERALINK;
    aquire_flag|=PCO_SC2_CL_NON_BLOCKING_BUFFER;
    return PCO_NOERROR;
}
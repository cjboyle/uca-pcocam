#include "pco/include/me4/defs.h"
#include "pco/include/me4/Cpco_grab_cl_me4.h"
#include <stdio.h>

DWORD CPco_grab_cl_me4::Start_Acquire_NonBlock(int nr_of_ima)
{
    if (nr_of_ima < 0)
        nr_of_ima = GRAB_INFINITE;
    aquire_status = ACQ_STANDARD;

    dma_mem *pMem;
    if ((buf_manager & PCO_SC2_CL_EXTERNAL_BUFFER) == PCO_SC2_CL_EXTERNAL_BUFFER)
        pMem = pMem0;
    else if ((buf_manager & PCO_SC2_CL_INTERNAL_BUFFER) == PCO_SC2_CL_INTERNAL_BUFFER)
        pMem = pMemInt;
    else
        return PCO_ERROR_WRONGVALUE | PCO_ERROR_DRIVER | PCO_ERROR_DRIVER_CAMERALINK;

    if (Fg_AcquireEx(fg, port, nr_of_ima, aquire_status, pMem) != FG_OK)
        return PCO_ERROR_DRIVER_SYSERR | PCO_ERROR_DRIVER_CAMERALINK;

    aquire_flag |= PCO_SC2_CL_NON_BLOCKING_BUFFER;
    return PCO_NOERROR;
}

DWORD CPco_grab_cl_me4::Wait_For_Next_Image(int *nr_of_pic, int timeout)
{
    frameindex_t index = -1;

    dma_mem *pMem;
    if ((buf_manager & PCO_SC2_CL_EXTERNAL_BUFFER) == PCO_SC2_CL_EXTERNAL_BUFFER)
        pMem = pMem0;
    else if ((buf_manager & PCO_SC2_CL_INTERNAL_BUFFER) == PCO_SC2_CL_INTERNAL_BUFFER)
        pMem = pMemInt;
    else
        return PCO_ERROR_WRONGVALUE | PCO_ERROR_DRIVER | PCO_ERROR_DRIVER_CAMERALINK;

    index = Fg_getLastPicNumberBlockingEx(fg, *nr_of_pic, port, timeout, pMem);

    if (index < 0)
    {
        *nr_of_pic = 0;
        return PCO_ERROR_TIMEOUT | PCO_ERROR_DRIVER | PCO_ERROR_DRIVER_CAMERALINK;
    }

    *nr_of_pic = index;
    return PCO_NOERROR;
}

DWORD CPco_grab_cl_me4::Get_Framebuffer_adr(int nr_of_pic, void **adr)
{
    dma_mem *pMem;
    if ((buf_manager & PCO_SC2_CL_EXTERNAL_BUFFER) == PCO_SC2_CL_EXTERNAL_BUFFER)
        pMem = pMem0;
    else if ((buf_manager & PCO_SC2_CL_INTERNAL_BUFFER) == PCO_SC2_CL_INTERNAL_BUFFER)
        pMem = pMemInt;
    else
        return PCO_ERROR_WRONGVALUE | PCO_ERROR_DRIVER | PCO_ERROR_DRIVER_CAMERALINK;

    *adr = Fg_getImagePtrEx(fg, nr_of_pic, port, pMem);

    return PCO_NOERROR;
}
#include "pco/include/me4/Cpco_grab_cl_me4.h"
#include "pco/include/me4/reorderfunc.h"

static int sprintf_s(char *buf, int dwlen, const char *cp, ...)
{
    va_list arglist;
    va_start(arglist, cp);
    return vsnprintf(buf, dwlen, cp, arglist);
}

static int strcpy_s(char *buf, int dwlen, const char *src)
{
    strcpy(buf, src);
    return 0;
}

CPco_grab_cl_me4::CPco_grab_cl_me4(CPco_com_cl_me4 *camera)
{
    clog = NULL;
    hgrabber = NULL;

    fg = NULL;
    pco_hap_loaded = 0;
    act_width = act_height = 100;
    port = 0;
    me_boardnr = -1;
    DataFormat = 0;
    act_dmalength = 100 * 100;
    // act_sccmos_version = 0;

    buf_manager = 0;
    pMem0 = NULL;
    pMemInt = NULL;
    padr = NULL;
    size_alloc = 0;
    nr_of_buffer = 0;

    aquire_status = 0;
    aquire_flag = 0;
    last_picnr = 0;

    if (camera != NULL)
        cam = camera;
}

CPco_grab_cl_me4_edge::CPco_grab_cl_me4_edge(CPco_com_cl_me4 *camera)
    : CPco_grab_cl_me4(camera)
{
}

CPco_grab_cl_me4_camera::CPco_grab_cl_me4_camera(CPco_com_cl_me4 *camera)
    : CPco_grab_cl_me4(camera)
{
}

void CPco_grab_cl_me4::writelog(DWORD lev, PCO_HANDLE hdriver, const char *str, ...)
{
    char txt[1000];
    if (clog)
    {
        va_list arg;
        va_start(arg, str);
        vsprintf(txt, str, arg);
        clog->writelog(lev, hdriver, txt);
        va_end(arg);
    }
}

////////////////////////////////////////////////////////////////////////
// Fg_Error
//
// Auslesen des aktuellen Fehlerstatus und
// Ausgabe auf stdout
//
void CPco_grab_cl_me4::Fg_Error(Fg_Struct *fg)
{
    int error;
    PCO_HANDLE drv;
    // char* str;
    // str = Fg_getLastErrorDescription(fg);
    error = Fg_getLastErrorNumber(fg);
    if (!fg)
        drv = (PCO_HANDLE)0x0001;
    else
        drv = (PCO_HANDLE)0x1234;
    writelog(ERROR_M, drv, "FG_Error %d", error);
}

DWORD CPco_grab_cl_me4::Get_actual_size(unsigned int *width, unsigned int *height, unsigned int *bitpix)
{
    if (width)
        *width = act_width;
    if (height)
        *height = act_height;
    if (bitpix)
        *bitpix = 16;

    return PCO_NOERROR;
}

DWORD CPco_grab_cl_me4::Close_Grabber()
{
    //search if other port was opened before
    writelog(INIT_M, hgrabber, "close_grabber:");
    if (fg == NULL)
    {
        writelog(INIT_M, hgrabber, "close_grabber: Grabber was closed before");
        return PCO_NOERROR;
    }

    writelog(INIT_M, hgrabber, "close_grabber: Fg_FreeGrabber");
    Fg_FreeGrabber(fg);
    fg = NULL;
    me_boardnr = -1;

    return PCO_NOERROR;
}

void CPco_grab_cl_me4::SetLog(CPco_Log *elog) { clog = elog; }

DWORD CPco_grab_cl_me4::Set_DataFormat(DWORD dataformat)
{
    DataFormat = dataformat;
    return PCO_NOERROR;
}

DWORD CPco_grab_cl_me4_edge::Set_DataFormat(DWORD dataformat)
{
    DataFormat = dataformat;
    return PCO_NOERROR;
}

DWORD CPco_grab_cl_me4_camera::Set_DataFormat(DWORD dataformat)
{
    DataFormat = dataformat;
    return PCO_NOERROR;
}

DWORD CPco_grab_cl_me4::Set_Grabber_Timeout(int timeout)
{
    int err = PCO_NOERROR;
    int val = timeout;
    if (Fg_setParameter(fg, FG_TIMEOUT, &val, port) < 0)
    {
        Fg_Error(fg);
        writelog(ERROR_M, hgrabber, "Set_Grabber_Timeout: set FG_TIMEOUT failed");
        err = PCO_ERROR_DRIVER_DATAERROR | PCO_ERROR_DRIVER_CAMERALINK;
    }
    return err;
}

DWORD CPco_grab_cl_me4::Get_Grabber_Timeout(int *timeout)
{
    int err = PCO_NOERROR;
    int val;
    if (Fg_getParameter(fg, FG_TIMEOUT, &val, port) < 0)
    {
        Fg_Error(fg);
        writelog(ERROR_M, hgrabber, "Set_Grabber_Timeout: set FG_TIMEOUT failed");
        err = PCO_ERROR_DRIVER_DATAERROR | PCO_ERROR_DRIVER_CAMERALINK;
    }
    *timeout = val;
    return err;
}

DWORD CPco_grab_cl_me4::Allocate_Framebuffer(int bufnum)
{
    return Allocate_Framebuffer(bufnum, NULL);
}

DWORD CPco_grab_cl_me4::Allocate_Framebuffer(int bufnum, void *adr_in)
{
    int x, err;
    err = PCO_NOERROR;

    if (nr_of_buffer > 0)
    {
        writelog(INTERNAL_1_M, hgrabber, "Allocate_Framebuffer: buffers already allocated");
        return PCO_ERROR_SDKDLL_BUFFERNOTVALID | PCO_ERROR_DRIVER | PCO_ERROR_DRIVER_CAMERALINK;
    }

    int picsize = act_width * act_height * sizeof(WORD);

#ifndef _M_X64
    if (bufnum > 0x7FFFFFFF / picsize)
        return PCO_ERROR_NOMEMORY | PCO_ERROR_DRIVER | PCO_ERROR_DRIVER_CAMERALINK;
#endif

    size_t total_buffer_size = picsize * bufnum;

    if ((buf_manager & PCO_SC2_CL_INTERNAL_BUFFER) == PCO_SC2_CL_INTERNAL_BUFFER)
    {
        try
        {
            if ((pMemInt = Fg_AllocMemEx(fg, total_buffer_size, bufnum)) == NULL)
            {
                Fg_Error(fg);
                writelog(ERROR_M, hgrabber, "Allocate_Framebuffer: Fg_AllocMemEx failed");
                return PCO_ERROR_NOMEMORY | PCO_ERROR_DRIVER | PCO_ERROR_DRIVER_CAMERALINK;
            }
        }
        catch (...)
        {
            return PCO_ERROR_NOMEMORY | PCO_ERROR_DRIVER | PCO_ERROR_DRIVER_CAMERALINK;
        }
        nr_of_buffer = bufnum;
        return err;
    }

    if ((buf_manager & PCO_SC2_CL_EXTERNAL_BUFFER) == PCO_SC2_CL_EXTERNAL_BUFFER)
    {
        if ((pMem0 = Fg_AllocMemHead(fg, total_buffer_size, bufnum)) == NULL)
        {
            Fg_Error(fg);
            writelog(ERROR_M, hgrabber, "Allocate_Framebuffer: Fg_AllocMemHead failed");
            return PCO_ERROR_NOMEMORY | PCO_ERROR_DRIVER | PCO_ERROR_DRIVER_CAMERALINK;
        }

        if ((buf_manager & PCO_SC2_CL_ONLY_BUFFER_HEAD) == 0)
        {
            unsigned char **adr;
            if (adr_in == NULL)
            {
                x = 0;
                adr = (unsigned char **)malloc(bufnum * sizeof(unsigned char *));
                if (adr != NULL)
                {
                    padr = adr;
                    memset(padr, 0, bufnum * sizeof(unsigned char *));
                    for (x = 0; x < bufnum; x++)
                    {
                        padr[x] = (unsigned char *)malloc(picsize);
                        if (padr[x] == NULL)
                            break;
                    }
                }
                else
                    err = PCO_ERROR_NOMEMORY | PCO_ERROR_DRIVER | PCO_ERROR_DRIVER_CAMERALINK;
                nr_of_buffer = x;
            }
            else
            {
                padr = NULL;
                nr_of_buffer = bufnum;
                adr = (unsigned char **)adr_in;
            }

            for (x = 0; x < nr_of_buffer; x++)
            {
                if (adr[x])
                {
                    if (Fg_AddMem(fg, adr[x], picsize, x, pMem0) < FG_OK)
                    {
                        Fg_Error(fg);
                        writelog(ERROR_M, hgrabber, "Allocate_Framebuffer: Fg_AddMem failed");
                        err = PCO_ERROR_NOMEMORY | PCO_ERROR_DRIVER | PCO_ERROR_DRIVER_CAMERALINK;
                        break;
                    }
                }
            }

            if (err != PCO_NOERROR)
            {
                for (x = 0; x < nr_of_buffer; x++)
                {
                    if (adr[x])
                    {
                        if (Fg_DelMem(fg, pMem0, x) != FG_OK)
                        {
                            Fg_Error(fg);
                            writelog(ERROR_M, hgrabber, "Allocate_Framebuffer: Fg_DelMem failed");
                            err = PCO_ERROR_NOMEMORY | PCO_ERROR_DRIVER | PCO_ERROR_DRIVER_CAMERALINK;
                        }
                        if (padr)
                        {
                            free(adr[x]);
                        }
                    }
                }
                if (padr)
                    free(padr);
                padr = NULL;
                nr_of_buffer = 0;

                if (Fg_FreeMemHead(fg, pMem0) != FG_OK)
                {
                    Fg_Error(fg);
                    writelog(ERROR_M, hgrabber, "Allocate_Framebuffer: Fg_FreeMemHead failed");
                    err = PCO_ERROR_NOMEMORY | PCO_ERROR_DRIVER | PCO_ERROR_DRIVER_CAMERALINK;
                }
            }
            else
                size_alloc = picsize;
        }
        else
            nr_of_buffer = bufnum;
    }
    else
    {
        writelog(ERROR_M, hgrabber, "Allocate_Framebuffer: buf_manager_flag  0x%x not supported", buf_manager);
        return PCO_ERROR_WRONGVALUE | PCO_ERROR_DRIVER | PCO_ERROR_DRIVER_CAMERALINK;
    }

    return err;
}

DWORD CPco_grab_cl_me4::Free_Framebuffer()
{
    return Free_Framebuffer(NULL);
}

DWORD CPco_grab_cl_me4::Free_Framebuffer(void *adr_in)
{
    int err;

    err = PCO_NOERROR;

    if (nr_of_buffer == 0)
        return PCO_NOERROR;

    if ((buf_manager & PCO_SC2_CL_INTERNAL_BUFFER) == PCO_SC2_CL_INTERNAL_BUFFER)
    {
        if (Fg_FreeMemEx(fg, pMemInt) != FG_OK)
        {
            Fg_Error(fg);
            writelog(ERROR_M, hgrabber, "Free_Framebuffer: Fg_FreeMemEx failed");
            return PCO_ERROR_NOMEMORY | PCO_ERROR_DRIVER | PCO_ERROR_DRIVER_CAMERALINK;
        }
        nr_of_buffer = 0;
        pMemInt = NULL;
        return err;
    }

    if ((buf_manager & PCO_SC2_CL_EXTERNAL_BUFFER) == PCO_SC2_CL_EXTERNAL_BUFFER)
    {
        if ((buf_manager & PCO_SC2_CL_ONLY_BUFFER_HEAD) == 0)
        {
            unsigned char **adr;
            if (adr_in == NULL)
                adr = padr;
            else
                adr = (unsigned char **)adr_in;

            for (int x = 0; x < nr_of_buffer; x++)
            {
                if (adr[x])
                {
                    if (Fg_DelMem(fg, pMem0, x) != FG_OK)
                    {
                        Fg_Error(fg);
                        writelog(ERROR_M, hgrabber, "Free_Framebuffer: Fg_DelMem failed");
                        err = PCO_ERROR_NOMEMORY | PCO_ERROR_DRIVER | PCO_ERROR_DRIVER_CAMERALINK;
                    }
                    if (padr)
                    {
                        free(adr[x]);
                        adr[x] = 0;
                    }
                }
            }

            if (padr)
                free(padr);
            padr = NULL;

            nr_of_buffer = 0;
        }

        if (Fg_FreeMemHead(fg, pMem0) != FG_OK)
        {
            Fg_Error(fg);
            writelog(ERROR_M, hgrabber, "Free_Framebuffer: Fg_FreeMemHead failed");
            err = PCO_ERROR_NOMEMORY | PCO_ERROR_DRIVER | PCO_ERROR_DRIVER_CAMERALINK;
        }
    }
    else
    {
        writelog(ERROR_M, hgrabber, "Free_Framebuffer: buf_manager_flag  0x%x not supported", buf_manager);
        return PCO_ERROR_WRONGVALUE | PCO_ERROR_DRIVER | PCO_ERROR_DRIVER_CAMERALINK;
    }

    return err;
}

DWORD CPco_grab_cl_me4::Get_Framebuffer()
{
    return nr_of_buffer;
}

DWORD CPco_grab_cl_me4::Start_Acquire(int nr_of_ima)
{
    return Start_Acquire(nr_of_ima, 0);
}

DWORD CPco_grab_cl_me4::Start_Acquire_NonBlock(int nr_of_ima)
{
    return Start_Acquire(nr_of_ima, PCO_SC2_CL_NON_BLOCKING_BUFFER);
}

DWORD CPco_grab_cl_me4::Start_Acquire(int nr_of_ima, int flag)
{
    int err;
    frameindex_t index;

    err = PCO_NOERROR;

    aquire_flag &= ~(PCO_SC2_CL_START_CONT | PCO_SC2_CL_NON_BLOCKING_BUFFER | PCO_SC2_CL_BLOCKING_BUFFER);

    if (nr_of_ima <= 0)
        nr_of_ima = GRAB_INFINITE;

    if ((flag & PCO_SC2_CL_START_CONT) == PCO_SC2_CL_START_CONT)
    {
        nr_of_ima = GRAB_INFINITE;
        aquire_flag |= PCO_SC2_CL_START_CONT;
    }

    dma_mem *pMem;

    if ((buf_manager & PCO_SC2_CL_EXTERNAL_BUFFER) == PCO_SC2_CL_EXTERNAL_BUFFER)
        pMem = pMem0;
    else if ((buf_manager & PCO_SC2_CL_INTERNAL_BUFFER) == PCO_SC2_CL_INTERNAL_BUFFER)
        pMem = pMemInt;
    else
    {
        writelog(ERROR_M, hgrabber, "Start_Acquire: buf_manager_flag  0x%x not supported", buf_manager);
        return PCO_ERROR_WRONGVALUE | PCO_ERROR_DRIVER | PCO_ERROR_DRIVER_CAMERALINK;
    }

    aquire_status = ACQ_STANDARD;
    aquire_flag |= PCO_SC2_CL_NON_BLOCKING_BUFFER;
    writelog(PROCESS_M, hgrabber, "Start_Acquire: aquire_flag  PCO_SC2_CL_NON_BLOCKING_BUFFER", flag);

    if (Fg_AcquireEx(fg, port, nr_of_ima, aquire_status, pMem) != FG_OK)
    {
        Fg_Error(fg);
        writelog(ERROR_M, hgrabber, "Start_Acquire: Fg_AcquireEx failed");
        err = PCO_ERROR_DRIVER_SYSERR | PCO_ERROR_DRIVER_CAMERALINK;
    }
    index = Fg_getStatusEx(fg, NUMBER_OF_IMAGES_IN_PROGRESS, 1, port, pMem);
    writelog(PROCESS_M, hgrabber, "Start_Acquire: Start IMAGES_IN_PROGRESS: %d", index);

    //set startflag
    if (err == PCO_NOERROR)
        aquire_flag |= PCO_SC2_CL_STARTED;

    return err;
}

DWORD CPco_grab_cl_me4::Stop_Acquire()
{
    int err, ret;
    frameindex_t index;

    err = PCO_NOERROR;

    dma_mem *pMem;
    if ((buf_manager & PCO_SC2_CL_EXTERNAL_BUFFER) == PCO_SC2_CL_EXTERNAL_BUFFER)
        pMem = pMem0;
    else if ((buf_manager & PCO_SC2_CL_INTERNAL_BUFFER) == PCO_SC2_CL_INTERNAL_BUFFER)
        pMem = pMemInt;
    else
    {
        writelog(ERROR_M, hgrabber, "Stop_Acquire: buf_manager_flag  0x%x not supported", buf_manager);
        return PCO_ERROR_WRONGVALUE | PCO_ERROR_DRIVER | PCO_ERROR_DRIVER_CAMERALINK;
    }

    writelog(PROCESS_M, hgrabber, "Stop_Acquire: pMem %x port %d", pMem, port);

#ifdef _DEBUG
    index = Fg_getStatusEx(fg, NUMBER_OF_GRABBED_IMAGES, 1, port, pMem);
    writelog(INTERNAL_1_M, hgrabber, "Stop_Acquire: GRABBED_IMAGES: %d", index);
    index = Fg_getStatusEx(fg, NUMBER_OF_LOST_IMAGES, 1, port, pMem);
    writelog(INTERNAL_1_M, hgrabber, "Stop_Acquire:LOST_IMAGES: %d", index);
    index = Fg_getStatusEx(fg, NUMBER_OF_ACT_IMAGE, 1, port, pMem);
    writelog(INTERNAL_1_M, hgrabber, "Stop_Acquire: ACTUAL_IMAGE: %d", index);
    index = Fg_getStatusEx(fg, NUMBER_OF_LAST_IMAGE, 1, port, pMem);
    writelog(INTERNAL_1_M, hgrabber, "Stop_Acquire: LAST_IMAGE: %d", index);
    index = Fg_getStatusEx(fg, NUMBER_OF_NEXT_IMAGE, 1, port, pMem);
    writelog(INTERNAL_1_M, hgrabber, "Stop_Acquire: NEXT_IMAGE: %d", index);
    index = Fg_getStatusEx(fg, NUMBER_OF_IMAGES_IN_PROGRESS, 1, port, pMem);
    writelog(INTERNAL_1_M, hgrabber, "Stop_Acquire: IMAGES_IN_PROGRESS: %d", index);
#endif

    ret = Fg_stopAcquireEx(fg, port, pMem, STOP_ASYNC);
    if ((ret != FG_OK) && (ret != FG_TRANSFER_NOT_ACTIVE))
    {
        Fg_Error(fg);
        writelog(ERROR_M, hgrabber, "Stop_Acquire: Fg_stopAcquireEx failed");
        err = PCO_ERROR_DRIVER_SYSERR | PCO_ERROR_DRIVER_CAMERALINK;
    }
    writelog(PROCESS_M, hgrabber, "Stop_Acquire: Fg_stopAcquireEx return 0x%x", err);

    if (Fg_setStatusEx(fg, FG_UNBLOCK_ALL, nr_of_buffer, port, pMem) != FG_OK)
    {
        Fg_Error(fg);
        writelog(ERROR_M, hgrabber, "Stop_Acquire: setEx FG_UNBLOCK_ALL failed");
    }

    index = Fg_getStatusEx(fg, NUMBER_OF_BLOCKED_IMAGES, 1, port, pMem);
    writelog(INTERNAL_1_M, hgrabber, "Stop_Acquire: BLOCKED_IMAGES: %d", index);

    if (err == PCO_NOERROR)
        aquire_flag &= ~PCO_SC2_CL_STARTED;

    writelog(PROCESS_M, hgrabber, "Stop_Acquire: Fg_stopAcquire() and Free_Framebuffer done");
    return err;
}

BOOL CPco_grab_cl_me4::started()
{
    return aquire_flag & PCO_SC2_CL_STARTED;
}

DWORD CPco_grab_cl_me4::Wait_For_Next_Image(int *nr_of_pic, int timeout)
{
    int err;
    frameindex_t index = -1;

    err = PCO_NOERROR;

    //  void* pMem;
    dma_mem *pMem;
    if ((buf_manager & PCO_SC2_CL_EXTERNAL_BUFFER) == PCO_SC2_CL_EXTERNAL_BUFFER)
        pMem = pMem0;
    else if ((buf_manager & PCO_SC2_CL_INTERNAL_BUFFER) == PCO_SC2_CL_INTERNAL_BUFFER)
        pMem = pMemInt;
    else
    {
        writelog(ERROR_M, hgrabber, "Wait_for_image: buf_manager_flag  0x%x not supported", buf_manager);
        return PCO_ERROR_WRONGVALUE | PCO_ERROR_DRIVER | PCO_ERROR_DRIVER_CAMERALINK;
    }

    if (aquire_status == ACQ_STANDARD)
    {
        //wait for picture done, with blocking function
        index = Fg_getLastPicNumberBlockingEx(fg, *nr_of_pic, port, timeout, pMem);
    }

    if (index < 0)
    {
        Fg_Error(fg);
        writelog(ERROR_M, hgrabber, "Wait_for_image: timeout");
        err = PCO_ERROR_TIMEOUT | PCO_ERROR_DRIVER | PCO_ERROR_DRIVER_CAMERALINK;
        *nr_of_pic = 0;
    }
    else
        *nr_of_pic = (int)index;

    return err;
}

DWORD CPco_grab_cl_me4::Get_last_Image(int *nr_of_pic)
{
    int err;
    frameindex_t index = -1;

    err = PCO_NOERROR;

    //  void* pMem;
    dma_mem *pMem;
    if ((buf_manager & PCO_SC2_CL_EXTERNAL_BUFFER) == PCO_SC2_CL_EXTERNAL_BUFFER)
        pMem = pMem0;
    else if ((buf_manager & PCO_SC2_CL_INTERNAL_BUFFER) == PCO_SC2_CL_INTERNAL_BUFFER)
        pMem = pMemInt;
    else
    {
        writelog(ERROR_M, hgrabber, "Get_last_Image: buf_manager_flag  0x%x not supported", buf_manager);
        return PCO_ERROR_WRONGVALUE | PCO_ERROR_DRIVER | PCO_ERROR_DRIVER_CAMERALINK;
    }

    if (aquire_status == ACQ_STANDARD)
    {
        //wait for picture done, with blocking function
        index = Fg_getLastPicNumberEx(fg, port, pMem);
    }

    if (index < 0)
    {
        Fg_Error(fg);
        writelog(ERROR_M, hgrabber, "Get_last_Image: error");
        err = PCO_ERROR_TIMEOUT | PCO_ERROR_DRIVER | PCO_ERROR_DRIVER_CAMERALINK;
        *nr_of_pic = 0;
    }
    else
        *nr_of_pic = (int)index;

    return err;
}

DWORD CPco_grab_cl_me4::Get_Framebuffer_adr(int nr_of_pic, void **adr)
{
    int err;
    err = PCO_NOERROR;

    dma_mem *pMem;
    if ((buf_manager & PCO_SC2_CL_EXTERNAL_BUFFER) == PCO_SC2_CL_EXTERNAL_BUFFER)
        pMem = pMem0;
    else if ((buf_manager & PCO_SC2_CL_INTERNAL_BUFFER) == PCO_SC2_CL_INTERNAL_BUFFER)
        pMem = pMemInt;
    else
    {
        writelog(ERROR_M, hgrabber, "Get_image_buffer_adr: buf_manager_flag  0x%x not supported", buf_manager);
        return PCO_ERROR_WRONGVALUE | PCO_ERROR_DRIVER | PCO_ERROR_DRIVER_CAMERALINK;
    }

    *adr = (void *)Fg_getImagePtrEx(fg, nr_of_pic, port, pMem);
    return err;
}

DWORD CPco_grab_cl_me4::Check_DMA_Length(int num)
{
    size_t dmaLength;
    DWORD err = PCO_NOERROR;

    dma_mem *pMem;
    if ((buf_manager & PCO_SC2_CL_EXTERNAL_BUFFER) == PCO_SC2_CL_EXTERNAL_BUFFER)
        pMem = pMem0;
    else if ((buf_manager & PCO_SC2_CL_INTERNAL_BUFFER) == PCO_SC2_CL_INTERNAL_BUFFER)
        pMem = pMemInt;
    else
    {
        writelog(ERROR_M, hgrabber, "Check_DMA_Length: buf_manager_flag  0x%x not supported", buf_manager);
        return PCO_ERROR_WRONGVALUE | PCO_ERROR_DRIVER | PCO_ERROR_DRIVER_CAMERALINK;
    }

    if (pMem)
    {
        dmaLength = num;
        if (Fg_getParameterEx(fg, FG_TRANSFER_LEN, &dmaLength, port, pMem, num) != FG_OK)
            Fg_Error(fg);
    }
    else
    {
        dmaLength = num;
        if (Fg_getParameter(fg, FG_TRANSFER_LEN, &dmaLength, port) != FG_OK)
            Fg_Error(fg);
    }

    if (dmaLength < (size_t)act_dmalength)
    {
        writelog(ERROR_M, hgrabber, "dmaLenght %d != %d set error on this buffer", dmaLength, act_dmalength);
        err = (PCO_ERROR_DRIVER_BUFFER_DMASIZE | PCO_ERROR_DRIVER_CAMERALINK);
    }
    else
        writelog(PROCESS_M, hgrabber, "dmaLenght %d act_dmalength %d", dmaLength, act_dmalength);

    return err;
}

//-----------------------------------------------------------------//
// class CPco_grab_cl_me4_edge                                             //
//                                                                 //
// pco.edge in rolling shutter mode                                //
//                                                                 //
// start                                                           //
//-----------------------------------------------------------------//

DWORD CPco_grab_cl_me4_edge::Set_Grabber_Size(DWORD width, DWORD height)
{
    int w;
    writelog(PROCESS_M, hgrabber, "set_actual_size: start w:%d h:%d", width, height);

    if ((pco_hap_loaded == 0x02) || (pco_hap_loaded == 0x03))
    {
        int err;
        if ((act_height != height) || (act_width != width))
        {
            err = set_sccmos_size(width, height, width, height, 0, 0);

            if (err != PCO_NOERROR)
            {
                writelog(ERROR_M, hgrabber, "set_actual_size: set_sccmos_size error 0x%x", err);
                return err;
            }
            w = width * 2;
        }
    }
    else
    {
        if ((DataFormat & PCO_CL_DATAFORMAT_MASK) == PCO_CL_DATAFORMAT_4x16)
        {
            w = width * 2;
        }
        else if ((DataFormat & PCO_CL_DATAFORMAT_MASK) == PCO_CL_DATAFORMAT_5x16)
        {
            w = width * 2;
        }
        else if (((DataFormat & PCO_CL_DATAFORMAT_MASK) == PCO_CL_DATAFORMAT_5x12) || ((DataFormat & PCO_CL_DATAFORMAT_MASK) == PCO_CL_DATAFORMAT_5x12L) || ((DataFormat & PCO_CL_DATAFORMAT_MASK) == PCO_CL_DATAFORMAT_5x12R))
        {
            w = width * 2 * 12;
            w /= 16;
        }

        if (Fg_setParameter(fg, FG_WIDTH, &w, port) < 0)
        {
            Fg_Error(fg);
            writelog(ERROR_M, hgrabber, "set_actual_size: set FG_WIDTH failed");
            return PCO_ERROR_DRIVER_DATAERROR | PCO_ERROR_DRIVER_CAMERALINK;
        }

        if (Fg_setParameter(fg, FG_HEIGHT, &height, port) < 0)
        {
            Fg_Error(fg);
            writelog(ERROR_M, hgrabber, "set_actual_size: set FG_HEIGHT failed");
            return PCO_ERROR_DRIVER_DATAERROR | PCO_ERROR_DRIVER_CAMERALINK;
        }
    }
    writelog(PROCESS_M, hgrabber, "set_actual_size: done w:%d h:%d", width, height);
    act_width = width;
    act_height = height;

    act_dmalength = w * height;

    return PCO_NOERROR;
}

DWORD CPco_grab_cl_me4_edge::Open_Grabber(int board)
{
    int err = PCO_NOERROR;

    if (fg != NULL)
    {
        writelog(INIT_M, (PCO_HANDLE)1, "Open_Grabber: grabber was opened before");
        return PCO_NOERROR;
    }

    int num, type;

    //reset this settings
    me_boardnr = -1;
    port = PORT_A;

    //scan for me4_board and adjust me4_boardnr
    num = 0;
    for (int i = 0; i < 4; i++)
    {
        type = Fg_getBoardType(i);
        if (type < 0)
            break;
        if ((type == 0xA41) || (type == 0xA44))
        {
            if (((num * 2) == board) || ((num * 2) + 1 == board))
            {
                me_boardnr = i;
                break;
            }
            num++;
        }
    }

    if (me_boardnr < 0)
    {
        writelog(ERROR_M, hgrabber, "open_grabber: driver supports only up to 4 boards");

        return PCO_ERROR_DRIVER_NODRIVER | PCO_ERROR_DRIVER_CAMERALINK;
    }

    hgrabber = (PCO_HANDLE)(0x1000 + board);

    char buf[MAX_PATH];
    //  int hFile;
    //  char *sisodir;

    memset(buf, 0, MAX_PATH);

    strcpy_s(buf, sizeof(buf), "libFullAreaGray8_HS.so");
    pco_hap_loaded = 0;

    writelog(INIT_M, hgrabber, "open_grabber: Fg_Init(%s,%d)", buf, me_boardnr);
    if ((fg = Fg_Init(buf, me_boardnr)) == NULL)
    {
        err = Fg_getLastErrorNumber(NULL);
        writelog(ERROR_M, hgrabber, "open_grabber: Fg_Init(%s,%d) failed with err %d", buf, me_boardnr, err);
        if (err == FG_NO_VALID_LICENSE)
            writelog(ERROR_M, hgrabber, "open_grabber: Missing license for this mode");
        Fg_Error(fg);
        hgrabber = NULL;
        me_boardnr = -1;
        return PCO_ERROR_DRIVER_NODRIVER | PCO_ERROR_DRIVER_CAMERALINK;
    }

    DataFormat = SCCMOS_FORMAT_TOP_CENTER_BOTTOM_CENTER | PCO_CL_DATAFORMAT_5x16;

    err = PCO_NOERROR;
    if (Fg_getParameter(fg, FG_CAMSTATUS, &num, port) < 0)
    {
        Fg_Error(fg);
        writelog(ERROR_M, hgrabber, "open_grabber: Fg_getParameter(,FG_CAMSTATUS,...) failed");
        err = PCO_ERROR_DRIVER_DATAERROR | PCO_ERROR_DRIVER_CAMERALINK;
    }

    if ((num == 0) && (err == PCO_NOERROR))
    {
        writelog(ERROR_M, hgrabber, "open_grabber: Fg_getParameter(,FG_CAMSTATUS,...) no camera connected");
        err = PCO_ERROR_DRIVER_NOTINIT | PCO_ERROR_DRIVER_CAMERALINK;
    }
    int val, Id;

    if (err == PCO_NOERROR)
    {
        if (pco_hap_loaded == 0x03)
        {
            val = PCO_SC2_CL_ME4_PCO_APP_TAP10;
            Id = Fg_getParameterIdByName(fg, "Device1_Process0_Camera_FullMode");
            err = Fg_setParameterWithType(fg, Id, &val, 0, FG_PARAM_TYPE_INT32_T);
            if (err)
                err = PCO_ERROR_DRIVER_DATAERROR | PCO_ERROR_DRIVER_CAMERALINK;
        }
        else
        {
            val = FG_CL_8BIT_FULL_10;
            if (Fg_setParameter(fg, FG_CAMERA_LINK_CAMTYP, &val, port) < 0)
            {
                Fg_Error(fg);
                writelog(ERROR_M, hgrabber, "open_grabber: FG_CAMERA_LINK_CAMTYP failed");
                err = PCO_ERROR_DRIVER_DATAERROR | PCO_ERROR_DRIVER_CAMERALINK;
            }
            if (err == PCO_NOERROR)
            {
                val = FG_GRAY;
                if (Fg_setParameter(fg, FG_FORMAT, &val, port) < 0)
                {
                    Fg_Error(fg);
                    writelog(ERROR_M, hgrabber, "open_grabber: FG_FORMAT failed");
                    err = PCO_ERROR_DRIVER_DATAERROR | PCO_ERROR_DRIVER_CAMERALINK;
                }
            }

            if (err == PCO_NOERROR)
            {
                val = FREE_RUN;
                if (Fg_setParameter(fg, FG_TRIGGERMODE, &val, port) < 0)
                {
                    Fg_Error(fg);
                    writelog(ERROR_M, hgrabber, "init_grabber: set FG_TRIGGERMODE failed");
                    err = PCO_ERROR_DRIVER_DATAERROR | PCO_ERROR_DRIVER_CAMERALINK;
                }
            }
        }
    }

    if (err == PCO_NOERROR)
    {
        val = 0xFFFFFFF; //(PCO_SC2_IMAGE_TIMEOUT_L*10)/1000;
        if (Fg_setParameter(fg, FG_TIMEOUT, &val, port) < 0)
        {
            Fg_Error(fg);
            writelog(ERROR_M, hgrabber, "init_grabber: set FG_TIMEOUT failed");
            err = PCO_ERROR_DRIVER_DATAERROR | PCO_ERROR_DRIVER_CAMERALINK;
        }
    }

    //set default sizes
    if (err == PCO_NOERROR)
        err = Set_Grabber_Size(2560, 2160);

    if (err != PCO_NOERROR)
    {
        Fg_FreeGrabber(fg);
        fg = NULL;
        me_boardnr = -1;
        hgrabber = NULL;
    }

    buf_manager = PCO_SC2_CL_INTERNAL_BUFFER;

    return err;
}

int CPco_grab_cl_me4_edge::set_sccmos_size(int width, int height, int xlen, int ylen, int xoff, int yoff)
{
    int rcode, nError;
    int Idxoff1, Idxlen1, Idyoff1, Idylen1;
    int Idxoff2, Idxlen2, Idyoff2, Idylen2;
    int IdNum1, IdNum2, IdApp;
    int i, k;
    int ypos[4096];
    int granularity;
    unsigned int Value;

    granularity = 4;
    if ((DataFormat & PCO_CL_DATAFORMAT_MASK) == PCO_CL_DATAFORMAT_4x16)
        granularity = 4;
    else if ((DataFormat & PCO_CL_DATAFORMAT_MASK) == PCO_CL_DATAFORMAT_5x16)
        granularity = 5;
    else if ((DataFormat & PCO_CL_DATAFORMAT_MASK) == PCO_CL_DATAFORMAT_5x12)
    {
        granularity = 5;
        width *= 12;
        xlen = width / 16;
        if (width % 16) //sollte nicht vorkommen
            xlen++;
    }

    if ((xoff < 0) || (xoff > 4095) || ((xoff % granularity) != 0) || (xlen < granularity) || (xlen > 4095) || ((xlen % granularity) != 0) || ((xoff + xlen) > 4096) || (yoff < 0) || (yoff > 4095) || (ylen < 1) || (ylen > 4096) || ((yoff + ylen) > 4096))
    {
        return PCO_ERROR_DRIVER_DATAERROR | PCO_ERROR_DRIVER_CAMERALINK;
    }

    Idxoff1 = Fg_getParameterIdByName(fg, "Device1_Process0_DRAM1_XOffset");
    Idxlen1 = Fg_getParameterIdByName(fg, "Device1_Process0_DRAM1_XLength");
    Idyoff1 = Fg_getParameterIdByName(fg, "Device1_Process0_DRAM1_YOffset");
    Idylen1 = Fg_getParameterIdByName(fg, "Device1_Process0_DRAM1_YLength");

    Idxoff2 = Fg_getParameterIdByName(fg, "Device1_Process0_DRAM2_XOffset");
    Idxlen2 = Fg_getParameterIdByName(fg, "Device1_Process0_DRAM2_XLength");
    Idyoff2 = Fg_getParameterIdByName(fg, "Device1_Process0_DRAM2_YOffset");
    Idylen2 = Fg_getParameterIdByName(fg, "Device1_Process0_DRAM2_YLength");

    IdNum1 = Fg_getParameterIdByName(fg, "Device1_Process0_DRAM1_NumRoI");
    IdNum2 = Fg_getParameterIdByName(fg, "Device1_Process0_DRAM2_NumRoI");
    IdApp = Fg_getParameterIdByName(fg, "Device1_Process0_ImageHeight_AppendNumber");

    for (i = 0; i < 4096; i++)
    {
        switch (DataFormat & SCCMOS_FORMAT_MASK)
        {
        case SCCMOS_FORMAT_TOP_CENTER_BOTTOM_CENTER: //MODE_A
        {
            k = (yoff + i) % 4096;
            if (k < (height / 2))
            {
                ypos[i] = 2 * k;
            }
            else
            {
                ypos[i] = 2 * (height - k) - 1;
            }
            if (k >= ylen)
                ypos[i] = 0;
        }
        break;

        case SCCMOS_FORMAT_CENTER_TOP_CENTER_BOTTOM: //MODE_B
        {
            k = (yoff + i) % 4096;
            if (k < (height / 2))
            {
                ypos[i] = 2 * ((height / 2) - k - 1);
            }
            else
            {
                ypos[i] = 2 * (k - (height / 2)) + 1;
            }
            if (k >= ylen)
                ypos[i] = 0;
        }
        break;

        case SCCMOS_FORMAT_CENTER_TOP_BOTTOM_CENTER: //MODE_C:
        {
            k = (yoff + i) % 4096;
            if (k < (height / 2))
            {
                ypos[i] = 2 * ((height / 2) - k - 1);
            }
            else
            {
                ypos[i] = 2 * (height - k) - 1;
            }
            if (k >= ylen)
                ypos[i] = 0;
        }
        break;

        case SCCMOS_FORMAT_TOP_CENTER_CENTER_BOTTOM: //MODE_D:
        {
            k = (yoff + i) % 4096;
            if (k < (height / 2))
            {
                ypos[i] = 2 * k;
            }
            else
            {
                ypos[i] = 2 * (k - (height / 2)) + 1;
            }
            if (k >= ylen)
                ypos[i] = 0;
        }
        break;

        default:
            return PCO_ERROR_DRIVER_DATAERROR | PCO_ERROR_DRIVER_CAMERALINK;
        }
    }

    int32_t *Idxoff_values = new int32_t[ylen];
    int32_t *Idxlen_values = new int32_t[ylen];
    int32_t *Idyoff_values = new int32_t[ylen];
    int32_t *Idylen_values = new int32_t[ylen];

    nError = 0;
    for (i = 0; i < ylen; i++)
    {
        Idxoff_values[i] = xoff;
        Idxlen_values[i] = xlen;
        Idyoff_values[i] = ypos[i];
        Idylen_values[i] = 1;
    }

    FieldParameterAccess writeParameter;
    writeParameter.vtype = FG_PARAM_TYPE_INT32_T;
    writeParameter.index = 0;
    writeParameter.count = ylen;

    writeParameter.p_int32_t = Idxoff_values;
    rcode = Fg_setParameterWithType(fg, Idxoff1, &writeParameter, 0, FG_PARAM_TYPE_STRUCT_FIELDPARAMACCESS);
    if (rcode)
        nError++;

    rcode = Fg_setParameterWithType(fg, Idxoff2, &writeParameter, 0, FG_PARAM_TYPE_STRUCT_FIELDPARAMACCESS);
    if (rcode)
        nError++;

    writeParameter.p_int32_t = Idxlen_values;
    rcode = Fg_setParameterWithType(fg, Idxlen1, &writeParameter, 0, FG_PARAM_TYPE_STRUCT_FIELDPARAMACCESS);
    if (rcode)
        nError++;

    rcode = Fg_setParameterWithType(fg, Idxlen2, &writeParameter, 0, FG_PARAM_TYPE_STRUCT_FIELDPARAMACCESS);
    if (rcode)
        nError++;

    writeParameter.p_int32_t = Idyoff_values;
    rcode = Fg_setParameterWithType(fg, Idyoff1, &writeParameter, 0, FG_PARAM_TYPE_STRUCT_FIELDPARAMACCESS);
    if (rcode)
        nError++;

    rcode = Fg_setParameterWithType(fg, Idyoff2, &writeParameter, 0, FG_PARAM_TYPE_STRUCT_FIELDPARAMACCESS);
    if (rcode)
        nError++;

    writeParameter.p_int32_t = Idylen_values;
    rcode = Fg_setParameterWithType(fg, Idylen1, &writeParameter, 0, FG_PARAM_TYPE_STRUCT_FIELDPARAMACCESS);
    if (rcode)
        nError++;

    rcode = Fg_setParameterWithType(fg, Idylen2, &writeParameter, 0, FG_PARAM_TYPE_STRUCT_FIELDPARAMACCESS);
    if (rcode)
        nError++;

    delete[] Idxoff_values;
    delete[] Idxlen_values;
    delete[] Idyoff_values;
    delete[] Idylen_values;

    Value = ylen;

    rcode = Fg_setParameterWithType(fg, IdNum1, &Value, 0, FG_PARAM_TYPE_UINT32_T);
    if (rcode)
        nError++;

    rcode = Fg_setParameterWithType(fg, IdNum2, &Value, 0, FG_PARAM_TYPE_UINT32_T);
    if (rcode)
        nError++;

    rcode = Fg_setParameterWithType(fg, IdApp, &Value, 0, FG_PARAM_TYPE_UINT32_T);
    if (rcode)
        nError++;

    if (nError)
    {
        return PCO_ERROR_DRIVER_DATAERROR | PCO_ERROR_DRIVER_CAMERALINK;
    }

    return PCO_NOERROR;
}

DWORD CPco_grab_cl_me4_edge::Check_DRAM_Status(char *mess, int mess_size, int *fill)
{
    unsigned int Status1, Status2, Status3, Status4;
    int rcode;
    int Id1, Id2, Id3, Id4;

    if (pco_hap_loaded != 0x03)
    {
        *fill = 0;
        return PCO_NOERROR;
    }

    Id1 = Fg_getParameterIdByName(fg, "Device1_Process0_DRAM1_Overflow");
    Id2 = Fg_getParameterIdByName(fg, "Device1_Process0_DRAM2_Overflow");

    rcode = Fg_getParameter(fg, Id1, &Status1, FG_PARAM_TYPE_UINT32_T);
    if (rcode)
    {
        if (mess)
            sprintf_s(mess, mess_size, "\nError read Status1 from board\n");
        return PCO_ERROR_DRIVER_DATAERROR | PCO_ERROR_DRIVER_CAMERALINK;
    }
    rcode = Fg_getParameter(fg, Id2, &Status2, FG_PARAM_TYPE_UINT32_T);
    if (rcode)
    {
        if (mess)
            sprintf_s(mess, mess_size, "\nError read Status2 from board\n");
        return PCO_ERROR_DRIVER_DATAERROR | PCO_ERROR_DRIVER_CAMERALINK;
    }

    Id3 = Fg_getParameterIdByName(fg, "Device1_Process0_DRAM1_FillLevel");
    rcode = Fg_getParameter(fg, Id3, &Status3, FG_PARAM_TYPE_UINT32_T);
    if (rcode)
    {
        if (mess)
            sprintf_s(mess, mess_size, "\nError read Status3 from board\n");
        return PCO_ERROR_DRIVER_DATAERROR | PCO_ERROR_DRIVER_CAMERALINK;
    }
    *fill = Status3;

    if (Status1 || Status2)
    {
        Id4 = Fg_getParameterIdByName(fg, "Device1_Process0_DRAM2_FillLevel");
        rcode = Fg_getParameter(fg, Id3, &Status3, FG_PARAM_TYPE_UINT32_T);
        if (rcode)
        {
            if (mess)
                sprintf_s(mess, mess_size, "\nError read Status3 from board\n");
            return PCO_ERROR_DRIVER_DATAERROR | PCO_ERROR_DRIVER_CAMERALINK;
        }

        rcode = Fg_getParameter(fg, Id4, &Status4, FG_PARAM_TYPE_UINT32_T);
        if (rcode)
        {
            if (mess)
                sprintf_s(mess, mess_size, "\nError read Status4 from board\n");
            return PCO_ERROR_DRIVER_DATAERROR | PCO_ERROR_DRIVER_CAMERALINK;
        }

        if (mess)
            sprintf_s(mess, mess_size, "\nOverflow detected: DRAM0 (%d - %d percent) DRAM1 (%d - %d percent)\n",
                      Status1, Status3, Status2, Status4);
        return PCO_ERROR_DRIVER_DATAERROR | PCO_ERROR_DRIVER_CAMERALINK;
    }

    return PCO_NOERROR;
}

void CPco_grab_cl_me4_edge::Extract_Image(void *bufout, void *bufin, int width, int height)
{

    if (pco_hap_loaded == 0x03)
    {
        int x, y, off;
        //  DWORD *picadr_in;
        //  DWORD *picadr_out;
        DWORD *lineadr_in;
        DWORD *lineadr_out;
        DWORD a;

        //  picadr_in=(DWORD *)bufadr;
        //  picadr_out=(DWORD *)bufent->bufadr;
        off = (width * 12) / 32;
        lineadr_in = (DWORD *)bufin;   //picadr_in;//+y*off;
        lineadr_out = (DWORD *)bufout; //picadr_out;//+y*entry->act_width;

        for (y = 0; y < height; y++)
        {
            //          lineadr_in=picadr_in+y*off;
            //          lineadr_out=picadr_out+y*entry->act_width;
            for (x = 0; x < off;)
            {
                a = (*lineadr_in & 0x0000FFF0) >> 4;
                a |= (*lineadr_in & 0x0000000F) << 24;
                a |= (*lineadr_in & 0xFF000000) >> 8;
                *lineadr_out = a;
                lineadr_out++;

                a = (*lineadr_in & 0x00FF0000) >> 12;
                lineadr_in++;
                x++;
                a |= (*lineadr_in & 0x0000F000) >> 12;
                a |= (*lineadr_in & 0x00000FFF) << 16;
                *lineadr_out = a;
                lineadr_out++;
                a = (*lineadr_in & 0xFFF00000) >> 20;
                a |= (*lineadr_in & 0x000F0000) << 8;
                lineadr_in++;
                x++;
                a |= (*lineadr_in & 0x0000FF00) << 8;
                *lineadr_out = a;
                lineadr_out++;
                a = (*lineadr_in & 0x000000FF) << 4;
                a |= (*lineadr_in & 0xF0000000) >> 28;
                a |= (*lineadr_in & 0x0FFF0000);
                *lineadr_out = a;
                lineadr_out++;
                lineadr_in++;
                x++;
            }
        }
    }
    else
        reorder_image(bufout, bufin, width, height, DataFormat);
}

void CPco_grab_cl_me4_edge::Get_Image_Line(void *bufout, void *bufin, int linenumber, int width, int height)
{
    if (pco_hap_loaded == 0x03)
    {
        WORD *buf;
        buf = (WORD *)bufin;
        buf += (linenumber - 1) * width;
        memcpy(bufout, buf, width * sizeof(WORD));
    }
    else
        get_image_line(bufout, bufin, linenumber, width, height, DataFormat);
}

DWORD CPco_grab_cl_me4_edge::PostArm(int userset)
{
    return PCO_NOERROR;
}

DWORD CPco_grab_cl_me4_camera::PostArm(int userset)
{
    return PCO_NOERROR;
}

//-----------------------------------------------------------------//
// class CPco_grab_cl_me4_edge                                             //
//                                                                 //
// end                                                             //
//-----------------------------------------------------------------//

//-----------------------------------------------------------------//
// class CPco_grab_cl_me4_camera                                           //
//                                                                 //
// pco.1600, pco.2000, pco.4000, pco.dimax                         //
//                                                                 //
// start                                                           //
//-----------------------------------------------------------------//

DWORD CPco_grab_cl_me4_camera::Set_Grabber_Size(DWORD width, DWORD height)
{
    writelog(PROCESS_M, hgrabber, "set_actual_size: start w:%d h:%d", width, height);

    if (Fg_setParameter(fg, FG_WIDTH, &width, port) < 0)
    {
        Fg_Error(fg);
        writelog(ERROR_M, hgrabber, "set_actual_size: set FG_WIDTH failed");
        return PCO_ERROR_DRIVER_DATAERROR | PCO_ERROR_DRIVER_CAMERALINK;
    }

    if (Fg_setParameter(fg, FG_HEIGHT, &height, port) < 0)
    {
        Fg_Error(fg);
        writelog(ERROR_M, hgrabber, "set_actual_size: set FG_HEIGHT failed");
        return PCO_ERROR_DRIVER_DATAERROR | PCO_ERROR_DRIVER_CAMERALINK;
    }

    writelog(PROCESS_M, hgrabber, "set_actual_size: done w:%d h:%d", width, height);
    act_width = width;
    act_height = height;

    act_dmalength = width * height * sizeof(WORD);

    return PCO_NOERROR;
}

DWORD CPco_grab_cl_me4_camera::Open_Grabber(int board)
{
    int err = PCO_NOERROR;

    if (fg != NULL)
    {
        writelog(INIT_M, (PCO_HANDLE)1, "Open_Grabber: grabber was opened before");
        return PCO_NOERROR;
    }

    int num, type;

    //reset this settings
    me_boardnr = -1;
    port = PORT_A;

    //scan for me4_board and adjust me4_boardnr
    num = 0;
    for (int i = 0; i < 4; i++)
    {
        type = Fg_getBoardType(i);
        if (type < 0)
            break;
        if ((type == 0xA41) || (type == 0xA44))
        {
            if (((num * 2) == board) || ((num * 2) + 1 == board))
            {
                me_boardnr = i;
                break;
            }
            num++;
        }
    }

    if (me_boardnr < 0)
    {
        writelog(ERROR_M, hgrabber, "open_grabber: driver supports only up to 4 boards");

        return PCO_ERROR_DRIVER_NODRIVER | PCO_ERROR_DRIVER_CAMERALINK;
    }

    hgrabber = (PCO_HANDLE)(0x1000 + board);

    char buf[40];
    memset(buf, 0, 40);

    strcpy_s(buf, sizeof(buf), "libDualAreaGray16.so");
    writelog(INIT_M, hgrabber, "open_grabber: Fg_Init(%s,%d)", buf, me_boardnr);
    if ((fg = Fg_Init(buf, me_boardnr)) == NULL)
    {
        writelog(ERROR_M, hgrabber, "open_grabber: Fg_Init(%s,%d) failed", buf, me_boardnr);
        if (Fg_getLastErrorNumber(NULL) == FG_NO_VALID_LICENSE)
            writelog(ERROR_M, hgrabber, "open_grabber: Missing license for this mode");
        Fg_Error(fg);
        hgrabber = NULL;
        me_boardnr = -1;
        return PCO_ERROR_DRIVER_NODRIVER | PCO_ERROR_DRIVER_CAMERALINK;
    }

    err = PCO_NOERROR;
    if (Fg_getParameter(fg, FG_CAMSTATUS, &num, port) < 0)
    {
        Fg_Error(fg);
        writelog(ERROR_M, hgrabber, "open_grabber: Fg_getParameter(,FG_CAMSTATUS,...) failed");
        err = PCO_ERROR_DRIVER_DATAERROR | PCO_ERROR_DRIVER_CAMERALINK;
    }

    if ((num == 0) && (err == PCO_NOERROR))
    {
        writelog(ERROR_M, hgrabber, "open_grabber: Fg_getParameter(,FG_CAMSTATUS,...) no camera connected");
        err = PCO_ERROR_DRIVER_NOTINIT | PCO_ERROR_DRIVER_CAMERALINK;
    }
    int val;

    if (err == PCO_NOERROR)
    {
        val = 0;
        if (Fg_setParameter(fg, FG_XOFFSET, &val, port) < 0)
        {
            Fg_Error(fg);
            writelog(ERROR_M, hgrabber, "init_grabber: set FG_XOFFSET failed");
            err = PCO_ERROR_DRIVER_DATAERROR | PCO_ERROR_DRIVER_CAMERALINK;
        }
    }
    if (err == PCO_NOERROR)
    {
        val = 0;
        if (Fg_setParameter(fg, FG_YOFFSET, &val, port) < 0)
        {
            Fg_Error(fg);
            writelog(ERROR_M, hgrabber, "init_grabber: set FG_YOFFSET failed");
            err = PCO_ERROR_DRIVER_DATAERROR | PCO_ERROR_DRIVER_CAMERALINK;
        }
    }

    if (err == PCO_NOERROR)
    {
        val = FG_CL_SINGLETAP_16_BIT;
        if (Fg_setParameter(fg, FG_CAMERA_LINK_CAMTYP, &val, port) < 0)
        {
            Fg_Error(fg);
            writelog(ERROR_M, hgrabber, "set_grabber_par: FG_CAMERA_LINK_CAMTYP failed");
            err = PCO_ERROR_DRIVER_DATAERROR | PCO_ERROR_DRIVER_CAMERALINK;
        }
    }
    if (err == PCO_NOERROR)
    {
        val = FG_GRAY16;
        if (Fg_setParameter(fg, FG_FORMAT, &val, port) < 0)
        {
            Fg_Error(fg);
            writelog(ERROR_M, hgrabber, "set_grabber_par: FG_FORMAT failed");
            err = PCO_ERROR_DRIVER_DATAERROR | PCO_ERROR_DRIVER_CAMERALINK;
        }
    }

    if (err == PCO_NOERROR)
    {
        val = FREE_RUN;
        if (Fg_setParameter(fg, FG_TRIGGERMODE, &val, port) < 0)
        {
            Fg_Error(fg);
            writelog(ERROR_M, hgrabber, "init_grabber: set FG_TRIGGERMODE failed");
            err = PCO_ERROR_DRIVER_DATAERROR | PCO_ERROR_DRIVER_CAMERALINK;
        }
    }

    if (err == PCO_NOERROR)
    {
        val = 0xFFFFFFF; //(PCO_SC2_IMAGE_TIMEOUT_L*10)/1000;
        if (Fg_setParameter(fg, FG_TIMEOUT, &val, port) < 0)
        {
            Fg_Error(fg);
            writelog(ERROR_M, hgrabber, "init_grabber: set FG_TIMEOUT failed");
            err = PCO_ERROR_DRIVER_DATAERROR | PCO_ERROR_DRIVER_CAMERALINK;
        }
    }

    DataFormat = PCO_CL_DATAFORMAT_1x16;

    //set default sizes
    if (err == PCO_NOERROR)
        err = Set_Grabber_Size(1280, 1024);

    if (err != PCO_NOERROR)
    {
        Fg_FreeGrabber(fg);
        fg = NULL;
        me_boardnr = -1;
        hgrabber = NULL;
    }

    buf_manager = PCO_SC2_CL_INTERNAL_BUFFER;

    return err;
}

void CPco_grab_cl_me4_camera::Extract_Image(void *bufout, void *bufin, int width, int height)
{
    memcpy(bufout, bufin, width * height * sizeof(WORD));
}

void CPco_grab_cl_me4_camera::Get_Image_Line(void *bufout, void *bufin, int linenumber, int width, int height)
{
    WORD *buf;
    buf = (WORD *)bufin;
    buf += (linenumber - 1) * width;

    memcpy(bufout, buf, width * sizeof(WORD));
}
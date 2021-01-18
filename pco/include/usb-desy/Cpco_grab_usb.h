//-----------------------------------------------------------------//
// Name        | CPco_grab_usb.h             | Type: ( ) source    //
//-------------------------------------------|       (*) header    //
// Project     | pco.camera                  |       ( ) others    //
//-----------------------------------------------------------------//
// Platform    | Linux, WINDOWS 2000/XP                            //
//-----------------------------------------------------------------//
// Environment | Microsoft Visual C++ 6.0                          //
//             |                                                   //
//-----------------------------------------------------------------//
// Purpose     | pco.camera - USB API                              //
//-----------------------------------------------------------------//
// Author      | MBL, PCO AG                                       //
//-----------------------------------------------------------------//
// Revision    | rev. 1.01 rel. 0.00                               //
//-----------------------------------------------------------------//
// Notes       | In this file are all functions and definitions,   //
//             | for grabbing from usb camera                      //
//             |                                                   //
//             |                                                   //
//-----------------------------------------------------------------//
// (c) 2014 PCO AG * Donaupark 11 *                                //
// D-93309      Kelheim / Germany * Phone: +49 (0)9441 / 2005-0 *  //
// Fax: +49 (0)9441 / 2005-20 * Email: info@pco.de                 //
//-----------------------------------------------------------------//

#ifndef CPCO_GRAB_USB_H
#define CPCO_GRAB_USB_H

#include <vector>
#include "pco_includes.h"
#include <libusb-1.0/libusb.h>
#include "Cpco_com_usb.h"

#define ACQUIRE_RUN 0x01

/* 1 MB max size */
#define LARGE_PACKET_SIZE       65536*16

#define ASYNC_PACKET_SIZE       65536*16

#define ASYNC_ALLOC_1           0x0001
#define ASYNC_SETUP_1           0x0002
#define ASYNC_PENDING_1         0x0004

#define ASYNC_ALLOC_2           0x0010
#define ASYNC_SETUP_2           0x0020
#define ASYNC_PENDING_2         0x0040

#define ASYNC_NEED_LAST         0x0100
#define ASYNC_USE_COPY_BUFFER   0x0200

#define ASYNC_ERROR             0x1000

/// \cond


/// 
/// \brief Parameter struct for the pixelfly decoding threads
/// \anchor GL_CODING_PARAMS
/// This structure is used to pass parameters to a pixelfly usb decoding thread.
/// \param buffer Pointer to buffer that holds the (part of) the image.
/// \param len Length of the buffer
/// \param align MSB/LSB alignment
///
typedef struct
{
    void* buffer;
    int len;
    WORD align;
} GL_CODING_PARAMS;

#ifdef NEED_GL_CODING
//to avoid including that function several times when it isn't needed

///
/// \brief Threaded decoding for pixelfly usb
/// \anchor *GL_CodingpThread
/// This is the thread used to decode pixelfly usb data. This is used by the synchronous and asynchronous acquire functions to save time, although the synchronous function
/// waits for the threads to finish, while the asynchronous function only waits when a specific flag is set (to increase transfer speed).
/// \see Acquire_Image_Async_wait
/// \see Acquire_Image
/// \param GL_CODING_PARAMS structure
/// \return null
///
void *GL_CodingpThread(void* param);
#endif


/// \endcond



///
/// \brief The CPco_grab_usb class
///
/// This is the grabber class for USB. It it responsible for image transfers by allocating and submitting USB transfers. The
/// images are split up into smaller packets for performance reasons and/or other limitations. Libusb-1.0 is used for the transfers.
/// The CPco_grab_usb class provides Blocking and Non-Blocking functions for image transfers.
///
/// Some cameras use special coding for the transferred image data, which is queried from the camera during \ref Open_Grabber call. 
/// Decoding of the image data is done during the USB transfers in a special thread, which is setup for each image.
/// Therefore, when using the Non-Blocking functions for image transfers, only the last transfer should call one of the Blocking Asynch_wait functions.
/// All Blocking function always wait until decoding is finished.   
///
///\nosubgrouping

class CPco_grab_usb
{

public:


private:
    ///
    /// \brief Variable to hold the current coding setting. This should be set once at the beginning and, if possible, not get changed later. If you change this, remember to change it in ALL class instances!
    ///
    bool gl_Coding;
    ///
    /// \brief Variable to hold the alignment setting (MSB/LSB).
    ///
    WORD act_align;
    std::vector<pthread_t*> gl_codingthreads;

protected:
    PCO_HANDLE hgrabber;
    CPco_com_usb *cam;
    CPco_Log *clog;

    libusb_context           *libusb_ctx;
    libusb_device_handle     *libusb_hdev;

    int ImageInAddr;
    int ImageInMaxPacketSize;
    int ImageTimeout;
    int packet_timeout;

    unsigned char* last_buffer_1;
    unsigned char* last_buffer_2;
    libusb_transfer **async_transfer_1;
    libusb_transfer **async_transfer_2;
    int async_transfer_status;
    int async_transfer_num;
    int async_bytes_done;
    int async_packets_done;

    int async_transfer_size;
    int async_last_size;
    int async_copy_buffer_size;
    unsigned char* async_buffer_adr;

    WORD bitpix;
    int act_width,act_height;
    int act_line_width;
    int act_dmalength;

    int DataFormat;
    PCO_USB_TRANSFER_PARAM usbpar;
    SC2_Camera_Description_Response description;

    WORD  camtype;
    DWORD serialnumber;
    DWORD cam_pixelrate;
    WORD cam_timestampmode;
    WORD cam_doublemode;
    WORD cam_align;
    WORD cam_noisefilter;
    WORD cam_colorsensor;
    DWORD cam_width,cam_height;

public:
    ///
    /// Constructor for the class. It is possible (though not very useful) to create a class object without passing it a camera class object pointer as parameter.
    /// \anchor CPco_grab_usb
    /// \param camera A CPco_com_usb object with a previously opened pco.camera
    /// \return
    ///
    CPco_grab_usb(CPco_com_usb *camera=NULL);
    ~CPco_grab_usb();

    ///
    /// \brief Opens the grabber and retrieves the neccessary variables from the camera object.
    /// \anchor Open_Grabber
    /// \param board Set to zero if there is only one camera connected.
    /// Open_Cam() on the appropriate class object \b must be called first or this will fail!
    /// \return Error or 0 in case of success
    ///
    DWORD Open_Grabber(int board);

    ///
    /// \brief Opens the grabber and retrieves the neccessary variables from the camera object.
    /// \param board Set to zero if there is only one camera connected.
    /// Open_Cam() on the appropriate class object \b must be called first or this will fail!
    /// \return Error or 0 in case of success
    ///
    DWORD Open_Grabber(int board,int initmode ATTRIBUTE_UNUSED);

    ///
    /// Closes the grabber. This should be done before calling Close_Cam().
    /// \anchor Close_Grabber
    /// \return Error or 0 in case of success
    ///
    DWORD Close_Grabber();

    ///
    /// \brief Check if grabber is opened..
    /// \return openstatus
    /// \retval TRUE grabber is opened
    /// \retval FALSE grabber is closed
    ///
    BOOL IsOpen();

    ///
    /// \brief Sets the logging behaviour for the grabber class.
    /// \n
    /// If this function is not called no logging is performed.
    /// Logging might be useful to follow the program flow of the application.
    /// \param elog Pointer to a CPco_Log logging class object
    /// \return
    ///
    void SetLog(CPco_Log *elog);


    /// @name Image Acquire Functions
    ///
    /// These functions are used to acquire images from the camera.
    /// Blocking functions do not return until grab is finished 
    /// Non-Blocking functions do not return until grab is finished 
    ///
    
    /// \brief Transfers an image from the camera. (Blocking)
    /// \n
    /// This is a synchronous transfer and should be used in time-insensitive cases. (e.g. LiveView)
    /// \n Internal timeout setting is used, see \ref Set_Grabber_Timeout.
    /// \param adr Pointer to address where the image gets stored
    /// \return Error or 0 in case of success
    ///
    DWORD Acquire_Image(void *adr);

    ///
    /// \brief Transfers an image from the camera. (Blocking) \n
    /// This is a synchronous transfer and should be used in time-insensitive cases. (e.g. LiveView)
    /// \n Custom timeout setting is used.
    /// \param adr Pointer to address where the image gets stored
    /// \param timeout Sets a custom timeout
    /// \return Error or 0 in case of success
    ///
    DWORD Acquire_Image(void *adr,int timeout);

    ///
    /// \brief Transfers an image from the recorder buffer of the camera. (Blocking)
    /// \n
    /// This is a synchronous transfer and should be used in time-insensitive cases.
    /// \n Internal timeout of one second is used.
    /// \param adr Pointer to address where the image gets stored
    /// \param Segment select segment of recorder buffer
    /// \param ImageNr select image in recorder buffer
    /// \return Error or 0 in case of success
    ///
    DWORD Get_Image(WORD Segment,DWORD ImageNr,void *adr);

    ///
    /// \brief Setup an image transfer. (Non-Blocking)
    /// \n
    /// Prepare all neccessary data structures to start a (image)-data transfer from the camera over the usb-bus
    /// and return immediately.  
    /// Transfer is cancelled if no image data arrive before timeout run out.
    /// \n Internal timeout setting is used, see \ref Set_Grabber_Timeout.
    /// \param adr Pointer to address where the image gets stored
    /// \return Error or 0 in case of success
    ///
    DWORD Acquire_Image_Async(void *adr);

    ///
    /// \brief Setup an image transfer. (Non-Blocking)
    /// \n
    /// Prepare all neccessary data structures to start a (image)-data transfer from the camera over the usb-bus
    /// and return immediately.  
    /// Transfer is cancelled if no image data arrive before timeout run out.
    /// \n Internal timeout setting is used, see \ref Set_Grabber_Timeout.
    /// \param adr Pointer to address where the image gets stored
    /// \return Error or 0 in case of success
    ///
    DWORD Acquire_Image_Async(void *adr,int timeout);

    ///
    /// \brief Setup an image transfer and wait until image is decoded. (Blocking)
    /// \n
    /// Prepare all neccessary data structures to start a (image)-data transfer from the camera over the usb-bus.
    /// Wait until image data is sucessfully decoded and transferred to the given address.  
    /// Transfer is cancelled if no image data arrive before timeout run out.
    /// Transfers an image from the camera and waits for decoding to be finished
    /// \n Internal timeout setting is used, see \ref Set_Grabber_Timeout.
    /// \param adr Pointer to address where the image gets stored
    /// \return Error or 0 in case of success
    ///
    DWORD Acquire_Image_Async_wait(void *adr);

    ///
    /// \brief Setup an image transfer and wait until image is decoded. (Blocking)
    /// \n
    /// Prepare all neccessary data structures to start a (image)-data transfer from the camera over the usb-bus.
    /// Wait until image data is sucessfully decoded and transferred to the given address.  
    /// Transfer is cancelled if no image data arrive before timeout run out.
    /// Transfers an image from the camera and waits for decoding to be finished
    /// \n Custom timeout setting is used.
    /// \param adr Pointer to address where the image gets stored
    /// \param timeout Sets a custom timeout
    /// \return Error or 0 in case of success
    ///
    DWORD Acquire_Image_Async_wait(void *adr,int timeout);


    ///
    /// \brief Asynchronous image transfer function.
    /// \anchor Acquire_Image_Async
    /// Prepare all neccessary data structures to start a (image)-data transfer from the camera over the usb-bus
    /// and return immediately.  
    /// Transfer is cancelled if no image data arrive before timeout run out.
    /// Parameter waitforimage is used to determine if this is a Blocking or Non-Blocking function.
    /// The default behaviour is to \b not wait for the decoding (if neccessary) to finish. It is recommended to use the overloaded functions!
    /// Transfer is cancelled if no image data arrive before timeout run out.
    /// \n Custom timeout setting is used.
    /// \param adr Pointer to address where the image gets stored
    /// \param timeout Sets a custom timeout
    /// \param waitforimage If set to TRUE, this function waits until \b all transferred images up to this one are completely decoded. Otherwise set this to false, or use the overloaded member functions.
    /// \return Error or 0 in case of success.
    ///
    DWORD Acquire_Image_Async(void *adr, int timeout,BOOL waitforimage);

    /// \cond

    /// \brief This function gets invoked after a libusb transfer is complete.
    /// \param transfer
    /// \return
    ///
    static void async_callback(struct libusb_transfer *transfer);
    ///
    /// \brief This function decodes a transferred block (or the whole image) from a pixelfly usb camera. This is a blocking function!
    /// \anchor GL_Coding
    /// \param buf Pointer to the buffer where the coded image is stored
    /// \param len Length of the buffer
    /// \param align MSB/LSB alignment
    /// \return
    ///
    static void GL_Coding(void* buf,int len,WORD align);

    /// \endcond


    /// @name Class Control Functions
    ///
    /// These functions are used to control and retrieve some internal variables of the class.
    ///

    ///
    /// Sets general timeout for all image acquire functions without timeout parameter.
    /// \param timeout timeout for image acquire in ms
    /// \anchor Set_Grabber_Timeout
    /// \return Error or 0 in case of success
    ///
    DWORD Set_Grabber_Timeout(int timeout);

    ///
    /// Gets the actual timeout for all image acquire functions without timeout parameter.
    /// \param timeout Pointer to integer variable to get the timeout for image acquire in ms
    /// \anchor Set_Grabber_Timeout
    /// \return Error or 0 in case of success
    ///
    DWORD Get_Grabber_Timeout(int *timeout);

    ///
    /// \brief Get camera settings and set internal parameters
    /// \n
    /// This function call should be called after Arm_Camera is called and is an overall replacement for the following functions.
    /// Parameter userset is used to determine if the grabber parameters are changed (recommended) or not.
    /// \param userset If set to 0 (default), this function does setup the grabber class correctly for following image transfers.
    /// If set to any other value grabber class \b must be setup with the following functions.  
    /// \return Error or 0 in case of success
    ///
    DWORD PostArm(int userset=0);

    ///
    /// \brief Sets the grabber size.
    /// \anchor Set_Grabber_Size
    /// It is extremely important to set this before any images are transferred! If any of the settings are changed that influence the image size
    /// Set_Grabber_Size \b must be called again before any images are transferred! If this is not done, memory or segmentation faults will occur!
    /// \param width Actual width of the picture
    /// \param height Actual height of the picture
    /// \param bitpix Actual number of bits per pixel. This value is rounded up to a multiple of 8.
    /// \return Error or 0 in case of success
    ///
    DWORD Set_Grabber_Size(int width,int height,int bitpix);

    ///
    /// \overload
    ///
    /// \return
    ///
    DWORD Set_Grabber_Size(int width,int height);

    ///
    /// \brief Set the dataformat for the following image transfers
    /// \n
    /// At the moment this call is only a dummy function without really usefulness
    /// \param dataformat New Dataformat
    /// \return
    ///
    DWORD Set_DataFormat(DWORD dataformat);

    ///
    /// \brief Returns the current grabber format
    /// \return Returns always the actual DataFormat
    ///
    DWORD Get_DataFormat(){return DataFormat;}

    ///
    /// \brief Sets a new bit alignment (MSB/LSB) in case it gets changed.
    /// \param act_align New alignment
    /// \return
    ///
    void SetBitAlignment(int act_align);

    ///
    /// \brief Returns the current grabber sizes
    /// \param width pointer to variable 
    /// \param height pointer to variable
    /// \param bitpix pointer to variabl
    /// \return Returns always 0.
    /// \retval *width Current width
    /// \retval *height Current height
    /// \retval *bitpix current bits per pixel
    ///
    int Get_actual_size(unsigned int *width,unsigned int *height,unsigned int *bitpix);

    ///
    /// \brief Returns the current grabber height
    /// \return Current height.
    ///
    int Get_Height();

    ///
    /// \brief Returns the current grabber width
    /// \return Current height.
    ///
    int Get_Width();


    /// \cond

    int Get_Line_Width();
    void Set_Async_Packet_Size(int size);
    int Get_Async_Packet_Size();

    DWORD Get_Camera_Settings();
    DWORD Allocate_Framebuffer(int nr_of_buffer ATTRIBUTE_UNUSED);
    DWORD Free_Framebuffer();


protected:
    ///
    /// \implements writelog
    ///
    void writelog(DWORD lev,PCO_HANDLE hdriver,const char *str,...);

    ///
    /// \brief Reads one image from running camera or from recorder buffer. USB transfer is divided in several smaller packets.
    /// \anchor usb_read_image
    /// Please use the Acquire_Image functions instead and this function only if you want more control over the transfer.
    /// \param buf Pointer to the buffer where the image gets stored
    /// \param buflen Length of the buffer
    /// \param timeout Sets a custom timeout
    /// \param Segment select segment of recorder buffer in camera 
    /// \param ImageNr select image of recorder buffer in camera 
    /// \return Error or 0 on success
    ///
    DWORD usb_read_image(void *buf,int buflen,DWORD timeout,WORD Segment,DWORD ImageNr);

    ///
    /// \implements usb_clear_input
    ///
    DWORD usb_clear_input();

    ///
    /// \implements usb_get_endpoints
    ///
    DWORD usb_get_endpoints();

    ///
    /// \brief Reads one image from USB asynchronously divided in several smaller packets and joins them together.
    /// \anchor usb_async_image
    /// Please use the Acquire_Image functions instead and this function only if you want more control over the transfer.
    /// \param buf Pointer to the buffer where the image gets stored
    /// \param buflen Length of the buffer
    /// \param timeout Sets a custom timeout
    /// \param waitforimage if set to TRUE, this function waits until \b all transferred images are completely decoded (pixelfly usb only). Otherwise, set to zero.
    /// \return Error or 0 on success
    ///
    DWORD usb_async_image(void *buf,int buflen,DWORD timeout, BOOL waitforimage);

#ifndef WIN32
    ///
    /// \brief GetTickCount function for linux
    /// \anchor GetTickCount
    ///
    /// Example:
    /// DWORD tc1,tc2,time;
    /// tc1 = GetTickCount();
    /// DoLengthyOperation();
    /// tc2 = GetTickCount();
    /// time = tc2 - tc1;
    /// time now holds the time (in ms) beween both function calls. Please note that the accuracy depends on the underlying operating system and is not guaranteed to have 1-ms accuracy.
    /// \return Time variable that should be used in relation to another function call.
    ///
    DWORD GetTickCount(void);

    ///
    /// \implements Sleep_ms
    ///
    void Sleep_ms(int time);
#endif

    /// \endcond

};

#endif

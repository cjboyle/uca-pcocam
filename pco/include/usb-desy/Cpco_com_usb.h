//-----------------------------------------------------------------//
// Name        | Cpco_usb_com.h              | Type: ( ) source    //
//-------------------------------------------|       (*) header    //
// Project     | pco.camera                  |       ( ) others    //
//-----------------------------------------------------------------//
// Platform    | WINDOWS 2000/XP                                   //
//-----------------------------------------------------------------//
// Environment | Microsoft Visual C++ 6.0                          //
//             |                                                   //
//-----------------------------------------------------------------//
// Purpose     | pco.camera - Libusb Communication                 //
//-----------------------------------------------------------------//
// Author      | MBL, PCO AG                                       //
//-----------------------------------------------------------------//
// Revision    | rev. 0.01 rel. 0.00                               //
//-----------------------------------------------------------------//
// Notes       | Common functions                                  //
//             |                                                   //
//             |                                                   //
//-----------------------------------------------------------------//
// (c) 2010 - 2014 PCO AG                                          //
// Donaupark 11 D-93309  Kelheim / Germany                         //
// Phone: +49 (0)9441 / 2005-0   Fax: +49 (0)9441 / 2005-20        //
// Email: info@pco.de                                              //
//-----------------------------------------------------------------//


//-----------------------------------------------------------------//
// Revision History:                                               //
//  see Cpco_cl_com.cpp                                            //
//-----------------------------------------------------------------//

#ifndef CPCO_COM_USB_H
#define CPCO_COM_USB_H

#include "pco_includes.h"
#include "Cpco_com.h"
#include <libusb-1.0/libusb.h>

#define MAXNUM_DEVICES 8

///
/// \brief The CPco_com_usb class, extends CPco_com
///
/// This is the communication class to exchange messages (telegrams) with a pco.camera.
///

class CPco_com_usb : public CPco_com
{

public:
    libusb_context           *libusb_ctx;

protected:
    libusb_device_handle     *libusb_hdev;
    libusb_config_descriptor *libusb_ConfigDescr;
    int alt_set;

    int                      ComInAddr;
    int                      ComOutAddr;
    int                      ComInMaxPacketSize;
    int                      ComOutMaxPacketSize;

    ///
    /// \brief Semaphore to lock the Control_Command()
    ///

    sem_t sMutex;
    ///
    /// \brief Value that holds the coding setting (pco.pixelfly only!)
    ///
    bool gl_Coding;
    volatile int transfer_done;
    int iftype;

    PCO_USB_TRANSFER_PARAM usb_param;

    //functions
public:
    CPco_com_usb();
    ~CPco_com_usb();

    ///
    /// \implements Open_Cam
    ///
    DWORD Open_Cam(DWORD num);

    ///
    /// \implements Open_Cam_Ext
    ///
    DWORD Open_Cam_Ext(DWORD num,SC2_OpenStruct *open);

    ///
    /// \brief Closes a connection to a pco camera.
    /// 
    /// Call this function to end the connection with a camera, e.g. when exiting the program, switching to another camera while the program is running, etc.
    /// \return Error or 0 on success
    ///
    DWORD Close_Cam();

    ///
    /// \brief Returns connection status
    /// \anchor IsOpen
    /// \return 1 if a connection is open, 0 else
    ///
    int  IsOpen();

    ///
    /// \implements Control_Command
    ///
    DWORD Control_Command(void *buf_in,DWORD size_in,void *buf_out,DWORD size_out);

/*
    ///
    /// \override PCO_GetCameraType
    ///
    DWORD PCO_GetCameraType(WORD* wCamType,DWORD* dwSerialNumber,WORD* wIfType);
    DWORD PCO_GetCameraType(WORD* wCamType,DWORD* dwSerialNumber);
*/
    ///
    /// \brief Returns the value of the gl_Coding member variable
    /// \anchor getCoding
    /// \return 1 if pixelfly coding is active, 0 else
    ///
    bool getCoding();

    libusb_device_handle* get_device_handle(){return libusb_hdev;}
    int get_altsetting(){return alt_set;}

    ///
    /// \brief Logger out function for Control_Command()
    /// \anchor usbcom_out
    /// \param com_out The command that was sent
    /// \param size_out Size of the telegram that was sent
    /// \param err Error to be written to the logfile
    /// \return
    ///
    void usbcom_out(WORD com_out,DWORD size_out,DWORD err);
    ///
    /// \brief Get status flag to indicate a completed image transfer
    /// \anchor transfer_msgwait
    /// \return transfer status. 1 = done, 0 = not done
    ///
    int transfer_msgwait();
    ///
    /// \brief Set status flag to indicate a completed image transfer
    /// \anchor set_transfer_msgwait
    /// Set this to zero everytime a new image transfer is started and check the variable for change periodically.
    /// \param wait
    /// \return
    ///
    void set_transfer_msgwait(int wait);

protected:

    ///
    /// \implements scan_camera
    ///
    DWORD scan_camera();
    ///
    /// \brief Clears any input from the camera that might still be on the bus.
    /// \anchor usb_clear_input
    /// This can be used in case of an error to clear any transfers from the bus that might still be active.
    /// \return Error or 0 on success
    ///
    DWORD usb_clear_input();
    ///
    /// \brief Reads output from a connected usb camera.
    /// \anchor usb_read
    /// Open_Cam() must be called first successfully before using this function!
    /// \param buf Pointer to a buffer to store the data from the camera
    /// \param buflen Pointer to a variable that holds the buffer length
    /// \param timeout Defines a timeout after which the function returns with an error.
    /// \return Error or 0 on success
    ///
    DWORD usb_read(void *buf,int *buflen,DWORD timeout);
    ///
    /// \brief Writes the buffer to a connected usb camera.
    /// \anchor usb_write
    /// Open_Cam() must be called first successfully before using this function!
    /// \param buf Pointer to a buffer to write to the camera
    /// \param buflen Pointer to a variable that holds the buffer length
    /// \param timeout Defines a timeout after which the function returns with an error.
    /// \return Error or 0 on success
    ///
    DWORD usb_write(void *buf,int *buflen,DWORD timeout);
    ///
    /// \brief Gets the endpoints for the USB interface
    /// \anchor usb_get_endpoints
    /// This depends on the connected camera. The endpoints (if found) are then stored in the appropriate member variables.
    /// Used by Open_Cam()
    /// \return Error or 0 on success
    ///
    DWORD usb_get_endpoints();

    ///
    /// \brief Enable camera <-> PC handshake 
    /// For certain cameras and mainly USB2 interface handshake mode must be enabled else imagedata will be lost
    /// Called in Open_Cam()
    /// \return Error or 0 on success
    ///
    DWORD EnableHandshake();

};

#endif

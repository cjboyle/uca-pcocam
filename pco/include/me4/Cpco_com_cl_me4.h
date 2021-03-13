//-----------------------------------------------------------------//
// Name        | Cpco_cl_com.h               | Type: ( ) source    //
//-------------------------------------------|       (*) header    //
// Project     | pco.camera                  |       ( ) others    //
//-----------------------------------------------------------------//
// Platform    | WINDOWS 2000/XP                                   //
//-----------------------------------------------------------------//
// Environment | Microsoft Visual C++ 6.0                          //
//             |                                                   //
//-----------------------------------------------------------------//
// Purpose     | pco.camera - CameraLink Communication             //
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

#ifndef CPCO_COM_CL_H
#define CPCO_COM_CL_H

#define MAXNUM_DEVICES 8

#include "pco_includes.h"
#include "Cpco_com.h"


#ifdef __linux__
#include <sys/param.h>
#include <clser.h>
const int baud[8]={9600,19200,38400,57600,115200,230400,460800,0};
#endif
// unused
void close_SerialLib();

///
/// \brief The CPco_com_cl_me4 class, extends CPco_com
///
/// This is the communication class to exchange messages (telegrams) with a pco.camera via CameraLink microEnable IV board.
///

class CPco_com_cl_me4 : public CPco_com
{

public:
//    CPco_Log *log;
    WORD   num_lut;
    SC2_LUT_DESC cam_lut[10];

private:
    //for some reason linux and windows libraries use different parameters
#ifdef __linux__
    int(*pclSerialInit)(unsigned int serialIndex, void** serialRefPtr);
    int(*pclSerialRead)(void* serialRef, char* buffer, unsigned int* bufferSize, unsigned int serialTimeout);
    int(*pclSerialWrite)(void* serialRef, char* buffer, unsigned int* bufferSize, unsigned int serialTimeout);
    void(*pclSerialClose)(void* serialRef);
    int(*pclSetBaudRate)(void* serialRef, unsigned int baudRate);
    int(*pclGetSupportedBaudRates)(void* serialRef, unsigned int* buf);
    int(*pclGetNumBytesAvail)(void* serialRef, unsigned int* bufferSize);
    int(*pclFlushPort)(void* serialRef);
    int(*pclSetParity)(void* serialRef, unsigned int parityOn);
    int(*pclGetNumSerialPorts)(unsigned int* numSerialPorts);
    int(*pclGetManufacturerInfo)(char* manufacturerName, unsigned int* bufferSize, unsigned int* version);
    int(*pclGetSerialPortIdentifier)(unsigned int index, char *type, unsigned int *buffersize);
#endif

#ifdef WIN32
    int(*pclSerialInit)(unsigned long serialIndex, void** serialRefPtr);
    int(*pclSerialRead)(void* serialRef, char* buffer, unsigned long* bufferSize, unsigned long serialTimeout);
    int(*pclSerialWrite)(void* serialRef, char* buffer, unsigned long* bufferSize, unsigned long serialTimeout);
    int(*pclSerialClose)(void* serialRef);
    int(*pclSetBaudRate)(void* serialRef, unsigned int baudRate);
    int(*pclGetSupportedBaudRates)(void* serialRef, unsigned int* buf);
    int(*pclGetNumBytesAvail)(void* serialRef, unsigned long* bufferSize);
    int(*pclFlushPort)(void* serialRef);
    int(*pclSetParity)(void* serialRef, unsigned int parityOn);
    int(*pclGetNumSerialPorts)(unsigned int* numSerialPorts);
    int(*pclGetManufacturerInfo)(char* manufacturerName, unsigned long* bufferSize, unsigned int* version);
    int(*pclGetSerialPortIdentifier)(unsigned long index, char *type, unsigned long *buffersize);
#endif

protected:
    //common cameralink
    void*  serialRef;
    ///
    /// \brief Semaphore to lock the Control_Command()
    ///
    sem_t sComMutex;

    PCO_SC2_CL_TRANSFER_PARAM transferpar;

    //functions
public:
    CPco_com_cl_me4();
    CPco_com_cl_me4(int num);
    ~CPco_com_cl_me4();

    ///
    /// \implements Open_Cam
    ///
    DWORD Open_Cam(DWORD num);

    ///
    /// \implements Open_Cam_Ext
    ///
    DWORD Open_Cam_Ext(DWORD num, SC2_OpenStruct *open);
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
    BOOLEAN  IsOpen();
    ///
    /// \implements Control_Command
    ///
    DWORD Control_Command(void *buf_in, DWORD size_in, void *buf_out, DWORD size_out);
    ///
    /// \brief Logger out function for Control_Command()
    /// \anchor sercom_out
    /// \param com_out The command that was sent
    /// \param err Error to be written to the logfile
    ///
    void sercom_out(WORD com_out,DWORD err);

    ///
    /// \brief Cameralink Interface only:
    /// \anchor PCO_GetTransferParameter
    /// \see PCO_SetTransferParameter
    /// \param buf Pointer to _PCO_SC2_CL_TRANSFER_PARAMS structure
    /// \param length Length of the structure. Usually sizeof() is enough.
    /// \see PCO_SetTransferParameter
    /// \return Error code or 0 in case of success
    ///
    DWORD PCO_GetTransferParameter(void* buf, int length);
    ///
    /// \brief CameraLink Interface only:
    /// \anchor PCO_SetTransferParameter
    /// The user can instantiate a structure _PCO_SC2_CL_TRANSFER_PARAMS, which is defined in SC2_SDKAddendum.h.
    /// - Baud rate: sets the baud rate of the cameralink serial port interface. Valid values are: 9600, 19200, 38400, 57600 and 115200.
    /// - Cameralink-ClockFrequency: sets the clock rate of the cameralink interface (note: different to sensor pixel clock!!!). See SC2_SDKAddendum.h for valid parameters.
    /// - CCline: sets the usage of CC1-CC4 lines, use parameter returned by the GetTransferParameter command.
    /// - Data Format: sets the Test pattern and Data splitter switch, use parameter returned by the GetTransferParameter command.
    /// - Transmit: sets the transmitting format, 0-single images, 1-continuous image transfer.
    ///
    /// For pco.edge:
    ///
    /// In case a pco.edge in rolling shutter mode is controlled, the user can call this function with
    /// appropriate parameters. With sensor-pixelclock = 286MHz and an x-resolution \f$\geq\f$ 1920 the
    /// camera and interface must be switched to data format PCO_CL_DATAFORMAT_5x12 (switches
    /// to 12bit, whereas upper bits are moved to LSB) or PCO_CL_DATAFORMAT_5x12L (switches
    /// to 12bit, whereas all data above 255 is calculated by a square-root LUT. The driver tries to
    /// recalculate to 16bit values.). Also the current lookup table must be set to 0x1612.
    /// For all other cases (sensor-pixelclock = 95.3MHz or x-resolution < 1920) the camera and interface
    /// must be switched to data format PCO_CL_DATAFORMAT_5x16.
    /// For global shutter mode set data format PCO_CL_DATAFORMAT_5x12. This is the default
    /// setting and thus does not have to be changed after powering up or a reboot of the camera.
    ///
    /// \f[
    /// \begin{tabular}{| p{5.5cm} | p{5cm} | l |}
    /// \hline
    /// \rowcolor{lightgray} \textbf{Shutter Mode} & PCO\_CL\_Dataformat & Lookup Table \\ \hline
    /// Rolling or Global Reset; 95 Mhz & PCO\_CL\_DATAFORMAT5\_16 & 0 \\ \hline
    /// Rolling or Global Reset; 286 Mhz,\linebreak x $<$ 1920 & PCO\_CL\_DATAFORMAT\_5x16 & 0 \\ \hline
    /// Rolling or Global Reset; 286 Mhz,\linebreak x $\geq$ 1920 & PCO\_CL\_DATAFORMAT\_5x12L & 0x1612 \\ \hline
    /// Global & PCO\_CL\_DATAFORMAT\_5x12 & 0 \\ \hline
    /// \end{tabular}
    /// \f]
    ///
    /// E.g. rolling shutter, 286MHz (Sensor-Pixelclock), xRes=2560:
    ///
    /// Dataformat = SCCMOS_FORMAT_TOP_CENTER_BOTTOM_CENTER | PCO_CL_DATAFORMAT_5x12L
    ///
    /// In this case also a call PCO_ SetActiveLookuptable must follow (wIdentifier=0x1612).
    /// \param buf Pointer to _PCO_SC2_CL_TRANSFER_PARAMS structure
    /// \param length Length of the structure. Usually sizeof() is enough.
    /// \return Error code or 0 in case of success
    ///
    DWORD PCO_SetTransferParameter(void* buf, int length);


protected:
    ///
    /// \brief Gets the serial function pointers.
    /// \return Error code or 0 on success
    ///
    DWORD Get_Serial_Functions();
    ///
    /// \implements scan_camera
    ///
    DWORD scan_camera();

    ///
    /// \brief Set the baudrate. This must be a valid number (e.g. 9600, 38400, 115000)!
    /// \anchor set_baudrate
    /// The function verifies that the baudrate was successful. If setting the baudrate fails, the function switches back to the default 9600!
    /// \param baudrate The baudrate to be set
    /// \return Error code or 0 on success
    ///
    DWORD set_baudrate(int baudrate);

    ///
    /// \brief Gets the actual CL config and stores it in the appropriate class member variables
    /// \anchor get_actual_cl_config
    /// \return Error code or 0 on success
    ///
    DWORD get_actual_cl_config();

    ///
    /// \brief Sets the CL config.
    /// \param cl_par PCO_SC2_CL_TRANSFER_PARAM struct
    /// \return Error code or 0 on success
    ///
    DWORD set_cl_config(PCO_SC2_CL_TRANSFER_PARAM cl_par);
};

#endif

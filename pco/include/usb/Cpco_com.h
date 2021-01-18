//-----------------------------------------------------------------//
// Name        | Cpco_com.h                  | Type: ( ) source    //
//-------------------------------------------|       (*) header    //
// Project     | pco.camera                  |       ( ) others    //
//-----------------------------------------------------------------//
// Platform    | LINUX                                             //
//-----------------------------------------------------------------//
// Environment | gcc                                               //
//             |                                                   //
//-----------------------------------------------------------------//
// Purpose     | pco.camera - Communication                        //
//-----------------------------------------------------------------//
// Author      | MBL, PCO AG                                       //
//-----------------------------------------------------------------//
// Revision    | rev. 1.04                                         //
//-----------------------------------------------------------------//
// Notes       | Common functions                                  //
//             |                                                   //
//             |                                                   //
//-----------------------------------------------------------------//
// (c) 2010 - 2015 PCO AG                                          //
// Donaupark 11 D-93309  Kelheim / Germany                         //
// Phone: +49 (0)9441 / 2005-0   Fax: +49 (0)9441 / 2005-20        //
// Email: info@pco.de                                              //
//-----------------------------------------------------------------//


//-----------------------------------------------------------------//
// Revision History:                                               //
//  see Cpco_com.cpp                                               //
//-----------------------------------------------------------------//

#ifndef CPCO_COM_H
#define CPCO_COM_H

// GNU specific __attribute__((unused)) define
#ifdef __GNUC__
#define ATTRIBUTE_UNUSED __attribute__((unused))
#else
#define ATTRIBUTE_UNUSED
#endif

#include "pco_includes.h"
#include "../../pco_common/VersionNo.h"

#if !defined (MAX_PATH)
#define MAX_PATH 1024
#endif

/*!\page page1 Introduction
  This document provides an detailed description of all functions of the PCO Camera Linux API.
  The API is a class interface and provides the functionality to write own Applications in a Linux environment.\n
  Any C++ compiler can be used for development.\n 
  The intention was to provide a simple interface, which can be used with every PCO camera of
  the pco.camera series independent from the used interface.\n
  The \b PCO Linux Development Kit includes:
   - all header files for compilation
   - all source code files for building the common and specific classes
   - example code 
   - precompiled libraries, which are used in the examples 
   - compiled binaries of the examples 
  \n\n
 
  \section sec General
  The API consist of two classes a communication class which is used to control the camera settings
  and a grabber class which is used to transfer single or multiple images from the camera to the PC.
  Because communication with the camera and image transfer depends on the used hardware interface,
  interface specific classes exist.\n
  For controlling the camera settings a common base class exists. The interface specific classes
  are subclasses derived from this base class.  
  For image transfers only interface specific classes exist, but main functions use equal function declarations.
  \n
  Error and warning codes can be found in file pco_err.h.\n
  PCO_NOERROR is defined as value '0'.

  \latexonly \newpage \endlatexonly
*/


/// \brief Base interface class
///
/// This is the communication class which includes all functions, which build the commands codes which can then be 
/// sent to the pco.cameras. Derived from this class are all interface specific classes.
/// This class includes some common functions and defines the mandatory functions that each subclass has to implement. 
/// \nosubgrouping


class CPco_com {

public:
    WORD   num_lut;
    SC2_LUT_DESC cam_lut[10];

protected:
    //common
    CPco_Log* clog;
    PCO_HANDLE hdriver;
    DWORD  initmode;
    DWORD  boardnr;
    DWORD  camerarev;
    bool internal_open;
    DWORD connected;

    PCO_SC2_TIMEOUTS tab_timeout;
    SC2_Camera_Description_Response description;
    BOOL scan;

    //functions
public:
    CPco_com();
    virtual ~CPco_com(){}


/// @name Base Class Functions
/// Open a communication class and establish a connection with the camera.
/// Communication is done through the Control_Command function.

//@{
    ///
    /// \latexonly pcotag Base Class Functions \endlatexonly
    /// Opens a connection to a pco.camera
    /// This is a virtual function the implementation is in the interface specific class
    /// \anchor Open_Cam
    /// \param num Number of the camera to open starting with zero.
    /// \return Error code or PCO_NOERROR on success
    ///
    virtual DWORD Open_Cam(DWORD num)=0;

    /// \cond

    /// \brief Opens a connection to a pco.camera
    /// \anchor Open_Cam_Ext
    /// \param num Number of the camera starting with zero.
    /// \param strOpen Unused.
    /// \return Error code or PCO_NOERROR on success
    ///
    virtual DWORD Open_Cam_Ext(DWORD num,SC2_OpenStruct* strOpen)=0;
    /// \endcond


    ///
    /// \brief Closes a connection to a pco.camera
    /// \anchor Close_Cam
    /// Not all classes derived from this must implement a close function.
    /// \return Error code or PCO_NOERROR on success
    ///
    virtual DWORD Close_Cam(){return PCO_NOERROR;}

    ///
    /// \brief The main function to communicate with the pco.camera via \b PCO telegrams
    /// \anchor Control_Command
    /// See sc2_telegram.h for a list of telegram definitions and sc2_command.h for a list of all public commands.
    /// Checksum calculation is done in this function, therefore there is no need to pre-calculate it.
    /// \param buf_in Pointer to the buffer where the telegram is stored
    /// \param size_in Size of the input buffer in bytes
    /// \param buf_out Pointer to the buffer where the response gets stored
    /// \param size_out Size of the output buffer in bytes. If the returned telegram does not fit into the output buffer 
    /// \return Error code or PCO_NOERROR on success
    ///
    virtual DWORD Control_Command(void* buf_in,DWORD size_in,void* buf_out,DWORD size_out)=0;

    ///
    /// \brief Sets the logging behaviour for the communication class.
    /// If this function is not called no logging is performed.
    /// Logging might be useful to follow the program flow of the application.
    /// Logging class is available through the library libpcolog
    /// \param Log Pointer to a CPco_Log logging class object
    /// \return None
    ///
    void SetLog(CPco_Log* Log);


    ///
    /// \brief Returns the currently used logging class object.
    /// \return logging class object or NULL if no logging was set.
    ///
    CPco_Log* GetLog(){return clog;}

//@}


    /// \cond

protected:
    ///
    /// \brief Writes a log entry
    /// \param level Type of the entry, e.g. Error, Status, Message, ...
    /// \param hdriver The source of the error
    /// \param message The actual log message as printf formatted string
    /// \return None
    ///
    void writelog(DWORD level,PCO_HANDLE hdriver,const char *message,...);

    ///
    /// \brief Connect to camera on given interface number.
    /// \anchor scan_camera
    /// \return Error code or 0 on success
    ///
    virtual DWORD scan_camera()=0;

    ///
    /// \brief Builds the checksum for Control_Command() telegrams
    /// \anchor build_checksum
    /// \param buf Pointer to buffer that contains the telegram
    /// \param size Pointer to size of the buffer
    /// \return Error code or PCO_NOERROR on success
    ///
    DWORD build_checksum(unsigned char* buf,int* size);

    ///
    /// \brief Verifies the checksum for a telegram
    /// \anchor test_checksum
    /// \param buf Pointer to buffer that contains the telegram
    /// \param size Pointer to size variable 
    ///  - On input number of received bytes
    ///  - On output size of received and validated telegram
    /// \return Error code or PCO_NOERROR on success
    ///
    DWORD test_checksum(unsigned char* buf,int* size);

    ///
    /// \brief Gets the camera descriptor and caches it
    /// \anchor get_description
    /// \return Error code or PCO_NOERROR on success
    ///
    DWORD get_description();

    ///
    /// \brief Gets camera main processor and main FPGA firmware versions and writes them to the logfile
    /// \anchor get_firmwarerev
    /// \return Error code or PCO_NOERROR on success
    ///
    DWORD get_firmwarerev();

    ///
    /// \brief Gets installed LUTs and writes them to the logfile
    /// \anchor get_lut_info
    /// \return Error code or PCO_NOERROR on success
    ///
    DWORD get_lut_info();

    /// \endcond

public:

#include "Cpco_com_func.h"
#include "Cpco_com_func_2.h"

    /// @name Class Control Functions
    ///
    /// These functions are used to control some internal variables of the class.
    ///

    ///
    /// \brief Gets the current timeouts for images and telegrams
    /// \anchor gettimeouts
    /// \param strTimeouts Pointer to a PCO_SC2_TIMEOUTS structure
    /// \return
    ///
    void gettimeouts(PCO_SC2_TIMEOUTS* strTimeouts);

    ///
    /// \brief Sets the timeouts of the camera communication class
    /// \anchor Set_Timeouts
    /// \param timetable Pointer to a DWORD array.
    /// First DWORD is the command timeout, second the image timeout, third the transfer timeout.
    /// \param length Length of the array in bytes, a maximum of 12 bytes are used.
    /// \return 
    ///
    void Set_Timeouts(void* timetable,DWORD length);


    ///
    /// \brief Common sleep function 
    /// \anchor Sleep_ms
    /// \param time_ms Time to sleep in ms
    /// \return None
    ///
    void Sleep_ms(DWORD time_ms);


    /// \cond

    ///
    /// \brief GetConnectionStatus
    /// \anchor GetConnectionStatus
    /// \return connection status
    int GetConnectionStatus();

    ///
    /// \brief Sets the connection status
    /// \anchor SetConnectionStatus
    /// \param status 
    /// \return n
    ///
    void SetConnectionStatus(DWORD status);

    /// \endcond
};

#endif






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

#if !defined (MAX_PATH)
#define MAX_PATH 1024
#endif


/*! \page page1 Introduction
  This manual provides an detailed description of all functions of the PCO Camera Linux API.
  The API is a class interface and provides the functionality to write own Applications in a Linux environment.
  Any C++ compiler can be used for development. 
  The intention was to provide a simple interface, which can be used with every PCO camera of
  the pco.camera series independent from the used interface.
 
  \section sec General
  The API consist of two classes a communication class which is used to control the camera settings
  and a grabber class which is used to transfer single or multiple images from the camera to the PC.
  Because communication with the camera and image transfer depends on the used hardware interface,
  interface specific classes exist.
  For controlling the camera settings a common base class exists. The interface specific classes
  are subclasses derived from the base class.  
  For image transfers only interface specific classes exist, but main functions use equal function declarations. 
  \latexonly \newpage \endlatexonly
*/


//nochmalige unterteilung funktioniert so
//  This page contains the subsections \ref subsection1 and \ref subsection2.
//  For more info see page \ref page2.
//  \subsection subsection1 The first subsection
//  Text.
//  \subsection subsection2 The second subsection
//  More text.


///
/// \brief Base interface class
///
/// Derived from this class are all interface specific classes. It includes some common functions and defines the mandatory functions that each subclass has to implement.
///
///\nosubgrouping



class CPco_com {
public:
    WORD   num_lut;
    SC2_LUT_DESC cam_lut[10];

protected:
    //common
    CPco_Log *clog;
    PCO_HANDLE hdriver;
    DWORD  initmode;
    DWORD  boardnr;
    DWORD  camerarev;
    bool internal_open;
    DWORD connected;

    PCO_SC2_TIMEOUTS tab_timeout;
    SC2_Camera_Description_Response description;

    //functions
public:
    CPco_com();
    virtual ~CPco_com(){}

    ///
    /// \brief Opens a connection to a pco.camera
    /// \anchor Open_Cam
    /// \param num Number of the camera starting with zero.
    /// \return Error code or 0 on success
    ///
    virtual DWORD Open_Cam(DWORD num)=0;

    ///
    /// \brief Opens a connection to a pco.camera
    /// \anchor Open_Cam_Ext
    /// \param num Number of the camera starting with zero.
    /// \param strOpen Unused.
    /// \return Error code or 0 on success
    ///
    virtual DWORD Open_Cam_Ext(DWORD num,SC2_OpenStruct *strOpen)=0;

    ///
    /// \brief Closes a connection with a pco.camera
    /// \anchor Close_Cam
    /// Not all classes derived from this must implement a close function.
    /// \return Error code or 0 on success
    ///
    virtual DWORD Close_Cam(){return PCO_NOERROR;}

    ///
    /// \brief The main function to communicate with the camera via PCO telegrams
    /// \anchor Control_Command
    /// See sc2_telegram.h for a list of telegrams and sc2_command.h for a list of possible commands
    ///
    /// Checksum calculation is done in this function, no need to pre-calculate it.
    /// \param buf_in Pointer to the buffer where the telegram is stored
    /// \param size_in Pointer to a DWORD that holds the input size of the buffer
    /// \param buf_out Pointer to the buffer where the response gets stored
    /// \param size_out Pointer to a DWORD that holds the size of the buffer. This is updated with the returned size.
    /// \return Error code or 0 on success
    ///
    virtual DWORD Control_Command(void *buf_in,DWORD size_in,void *buf_out,DWORD size_out)=0;

    ///
    /// \brief Sets the logging behaviour for the communication class.
    /// \n
    /// If this function is not called no logging is performed.
    /// Logging might be useful to follow the program flow of the application.
    /// \param Log Pointer to a CPco_Log logging class object
    /// \return
    ///
    void SetLog(CPco_Log *Log);

    /// \cond

protected:
    ///
    /// \brief Writes a log entry
    /// \anchor writelog
    /// \param level Type of the entry, e.g. Error, Status, Message, ...
    /// \param hdriver The source of the error
    /// \param message The actual log message as printf formatted string
    /// \return Error code or 0 on success
    ///
    void writelog(DWORD level,PCO_HANDLE hdriver,const char *message,...);

    ///
    /// \brief Scans the connected camera and gets basic informations
    /// \anchor scan_camera
    /// \return Error code or 0 on success
    ///
    virtual DWORD scan_camera()=0;

    ///
    /// \brief Builds the checksum for Control_Command() telegrams
    /// \anchor build_checksum
    /// \param buf Pointer to buffer that contains the telegram
    /// \param size Pointer to size of the buffer
    /// \return Error code or 0 on success
    ///
    DWORD build_checksum(unsigned char *buf,int *size);

    ///
    /// \brief Verifies the checksum for a telegram
    /// \anchor test_checksum
    /// \param buf Pointer to buffer that contains the telegram
    /// \param size Pointer to size of the buffer
    /// \return Error code or 0 on success
    ///
    DWORD test_checksum(unsigned char *buf,int *size);

    ///
    /// \brief Gets the camera descriptor and caches it
    /// \anchor get_description
    /// \return Error code or 0 on success
    ///
    DWORD get_description();

    ///
    /// \brief Gets camera main processor and main FPGA firmware versions and writes them to the logfile
    /// \anchor get_firmwarerev
    /// \return Error code or 0 on success
    ///
    DWORD get_firmwarerev();

    ///
    /// \brief Gets installed LUTs and writes them to the logfile
    /// \anchor get_lut_info
    /// \return Error code or 0 on success
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
    void gettimeouts(PCO_SC2_TIMEOUTS *strTimeouts);

    ///
    /// \brief Sets the timeouts
    /// \anchor Set_Timeouts
    /// \param timetable Pointer to an DWORD array. First parameter is the command timeout, second the image timeout, third the transfer timeout.
    /// \param length Length of the array in bytes, a maximum of 12 bytes are used.
    /// \return 
    ///
    void Set_Timeouts(void *timetable,DWORD length);

    ///
    /// \brief GetConnectionStatus
    /// \anchor GetConnectionStatus
    /// \return Connectionstatus
    /// \retval 1 connected
    /// \retval 0 not connected
    int GetConnectionStatus();

    ///
    /// \brief Sets the connection status
    /// \anchor SetConnectionStatus
    /// \param status 
    /// \return 
    ///
    void SetConnectionStatus(DWORD status);

    ///
    /// \brief Common sleep function for Linux/Windows
    /// \anchor Sleep_ms
    /// \param time_ms Time to sleep in ms
    /// \return 
    ///
    void Sleep_ms(DWORD time_ms);

};

#endif






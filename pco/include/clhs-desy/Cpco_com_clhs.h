//-----------------------------------------------------------------//
// Name        | Cpco_com_clhs.h             | Type: ( ) source    //
//-------------------------------------------|       (*) header    //
// Project     | pco.camera                  |       ( ) others    //
//-----------------------------------------------------------------//
// Platform    | Linux                                             //
//-----------------------------------------------------------------//
// Environment |                                                   //
//             |                                                   //
//-----------------------------------------------------------------//
// Purpose     | pco.camera - clhs communication                   //
//-----------------------------------------------------------------//
// Author      | MBL, PCO AG                                       //
//-----------------------------------------------------------------//
// Revision    | rev. 0.01 rel. 0.00                               //
//-----------------------------------------------------------------//
// Notes       | Common functions                                  //
//             |                                                   //
//             |                                                   //
//-----------------------------------------------------------------//
// (c) 2016 - 2016 PCO AG                                          //
// Donaupark 11 D-93309  Kelheim / Germany                         //
// Phone: +49 (0)9441 / 2005-0   Fax: +49 (0)9441 / 2005-20        //
// Email: info@pco.de                                              //
//-----------------------------------------------------------------//

//-----------------------------------------------------------------//
// Revision History:                                               //
//  see Cpco_com_clhs.cpp                                          //
//-----------------------------------------------------------------//

#ifndef CPCO_COM_CLHS_H
#define CPCO_COM_CLHS_H

#include "pco_includes.h"
#include "Cpco_com.h"
#include "VersionNo.h"

#include "pco_clhs_cam.h"

#define MAXNUM_DEVICES 8


///
/// \brief The CPco_com_clhs class, extends CPco_com
///
/// This is the communication class to exchange messages (telegrams) with a pco.camera.
///

class CPco_com_clhs : public CPco_com
{

public:

protected:

    ///
    /// \brief Semaphore to lock the Control_Command()
    ///

    sem_t sMutex;
    CPco_clhs_cam* Cclhs_cam;

    //functions
public:
    CPco_com_clhs();
    ~CPco_com_clhs();

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


    DWORD scan_camera();

    CPco_clhs_cam* get_C_clhs_cam(void);

protected:

};

#endif

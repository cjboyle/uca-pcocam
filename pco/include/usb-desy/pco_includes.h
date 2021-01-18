//-----------------------------------------------------------------//
// Name        | pco_includes.h              | Type: ( ) source    //
//-------------------------------------------|       (*) header    //
// Project     | SC2                         |       ( ) others    //
//-----------------------------------------------------------------//
// Platform    | Windows / Linux                                   //
//-----------------------------------------------------------------//
// Environment |                                                   //
//             |                                                   //
//-----------------------------------------------------------------//
// Purpose     | SC2 - libusb Includes Header                      //
//-----------------------------------------------------------------//
// Author      | MBL, PCO AG                                       //
//-----------------------------------------------------------------//
// Revision    | rev. 0.02                                         //
//-----------------------------------------------------------------//
// Notes       | Common include files                              //
//             |                                                   //
//             |                                                   //
//-----------------------------------------------------------------//
// (c) 2004 - 2015 PCO AG * Donaupark 11 *                         //
// D-93309      Kelheim / Germany * Phone: +49 (0)9441 / 2005-0 *  //
// Fax: +49 (0)9441 / 2005-20 * Email: info@pco.de                 //
//-----------------------------------------------------------------//


//-----------------------------------------------------------------//
// Revision History:                                               //
//-----------------------------------------------------------------//
// Rev.:     | Date:      | Changed:                               //
// --------- | ---------- | ---------------------------------------//
//  0.01     | 20.02.2014 |  new file from CL project              //
//-----------------------------------------------------------------//
//  0.02     | 20.06.2015 |  with pthreads                         //
//-----------------------------------------------------------------//
//  0.0x     | xx.xx.200x |                                        //
//-----------------------------------------------------------------//


#ifndef PCO_INCLUDES_H
#define PCO_INCLUDES_H

//#define _CRT_SECURE_NO_WARNINGS

#include "defs.h"
#include <time.h>
#include <stdlib.h>
#include <stdint.h>
#include <locale.h>
#include <stdio.h>
#include <fcntl.h>
#include <math.h>
#include <time.h>
#include "file12.h"

#ifdef WIN32
#include <windows.h>
#include <tchar.h>
#include <winioctl.h>
#include <conio.h>
#include <io.h>
#include <iostream>
#include <fstream>
#include <shlobj.h>
#include "../pco_include/pthreads-win/pthread.h"
#include "../pco_include/pthreads-win/semaphore.h"

#pragma warning(disable : 4200)

#else
#include <fcntl.h>      /* open */
#include <unistd.h>     /* exit */
#include <stdarg.h>
#include <memory.h>
#include <ctype.h>
#include <pthread.h>
#include <sys/time.h>
#include <semaphore.h>
#include <errno.h>
#include <assert.h>
#include <signal.h>

typedef bool BOOL;
typedef bool BOOLEAN;
#endif

#define istrlen (int)strlen


#include "Cpco_log.h"
#include "reorderfunc.h"
#include "SC2_drv_struct.h"
#include "SC2_SDKAddendum.h"
#include "PCO_err.h"
#include "sc2_command.h"
#include "sc2_telegram.h"
#include "sc2_defs.h"
#include "sc2_command_toString.h"
#include "correctionmode.h"

#endif

//-----------------------------------------------------------------//
// Name        | Cpco_log.h                  | Type: ( ) source    //
//-------------------------------------------|       (*) header    //
// Project     | pco.camera                  |       ( ) others    //
//-----------------------------------------------------------------//
// Platform    | LINUX                                             //
//-----------------------------------------------------------------//
// Environment |                                                   //
//-----------------------------------------------------------------//
// Purpose     | pco.camera - Logging class                        //
//-----------------------------------------------------------------//
// Author      | MBL, PCO AG                                       //
//-----------------------------------------------------------------//
// Revision    | rev. 1.03                                         //
//-----------------------------------------------------------------//
// Notes       | Common functions                                  //
//             |                                                   //
//             |                                                   //
//-----------------------------------------------------------------//
// (c) 2010 - 2012 PCO AG                                          //
// Donaupark 11 D-93309  Kelheim / Germany                         //
// Phone: +49 (0)9441 / 2005-0   Fax: +49 (0)9441 / 2005-20        //
// Email: info@pco.de                                              //
//-----------------------------------------------------------------//


//-----------------------------------------------------------------//
// Revision History:                                               //
//-----------------------------------------------------------------//
// Rev.:     | Date:      | Changed:                               //
// --------- | ---------- | ---------------------------------------//
//  1.03     | 24.01.2012 |  from Cpco_cl_com.cpp                  //
//-----------------------------------------------------------------//
//  0.0x     | xx.xx.200x |                                        //
//-----------------------------------------------------------------//

#ifndef CPCO_LOG_H
#define CPCO_LOG_H

#if !defined (MAX_PATH)
#define MAX_PATH 1024
#endif

#include "defs.h"

#ifndef WIN32
typedef struct SYSTEMTIME {
  WORD wYear;
  WORD wMonth;
  WORD wDayOfWeek;
  WORD wDay;
  WORD wHour;
  WORD wMinute;
  WORD wSecond;
  WORD wMilliseconds;
} SYSTEMTIME;
#endif
;
class CPco_Log
{
 char logname[MAX_PATH];

 public:
//  CPco_Log(int logbits=0);
  CPco_Log(const char *name=NULL);
  ~CPco_Log();
  void writelog(DWORD lev,const char *str,...);
  void writelog(DWORD lev,const char *str,va_list arg);
  void writelog(DWORD lev,PCO_HANDLE hdriver,const char *str,...);
  void writelog(DWORD lev,PCO_HANDLE hdriver,const char *str,va_list args);
  void set_logbits(DWORD logbit);
  DWORD get_logbits(void);
  void start_time_mess(void);
  double stop_time_mess(void);

#ifndef WIN32
  void GetLocalTime(SYSTEMTIME* st);
  void QueryPerformanceCounter(__int64* someint);
  void QueryPerformanceFrequency(__int64* someint);
#endif

protected:
  int hflog;
  DWORD log_bits;
  __int64 lpFrequency,lpPCount1,lpPCount2,lpPCount_p;
  __int64 stamp1,stamp2;
};

//loglevels for interface dll
#define ERROR_M     0x0001
#define INIT_M      0x0002
#define BUFFER_M    0x0004
#define PROCESS_M   0x0008

#define COC_M       0x0010
#define INFO_M      0x0020
#define COMMAND_M   0x0040

#define PCI_M       0x0080


#define TIME_M      0x1000
#define TIME_MD     0x2000

#define NONE_M      0x01000000


#endif

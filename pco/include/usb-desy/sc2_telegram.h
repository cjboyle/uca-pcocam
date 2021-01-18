//-----------------------------------------------------------------//
// Name        | sc2_telegram.h              | Type: ( ) source    //
//-------------------------------------------|       (*) header    //
// Project     | SC2                         |       ( ) others    //
//-----------------------------------------------------------------//
// Platform    | - Embedded platforms like M16C, AVR32, PIC32 etc. //
//             | - PC with several Windows versions, Linux etc.    //
//-----------------------------------------------------------------//
// Environment | - Platform dependent                              //
//-----------------------------------------------------------------//
// Purpose     | SC2 - Packet Structure defines                    //
//-----------------------------------------------------------------//
// Author      | MBl/FRe/LWa/AGr and others, PCO AG                //
//-----------------------------------------------------------------//
// Revision    | versioned using SVN                               //
//-----------------------------------------------------------------//
// Notes       |                                                   //
//-----------------------------------------------------------------//
// Attention!! | Attention!! If these structures are released to   //
//             | market in any form, the position of each data     //
//             | entry must not be moved any longer! Enhancements  //
//             | can be done by exploiting the dummy entries and   //
//             | dummy fields.                                     //
//-----------------------------------------------------------------//
// (c) 2003-2014 PCO AG * Donaupark 11 * D-93309 Kelheim / Germany //
// *  Phone: +49 (0)9441 / 2005-0  *                               //
// *  Fax:   +49 (0)9441 / 2005-20 *  Email: info@pco.de           //
//-----------------------------------------------------------------//


//-----------------------------------------------------------------//
// Revision History:                                               //
//-----------------------------------------------------------------//
// Rev.:     | Date:      | Changed:                               //
// --------- | ---------- | ---------------------------------------//
//  0.01     | 10.06.2003 |  MBL/new file                          //
//-----------------------------------------------------------------//
//  0.02     | 26.06.2003 |  LWA: Checksum added to all telegrams  //
//           |            |       (see also comments)              //
//-----------------------------------------------------------------//
//  0.12     | xx.07.2003 |  New: REQUEST IMAGE command /LWA       //
//-----------------------------------------------------------------//
//  0.13     | 24.07.2003 |  Removed:                              //
//           |            |  - ISOCHRONOUS_START_MSG structure     //
//           |            |  - ISOCHRONOUS_STOP_MSG structure      //
//           |            |  - INTERNAL_SEND_IMAGES_MSG structure  //
//           |            |  - INTERNAL_STOP_IMAGES_MSG structure  //
//           |            |    (not used any longer)               //
//-----------------------------------------------------------------//
//  0.14     | 01.09.2003 |  Renamed: /LWA                         //
//           |            |  - SC2_Preview_Mode_Response to        //
//           |            |      SC2_Liveview_Mode_Response        //
//           |            |  - SC2_Set_Preview_Mode to             //
//           |            |      SC2_Set_Liveview_Mode             //
//           |            |  Due to renaming Preview to Live View. //
//-----------------------------------------------------------------//
//  0.15     | 01.09.2003 |  Changed /LWA                          //
//           |            |  Removed: - SC2_Image_Ready_Message    //
//           |            |  Added:   - SC2_Image_Avail_Message    //
//           |            |           - SC2_No_Image_Avail_Message //
//           |            |           - SC2_Record_On_Message      //
//           |            |           - SC2_Record_Off_Message     //
//           | 24.09.2003 |  Added /LWA                            //
//           |            |    - SC2_Load_COC_Data                 //
//           |            |    - SC2_Send_COC_Command              //
//           | 16.10.2003 |  Added /MBL                            //
//           |            |    - SC2_COC_Operation_Mode_Response   //
//           |            |    - SC2_Set_COC_Operation_Mode        //
//           |            |    - SC2_COC_Runtime_Response          //
//-----------------------------------------------------------------//
//  0.16     | 19.11.2003 |  Changed LWA                           //
//           |            |  - SC2_Camera_Description_Response     //
//           |            |    changed. Added descriptors for ROI  //
//           |            |    stepping and min. delay / exposure  //
//           |            |    time steps.                         //
//           |            |  - SC2_Camera_Health_Status_Response   //
//           |            |    Status field included               //
//-----------------------------------------------------------------//
//  0.17     | 19.11.2003 |  Added LWA                             //
//           |            |  - structures for fixed data area,     //
//           |            |    like camera serial no, hardware     //
//           |            |    version, etc.                       //
//           | 16.01.2004 |  Changed LWA                           //
//           |            |  - Hardware revison (structure         //
//           |            |    (SC2_HWREVISION) changed: now one   //
//           |            |    revision code as word instead of    //
//           |            |    two bytes wih minor and major rev.  //
//           | 03.02.2004 |  Added MBL                             //
//           |            |    Chip descriptor color_pattern,      //
//           |            |    pattern typ                         //
//           |            |                                        //
//           | 17.02.2004 |  Added structures: LWa                 //
//           |            |  - Start_DSNU_Calibration              //
//           |            |  - Start_DSNU_Calibration_Response     //
//           |            |  - Set_DSNU_Correction_Mode            //
//           |            |  - Get_DSNU_Correction_Mode_Response   //
//           |            |                                        //
//           | 26.02.2004 |  Added structures: LWa                 //
//           |            |  - SC2_Read_Head_EE_Data               //
//           |            |  - SC2_Write_Head_EE_Data              //
//           |            |  - SC2_Head_EE_Data_Response           //
//           |            |                                        //
//-----------------------------------------------------------------//
//  0.18     | 22.03.2004 |  Changed LWA                           //
//           |            |  - reorganized update failure tele-    //
//           |            |    grams such that they are similar to //
//           |            |    standard failure telegram, i.e.     //
//           |            |    failure code is dword after size.   //
//           | 02.04.2004 |  Added telegrams: /LWA                 //
//           |            |  - SC2_Get_FPS_Exposure_Mode           //
//           |            |  - SC2_Set_FPS_Exposure_Mode           //
//           |            |  - SC2_FPS_Exposure_Mode_Response      //
//           | 25.06.2004 |  Added telegrams: /FRE                 //
//           |            |  - SC2_Get_Bit_Alignment               //
//           |            |  - SC2_Bit_Alignment_Response          //
//-----------------------------------------------------------------//
//  0.19     | 01.07.2004 |  Changed LWA                           //
//           |            |  - the member dwtime_us in structure   //
//           |            |    SC2_COC_Runtime_Response changed to //
//           |            |    dwtime_ns.                          //
//           | 06.07.2004 |  Added telegrams: /MBL CameraLink      //
//           |            |  - SC2_Set_CL_Baudrate                 //
//           |            |  - SC2_Get_CL_Baudrate                 //
//           |            |  - SC2_Set_CL_Configuration            //
//           |            |  - SC2_Get_CL_Confuguration            //
//-----------------------------------------------------------------//
//  0.20     | 23.07.2004 |  LWA:  telegrams for internal use like //
//           |            |        debug and update telegrams      //
//           |            |        moved to SC2_TELEGRAM_INTERN.H  //
//           | 16.09.2004 |  LWA:  telegrams added:                //
//           |            |                                        //
//           |            |  SC2_IEEE1394_Iso_Byte_Order_Response  //
//           |            |  SC2_Get_IEEE1394_Iso_Byte_Order       //
//           |            |                                        //
//           |            |  Used for OEM camera, not for          //
//           |            |  pco.camera series                     //
//           |            |                                        //
//           | 19.10.2004 |  LWA:  telegrams added:                //
//           |            |                                        //
//           |            |  SC2_Repeat_Image                      //
//           |            |  SC2_Repeat_Image_Response             //
//           |            |    (commands sends last image again)   //
//           |            |  SC2_Cancel_Image_Transfer             //
//           |            |  SC2_Cancel_Image_Transfer_Response    //
//           |            |    (aborts running image transfer)     //
//-----------------------------------------------------------------//
//  0.21     | 11.04.2005 |  LWA:  added telegrams:                //
//           |            |  SC2_Get_Noise_Filter_Mode             //
//           |            |  SC2_Set_Noise_Filter_Mode             //
//           |            |  SC2_Noise_Filter_Mode_Response        //
//           |            |  SC2_Camera_Desription: new member     //
//           |            |    wNoiseFilterDESC                    //
//-----------------------------------------------------------------//
//  0.21     | 04.05.2005 |  LWA:  changed telegram:               //
//           |            |  SC2_Camera_Desription:                //
//           |            |    - dwGeneralCaps1 added              //
//           |            |    - wNoiseFilterDESC removed          //
//-----------------------------------------------------------------//
//  0.22     | 19.10.2005 |  MBL:  definitionen aus intern:        //
//           |            |    - SC2_Image_Transfer_Done_Message   //
//           |            |                                        //
//           | 27.02.2006 |  LWA:  added telegrams:                //
//           |            |  SC2_Camera_Name_Response              //
//           |            |                                        //
//           | 09.03.2006 |  LWA:  added telegrams:                //
//           |            |  SC2_Get_Hot_Pixel_Correction_Mode     //
//           |            |  SC2_Set_Hot_Pixel_Correction_Mode     //
//           |            |  SC2_Hot_Pixel_Correction_Mode_Response//
//-----------------------------------------------------------------//
//  0.23     | 01.06.2006 |  Preparation for modulation mode:      //
//           |   (FRe)    |  Added modulation mode parameters and  //
//           |            |  telegrams:                            //
//           |            |    do_S(G)ET_MODULATION_MODE           //
//           |            |  Added second descriptor, flags and    //
//           |            |  telegram:                             //
//           |            |    do_GET_DESCRIPTION_EX               //
//           |            |  Changed header to local               //
//           |            |  c:\pco_include\include in order to    //
//           |            |   support header file repository       //
//-----------------------------------------------------------------//
//  0.24     | 19.09.2007 | FRE:Added tel. for GET_INFO_STRING     //
//-----------------------------------------------------------------//
//  0.25     | 05.03.2009 | FRE: Added Get/SetFrameRate            //
//           |            | Added HW IO functions and desc.        //
//           | 01.07.2009 | FRE:Add. SC2_Get_Image_Timing_Response //
//-----------------------------------------------------------------//
//  0.26     | 31.08.2010 | FRE:Added tel. for Lookup Tables       //
//-----------------------------------------------------------------//
//  0.27     | 09.12.2013 | RFR: Added telegrams for pco.flim      //
//           |            | The telegram names contain the         //
//           |            | additional element "FLIM":             //
//           |            | SC2_Set_FLIM_*, SC2_Get_FLIM_*         //
//-----------------------------------------------------------------//
//  0.28     | 17.02.2014 | AGR:  Added Get_Cooling_Setpoints      //
//-----------------------------------------------------------------//

  // LWA 26.06.03: Checksum byte was added because now, if declaring
  //               a telegram variable the checksum byte is included
  //               and writing the checksum byte will not fail.
  //               For the length of the telegram now take simply the
  //               sizeof operator: Telegram.Length = sizeof(Telegram)


#ifndef  SC2_TELEGRAM_H
#define  SC2_TELEGRAM_H

#include "sc2_common.h"

#define FWREVISION 0x00009000

#if defined(WIN32) || defined(__linux__)
//#pragma message("Structures packed to 1 byte boundary!")
#pragma pack(push) 
#pragma pack(1)            
#endif


#ifdef __MICROBLAZE__
#define struct struct __attribute__ ((packed))
#endif

#ifdef _PIC32_
#define struct struct __attribute__ ((packed))
#endif


// ================================================================================== //
// ======== FireWire address space definition for telegram transmission ============= //
// ================================================================================== //


#define IEEE1394_MasterResponseAddress  0xF8000000L

  // this is the FireWire address space for the master (PC / driver) to get   //
  // and detect response telegrams.   -> cameras write telegrams to address:  //
  //           0xFFFF_0000_0000 + <IEEE1394_MasterResponseAddress>            //


#define IEEE1394_MasterInterruptAddress 0xF9000000L

  // this is the FireWire address space for the master (PC / driver) to get   //
  // and detect telegrams sent by the camera initiated by itsself (error      //
  // messages etc.  -> cameras write (interrupt) telegrams to address:        //
  //           0xFFFF_0000_0000 + <IEEE1394_MasterInterruptAddress>           //


#define IEEE1394_CameraCommandAddress   0xFA000000L

  // this is the FireWire address space for the client (pco.camera) to get and //
  // detect command telegrams.   -> master (PC) write telegrams to address:    //
  //          0xFFFF_0000_0000 + <IEEE1394_CameraCommandAddress>               //



////////////////////////////////////////////////////////////////////////////////////////
// General Telegram Prototypes                                                        //
////////////////////////////////////////////////////////////////////////////////////////

typedef struct                         // telegram header prototype
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
} SC2_Telegram_Header;


typedef struct                         // general purpose telegram, 
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  BYTE        bData[268];              // longest telegram (update) padded to DWORD
} SC2_Telegram;


typedef struct                         // simple telegram with no parameters
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  BYTE        bCks;
} SC2_Simple_Telegram;



////////////////////////////////////////////////////////////////////////////////////////
// Telegrams for version management (firmware & hardware)                             //
////////////////////////////////////////////////////////////////////////////////////////

typedef struct           // structure for saving HW info in device flash/eeprom etc.
{
  WORD  wBatchNo; 
  WORD  wRevision;       // revision code
  WORD  wVariant;        // variant
}
SC2_HWVERSION;    


typedef struct           // structure for saving FW info in device flash/eeprom etc.
{
  BYTE  bMinorRev;       // use range 0 to  99
  BYTE  bMajorRev;       // use range 0 to 255
  WORD  wVariant;        // variant
  BYTE  bHour;           // build date of version
  BYTE  bMinute;
  BYTE  bDay;
  BYTE  bMonth;
  WORD  wYear;
}
SC2_FWVERSION;           // length is trimmed to 32 byte


typedef struct
{
  char  szName[16];      // string with board name
  WORD  wBatchNo;        // production batch no
  WORD  wRevision;       // revision code
  WORD  wVariant;        // variant
}
SC2_Hardware_DESC;


typedef struct
{
  WORD                   wCode;          // telegram code 
  WORD                   wSize;          // telegram length
  WORD                   BoardNum;       // number of devices
  SC2_Hardware_DESC      Board[10];
  BYTE                   bCks;           // checksum byte
}
SC2_Hardware_Versions_Response;    


typedef struct
{
  char  szName[16];      // string with device name
  BYTE  bMinorRev;       // use range 0 to 99
  BYTE  bMajorRev;       // use range 0 to 255
  WORD  wVariant;        // variant
}
SC2_Firmware_DESC;


typedef struct
{
  WORD                   wCode;           // telegram code 
  WORD                   wSize;           // telegram length
  WORD                   DeviceNum;       // number of devices
  SC2_Firmware_DESC      Device[10];
  BYTE                   bCks;            // checksum byte
}
SC2_Firmware_Versions_Response;    


#define SC2_Get_Hardware_Versions SC2_Simple_Telegram
#define SC2_Get_Firmware_Versions SC2_Simple_Telegram

typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  BYTE        bNum;                    // block enumeration (0->1...10, 1-> 11...20 ...)
  BYTE        bCks;                    // checksum byte
}
SC2_Get_Ext_Firmware_Versions;

#define SC2_Get_Ext_Hardware_Versions SC2_Get_Ext_Firmware_Versions


typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  WORD        wMailboxNo;              // No of mailbox
  BYTE        bData[11];               // data
  BYTE        bCks;                    // checksum byte
}
SC2_Write_Mailbox;

typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  WORD        wMailboxNo;              // No of mailbox
  BYTE        bCks;                    // checksum byte
}
SC2_Write_Mailbox_Response;


typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  WORD        wMailboxNo;              // 
  BYTE        bCks;                    // checksum byte
}
SC2_Read_Mailbox;

typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  WORD        wMailboxNo;              // 
  WORD        wReadStatus;             // status of operation
  BYTE        bData[11];               // data
  BYTE        bCks;                    // checksum byte
}
SC2_Read_Mailbox_Response;


typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  BYTE        bCks;                    // checksum byte
}
SC2_Get_Mailbox_Status;

typedef struct
{
  WORD        wCode;                    // telegram code 
  WORD        wSize;                    // telegram length
  WORD        wNumberOfMailboxes;       // number of mailboxes implemented
  WORD        wMailboxStatus[8];        // status of up to 8 mailboxes
  BYTE        bCks;                     // checksum byte
}
SC2_Get_Mailbox_Status_Response;

#define NUMSETUPFLAGS 4
typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  WORD        wType;                   // general type 
  DWORD       dwSetupFlags[NUMSETUPFLAGS];// Camera setup flags
  BYTE        bCks;                    // checksum byte
}
SC2_Set_Camera_Setup;

#define SC2_Set_Camera_Setup_Response SC2_Set_Camera_Setup

#define SC2_Get_Camera_Setup SC2_Set_Camera_Setup
#define SC2_Get_Camera_Setup_Response SC2_Set_Camera_Setup



/////////////////////////////////////////////////////////////////////
// Interface specific Commands 
/////////////////////////////////////////////////////////////////////

typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  WORD        wMasterNode;             // Master Node to respond to
  WORD        wIsochChannel;           // Used isochronous channel
  WORD        wIsochPacketLen;         // Length of a single packet
  WORD        wIsochPacketNum;         // Number of packets per image
  BYTE        bCks;                    // checksum byte
} SC2_IEEE1394_Interface_Params_Response;  

#define SC2_Set_IEEE1394_Interface_Params SC2_IEEE1394_Interface_Params_Response


typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  BYTE        bCks;                    // checksum byte
} SC2_Get_IEEE1394_Interface_Params;  


typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  WORD        wMode;                   // mode
  BYTE        bCks;                    // checksum byte
} SC2_IEEE1394_Iso_Byte_Order_Response;  


typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  BYTE        bCks;                    // checksum byte
} SC2_Get_IEEE1394_Iso_Byte_Order;  

#define SC2_Set_IEEE1394_Iso_Byte_Order SC2_IEEE1394_Iso_Byte_Order_Response


/////////////////////////////////////////////////////////////////////
// interface specific commands CameraLink
/////////////////////////////////////////////////////////////////////

typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  DWORD       dwBaudrate;              // Baudrate of CameraLink serial interface
  BYTE        bCks;                    // checksum byte
} SC2_Get_CL_Baudrate_Response;  

#define SC2_Set_CL_Baudrate SC2_Get_CL_Baudrate_Response




typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  DWORD       dwClockFrequency;        // Pixelclock in Hz
  BYTE        bCCline;                 // Usage of CameraLink CC1-CC4 lines 
  BYTE        bDataFormat;             // register with Testpattern and Datasplitter switch     
  BYTE        bTransmit;               // single or continuous transmitting images 
  BYTE        bCks;                    // checksum byte
} SC2_Get_CL_Configuration_Response;  

#define SC2_Set_CL_Configuration SC2_Get_CL_Configuration_Response



/////////////////////////////////////////////////////////////////////
// General Control/Status
/////////////////////////////////////////////////////////////////////


typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  DWORD       dwerrmess;               // 
  BYTE        bCks;                    // checksum byte
} SC2_Failure_Response;


typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  WORD        wCamType;                // Camera type
  WORD        wCamSubType;             // Camera sub type
  DWORD       dwSerialNumber;          // Serial number of camera
  DWORD       dwHWVersion;             // Hardware version number
  DWORD       dwFWVersion;             // Firmware version number
  WORD        wInterfaceType;          // Interface type
  BYTE        bCks;                    // checksum byte
} SC2_Camera_Type_Response;


typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  char        szName[40];              // camera name, null terminated
  BYTE        bCks;                    // checksum byte
} SC2_Camera_Name_Response;


typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  DWORD       dwWarnings;              // Warnings
  DWORD       dwErrors;                // Errors
  DWORD       dwStatus;                // Status info
  BYTE        bCks;                    // checksum byte
} SC2_Camera_Health_Status_Response;


typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  BYTE        bCks;                    // checksum byte
} SC2_Reset_Settings_To_Default_Response;


typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  DWORD       dwWarnings;              // Warnings
  DWORD       dwErrors;                // Errors
  BYTE        bCks;                    // checksum byte
} SC2_Initiate_Selftest_Procedure_Response;


typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  WORD        wCCDtemp;
  WORD        wCamtemp;
  WORD        wPStemp;
  BYTE        bCks;                    // checksum byte
} SC2_Temperature_Response;

typedef struct
{
 WORD        wCode;                   // telegram code 
 WORD        wSize;                   // telegram length
 SHORT       sCCDtemp;
 SHORT       sCamtemp;
 SHORT       sPStemp;
 BYTE        bCks;                    // checksum byte
} SC2_Temperatures_Response;

typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  DWORD       dwType;                  // info type
  BYTE        bCks;                    // checksum byte
} SC2_Get_Info_String;

typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  char        szName[40];              // info name, null terminated
  BYTE        bCks;                    // checksum byte
} SC2_Get_Info_String_Response;

typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  WORD        wFanMode;                // actual Mode 
  WORD        wFanMin;                 // minimum fan setting (0 = off) 
  WORD        wFanMax;                 // maximum fan setting (100 = standard) 
  WORD        wStepSize;               // step size for set value
  WORD        wSetValue;               // set value,
  WORD        wActualValue;            // actual value, set value may be overridden by camera
  WORD        wReserved;               // reserved for future use
  BYTE        bCks;                    // checksum byte
} SC2_Get_Fan_Control_Status_Response;

#define SC2_Get_Fan_Control_Status SC2_Simple_Telegram

typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  WORD        wFanMode;                // set mode,
  WORD        wSetValue;               // set value,
  WORD        wReserved;               // reserved for future use
  BYTE        bCks;                    // checksum byte
} SC2_Set_Fan_Control_Params;

#define SC2_Set_Fan_Control_Params_Response SC2_Set_Fan_Control_Params


typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  BYTE        bCks;                    // checksum byte
} SC2_Get_Powersave_Mode;


typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  WORD        wMode;                   // Powersave mode on, off
  WORD        wDelayMinutes;           // delay for switching to powersave
  BYTE        bCks;                    // checksum byte
} SC2_Set_Powersave_Mode;


typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  WORD        wMode;                   // Powersave mode on, off
  WORD        wDelayMinutes;           // delay for switching to powersave
  BYTE        bCks;                    // checksum byte
} SC2_Powersave_Mode_Response;


typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  BYTE        bCks;                    // checksum byte
} SC2_Get_Battery_Status;


typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  WORD        wBatteryType;            // Battery type, 0 = no batt. 0xFFFF = unknown
  WORD        wBatteryLevel;           // charge level in percent of full 
  WORD        wPowerStatus;            // power status of mains, battery etc.
  WORD        wRFU[3];                 // 4 words reserved for future use.
  BYTE        bCks;                    // checksum byte
} SC2_Battery_Status_Response;


typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length (including data)
  WORD        wID;                     // ID of destination register for writting
  BYTE        bData[256];              // data for writing into destination register (up to 256 Byte)
  BYTE        bCks;                    // checksum byte
} SC2_Set_External_Register;

typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  WORD        wID;                     // ID of destination register
  WORD        wDataSize;               // size of data written to destination ID
  BYTE        bCks;                    // checksum byte
} SC2_Set_External_Register_Response;

typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  WORD        wID;                     // ID of source register for reading
  BYTE        bCks;                    // checksum byte
} SC2_Get_External_Register;

typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length (including data)
  WORD        wID;                     // ID of source register
  BYTE        bData[256];              // data read from source register (max. 256 Byte)
  BYTE        bCks;                    // checksum byte
} SC2_Get_External_Register_Response;




/////////////////////////////////////////////////////////////////////
// Image Sensor Control
/////////////////////////////////////////////////////////////////////

typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // Sizeof this struct
  WORD        wSensorTypeDESC;         // Sensor type
  WORD        wSensorSubTypeDESC;      // Sensor subtype
  WORD        wMaxHorzResStdDESC;      // Maxmimum horz. resolution in std.mode       (10)
  WORD        wMaxVertResStdDESC;      // Maxmimum vert. resolution in std.mode
  WORD        wMaxHorzResExtDESC;      // Maxmimum horz. resolution in ext.mode
  WORD        wMaxVertResExtDESC;      // Maxmimum vert. resolution in ext.mode
  WORD        wDynResDESC;             // Dynamic resolution of ADC in bit
  WORD        wMaxBinHorzDESC;         // Maxmimum horz. binning                      (20)
  WORD        wBinHorzSteppingDESC;    // Horz. bin. stepping (0:bin, 1:lin)
  WORD        wMaxBinVertDESC;         // Maxmimum vert. binning
  WORD        wBinVertSteppingDESC;    // Vert. bin. stepping (0:bin, 1:lin)
  WORD        wRoiHorStepsDESC;        // Roi hor can be changed in these steps
  WORD        wRoiVertStepsDESC;       // Roi vert can be changed in these steps      (30)
  WORD        wNumADCsDESC;            // Number of ADCs in system
  DWORD       dwPixelRateDESC[4];      // Possible pixelrate in Hz                    (48)
  WORD        wConvFactDESC[4];        // Possible conversion factor in e/cnt         (56)
  WORD        wIRDESC;                 // IR enhancment possibility
  DWORD       dwMinDelayDESC;          // Minimum delay time in ns                    (62)
  DWORD       dwMaxDelayDESC;          // Maximum delay time in ms
  DWORD       dwMinDelayStepDESC;      // Minimum delay time step in ns               (70)
  DWORD       dwMinExposureDESC;       // Minimum exposure time in ns
  DWORD       dwMaxExposureDESC;       // Maximum exposure time in ms
  DWORD       dwMinExposureStepDESC;   // Minimum exposure time step in ns            (82)
  DWORD       dwMinDelayIRDESC;        // Minimum delay time in ns IR mode
  DWORD       dwMaxDelayIRDESC;        // Maximum delay time in ms IR mode            (90)
  DWORD       dwMinExposureIRDESC;     // Minimum exposure time in ns IR mode
  DWORD       dwMaxExposureIRDESC;     // Maximum exposure time in ms IR mode
  WORD        wTimeTableDESC;          // Timetable for exp/del possibility           (100)
  WORD        wDoubleImageDESC;        // Double image mode possibility
  SHORT       sMinCoolSetDESC;         // Minimum value for cooling
  SHORT       sMaxCoolSetDESC;         // Maximum value for cooling
  SHORT       sDefaultCoolSetDESC;     // Default value for cooling
  WORD        wPowerDownModeDESC;      // Power down mode possibility                 (110)
  WORD        wOffsetRegulationDESC;   // Offset regulation possibility
  WORD        wColorPattern;           // Color Pattern of color chip 
                                       // four nibbles (0,1,2,3) in word 
                                       //  ----------------- 
                                       //  | 3 | 2 | 1 | 0 |
                                       //  ----------------- 
                                       //   
                                       // describe row,column  2,2 2,1 1,2 1,1
                                       // 
                                       //   column1 column2
                                       //  ----------------- 
                                       //  |       |       |
                                       //  |   0   |   1   |   row1
                                       //  |       |       |
                                       //  -----------------
                                       //  |       |       |
                                       //  |   2   |   3   |   row2
                                       //  |       |       |
                                       //  -----------------
                                       // 
  WORD        wColorPatternTyp;        // Pattern type of color chip          
  WORD        wDSNUCorrectionModeDESC; // DSNU correction mode
  DWORD       dwGeneralCaps1;          // general capabilites descr. 1 see also SC2_DEFS.h (122)
  DWORD       dwGeneralCaps2;          // General capabilities 2 internal use see also SC2_DEFS_INTERN.h
  DWORD       dwExtSyncFrequency[2];   // lists two frequencies for external sync feature
  DWORD       dwReserved[4];           // 32bit dummy
  WORD        wReserved;                                                            //(152)
  BYTE        bCks;                    // checksum byte
} SC2_Camera_Description_Response;

typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // Sizeof this struct
  DWORD       dwMinPeriodicalTimeDESC2;// Minimum periodical time tp in (nsec)
  DWORD       dwMaxPeriodicalTimeDESC2;// Maximum periodical time tp in (msec)        (12)
  DWORD       dwMinPeriodicalConditionDESC2;// System imanent condition in (nsec)
                                       // tp - (td + te) must be equal or longer than
                                       // dwMinPeriodicalCondition
  DWORD       dwMaxNumberOfExposuresDESC2; // Maximum number of exporures possible        (20)
  LONG        lMinMonitorSignalOffsetDESC2; // Minimum monitor signal offset tm in (nsec)
                                       // if(td + tstd) > dwMinMon.)
                                       //   tm must not be longer than dwMinMon
                                       // else
                                       //   tm must not be longer than td + tstd
  DWORD       dwMaxMonitorSignalOffsetDESC2;// Maximum -''- in (nsec)                      
  DWORD       dwMinPeriodicalStepDESC2;// Minimum step for periodical time in (nsec)  (32)
  DWORD       dwStartTimeDelayDESC2;   // Minimum monitor signal offset tstd in (nsec)
                                       // see condition at dwMinMonitorSignalOffset
  DWORD       dwMinMonitorStepDESC2;   // Minimum step for monitor time in (nsec)     (40)
  DWORD       dwMinDelayModDESC2;      // Minimum delay time for modulate mode in (nsec)
  DWORD       dwMaxDelayModDESC2;      // Maximum delay time for modulate mode in (nsec)
  DWORD       dwMinDelayStepDESC2;     // Minimum delay time step for modulate mode in (nsec)(52)
  DWORD       dwMinExposureModDESC2;   // Minimum exposure time for modulate mode in (nsec)
  DWORD       dwMaxExposureModDESC2;   // Maximum exposure time for modulate mode in (nsec)(60)
  DWORD       dwMinExposureStepDESC2;  // Minimum exposure time step for modulate mode in (nsec)
  DWORD       dwModulateCapsDESC2;     // Modulate capabilities descriptor
  DWORD       dwReserved[16];                                                         //(132)
  BYTE        bCks;                    // checksum byte
} SC2_Camera_Description_2_Response;

typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  WORD        wType;                   // descriptor type
  BYTE        bCks;                    // checksum byte
} SC2_Get_Camera_Description;


typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  WORD        wFormat;
  BYTE        bCks;                    // checksum byte
} SC2_Sensor_Format_Response;

#define SC2_Set_Sensor_Format SC2_Sensor_Format_Response


typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  WORD        wROI_x0;
  WORD        wROI_y0;
  WORD        wROI_x1;
  WORD        wROI_y1;
  BYTE        bCks;                    // checksum byte
} SC2_ROI_Response;

#define SC2_Set_ROI SC2_ROI_Response


typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  WORD        wBinningx;
  WORD        wBinningy;
  BYTE        bCks;                    // checksum byte
} SC2_Binning_Response;


#define SC2_Set_Binning SC2_Binning_Response


typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  DWORD       dwPixelrate;
  BYTE        bCks;                    // checksum byte
} SC2_Pixelrate_Response;

#define SC2_Set_Pixelrate SC2_Pixelrate_Response


typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  WORD        wGain;
  BYTE        bCks;                    // checksum byte
} SC2_Conversion_Factor_Response;

#define SC2_Set_Conversion_Factor SC2_Conversion_Factor_Response


typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  WORD        wMode;
  BYTE        bCks;                    // checksum byte
} SC2_Double_Image_Mode_Response;

#define SC2_Set_Double_Image_Mode SC2_Double_Image_Mode_Response


typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  WORD        wMode;
  BYTE        bCks;                    // checksum byte
} SC2_ADC_Operation_Response;

#define SC2_Set_ADC_Operation SC2_ADC_Operation_Response


typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  WORD        wMode;
  BYTE        bCks;                    // checksum byte
} SC2_IR_Sensitivity_Response;

#define SC2_Set_IR_Sensitivity SC2_IR_Sensitivity_Response


typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  SHORT       sTemp;
  BYTE        bCks;                    // checksum byte
} SC2_Cooling_Setpoint_Response;

#define SC2_Set_Cooling_Setpoint SC2_Cooling_Setpoint_Response


typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  WORD        wMode;
  BYTE        bCks;                    // checksum byte
} SC2_Offset_Mode_Response;

#define SC2_Set_Offset_Mode SC2_Offset_Mode_Response


typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  WORD        wMode;
  BYTE        bCks;                    // checksum byte
} SC2_Noise_Filter_Mode_Response;

#define SC2_Set_Noise_Filter_Mode  SC2_Noise_Filter_Mode_Response
#define SC2_Get_Noise_Filter_Mode  SC2_Simple_Telegram

typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  WORD        wNumOfSignals;
  BYTE        bCks;                    // checksum byte
} SC2_Get_Num_HW_IO_Signals_Response;

typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  WORD        wNumSignal;              // signal to query
  BYTE        bCks;                    // checksum byte
} SC2_Get_HW_IO_Signal_Descriptor;

typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  char        szSignalName[4][24];     // signal name, null terminated
  WORD wSignalDefinitions;             // Flags showing signal options
                                       // 0x01: Signal can be enabled/disabled
                                       // 0x02: Signal is a status (output)
                                       // Rest: future use, set to zero!
  WORD wSignalTypes;                   // Flags showing the selectability of signal types
                                       // 0x01: TTL
                                       // 0x02: High Level TTL
                                       // 0x04: Contact Mode
                                       // 0x08: RS485 diff.
                                       // Rest: future use, set to zero!
  WORD wSignalPolarity;                // Flags showing the selectability
                                       // of signal levels/transitions
                                       // 0x01: Low Level active
                                       // 0x02: High Level active
                                       // 0x04: Rising edge active
                                       // 0x08: Falling edge active
                                       // Rest: future use, set to zero!
  WORD wSignalFilter;                  // Flags showing the selectability of filter
                                       // settings
                                       // 0x01: Filter can be switched off (t > ~65ns)
                                       // 0x02: Filter can be switched to medium (t > ~1us)
                                       // 0x04: Filter can be switched to high (t > ~100ms)
  BYTE        bCks;                    // checksum byte
} SC2_Get_HW_IO_Signal_Descriptor_Response;


typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  WORD        wMode; 
  WORD        wMul[4];                 // multiplier
  BYTE        bCks;                    // checksum byte
} SC2_Set_Bayer_Multiplier;

#define SC2_Get_Bayer_Multiplier           SC2_Simple_Telegram
#define SC2_Set_Bayer_Multiplier_Response  SC2_Set_Bayer_Multiplier
#define SC2_Get_Bayer_Multiplier_Response  SC2_Set_Bayer_Multiplier


typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  char        szCCM[3][3][8];          // matrix elements
  BYTE        bCks;                    // checksum byte
} SC2_Get_Color_Correction_Matrix_Response;

#define SC2_Get_Color_Correction_Matrix  SC2_Simple_Telegram


typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  WORD        wMode;                   // DSNU operating mode
  WORD        wRsrvd;                  // RFU
  BYTE        bCks;                    // checksum byte
} SC2_Set_DSNU_Adjust_Mode;


typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  BYTE        bCks;                    // checksum byte
} SC2_Get_DSNU_Adjust_Mode;


typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  WORD        wMode;                   // DSNU operating mode
  WORD        wRsrvd;                  // RFU
  BYTE        bCks;                    // checksum byte
} SC2_Get_DSNU_Adjust_Mode_Response;


typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  WORD        wMode;                   // DSNU init mode
  WORD        wRsrvd;                  // RFU
  BYTE        bCks;                    // checksum byte
} SC2_Init_DSNU_Adjustment;


typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  WORD        wMode;                   // DSNU init mode
  WORD        wRsrvd;                  // RFU
  BYTE        bCks;                    // checksum byte
} SC2_Init_DSNU_Adjustment_Response;


typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  WORD        wMode;                   // CDI operating mode
  WORD        wRsrvd;                  // RFU
  BYTE        bCks;                    // checksum byte
} SC2_Set_CDI_Mode;


typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  BYTE        bCks;                    // checksum byte
} SC2_Get_CDI_Mode;

#define SC2_Get_CDI_Mode_Response SC2_Set_CDI_Mode

/////////////////////////////////////////////////////////////////////
// hot pixel commands, belongs to sensor related commands
/////////////////////////////////////////////////////////////////////

typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  WORD        wMode;                   // mode
  BYTE        bCks;                    // checksum byte
} SC2_Hot_Pixel_Correction_Mode_Response;

#define SC2_Set_Hot_Pixel_Correction_Mode  SC2_Hot_Pixel_Correction_Mode_Response
#define SC2_Get_Hot_Pixel_Correction_Mode  SC2_Simple_Telegram


typedef struct
{
  WORD        wX;                   // HotPixel X-value 
  WORD        wY;                   // HotPixel y-value
}
SC2_HOTPIX;


typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  WORD        wListNo;                 // list no to access several lists
  WORD        wIndex;                  // index of this telegram (0 = first)
  WORD        wNumValid;               // number of valid pixels in total!
  SC2_HOTPIX  strHotPix[64];           // array of 64 Hot Pixels
  BYTE        bCks;                    // checksum
} SC2_WRITE_HOT_PIXEL_LIST;


typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  WORD        wListNo;                 // list no to access several lists
  WORD        wIndex;                  // no. of this telegram (0 = first)
  BYTE        bCks;                    // checksum
} SC2_WRITE_HOT_PIXEL_LIST_RESPONSE;


typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  WORD        wListNo;                 // list no to access several lists
  WORD        wIndex;                  // index of this telegram (0 = first)
  BYTE        bCks;                    // checksum
} SC2_READ_HOT_PIXEL_LIST;


typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  WORD        wListNo;                 // list no to access several lists
  WORD        wIndex;                  // index of this telegram (0 = first)
  WORD        wNumValid;               // number of valid pixels in total!
  WORD        wNumMax;                 // max. number of pixels for this list
  SC2_HOTPIX  strHotPix[64];           // array of 64 Hot Pixels
  BYTE        bCks;                    // checksum
} SC2_READ_HOT_PIXEL_LIST_RESPONSE;


typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  WORD        wListNo;                 // list no to access several lists
  DWORD       dwMagic1;                // Magic1 and Magic2 for prohibiting
  DWORD       dwMagic2;                //   unauthorized or accidental clearing
  BYTE        bCks;                    // checksum
} SC2_CLEAR_HOT_PIXEL_LIST;


typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  WORD        wListNo;                 // list no to access several lists
  BYTE        bCks;                    // checksum
} SC2_CLEAR_HOT_PIXEL_LIST_RESPONSE;

/////////////////////////////////////////////////////////////////////
// lookup table commands, belongs to sensor related commands (edge)
/////////////////////////////////////////////////////////////////////

#define SC2_Get_Lookuptable_Info SC2_Simple_Telegram

typedef struct
{
  WORD          wCode;                   // telegram code
  WORD          wSize;                   // telegram length
  WORD          wIndex;                  // for requesting more than 10 LUT descs!
  BYTE          bCks;
}
SC2_Get_Lookuptable_Info_Ext;


typedef struct
{
  char        Description[20];         // e.g. "HD/SDI 12 to 10"
  WORD        wIdentifier;             // define loadable LUTs, range 0x0001 to 0xFFFF
  BYTE        bInputWidth;             // maximal Input in Bits
  BYTE        bOutputWidth;            // maximal Output in Bits
  WORD        wFormat;                 // accepted data structures (see defines)
}
SC2_LUT_DESC;


typedef struct
{
  WORD          wCode;                   // telegram code
  WORD          wSize;                   // telegram length
  WORD          wLutNumber;              // number of LUTs
  SC2_LUT_DESC  LutDesc[10];
  BYTE          bCks;                    // checksum byte
} SC2_Get_Lookuptable_Info_Response;


typedef struct
{
  WORD        wCode;                   // telegram code
  WORD        wSize;                   // telegram length
  WORD        wIdentifier;             // define LUT to be activated, 0x0000 for no LUT
  WORD        wParameter;              // optional parameter
  BYTE        bCks;
} SC2_Set_Lookuptable;

#define SC2_Set_Lookuptable_Response SC2_Set_Lookuptable
#define SC2_Get_Lookuptable          SC2_Simple_Telegram
#define SC2_Get_Lookuptable_Response SC2_Set_Lookuptable


typedef struct
{
  WORD        wCode;                   // telegram code
  WORD        wSize;                   // telegram length
  WORD        wIdentifier;             // define LUT to be loaded
  WORD        wPacketNum;              // for loading LUT in several steps
  WORD        wFormat;                 // data structure in *bData (see defines)
  WORD        wLength;                 // valid number of bytes within this call
  BYTE        bData[256];
  BYTE        bCks;                    // checksum byte
} SC2_Load_Lookuptable;

//  NOTE:
//    no additional parameters as max. telegram length is already reached!


typedef struct
{
  WORD        wCode;                   // telegram code
  WORD        wSize;                   // telegram length
  WORD        wIdentifier;             // define LUT to be loaded
  WORD        wPacketNum;              // for loading LUT in several steps
  WORD        wFormat;                 // data structure in *bData (see defines)
  WORD        wLength;                 // valid number of bytes within this call
  BYTE        bCks;                    // checksum byte
} SC2_Load_Lookuptable_Response;


typedef struct
{
  WORD        wCode;                   // telegram code
  WORD        wSize;                   // telegram length
  WORD        wIdentifier;             // define LUT to be loaded
  WORD        wPacketNum;              // for loading LUT in several steps
  BYTE        bCks;                    // checksum byte
} SC2_Read_Lookuptable;

typedef struct
{
  WORD        wCode;                   // telegram code
  WORD        wSize;                   // telegram length
  WORD        wIdentifier;             // define LUT to be loaded
  WORD        wPacketNum;              // for loading LUT in several steps
  WORD        wFormat;                 // data structure in *bData (see defines)
  WORD        wLength;                 // valid number of bytes within this call
  BYTE        bData[256];
  BYTE        bCks;                    // checksum byte
} SC2_Read_Lookuptable_Response;

//  NOTE:
//    no additional parameters as max. telegram length is already reached!


/////////////////////////////////////////////////////////////////////
// cooling setpoint commands, belongs to sensor related commands (edge)
/////////////////////////////////////////////////////////////////////


typedef struct
{
  WORD        wCode;                                    // telegram code 
  WORD        wSize;                                    // telegram length
  WORD        wBlockID;                                 // current block ID; usually 0; in case more than 10 setpoints are valid use next block
  BYTE        bCks;                                     // checksum byte
} SC2_Get_Cooling_Setpoints;

typedef struct
{
  WORD        wCode;                                    // telegram code 
  WORD        wSize;                                    // telegram length
  WORD        wBlockID;                                 // current block ID; usually 0; in case more than 10 setpoints are valid use next block
  WORD        wValidSetPoints;                          // number of valid setpoints; if (valid / 10) > 0 query next block(s)
  SHORT       sSetPoints[10];
  BYTE        bCks;                                     // checksum byte
} SC2_Get_Cooling_Setpoints_Response;


/////////////////////////////////////////////////////////////////////
// timing control 
/////////////////////////////////////////////////////////////////////

typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  WORD        wTimebaseDelay;
  WORD        wTimebaseExposure;
  BYTE        bCks;                    // checksum byte
} SC2_Timebase_Response;

#define SC2_Set_Timebase SC2_Timebase_Response


typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  DWORD       dwDelay;
  DWORD       dwExposure;
  BYTE        bCks;                    // checksum byte
} SC2_Delay_Exposure_Response;

#define SC2_Set_Delay_Exposure SC2_Delay_Exposure_Response


typedef struct 
{
  DWORD       dwDelay;
  DWORD       dwExposure;
} SC2_Delay_Exposure_pair;


typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  SC2_Delay_Exposure_pair delexp[16];
  BYTE        bCks;                    // checksum byte
} SC2_Delay_Exposure_Table_Response;

#define SC2_Set_Delay_Exposure_Table SC2_Delay_Exposure_Table_Response


typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  WORD        wMode;                   // mode to be set
  BYTE        bCks;                    // checksum byte
} SC2_Set_FPS_Exposure_Mode;


typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  WORD        wMode;                   // current mode
  DWORD       dwExposure;
  BYTE        bCks;                    // checksum byte
} SC2_FPS_Exposure_Mode_Response;

#define SC2_Get_FPS_Exposure_Mode SC2_Simple_Telegram


typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  BYTE        bCks;                    // checksum byte
} SC2_Get_Framerate;


typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  WORD        wStatus;                 // return status of set operation
  DWORD       dwFramerate;             // requested fram rate in mHz
  DWORD       dwExposure;              // requested exposure time in ns
  BYTE        bCks;                    // checksum byte
} SC2_Get_Framerate_Response;


typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  WORD        wMode;                   // mode to be set
  DWORD       dwFramerate;             // requested fram rate in mHz
  DWORD       dwExposure;              // requested exposure time in ns
  BYTE        bCks;                    // checksum byte
} SC2_Set_Framerate;


typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  WORD        wStatus;                 // return status of set operation
  DWORD       dwFramerate;             // requested fram rate in mHz
  DWORD       dwExposure;              // requested exposure time in ns
  BYTE        bCks;                    // checksum byte
} SC2_Set_Framerate_Response;


typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  DWORD       dwtime_s;                  
  DWORD       dwtime_ns;
  BYTE        bCks;                    // checksum byte
}SC2_COC_Runtime_Response;


typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  DWORD       dwtime_s;                  
  DWORD       dwtime_ns;
  BYTE        bCks;                    // checksum byte
}SC2_COC_Exptime_Response;


typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  WORD        wMode;
  BYTE        bCks;                    // checksum byte
} SC2_Trigger_Mode_Response;

#define SC2_Set_Trigger_Mode SC2_Trigger_Mode_Response


typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  WORD        wMode;
  BYTE        bCks;                    // checksum byte
} SC2_Camera_Sync_Mode_Response;

#define SC2_Set_Camera_Sync_Mode SC2_Camera_Sync_Mode_Response
#define SC2_Get_Camera_Sync_Mode SC2_Simple_Telegram


typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  WORD        wReturn;
  BYTE        bCks;                    // checksum byte
} SC2_Force_Trigger_Response;


typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  WORD        wMode;                   // PIV mode 0 = off, 1 = on, others RFU
  WORD        wReserved[4];            // RFU, set to 0
  BYTE        bCks;                    // checksum byte
} SC2_Set_Fast_Timing_Mode;


typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  WORD        wMode;                   // PIV mode 0 = off, 1 = on, others RFU
  WORD        wReserved[4];            // RFU, set to 0
  BYTE        bCks;                    // checksum byte
} SC2_Set_Fast_Timing_Mode_Response;


typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  BYTE        bCks;                    // checksum byte
} SC2_Get_Fast_Timing_Mode;


typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  WORD        wMode;                   // PIV mode 0 = off, 1 = on, others RFU
  WORD        wReserved[4];            // RFU, set to 0
  BYTE        bCks;                    // checksum byte
} SC2_Get_Fast_Timing_Mode_Response;


typedef struct
{
  WORD        wCode;                   // telegram code
  WORD        wSize;                   // telegram length
  WORD        wStatus;
  BYTE        bCks;                    // checksum byte
} SC2_Camera_Busy_Status_Response;

typedef struct
{
  WORD        wCode;                   // telegram code
  WORD        wSize;                   // telegram length
  DWORD       dwStatus;                // Status signal flags
  DWORD       dwImageCount;            // Image count of last finished image
  DWORD       dwReserved1;             // for future use, set to zero
  DWORD       dwReserved2;             // for future use, set to zero
  BYTE        bCks;                    // checksum byte
} SC2_Camera_Sensor_Signal_Status_Response;


typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  WORD        wPdnMode;
  BYTE        bCks;                    // checksum byte
} SC2_Power_Down_Mode_Response;

#define SC2_Set_Power_Down_Mode SC2_Power_Down_Mode_Response


typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  DWORD       dwPdnTime;
  BYTE        bCks;                    // checksum byte
} SC2_User_Power_Down_Time_Response;

#define SC2_Set_User_Power_Down_Time SC2_User_Power_Down_Time_Response


typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  WORD        wStatus;
  BYTE        bCks;                    // checksum byte
} SC2_ExpTrig_Signal_Status_Response;

typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  WORD        wModulationMode;         // Mode for modulation (0 = modulation off, 1 = modulation on)
  DWORD       dwPeriodicalTime;        // Periodical time (unit depending on timebase) for modulation
  WORD        wTimebasePeriodical;     // timebase for periodical time for modulation  0 -> ns, 1 -> us, 2 -> ms
  DWORD       dwNumberOfExposures;     // Number of exposures during modulation
  LONG        lMonitorOffset;          // Monitor offset value
  BYTE        bCks;                    // checksum byte
} SC2_Modulation_Mode_Response;

#define SC2_Set_Modulation_Mode SC2_Modulation_Mode_Response

typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  WORD        wNumSignal;              // signal to query
  BYTE        bCks;                    // checksum byte
} SC2_Get_HW_IO_Signal;

typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  WORD        wEnabled;                // Flag shows enable state of the signal (0: off, 1: on)
  WORD        wType;                   // Selected signal type
  WORD        wPolarity;               // Selected signal polarity
  WORD        wFilterSetting;          // Selected signal filter
  WORD        wSelected;               // Select signal
  BYTE        bCks;                    // checksum byte
} SC2_Get_HW_IO_Signal_Response;

typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  WORD        wNumSignal;              // signal to query
  WORD        wEnabled;                // Flag shows enable state of the signal (0: off, 1: on)
  WORD        wType;                   // Selected signal type
  WORD        wPolarity;               // Selected signal polarity
  WORD        wFilterSetting;          // Selected signal filter
  WORD        wSelected;               // Select signal
  BYTE        bCks;                    // checksum byte
} SC2_Set_HW_IO_Signal;


//#define SC2_Set_HW_IO_Signal_Response SC2_Set_HW_IO_Signal 

  // Note:
  //  
  // The pco.dimax as well as the pco.edge do not send a SC2_Set_HW_IO_Signal_Response as
  // define above but the SC2_Get_HW_IO_Signal_Response! The wrong behaviour is implemented
  // from beginning (dimax and edge).
  // Therefore not the behaviour of the cameras will be changed, but the structure of the
  // response telegram will be changed to represent the actual behaviour!

typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  WORD        wEnabled;                // Flag shows enable state of the signal (0: off, 1: on)
  WORD        wType;                   // Selected signal type
  WORD        wPolarity;               // Selected signal polarity
  WORD        wFilterSetting;          // Selected signal filter
  WORD        wSelected;               // Select signal
  BYTE        bCks;                    // checksum byte
} SC2_Set_HW_IO_Signal_Response;


typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  DWORD       FrameTime_ns;            // Frametime replaces COC_Runtime
  DWORD       FrameTime_s;   
  DWORD       ExposureTime_ns;
  DWORD       ExposureTime_s;
  DWORD       TriggerSystemDelay_ns;   // System internal min. trigger delay
  DWORD       TriggerSystemJitter_ns;  // Max. possible trigger jitter -0/+ ... ns
  DWORD       TriggerDelay_ns;         // Resulting trigger delay = system delay
  DWORD       TriggerDelay_s;          // + delay of SetDelayExposureTime ...
  DWORD       Reserved[4];
  BYTE        bCks;                    // checksum byte
} SC2_Get_Image_Timing_Response;


typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  WORD        wParameter;              // Parameter (on/off)
  WORD        wTimebase;               // timebase for dwLineTime
  DWORD       dwLineTime;              // time for one line
  DWORD       dwReserved[4];           // Reserved
  BYTE        bCks;                    // checksum byte
} SC2_Set_CMOS_Line_Timing;

#define SC2_Set_CMOS_Line_Timing_Response SC2_Set_CMOS_Line_Timing

#define SC2_Get_CMOS_Line_Timing          SC2_Simple_Telegram
#define SC2_Get_CMOS_Line_Timing_Response SC2_Set_CMOS_Line_Timing

typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  DWORD       dwExposureLines;         // number of lines for Exposure
  DWORD       dwDelayLines;            // number of lines between 2 frames
  DWORD       dwReserved[4];           // Reserved
  BYTE        bCks;                    // checksum byte
} SC2_Set_CMOS_Line_Exposure_Delay;

#define SC2_Set_CMOS_Line_Exposure_Delay_Response SC2_Set_CMOS_Line_Exposure_Delay

#define SC2_Get_CMOS_Line_Exposure_Delay          SC2_Simple_Telegram
#define SC2_Get_CMOS_Line_Exposure_Delay_Response SC2_Set_CMOS_Line_Exposure_Delay



typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  WORD        wNumSignal;              // signal to query
  WORD        wSelected;               // select to query (range 0 .. 3)
  BYTE        bCks;                    // checksum byte
  
} SC2_Get_HW_IO_Signal_Timing;


typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  WORD        wNumSignal;              // signal to query
  WORD        wSelected;               // select to query (range 0 .. 3)
  DWORD       dwType;                  // type of queried signal 
  DWORD       dwParameter;             // actual selected parameter
  DWORD       dwReserved[4];           // for future use
  BYTE        bCks;                    // checksum byte
  
} SC2_Get_HW_IO_Signal_Timing_Response;

typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  WORD        wNumSignal;              // signal to query
  WORD        wSelected;               // select to query (range 0 .. 3)
  DWORD       dwParameter;             // actual selected parameter
  DWORD       dwReserved[4];           // for future use
  BYTE        bCks;                    // checksum byte
} SC2_Set_HW_IO_Signal_Timing;


#define SC2_Set_HW_IO_Signal_Timing_Response SC2_Get_HW_IO_Signal_Timing_Response


typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  DWORD       dwParameter;             // Parameter (off/ ... /on)
  BYTE        bCks;                    // checksum byte
} SC2_Set_HW_LED_Signal;

#define SC2_Set_HW_LED_Signal_Response    SC2_Set_HW_LED_Signal
#define SC2_Get_HW_LED_Signal             SC2_Simple_Telegram
#define SC2_Get_HW_LED_Signal_Response    SC2_Set_HW_LED_Signal



/////////////////////////////////////////////////////////////////////
// storage control
/////////////////////////////////////////////////////////////////////

typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  DWORD       dwRamSize;
  WORD        wPageSize;
  BYTE        bCks;                    // checksum byte
} SC2_Camera_RAM_Size_Response;


typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  DWORD       dwSegment1Size;
  DWORD       dwSegment2Size;
  DWORD       dwSegment3Size;
  DWORD       dwSegment4Size;
  BYTE        bCks;                    // checksum byte
} SC2_Camera_RAM_Segment_Size_Response;

#define SC2_Set_Camera_RAM_Segment_Size SC2_Camera_RAM_Segment_Size_Response


typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  BYTE        bCks;                    // checksum byte
} SC2_Clear_RAM_Segment_Response;


typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  WORD        wSegment;
  BYTE        bCks;                    // checksum byte
} SC2_Active_RAM_Segment_Response;

#define SC2_Set_Active_RAM_Segment SC2_Active_RAM_Segment_Response



/////////////////////////////////////////////////////////////////////
// recording control
/////////////////////////////////////////////////////////////////////

typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  WORD        wMode;
  BYTE        bCks;                    // checksum byte
} SC2_Storage_Mode_Response;

#define SC2_Set_Storage_Mode SC2_Storage_Mode_Response


typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  WORD        wMode;
  BYTE        bCks;                    // checksum byte
} SC2_Recorder_Submode_Response;

#define SC2_Set_Recorder_Submode SC2_Recorder_Submode_Response


typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  WORD        wState;
  BYTE        bCks;                    // checksum byte
} SC2_Recording_State_Response;

#define SC2_Set_Recording_State SC2_Recording_State_Response


typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  BYTE        bCks;                    // checksum byte
} SC2_Arm_Camera_Response;


typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  WORD        wMode;
  BYTE        bCks;                    // checksum byte
} SC2_Acquire_Mode_Response;

#define SC2_Set_Acquire_Mode SC2_Acquire_Mode_Response


typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  BYTE        bCks;                    // checksum byte
} SC2_Get_Acquire_Control;


typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  WORD        wMode;
  BYTE        bCks;                    // checksum byte
} SC2_Set_Acquire_Control;


typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  WORD        wMode;
  BYTE        bCks;                    // checksum byte
} SC2_Acquire_Control_Response;


typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  WORD        wMode;                   // Acquire Mode
  DWORD       dwNumberImages;          // number of Images per Sequencer  
  DWORD       dwReserved[4];           // reserved for future use
  BYTE        bCks;                    // checksum byte
} SC2_Acquire_Mode_Ex_Response;

#define SC2_Set_Acquire_Mode_Ex      SC2_Acquire_Mode_Ex_Response 
#define SC2_Get_Acquire_Mode_Ex      SC2_Simple_Telegram


typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  WORD        wStatus;
  BYTE        bCks;                    // checksum byte
} SC2_acqenbl_Signal_Status_Response;


typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  BYTE        bDay;
  BYTE        bMonth;
  WORD        wYear;
  WORD        wHours;
  BYTE        bMinutes;
  BYTE        bSeconds;
  BYTE        bCks;                    // checksum byte
} SC2_Date_Time_Response;

#define SC2_Set_Date_Time SC2_Date_Time_Response


typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  WORD        wMode;
  BYTE        bCks;                    // checksum byte
} SC2_Timestamp_Mode_Response;

#define SC2_Set_Timestamp_Mode SC2_Timestamp_Mode_Response


typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  WORD        wMode;                   // 0 = 
  DWORD       dwDelayImages;           //   
  BYTE        bCks;                    // checksum byte
} SC2_Record_Stop_Event_Response;

#define SC2_Set_Record_Stop_Event SC2_Record_Stop_Event_Response 
#define SC2_Get_Record_Stop_Event SC2_Simple_Telegram


typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  WORD        wReserved0;              // rfu, set to 0
  DWORD       dwReserved1;             // rfu, set to 0  
  BYTE        bCks;                    // checksum byte
} SC2_Stop_Record_Response;

#define SC2_Stop_Record SC2_Stop_Record_Response 


typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  WORD        wReserved;               // reserved for future use  
  BYTE        bCks;                    // checksum byte
} SC2_Get_Event_Monitor_Configuration;


typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  WORD        wConfig;                 // 0 = 
  WORD        wReserved0;              //   
  WORD        wReserved1;              //   
  WORD        wReserved2;              //   
  BYTE        bCks;                    // checksum byte
} SC2_Set_Event_Monitor_Configuration;


typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  WORD        wConfig;                 // 0 = 
  WORD        wReserved0;              //   
  WORD        wReserved1;              //   
  WORD        wReserved2;              //   
  BYTE        bCks;                    // checksum byte
} SC2_Event_Monitor_Configuration_Response;


typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  WORD        wIndex;                  // index for reading a complete list
  WORD        wReserved;               //   
  BYTE        bCks;                    // checksum byte
} SC2_Get_Event_List;


typedef struct
{
  DWORD  dwImageNumber;              // Number of Image (same as for ReadImage command!)
  BYTE   bEVENT_TIME_US_BCD[3];      // 0x000000 to 0x999999, where first byte is least significant byte
  BYTE   bEVENT_TIME_SEC_BCD;        // 0x00 to 0x59
  BYTE   bEVENT_TIME_MIN_BCD;        // 0x00 to 0x59
  BYTE   bEVENT_TIME_HOUR_BCD;       // 0x00 to 0x23
  BYTE   bEVENT_TIME_DAY_BCD;        // 0x01 to 0x31  
  BYTE   bEVENT_TIME_MON_BCD;        // 0x01 to 0x12
  BYTE   bEVENT_TIME_YEAR_BCD;       // 0x00 to 0x99 only last two digits, 2000 has to be added
  BYTE   bFiller;                    
  WORD   wEventMask;                 // which events occured, see also Mode parameter of  Set Event
} SC2_EVENT_LIST_ENTRY;              //   Monitor Configuration


typedef struct
{
  WORD        wCode;                    // telegram code 
  WORD        wSize;                    // telegram length
  WORD        wIndex;                   // index for reading a complete list
  WORD        wMaxEvents;               // max. size of event list (entries) (Camera feature)
  WORD        wValidEvents;             // current number of events in list (total)
  WORD        wValidEventsInTelegram;   // valid number of events in this response telegram! 

  SC2_EVENT_LIST_ENTRY   strEvent[8];  // 8 events per call

  BYTE        bCks;                    // checksum byte
} SC2_Get_Event_List_Response;


/////////////////////////////////////////////////////////////////////
// image read commands
/////////////////////////////////////////////////////////////////////

typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  WORD        wSegment;
  WORD        wRes_hor;
  WORD        wRes_ver;
  WORD        wBin_x;
  WORD        wBin_y;
  WORD        wRoi_x0;
  WORD        wRoi_y0;
  WORD        wRoi_x1;
  WORD        wRoi_y1;
  BYTE        bCks;                    // checksum byte
} SC2_Segment_Image_Settings_Response;

// Note: was formerly: SC2_Image_Settings_related_to_Segment_Response;


typedef struct                        // Structure for telegram
{
  WORD        wCode;                // command code
  WORD        wSize;                 // length of command
  WORD        wSegment;
  BYTE        bCks;                    // checksum byte
} SC2_Segment_Image_Settings;


// Note: was formerly: SC2_Image_Settings_related_to_Segment;


typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  WORD        wSegment;
  DWORD       dwValid;
  DWORD       dwMax;
  BYTE        bCks;                    // checksum byte
} SC2_Number_of_Images_Response;


typedef struct                         // Structure for telegram
{
  WORD        wCode;                   // command code
  WORD        wSize;                   // length of command
  WORD        wSegment;
  BYTE        bCks;                    // checksum byte
} SC2_Number_of_Images;


typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  WORD        wSegment;
  DWORD       dwStartImage; 
  DWORD       dwLastImage; 
  BYTE        bCks;                    // checksum byte
} SC2_Read_Images_from_Segment_Response;

#define SC2_Read_Images_from_Segment SC2_Read_Images_from_Segment_Response


typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  BYTE        bCks;                    // checksum byte
} SC2_Request_Image_Response;

#define SC2_Request_Image SC2_Request_Image_Response


typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  WORD        Reserved[4];             // reserved for future use, set to 0!
  BYTE        bCks;                    // checksum byte
} SC2_Repeat_Image_Response;

#define SC2_Repeat_Image SC2_Repeat_Image_Response


typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  WORD        Reserved;                // reserved for future use, set to 0!
  BYTE        bCks;                    // checksum byte
} SC2_Cancel_Image_Transfer_Response;

#define SC2_Cancel_Image_Transfer SC2_Cancel_Image_Transfer_Response


typedef struct
{
  WORD        wCode;               // command code
  WORD        wSize;               // length of command
  WORD        wAlignment;          // bit alignment
  BYTE        bCks;                // checksum byte
} SC2_Bit_Alignment_Response;

#define SC2_Set_Bit_Alignment SC2_Bit_Alignment_Response 
#define SC2_Get_Bit_Alignment SC2_Simple_Telegram


typedef struct
{
  WORD        wCode;               // command code
  WORD        wSize;               // length of command
  BYTE        bCks;                // checksum byte
} SC2_Get_Metadata_Mode;


typedef struct
{
  WORD        wCode;               // command code
  WORD        wSize;               // length of command
  WORD        wMode;               // mode 0x0000 = w/o, 0x0001 w. metadata
  WORD        wReserved1;          // RFU, set to 0x0000
  WORD        wReserved2;          // RFU, set to 0x0000
  BYTE        bCks;                // checksum byte
} SC2_Set_Metadata_Mode;


typedef struct
{
  WORD        wCode;               // command code
  WORD        wSize;               // length of command
  WORD        wMode;               // mode 0x0000 = w/o, 0x0001 w. metadata
  WORD        wReserved1;          // RFU, set to 0x0000
  WORD        wReserved2;          // RFU, set to 0x0000
  WORD        wMetadataSize;       // size in byte = size in pixel
  WORD        wMetadataVersion;    // version of the metadata
  BYTE        bCks;                // checksum byte
} SC2_Metadata_Mode_Response;

#define PCO_METADATA_VERSION         0x0001     // current version!


typedef struct
{
  WORD        wCode;               // command code
  WORD        wSize;               // length of command
  WORD        wReserved;           // RFU, set to 0
  BYTE        bCks;                // checksum byte
} SC2_Get_Color_Settings;


typedef struct
{
  WORD        wCode;               // command code
  WORD        wSize;               // length of command
  SHORT       sSaturation;         // Saturation from -100 to 100, 0 is default
  SHORT       sVibrance;           // Vibrance   from -100 to 100, 0 is default
  WORD        wColorTemp;          // Color Temperature from 2000 t0 20000 K
  SHORT       sTint;               // Tint       from -100 to 100, 0 is default
  WORD        wMulNormR;           // for  setting color ratio (when not using Color
  WORD        wMulNormG;           //   Temp. and Tint! 1000 corresponds to 1.0
  WORD        wMulNormB;           //   normalized: wMulNorm(R + G + B) / 3 = 1000!
  SHORT       sContrast;           // Contrast   from -100 to 100, 0 is default
  WORD        wGamma;              // Gamma * 0.01 from 40 to 250 => 0.40 to 2.5
  WORD        wSharpFixed;         // 0 = off, 100 = max.
  WORD        wSharpAdaptive;      // 0 = off, 100 = max.
  WORD        wScaleMin;           // 0 to 4095
  WORD        wScaleMax;           // 0 to 4095
  WORD        wProcOptions;        // Processing Options as bit mask
  WORD        wReserved1;    
  WORD        wReserved2;    
  WORD        wReserved3;    
  BYTE        bCks;                // checksum byte
} SC2_Set_Color_Settings;

#define SC2_Get_Color_Settings_Response   SC2_Set_Color_Settings
#define SC2_Set_Color_Settings_Response   SC2_Set_Color_Settings


typedef struct
{
  WORD        wCode;               // command code
  WORD        wSize;               // length of command
  WORD        wMode;               // 0x0001 = do white balance on next transmitted image
                                   // other values reserved for future use

  WORD        wParam[4];           // not used now, meaning will depend on wMode, set to 0x0000!

  BYTE        bCks;                // checksum byte
} SC2_Do_White_Balance;


typedef struct
{
  WORD        wCode;               // command code
  WORD        wSize;               // length of command
  WORD        wMode;               // 0x0001 = do white balance on next transmitted image
                                   // other values reserved for future use

  WORD        wParam[4];           // not used now, meaning will depend on wMode

  BYTE        bCks;                // checksum byte
} SC2_White_Balance_Response;


typedef struct
{
  WORD        wCode;               // command code
  WORD        wSize;               // length of command
  BYTE        bCks;                // checksum byte
} SC2_Get_White_Balance_Status;


typedef struct
{
  WORD        wCode;               // command code
  WORD        wSize;               // length of command
  WORD        wStatus;             // 0x0001 = do white balance on next transmitted image
  WORD        wColorTemp;
  SHORT       sTint;
  WORD        wReserved[4];        // not used now, meaning will depend on wMode
  BYTE        bCks;                // checksum byte
} SC2_White_Balance_Status_Response;



/////////////////////////////////////////////////////////////////////
// Telegram structures for HD/SDI image transfer
/////////////////////////////////////////////////////////////////////

typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  WORD        wSegment;
  WORD        wInterface;
  WORD        wMode;
  WORD        wSpeed; 
  DWORD       dwRangeLow; 
  DWORD       dwRangeHigh;
  DWORD       dwStartPos;
  BYTE        bCks;                    // checksum byte
} SC2_Play_Images_from_Segment;

#define SC2_Play_Images_from_Segment_Response SC2_Play_Images_from_Segment


typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  BYTE        bCks;                    // checksum byte
} SC2_Get_Play_Position;


typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  WORD        wStatus;                 // current status
  DWORD       dwPosition;              // current play position
  BYTE        bCks;                    // checksum byte
} SC2_Get_Play_Position_Response;	


typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  WORD        wSegment;
  WORD        wMode1;
  WORD        wMode2;
  DWORD       dwSetPos1; 
  DWORD       dwClrPos1; 
  DWORD       dwSetPos2; 
  DWORD       dwClrPos2; 
  DWORD       dwSetPos3; 
  DWORD       dwClrPos3; 
  DWORD       dwSetPos4; 
  DWORD       dwClrPos4; 
  BYTE        bCks;                    // checksum byte
} SC2_Set_Video_Payload_Identifier;

#define SC2_Set_Video_Payload_Identifier_Response SC2_Set_Video_Payload_Identifier


typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  WORD        wInterface;
  BYTE        bCks;                    // checksum byte
} SC2_Get_Interface_Output_Format;

typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  WORD        wInterface;
  WORD        wFormat;
  WORD        wReserved1; 
  WORD        wReserved2; 
  BYTE        bCks;                    // checksum byte
} SC2_Set_Interface_Output_Format;

#define SC2_Set_Interface_Output_Format_Response SC2_Set_Interface_Output_Format
#define SC2_Get_Interface_Output_Format_Response SC2_Set_Interface_Output_Format


typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  WORD        wInterface;
  BYTE        bCks;                    // checksum byte
} SC2_Get_Interface_Status;

typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  WORD        wInterface;
  WORD        wReserved0;
  DWORD       dwWarnings;              // Warnings, bit flags
  DWORD       dwErrors;                // Errors,   bit flags
  DWORD       dwStatus;                // Status info, bit flags
    // Note: Meaning of dwWarnings, dwErrors and dwStatus will
    //       depend on selected interface!
  DWORD       dwReserved[4];           
  BYTE        bCks;                    // checksum byte
} SC2_Get_Interface_Status_Response;


typedef struct
{
  WORD        wCode;               // telegram code 
  WORD        wSize;               // telegram length
  BYTE        bCks;                // checksum byte
} SC2_Get_Image_Transfer_Mode;

typedef struct
{
  WORD        wCode;               // telegram code 
  WORD        wSize;               // telegram length
  WORD        wMode;               // transfer mode, e.g. full, scaled, cutout etc.
  WORD        wImageWidth;         // original image width
  WORD        wImageHeight;        // original image height
  WORD        wTxWidth;            // width of transferred image (scaled or cutout)
  WORD        wTxHeight;           // width of transferred image (scaled or cutout)
  WORD        wTxLineWordCnt;      // Number of WORDs of a line (padded, if not divisible by two)
  WORD        wParam[8];           // word params, meaning depends on selected mode
  BYTE        bCks;                // checksum byte
} SC2_Set_Image_Transfer_Mode;

typedef struct
{
  WORD        wCode;               // telegram code 
  WORD        wSize;               // telegram length
  WORD        wMode;               // transfer mode, e.g. full, scaled, cutout etc.
  WORD        wImageWidth;         // original image width
  WORD        wImageHeight;        // original image height
  WORD        wTxWidth;            // width of transferred image (scaled or cutout)
  WORD        wTxHeight;           // width of transferred image (scaled or cutout)
  WORD        wTxLineWordCnt;      // Number of WORDs of a line (padded, if not divisible by two)
  WORD        wParam[8];           // word params, meaning depends on selected mode
  BYTE        bCks;                // checksum byte
} SC2_Image_Transfer_Mode_Response;



/////////////////////////////////////////////////////////////////////
// "Interrupt" Messages sent from camera to PC
/////////////////////////////////////////////////////////////////////

//#define SC2_Image_Avail_Message           SC2_Simple_Telegram
//#define SC2_No_Image_Avail_Message        SC2_Simple_Telegram
//#define SC2_Record_Off_Message            SC2_Simple_Telegram

typedef struct
{
  WORD        wCode;               // telegram code
  WORD        wSize;               // telegram length
  DWORD       dwImageDataCount;    // number of image data in byte, which is sent to the host
  BYTE        bCks;                // checksum byte
} SC2_Image_Transfer_Done_Message;



/////////////////////////////////////////////////////////////////////
// pco.flim Configuration/Control
/////////////////////////////////////////////////////////////////////

typedef struct
{
  WORD wCode;           // telegram code
  WORD wSize;           // telegram length
  WORD wSourceSelect;   // modulation source (internal/external)
  WORD wOutputWaveform; // modulation output waveform
  WORD wReserved1;      // reserved for future use
  WORD wReserved2;      // reserved for future use
  BYTE bCks;            // checksum byte
} SC2_FLIM_Modulation_Params;

#define SC2_Set_FLIM_Modulation_Params          SC2_FLIM_Modulation_Params
#define SC2_Set_FLIM_Modulation_Params_Response SC2_FLIM_Modulation_Params

#define SC2_Get_FLIM_Modulation_Params          SC2_Simple_Telegram
#define SC2_Get_FLIM_Modulation_Params_Response SC2_FLIM_Modulation_Params

typedef struct
{
  WORD wCode;          // telegram code
  WORD wSize;          // telegram length
  WORD wPhaseNumber;   // number of phases per modulation period
  WORD wPhaseSymmetry; // modulation taps gather each phase information singularly by the appropriate tap or symmetrically by both taps
  WORD wPhaseOrder;    // recording order of (symmetrically) gathered phases
  WORD wTapSelect;     // additional selection of one of both or both taps
  WORD wReserved1;     // reserved for future use
  WORD wReserved2;     // reserved for future use
  BYTE bCks;           // checksum byte
} SC2_FLIM_Phase_Sequence_Params;

#define SC2_Set_FLIM_Phase_Sequence_Params          SC2_FLIM_Phase_Sequence_Params
#define SC2_Set_FLIM_Phase_Sequence_Params_Response SC2_FLIM_Phase_Sequence_Params

#define SC2_Get_FLIM_Phase_Sequence_Params          SC2_Simple_Telegram
#define SC2_Get_FLIM_Phase_Sequence_Params_Response SC2_FLIM_Phase_Sequence_Params

typedef struct
{
  WORD wCode;                // telegram code
  WORD wSize;                // telegram length
  WORD wAsymmetryCorrection; // averaging mode of both taps holding the same phase information
  WORD wCalculationMode;     // reserved for future use (method/parameters for phasor calculation etc.)
  WORD wReferencingMode;     // reserved for future use (sequence is stored as reference, reference is used etc.)
  WORD wThresholdLow;        // reserved for future use (lower threshold for clipping calculated pixel data containing no information)
  WORD wThresholdHigh;       // reserved for future use (upper threshold for clipping calculated pixel data containing no information)
  WORD wOutputMode;          // reserved for future use (image output format and types)
  WORD wReserved1;           // reserved for future use
  WORD wReserved2;           // reserved for future use
  WORD wReserved3;           // reserved for future use
  WORD wReserved4;           // reserved for future use
  BYTE bCks;                 // checksum byte
} SC2_FLIM_Image_Processing_Flow;

#define SC2_Set_FLIM_Image_Processing_Flow          SC2_FLIM_Image_Processing_Flow
#define SC2_Set_FLIM_Image_Processing_Flow_Response SC2_FLIM_Image_Processing_Flow

#define SC2_Get_FLIM_Image_Processing_Flow          SC2_Simple_Telegram
#define SC2_Get_FLIM_Image_Processing_Flow_Response SC2_FLIM_Image_Processing_Flow

typedef struct
{
  WORD  wCode;       // telegram code
  WORD  wSize;       // telegram length
  DWORD dwFrequency; // modulation frequency in Hz
  BYTE  bCks;        // checksum byte
} SC2_FLIM_Master_Modulation_Frequency;

#define SC2_Set_FLIM_Master_Modulation_Frequency          SC2_FLIM_Master_Modulation_Frequency
#define SC2_Set_FLIM_Master_Modulation_Frequency_Response SC2_FLIM_Master_Modulation_Frequency

#define SC2_Get_FLIM_Master_Modulation_Frequency          SC2_Simple_Telegram
#define SC2_Get_FLIM_Master_Modulation_Frequency_Response SC2_FLIM_Master_Modulation_Frequency

/*
typedef struct
{
  WORD  wCode;           // telegram code
  WORD  wSize;           // telegram length
  DWORD dwFrequency[16]; // modulation frequency table in Hz
  BYTE  bCks;            // checksum byte
} SC2_FLIM_Master_Modulation_Frequency_Table;

#define SC2_Set_FLIM_Master_Modulation_Frequency_Table          SC2_FLIM_Master_Modulation_Frequency_Table
#define SC2_Set_FLIM_Master_Modulation_Frequency_Table_Response SC2_FLIM_Master_Modulation_Frequency_Table

#define SC2_Get_FLIM_Master_Modulation_Frequency_Table          SC2_Simple_Telegram
#define SC2_Get_FLIM_Master_Modulation_Frequency_Table_Response SC2_FLIM_Master_Modulation_Frequency_Table
*/

typedef struct
{
  WORD  wCode;           // telegram code
  WORD  wSize;           // telegram length
  DWORD dwPhaseMilliDeg; // relative phase between image sensor and modulation signal in milli-degrees
  BYTE  bCks;            // checksum byte
} SC2_FLIM_Relative_Phase;

#define SC2_Set_FLIM_Relative_Phase          SC2_FLIM_Relative_Phase
#define SC2_Set_FLIM_Relative_Phase_Response SC2_FLIM_Relative_Phase

#define SC2_Get_FLIM_Relative_Phase          SC2_Simple_Telegram
#define SC2_Get_FLIM_Relative_Phase_Response SC2_FLIM_Relative_Phase



#if defined(WIN32) || defined(__linux__)
//#pragma message("Structures packed back to normal!")
#pragma pack(pop)  
#endif



#ifdef __MICROBLAZE__
#undef struct
#endif

#ifdef _PIC32_
#undef struct
#endif

#endif  // -- ifndef SC2_TELEGRAM_H --------------------------------------- //



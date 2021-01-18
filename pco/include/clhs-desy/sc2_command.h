//-----------------------------------------------------------------//
// Name        | sc2_command.h               | Type: ( ) source    //
//-------------------------------------------|       (*) header    //
// Project     | SC2                         |       ( ) others    //
//-----------------------------------------------------------------//
// Platform    | - Embedded platforms like M16C, AVR32, PIC32 etc. //
//             | - PC with several Windows versions, Linux etc.    //
//-----------------------------------------------------------------//
// Environment | - Platform dependent                              //
//-----------------------------------------------------------------//
// Purpose     | SC2 - Command defines                             //
//-----------------------------------------------------------------//
// Author      | MBl/FRe/LWa/AGr and others, PCO AG                //
//-----------------------------------------------------------------//
// Revision    | versioned using SVN                               //
//-----------------------------------------------------------------//
// Notes       | Rev 0.23 covers eight groups of commands:         //
//             | 1. General control group.                         //
//             | 2. Image sensor control group.                    //
//             | 3. Timing control group.                          //
//             | 4. Storage control group.                         //
//             | 5. Recording control group.                       //
//             | 6. Image read group.                              //
//             |                                                   //
//-----------------------------------------------------------------//
// (c) 2002-2014 PCO AG * Donaupark 11 * D-93309 Kelheim / Germany //
// *  Phone: +49 (0)9441 / 2005-0  *                               //
// *  Fax:   +49 (0)9441 / 2005-20 *  Email: info@pco.de           //
//-----------------------------------------------------------------//


//-----------------------------------------------------------------//
// Revision History:                                               //
//-----------------------------------------------------------------//
// Rev.:     | Date:      | Changed:                               //
// --------- | ---------- | ---------------------------------------//
//  0.01     | 08.10.2002 |  new file                              //
//-----------------------------------------------------------------//
//  0.02     | 14.11.2002 |  added telegramm structures / FRE      //
//-----------------------------------------------------------------//
//  0.03     | 27.11.2002 |  changed union for ROI / FRE           //
//-----------------------------------------------------------------//
//  0.04     | 02.12.2002 |  added strGenset, rename all struct to //
//           |            |  start with str_... in name / FRE      //
//-----------------------------------------------------------------//
//  0.05     | 12.12.2002 |  Flipped Highbyte and Lowbyte in Cmd   //
//-----------------------------------------------------------------//
//  0.06     | 16.12.2002 |  Added Error codes for SC2MAIN / LWA   //
//           |            |  Added power control group / FRE       //
//-----------------------------------------------------------------//
//  0.07     | 07.01.2003 |  Added ReadEE and ProgEE / FRE         //
//-----------------------------------------------------------------//
//  0.08     | 14.03.2003 |  changed almost all to new             //
//           |            |  command structure /JRE                //
//-----------------------------------------------------------------//
//  0.09     | 06.06.2003 |  Error codes for SDRAM configuration   //
//           |            |  added /LWA                            //
//-----------------------------------------------------------------//
//  0.10     | 10.06.2003 |  NOERROR definition with ifndef        //
//           |            |  dekoriert  /MBL                       //
//-----------------------------------------------------------------//
//  0.11     | 25.06.2003 |  Error codes moved to -:\Entwicklung\  //
//           |            |  Common\Include\Vxx.xx\PCO_err.h / FRE //
//-----------------------------------------------------------------//
//  0.12     | xx.07.2003 |  New: REQUEST IMAGE command /LWA       //
//-----------------------------------------------------------------//
//  0.13     | 24.07.2003 |  Removed: /LWA                         //
//           |            |  - ISOCHRONOUS_START_MSG               //
//           |            |  - ISOCHRONOUS_STOP_MSG                //
//           |            |  - INTERNAL_SEND_IMAGES_MSG            //
//           |            |  - INTERNAL_STOP_IMAGES_MSG            //
//           |            |    (not used any longer)               //
//-----------------------------------------------------------------//
//  0.14     | 01.09.2003 |  Renamed: /LWA                         //
//           |            |  - SET_PREVIEW_MODE to                 //
//           |            |      SET_LIVEVIEW_MODE                 //
//           |            |  - GET_PREVIEW_MODE to                 //
//           |            |      GET_LIVEVIEW_MODE                 //
//           |            |  Due to renaming Preview to Live View. //
//-----------------------------------------------------------------//
//  0.15     | 08.09.2003 |  Removed: /LWA                         //
//           |            |  - IMAGE_READY_MSG to                  //
//           |            |  NEW:   - IMAGE_AVAIL_MSG              //
//           |            |         - NO_IMAGE_AVAIL_MSG           //
//           |            |         - RECORD_ON_MSG                //
//           |            |         - RECORD_OFF_MSG               //
//           | 24.09.2003 |  NEW: telegram identifiers:  /LWA      //
//           |            |         - LOAD_COC_DATA                //
//           |            |         - SEND_COC_COMMAND             //
//           |            |       for SEND_COC_COMMAND:            //
//           |            |         - COC_CMD_START                //
//           |            |         - COC_CMD_STOP                 //
//           |            |         - COC_CMD_INTR                 //
//           |            |         - COC_CMD_SWTRIG               //
//           |            |         - COC_CMD_RESET                //
//           |            |         - COC_CMD_SET_STOPVEC          //
//           |            |         - COC_CMD_SET_INTRVEC          //
//           | 16.10.2003 |  NEW: telegram identifiers:  /MBL      //
//           |            |         - GET_COC_MODE                 //
//           |            |         - SET_COC_MODE                 //
//           |            |         - GET_COC_RUNTIME              //
//           | 15.11.2003 |  NEW: debug telegram identifiers.      //
//-----------------------------------------------------------------//
//  0.17     | 15.12.2003 |  Removed: /LWA                         //
//           |            |  -                                     //
//           |            |  NEW:   - OPENFIXVAL                   //
//-----------------------------------------------------------------//
//  0.17     | 17.02.2004 |  Added new command IDs:          /LWA  //
//           |            |  - START_SELF_CALIBRATION              //
//           |            |  - GET_DSNU_CORRECTION_MODE            //
//           |            |  - SET_DSNU_CORRECTION_MODE            //
//-----------------------------------------------------------------//
//  0.17     | 26.02.2004 |  Added new command IDs:          /LWA  //
//           |            |  - READ_HEAD_EE_DATA                   //
//           |            |  - WRITE_HEAD_EE_DATA                  //
//-----------------------------------------------------------------//
//  0.18     | 02.04.2004 |  Added new command IDs:          /LWA  //
//           |            |  - GET_FPS_EXPOSURE_MODE               //
//           |            |  - SET_FPS_EXPOSURE_MODE               //
//-----------------------------------------------------------------//
//  0.19     | xx.07.2004 |  Added new command IDs:          /FRE  //
//           |            |  - GET_BIT_ALIGNMENT                   //
//           |            |  - SET_BIT_ALIGNMENT                   //
//-----------------------------------------------------------------//
//  0.20     | xx.07.2004 |  internal, debug and programming  /LWA //
//           |            |  telegrams moved to                    //
//           |            |  SC2_command_intern.h                  //
//           |            |  -                                     //
//           | 13.09.2004 |  Added new command IDs:           /LWA //
//           |            |  - GET_IEEE1394_ISO_BYTEORDER          //
//           |            |  - SET_IEEE1394_ISO_BYTEORDER          //
//           |            |  -                                     //
//           | 19.10.2004 |  Added new command IDs:           /LWA //
//           |            |  - REPEAT_IMAGE                        //
//           |            |  - CANCEL_IMAGE_TRANSFER               //
//-----------------------------------------------------------------//
//  0.21     | 11.04.2005 |  Added new command IDs:           /LWA //
//           |            |  - GET_NOISE_FILTER_MODE               //
//           |            |  - SET_NOISE_FILTER_MODE               //
//           |            |                                        //
//  0.21     | 27.06.2005 |  Added new command IDs:           /LWA //
//           |            |  - SET_RECORD_STOP_EVENT               //
//           |            |  - STOP_RECORD                         //
//-----------------------------------------------------------------//
//  0.22     | 27.02.2006 |  Added new command IDs:           /LWA //
//           |            |  - GET_CAMERA_NAME                     //
//           |            |                                        //
//           | 01.03.2006 |  Added new command IDs:           /LWA //
//           |            |  - WRITE_HOT_PIXEL_LIST                //
//           |            |  - READ_HOT_PIXEL_LIST                 //
//           |            |  - CLEAR_HOT_PIXEL_LIST                //
//           |            |                                        //
//           | 09.03.2006 |  Added new command IDs:           /LWA //
//           |            |  - GET_HOT_PIXEL_CORRECTION_MODE       //
//           |            |  - SET_HOT_PIXEL_CORRECTION_MODE       //
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
//  0.23     | 19.09.2007 |  Added GET_INFO_STRING            /FRE //
//           | 01.07.2009 | Add. GET_HW_IO_SIGNAL SET_HW_IO_SIGNAL //
//           |            | GET_IMAGE_TIMING /FRE                  //
//           | 27.04.2010 | Added GET_SENSOR_SIGNAL_STATUS / FRE   //
//-----------------------------------------------------------------//
//  0.24     | 31.08.2010 |  Added Lookup Table commands      /FRE //
//-----------------------------------------------------------------//
//  0.25     | 09.12.2013 |  Added pco.flim commands, new     /RFR //
//           |            |  group code 0x001B                     //
//-----------------------------------------------------------------//

#ifndef SC2_COMMAND_H
#define SC2_COMMAND_H


/////////////////////////////////////////////////////////////////////
// General telegram defines
/////////////////////////////////////////////////////////////////////


#define RESPONSE_OK_CODE     0x0080
#define RESPONSE_ERROR_CODE  0x00C0
 
  // if responding to a telegram or RESPONSE_OK_CODE to the sent   //
  // telegram ID of telegram was processed successful and          //
  // RESPONSE_ERROR_CODE in case of failure, e.g.:                 //
  //                                                               //
  // ResponseTele.wCode = CommandTele.wCode | RESPONSE_OK_CODE     //



/////////////////////////////////////////////////////////////////////
// General Control/Status
/////////////////////////////////////////////////////////////////////

#define GET_CAMERA_TYPE                         0x0110
#define GET_CAMERA_HEALTH_STATUS                0x0210
#define RESET_SETTINGS_TO_DEFAULT               0x0310
#define INITIATE_SELFTEST_PROCEDURE             0x0510
#define GET_TEMPERATURE                         0x0610
#define GET_HARDWARE_VERSIONS                   0x0710
#define GET_FIRMWARE_VERSIONS                   0x0810
#define GET_CAMERA_NAME                         0x0910
#define GET_INFO_STRING                         0x0A10
#define GET_FAN_CONTROL_STATUS                  0x0B10
#define SET_FAN_CONTROL_PARAMS                  0x0C10
#define GET_FIRMWARE_VERSIONS_EXT               0x0D10

#define WRITE_MAILBOX                           0x0E10
#define READ_MAILBOX                            0x0F10
#define GET_MAILBOX_STATUS                      0x1010

#define GET_CAMERA_SETUP                        0x1110
#define SET_CAMERA_SETUP                        0x1210

#define GET_POWERSAVE_MODE                      0x1310
#define SET_POWERSAVE_MODE                      0x1410
#define GET_BATTERY_STATUS                      0x1510

#define GET_EXTERNAL_REGISTER                   0x1610
#define SET_EXTERNAL_REGISTER                   0x1710


/////////////////////////////////////////////////////////////////////
// Image Sensor Control
/////////////////////////////////////////////////////////////////////

#define GET_CAMERA_DESCRIPTION                  0x0111
#define GET_ROI                                 0x0211
#define SET_ROI                                 0x0311
#define GET_BINNING                             0x0411
#define SET_BINNING                             0x0511
#define GET_PIXELRATE                           0x0611
#define SET_PIXELRATE                           0x0711
#define GET_CONVERSION_FACTOR                   0x0811
#define SET_CONVERSION_FACTOR                   0x0911
#define GET_DOUBLE_IMAGE_MODE                   0x0A11
#define SET_DOUBLE_IMAGE_MODE                   0x0B11
#define GET_ADC_OPERATION                       0x0C11
#define SET_ADC_OPERATION                       0x0D11
#define GET_IR_SENSITIVITY                      0x0E11
#define SET_IR_SENSITIVITY                      0x0F11
#define GET_COOLING_SETPOINT_TEMPERATURE        0x1011
#define SET_COOLING_SETPOINT_TEMPERATURE        0x1111
#define GET_OFFSET_MODE                         0x1211
#define SET_OFFSET_MODE                         0x1311
#define GET_SENSOR_FORMAT                       0x1411
#define SET_SENSOR_FORMAT                       0x1511
#define GET_CAMERA_DESCRIPTION_EX               0x1611
#define GET_NOISE_FILTER_MODE                   0x1911
#define SET_NOISE_FILTER_MODE                   0x1A11

#define GET_HOT_PIXEL_CORRECTION_MODE           0x1E11
#define SET_HOT_PIXEL_CORRECTION_MODE           0x1F11

#define WRITE_HOT_PIXEL_LIST                    0x2011
#define READ_HOT_PIXEL_LIST                     0x2111
#define CLEAR_HOT_PIXEL_LIST                    0x2211

#define GET_NUMBER_HW_IO_SIGNALS                0x2511
#define GET_HW_IO_SIGNAL_DESCRIPTION            0x2611

#define GET_BAYER_MULTIPLIER                    0x2711
#define SET_BAYER_MULTIPLIER                    0x2811
#define GET_COLOR_CORRECTION_MATRIX             0x2911

  // 0x2A11   reserved for internal use!
  // 0x2B11   reserved for internal use!

#define GET_DSNU_ADJUST_MODE                    0x2C11
#define SET_DSNU_ADJUST_MODE                    0x2D11
#define INIT_DSNU_ADJUSTMENT                    0x2E11

#define GET_CDI_MODE                            0x2F11
#define SET_CDI_MODE                            0x3011

#define GET_LOOKUPTABLE_INFO                    0x3111
#define GET_LOOKUPTABLE                         0x3211
#define SET_LOOKUPTABLE                         0x3311
#define LOAD_LOOKUPTABLE                        0x3411
#define READ_LOOKUPTABLE                        0x3511
#define GET_LOOKUPTABLE_INFO_EXT                0x3611

#define GET_COOLING_SETPOINTS                   0x3711

/////////////////////////////////////////////////////////////////////
// timing control 
/////////////////////////////////////////////////////////////////////

#define GET_DELAY_EXPOSURE_TIME                 0x0112
#define SET_DELAY_EXPOSURE_TIME                 0x0212
#define GET_TRIGGER_MODE                        0x0312
#define SET_TRIGGER_MODE                        0x0412
#define FORCE_TRIGGER                           0x0512
#define GET_CAMERA_BUSY_STATUS                  0x0612
#define GET_USER_POWER_DOWN_TIME                0x0712
#define SET_USER_POWER_DOWN_TIME                0x0812
#define GET_EXP_TRIG_SIGNAL_STATUS              0x0912
#define GET_DELAY_EXPOSURE_TIME_TABLE           0x0A12
#define SET_DELAY_EXPOSURE_TIME_TABLE           0x0B12
#define GET_TIMEBASE                            0x0C12
#define SET_TIMEBASE                            0x0D12
#define GET_POWER_DOWN_MODE                     0x0E12
#define SET_POWER_DOWN_MODE                     0x0F12
#define GET_COC_RUNTIME                         0x1012

#define GET_FPS_EXPOSURE_MODE                   0x1312 
#define SET_FPS_EXPOSURE_MODE                   0x1412
#define GET_MODULATION_MODE                     0x1512 
#define SET_MODULATION_MODE                     0x1612 
#define GET_FRAMERATE                           0x1712 
#define SET_FRAMERATE                           0x1812 
#define GET_HW_IO_SIGNAL                        0x1912
#define SET_HW_IO_SIGNAL                        0x1A12
#define GET_COC_EXPTIME                         0x1B12 
#define GET_CAMERA_SYNC_MODE                    0x1C12
#define SET_CAMERA_SYNC_MODE                    0x1D12
#define GET_IMAGE_TIMING                        0x1E12

#define GET_FAST_TIMING_MODE                    0x1F12
#define SET_FAST_TIMING_MODE                    0x2012

#define GET_SENSOR_SIGNAL_STATUS                0x2112

#define GET_CMOS_LINETIMING                     0x2212
#define SET_CMOS_LINETIMING                     0x2312
#define GET_CMOS_LINE_EXPOSURE_DELAY            0x2412
#define SET_CMOS_LINE_EXPOSURE_DELAY            0x2512

#define GET_HW_IO_SIGNAL_TIMING                 0x2612
#define SET_HW_IO_SIGNAL_TIMING                 0x2712
#define GET_HW_LED_SIGNAL                       0x2812
#define SET_HW_LED_SIGNAL                       0x2912

/////////////////////////////////////////////////////////////////////
// storage control
/////////////////////////////////////////////////////////////////////

#define GET_CAMERA_RAM_SIZE                     0x0113
#define GET_CAMERA_RAM_SEGMENT_SIZE             0x0213
#define SET_CAMERA_RAM_SEGMENT_SIZE             0x0313
#define CLEAR_RAM_SEGMENT                       0x0413
#define GET_ACTIVE_RAM_SEGMENT                  0x0513
#define SET_ACTIVE_RAM_SEGMENT                  0x0613


/////////////////////////////////////////////////////////////////////
// recording control
/////////////////////////////////////////////////////////////////////

#define GET_STORAGE_MODE                        0x0114
#define SET_STORAGE_MODE                        0x0214
#define GET_RECORDER_SUBMODE                    0x0314
#define SET_RECORDER_SUBMODE                    0x0414
#define GET_RECORDING_STATE                     0x0514
#define SET_RECORDING_STATE                     0x0614
#define ARM_CAMERA                              0x0A14
#define GET_ACQUIRE_MODE                        0x0714
#define SET_ACQUIRE_MODE                        0x0814
#define GET_ACQ_ENBL_SIGNAL_STATUS              0x0914
#define SET_DATE_TIME                           0x0B14
#define GET_TIMESTAMP_MODE                      0x0C14
#define SET_TIMESTAMP_MODE                      0x0D14
#define GET_RECORD_STOP_EVENT                   0x0E14
#define SET_RECORD_STOP_EVENT                   0x0F14
#define STOP_RECORD                             0x1014
#define GET_EVENT_MON_CONFIGURATION             0x1114
#define SET_EVENT_MON_CONFIGURATION             0x1214
#define GET_EVENT_LIST                          0x1314
#define GET_ACQUIRE_CONTROL                     0x1414
#define SET_ACQUIRE_CONTROL                     0x1514
#define GET_ACQUIRE_MODE_EX                     0x1614
#define SET_ACQUIRE_MODE_EX                     0x1714



/////////////////////////////////////////////////////////////////////
// image read commands
/////////////////////////////////////////////////////////////////////

#define GET_SEGMENT_IMAGE_SETTINGS              0x0115
#define GET_NUMBER_OF_IMAGES_IN_SEGMENT         0x0215


#define READ_IMAGES_FROM_SEGMENT                0x0515
#define REQUEST_IMAGE                           0x0615
#define CANCEL_IMAGE_TRANSFER                   0x0715
#define REPEAT_IMAGE                            0x0815
#define GET_BIT_ALIGNMENT                       0x0915
#define SET_BIT_ALIGNMENT                       0x0A15
#define PLAY_IMAGES_FROM_SEGMENT                0x0B15
#define GET_PLAY_POSITION                       0x0C15
#define SET_VIDEO_PAYLOAD_IDENTIFIER            0x0D15
#define GET_METADATA_MODE                       0x0E15
#define SET_METADATA_MODE                       0x0F15
#define GET_COLOR_SETTINGS                      0x1015
#define SET_COLOR_SETTINGS                      0x1115
#define DO_WHITE_BALANCE                        0x1215
#define GET_WHITE_BALANCE_STATUS                0x1315



/////////////////////////////////////////////////////////////////////
// interface specific commands IEEE1394
/////////////////////////////////////////////////////////////////////

#define GET_IEEE1394_INTERFACE_PARAMS           0x0116
#define SET_IEEE1394_INTERFACE_PARAMS           0x0216

#define GET_IEEE1394_ISO_BYTEORDER              0x0316
#define SET_IEEE1394_ISO_BYTEORDER              0x0416

#define GET_INTERFACE_OUTPUT_FORMAT             0x1116
#define SET_INTERFACE_OUTPUT_FORMAT             0x1016

#define GET_IMAGE_TRANSFER_MODE                 0x1216
#define SET_IMAGE_TRANSFER_MODE                 0x1316

#define GET_INTERFACE_STATUS                    0x1416




/////////////////////////////////////////////////////////////////////
// Telegram (messages) sent by the camera without command
/////////////////////////////////////////////////////////////////////

#define FATAL_ERROR_MSG                 0x0117

//#define IMAGE_AVAIL_MSG                 0x0517
//#define NO_IMAGE_AVAIL_MSG              0x0617

#define IMAGE_TRANSFER_DONE_MSG         0x0717
#define DEBUG_ENVELOPE                  0x0817



/////////////////////////////////////////////////////////////////////
// interface specific commands CameraLink
/////////////////////////////////////////////////////////////////////

#define GET_CL_BAUDRATE                         0x3216
#define SET_CL_BAUDRATE                         0x3316

#define GET_CL_CONFIGURATION                    0x3416
#define SET_CL_CONFIGURATION                    0x3516

/////////////////////////////////////////////////////////////////////
// other control commands only for CameraLink
// checksum is not tested for this commands !!!
/////////////////////////////////////////////////////////////////////

#define ASCII_COMMAND                           0x011A

#define ASCII_COMMAND_EXPOSURE "exp:"
#define ASCII_COMMAND_ROI      "roi:"
#define ASCII_COMMAND_BINNING  "bin:"

//lenght of decimal strings including seperator ':' and ','
#define ASCII_COMMAND_COM_LEN 4
#define ASCII_COMMAND_EXP_LEN 10
#define ASCII_COMMAND_ROI_LEN 6
#define ASCII_COMMAND_BIN_LEN 6


/////////////////////////////////////////////////////////////////////
// FLI-Cam Light Source Commands, Group Code 0x001A
/////////////////////////////////////////////////////////////////////

// LED driver

#define LS_COMMAND_LED_DRIVER_ON                  0x021A
#define LS_COMMAND_LED_DRIVER_OFF                 0x031A
#define LS_COMMAND_LED_DRIVER_GET_STATE           0x041A

// General commands

#define LS_COMMAND_SET_OPERATING_MODE             0x101A
#define LS_COMMAND_GET_OPERATING_MODE             0x111A
#define LS_COMMAND_FREQUENCY_UPDATE               0x121A

// Analog switch

#define LS_COMMAND_ANALOG_SWITCH_SINE             0x201A
#define LS_COMMAND_ANALOG_SWITCH_SQUARE           0x211A
#define LS_COMMAND_ANALOG_SWITCH_EXTERNAL         0x221A
#define LS_COMMAND_ANALOG_SWITCH_GROUND           0x231A
#define LS_COMMAND_ANALOG_SWITCH_GET_STATE        0x241A

// DDS

#define LS_COMMAND_DDS_SET_RELATIVE_PHASE         0x441A
#define LS_COMMAND_DDS_GET_RELATIVE_PHASE         0x451A


/////////////////////////////////////////////////////////////////////
// pco.flim Commands, Group Code 0x001B
/////////////////////////////////////////////////////////////////////

#define GET_FLIM_MODULATION_PARAMS                 0x011B
#define SET_FLIM_MODULATION_PARAMS                 0x021B

#define GET_FLIM_PHASE_SEQUENCE_PARAMS             0x031B
#define SET_FLIM_PHASE_SEQUENCE_PARAMS             0x041B

#define GET_FLIM_IMAGE_PROCESSING_FLOW             0x051B
#define SET_FLIM_IMAGE_PROCESSING_FLOW             0x061B

#define GET_FLIM_MASTER_MODULATION_FREQUENCY       0x071B
#define SET_FLIM_MASTER_MODULATION_FREQUENCY       0x081B

/*
#define GET_FLIM_MASTER_MODULATION_FREQUENCY_TABLE 0x091B
#define SET_FLIM_MASTER_MODULATION_FREQUENCY_TABLE 0x0A1B
*/

#define GET_FLIM_RELATIVE_PHASE                    0x0B1B
#define SET_FLIM_RELATIVE_PHASE                    0x0C1B


#endif//SC2_COMMAND_H



//-----------------------------------------------------------------//
// Name        | sc2_drv_struct.h            | Type: ( ) source    //
//-------------------------------------------|       (*) header    //
// Project     | SC2                         |       ( ) others    //
//-----------------------------------------------------------------//
// Platform    | WINDOWS 2000/XP                                   //
//-----------------------------------------------------------------//
// Environment | Microsoft Visual C++ 6.0                          //
//             |                                                   //
//-----------------------------------------------------------------//
// Purpose     | SC2 - header with structures                      //
//             | used from 1394.dll, cameralink.dll's              //
//             | and SC2_Cam.dll                                   //
//-----------------------------------------------------------------//
// Author      | MBL, PCO AG                                       //
//-----------------------------------------------------------------//
// Revision    | rev. 0.30 rel. 0.00                               //
//-----------------------------------------------------------------//
// Notes       |                                                   //
//             |                                                   //
//             |                                                   // 
//-----------------------------------------------------------------//
// (c) 2003 PCO AG * Donaupark 11 *                                //
// D-93309      Kelheim / Germany * Phone: +49 (0)9441 / 2005-0 *  //
// Fax: +49 (0)9441 / 2005-20 * Email: info@pco.de                 //
//-----------------------------------------------------------------//


//-----------------------------------------------------------------//
// Revision History:                                               //
//-----------------------------------------------------------------//
// Rev.:     | Date:      | Changed:                               //
// --------- | ---------- | ---------------------------------------//
//  0.30     | 17.12.2004 |  new file                              //
//           | 22.02.2005 |  new define REALSIZE                   //
// --------- | ---------- | ---------------------------------------//
//  1.00     | 18.09.2013 |  MBL                                   //
//           |            |  new struct PCO_SC2_SOFTROI_PARAM      //
//           |            |  new define SOFTROI_META_ENABLED       //
//-----------------------------------------------------------------//
//  0.0x     | xx.xx.200x |                                        //
//-----------------------------------------------------------------//

#ifndef  SC2_DRV_STRUCT_H
#define  SC2_DRV_STRUCT_H

//default values
#define PCO_SC2_DEF_BLOCK_SIZE 512
#define PCO_SC2_MIN_COMMAND_SIZE 5

#define PCO_SC2_COMMAND_TIMEOUT 200
#define PCO_SC2_IMAGE_TIMEOUT   1500
#define PCO_SC2_IMAGE_TIMEOUT_L 3000

//timeout values in ms
typedef struct _PCO_SC2_TIMEOUTS {
   unsigned int command;
   unsigned int image;
   unsigned int transfer;
}PCO_SC2_TIMEOUTS;


#define PCO_SC2_CL_ONLY_SERIAL_INIT 0x00001000
#define PCO_SC2_CL_CLSER_NAME       0x00002000
#define PCO_SC2_CL_CONFIG_NAME      0x00004000
#define PCO_SC2_CL_NO_INIT_COMMAND  0x00008000
#define PCO_SC2_CL_USE_COMPORTS     0x00000100

#define PCO_SC2_FW_REALSIZE         0x00010000

typedef struct _SC2_OpenStruct {
  char* clser_file_name;
  char* config_file_name;
  void* dummy1;
  void* dummy2;
  void* dummy3;
  DWORD dummy[5];
} SC2_OpenStruct;

typedef struct _PCO_SC2_IMAGE_PARAM {
 unsigned int width;
 unsigned int height;
 unsigned int xoff;
 unsigned int yoff;
}PCO_SC2_IMAGE_PARAM;

typedef struct _PCO_SC2_SOFTROI_PARAM {
/*
   unsigned int width;       //camera width (same as width in PCO_SC2_IMAGE_PARAM)
   unsigned int height;      //camera height (same as height in PCO_SC2_IMAGE_PARAM)
   unsigned int xskip;       //Pixels to skip at start of line
   unsigned int yskip;       //Lines to skip 
   unsigned int xsize;       //Pixels to copy
   unsigned int ysize;       //Lines to copy
*/
   unsigned int camera_width; //camera width (same as width in PCO_SC2_IMAGE_PARAM)
   unsigned int camera_height;//camera height (same as height in PCO_SC2_IMAGE_PARAM)
   unsigned int xskip_softroi;//Pixels to skip at start of line
   unsigned int yskip_softroi;//Lines to skip 
   unsigned int xsize_softroi;//Pixels to copy
   unsigned int ysize_softroi;//Lines to copy
   unsigned int flag;         //Flags 
   unsigned int meta_off_camera;     //offset of meta_data start in lines in camera buffer
//   unsigned int meta_lines_camera;   //number of additional lines for meta_data in camera buffer
   unsigned int meta_lines_softroi;  //number of additional lines for meta_data in output buffer
   unsigned int dummy;
   unsigned int dummy1[16];
}PCO_SC2_SOFTROI_PARAM;

#define SOFTROI_META_ENABLED   0x00000001  //camera does transfer meta_data in image
#define SOFTROI_DOUBLE_ENABLED 0x00000002  //camera does transfer double image

#define SOFTROI_TIMESTAMP_MASK      0x00000030
#define SOFTROI_TIMESTAMP_BINARY_ON 0x00000010
#define SOFTROI_TIMESTAMP_ASCII_ON  0x00000020

#define SOFTROI_TIMESTAMP_RAW       0x00000040

#define SOFTROI_TIMESTAMP_BINARY_SIZE   14
#define SOFTROI_TIMESTAMP_ASCII_OFFSET  20
#define SOFTROI_TIMESTAMP_ASCII_SIZE   272
#define SOFTROI_TIMESTAMP_ASCII_LINES 8

#define SOFTROI_TIMESTAMP_BINARY_SIZE_RAW   11
#define SOFTROI_TIMESTAMP_ASCII_OFFSET_RAW  15
#define SOFTROI_TIMESTAMP_ASCII_SIZE_RAW   204



//internal message levels for dll
#define INTERNAL_1_M 0x00010000
#define INTERNAL_2_M 0x00020000
#define INTERNAL_3_M 0x00040000
#define INTERNAL_4_M 0x00080000


//defines for general bits
#define NO_START_IMAGE        0x00000001
#define SET_LSB               0x00000002
#define LOSTIMAGE_ERROR       0x00000004
#define ONLY_VALID_IMA_EVENT  0x00000008

#define PRE_ALLOC_BUFFER      0x00000010
#define PRE_ALLOC_BUFFER_OFF  0x00000020

#define ME4_FAST_STOP         0x00010000
#define ME4_NO_HS             0x00020000
#define ME4_NO_FAST_STOP      0x00040000

#define GLOBAL_SHUTTER_SWCALC 0x00008000

#define FW_FREE_TRANSFER      0x00010000

#endif

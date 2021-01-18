//-----------------------------------------------------------------//
// Name        | clhs_register.h             | Type: ( ) source    //
//-------------------------------------------|       (*) header    //
// Project     | SC2                         |       ( ) others    //
//-----------------------------------------------------------------//
// Platform    | WINDOWS                                           //
//-----------------------------------------------------------------//
// Environment | Microsoft Visual Studio                           //
//             |                                                   //
//-----------------------------------------------------------------//
// Purpose     | CLHS Header                                       //
//-----------------------------------------------------------------//
// Author      | MBL, PCO AG                                       //
//-----------------------------------------------------------------//
// Revision    | rev. 0.01 rel. 0.00                               //
//-----------------------------------------------------------------//
// Notes       |                                                   //
//             |                                                   //
//             |                                                   // 
//-----------------------------------------------------------------//
// (c) 2014 PCO AG  * Donaupark 11 *                               //
// D-93309      Kelheim / Germany * Phone: +49 (0)9441 / 2005-0 *  //
// Fax: +49 (0)9441 / 2005-20 * Email: info@pco.de                 //
//-----------------------------------------------------------------//


//-----------------------------------------------------------------//
// Revision History:                                               //
//-----------------------------------------------------------------//
// Rev.:     | Date:      | Changed:                               //
// --------- | ---------- | ---------------------------------------//
//  0.01     | 16.10.2014 |  new file                              //
//-----------------------------------------------------------------//
//  0.0x     | xx.xx.200x |                                        //
//-----------------------------------------------------------------//
#ifndef CLHS_REGISTER_H
#define CLHS_REGISTER_H

#pragma pack(push)
#pragma pack(1)

//Bootstrap register addresse
typedef struct _CLHS_GENCP_REG32 {
  DWORD adr;
  DWORD size;
}CLHS_GENCP_REG32;

typedef struct _CLHS_GENCP_REG {
  uint64_t adr;
  uint32_t size;
}CLHS_GENCP_REG;


#pragma pack(pop)

#define ABRM_GENCP_VERSION(reg)              reg.adr=0x00000,reg.size=4;
#define ABRM_VENDOR(reg)                     reg.adr=0x00004,reg.size=64;
#define ABRM_MODEL(reg)                      reg.adr=0x00044,reg.size=64;
#define ABRM_FAMILY(reg)                     reg.adr=0x00084,reg.size=64;
#define ABRM_DEVICE_VERSION(reg)             reg.adr=0x000C4,reg.size=64;
#define ABRM_MANUFACTOR(reg)                 reg.adr=0x00104,reg.size=64;
#define ABRM_SERIAL(reg)                     reg.adr=0x00144,reg.size=64;
#define ABRM_USER_NAME(reg)                  reg.adr=0x00184,reg.size=64;
#define ABRM_DEVICE_CAP(reg)                 reg.adr=0x001C4,reg.size=8;
#define ABRM_MAX_RESPONSE(reg)               reg.adr=0x001CC,reg.size=4;
#define ABRM_MANIFEST_OFFSET(reg)            reg.adr=0x001D0,reg.size=8;
#define ABRM_SBMR_OFFSET(reg)                reg.adr=0x001D8,reg.size=8;
#define ABRM_DEVICE_CONFIG(reg)              reg.adr=0x001E0,reg.size=8;
#define ABRM_HEARTBEAT_TIMEOUT(reg)          reg.adr=0x001E8,reg.size=4;
#define ABRM_MESSAGE_CHANNEL_ID(reg)         reg.adr=0x001EC,reg.size=4;
#define ABRM_TIMESTAMP(reg)                  reg.adr=0x001F0,reg.size=8;
#define ABRM_TIMESTAMP_LATCH(reg)            reg.adr=0x001F8,reg.size=4;
#define ABRM_TIMESTAMP_INC(reg)              reg.adr=0x001FC,reg.size=8;
#define ABRM_ACCESS_PRIVILEG(reg)            reg.adr=0x00204,reg.size=4;


#define ABRM_TEST_0(reg)                     reg.adr=0x08000,reg.size=4;
#define ABRM_TEST_1(reg)                     reg.adr=0x08004,reg.size=4;
#define ABRM_TEST_2(reg)                     reg.adr=0x08008,reg.size=4;
#define ABRM_TEST_3(reg)                     reg.adr=0x0800C,reg.size=4;
#define ABRM_TEST_4(reg)                     reg.adr=0x08010,reg.size=4;


//the offset of 0x10000 is only valid for pco camera
//in general the offset should be read from ABRM_SBMR_OFFSET register
//see below
#define SBRM_LLDEVICE_ID(reg)                reg.adr=0x10000,reg.size=4;
#define SBRM_PORT_ID(reg)                    reg.adr=0x10004,reg.size=4;
#define SBRM_AVAIL_PORT(reg)                 reg.adr=0x10008,reg.size=4;
#define SBRM_DEVICE_CLASS(reg)               reg.adr=0x1000C,reg.size=4;
#define SBRM_ACTUAL_DEVICE_CONFIG(reg)       reg.adr=0x10010,reg.size=4;
#define SBRM_NEXT_DEVICE_CONFIG(reg)         reg.adr=0x10014,reg.size=4;
#define SBRM_NUM_DEVICE_CONFIG(reg)          reg.adr=0x10018,reg.size=4;
#define SBRM_DEVICE_CONFIG_LIST(reg)         reg.adr=0x1001C,reg.size=256;  //8*32
#define SBRM_DEVICE_CONFIG_ENTRY0(reg)       reg.adr=0x1001C,reg.size=8;  //first
#define SBRM_DEVICE_CONFIG_ENTRY1(reg)       reg.adr=0x10024,reg.size=8;  //second
#define SBRM_DEVICE_CONFIG_ENTRY2(reg)       reg.adr=0x1002C,reg.size=8;  //third
#define SBRM_ACTIVE_LINK_GPIO(reg)           reg.adr=0x1011C,reg.size=4;
#define SBRM_GPIO_INPUT_CAP(reg)             reg.adr=0x10120,reg.size=4;
#define SBRM_GPIO_OUTPUT_CAP(reg)            reg.adr=0x10124,reg.size=4;
#define SBRM_ACTUAL_LINK_SPEED(reg)          reg.adr=0x10128,reg.size=4;
#define SBRM_SUPPORTED_LINK_SPEED_LIST(reg)  reg.adr=0x1012C,reg.size=64;  //4*16
#define SBRM_NUM_SUPPORTED_LINK_SPEEDS(reg)  reg.adr=0x1016C,reg.size=4;
#define SBRM_NEXT_LINK_SPEED(reg)            reg.adr=0x10170,reg.size=4;
#define SBRM_ACTIVATE_HOTPLUG(reg)           reg.adr=0x10174,reg.size=4;
#define SBRM_SENSOR_WIDTH(reg)               reg.adr=0x10178,reg.size=4;
#define SBRM_SENSOR_HEIGHT(reg)              reg.adr=0x1017C,reg.size=4;
#define SBRM_BINNING_HORIZONTAL(reg)         reg.adr=0x10180,reg.size=4;
#define SBRM_BINNING_VERTICAL(reg)           reg.adr=0x10184,reg.size=4;
#define SBRM_DECIMATION_HORIZONTAL(reg)      reg.adr=0x10188,reg.size=4;
#define SBRM_DECIMATION_VERTICAL(reg)        reg.adr=0x1018C,reg.size=4;
#define SBRM_WIDTH_MAX(reg)                  reg.adr=0x10190,reg.size=4;
#define SBRM_HEIGHT_MAX(reg)                 reg.adr=0x10194,reg.size=4;
#define SBRM_SINGLE_ROI(reg)                 reg.adr=0x10198,reg.size=8;
#define SBRM_PIXEL_TYPE(reg)                 reg.adr=0x101A0,reg.size=4;
#define SBRM_BIT_DEPTH(reg)                  reg.adr=0x101A4,reg.size=4;
#define SBRM_PORT_ROI(reg)                   reg.adr=0x101A8,reg.size=64;  //8*8
#define SBRM_DECIMATION_FACTOR(reg)          reg.adr=0x101E8,reg.size=4;
#define SBRM_ELECTRIC_CABLE_SWAP(reg)        reg.adr=0x101EC,reg.size=4;
#define SBRM_ROW_OVERLAP(reg)                reg.adr=0x101F0,reg.size=4;
#define SBRM_PULSE_MODE_CAP(reg)             reg.adr=0x101F4,reg.size=4;


//defines with offset from ABRM_SBMR_OFFSET register

#define SBRMO_LLDEVICE_ID(reg,off)                reg.adr=0x00000+off,reg.size=4;
#define SBRMO_PORT_ID(reg,off)                    reg.adr=0x00004+off,reg.size=4;
#define SBRMO_AVAIL_PORT(reg,off)                 reg.adr=0x00008+off,reg.size=4;
#define SBRMO_DEVICE_CLASS(reg,off)               reg.adr=0x0000C+off,reg.size=4;
#define SBRMO_ACTUAL_DEVICE_CONFIG(reg,off)       reg.adr=0x00010+off,reg.size=4;
#define SBRMO_NEXT_DEVICE_CONFIG(reg,off)         reg.adr=0x00014+off,reg.size=4;
#define SBRMO_NUM_DEVICE_CONFIG(reg,off)          reg.adr=0x00018+off,reg.size=4;
#define SBRMO_DEVICE_CONFIG_LIST(reg,off)         reg.adr=0x0001C+off,reg.size=256;  //8*32
#define SBRMO_DEVICE_CONFIG_ENTRY0(reg,off)       reg.adr=0x0001C+off,reg.size=8;  //first
#define SBRMO_DEVICE_CONFIG_ENTRY1(reg,off)       reg.adr=0x00024+off,reg.size=8;  //second
#define SBRMO_DEVICE_CONFIG_ENTRY2(reg,off)       reg.adr=0x0002C+off,reg.size=8;  //third
#define SBRMO_ACTIVE_LINK_GPIO(reg,off)           reg.adr=0x0011C+off,reg.size=4;
#define SBRMO_GPIO_INPUT_CAP(reg,off)             reg.adr=0x00120+off,reg.size=4;
#define SBRMO_GPIO_OUTPUT_CAP(reg,off)            reg.adr=0x00124+off,reg.size=4;
#define SBRMO_ACTUAL_LINK_SPEED(reg,off)          reg.adr=0x00128+off,reg.size=4;
#define SBRMO_SUPPORTED_LINK_SPEED_LIST(reg,off)  reg.adr=0x0012C+off,reg.size=64;  //4*16
#define SBRMO_NUM_SUPPORTED_LINK_SPEEDS(reg,off)  reg.adr=0x0016C+off,reg.size=4;
#define SBRMO_NEXT_LINK_SPEED(reg,off)            reg.adr=0x00170+off,reg.size=4;
#define SBRMO_ACTIVATE_HOTPLUG(reg,off)           reg.adr=0x00174+off,reg.size=4;
#define SBRMO_SENSOR_WIDTH(reg,off)               reg.adr=0x00178+off,reg.size=4;
#define SBRMO_SENSOR_HEIGHT(reg,off)              reg.adr=0x0017C+off,reg.size=4;
#define SBRMO_BINNING_HORIZONTAL(reg,off)         reg.adr=0x00180+off,reg.size=4;
#define SBRMO_BINNING_VERTICAL(reg,off)           reg.adr=0x00184+off,reg.size=4;
#define SBRMO_DECIMATION_HORIZONTAL(reg,off)      reg.adr=0x00188+off,reg.size=4;
#define SBRMO_DECIMATION_VERTICAL(reg,off)        reg.adr=0x0018C+off,reg.size=4;
#define SBRMO_WIDTH_MAX(reg,off)                  reg.adr=0x00190+off,reg.size=4;
#define SBRMO_HEIGHT_MAX(reg,off)                 reg.adr=0x00194+off,reg.size=4;
#define SBRMO_SINGLE_ROI(reg,off)                 reg.adr=0x00198+off,reg.size=8;
#define SBRMO_PIXEL_TYPE(reg,off)                 reg.adr=0x001A0+off,reg.size=4;
#define SBRMO_BIT_DEPTH(reg,off)                  reg.adr=0x001A4+off,reg.size=4;
#define SBRMO_PORT_ROI(reg,off)                   reg.adr=0x001A8+off,reg.size=64;  //8*8
#define SBRMO_DECIMATION_FACTOR(reg,off)          reg.adr=0x001E8+off,reg.size=4;
#define SBRMO_ELECTRIC_CABLE_SWAP(reg,off)        reg.adr=0x001EC+off,reg.size=4;
#define SBRMO_ROW_OVERLAP(reg,off)                reg.adr=0x001F0+off,reg.size=4;
#define SBRMO_PULSE_MODE_CAP(reg,off)             reg.adr=0x001F4+off,reg.size=4;


#define MANIFEST_ENTRY_COUNT_OFF(reg)             reg.adr=0x00000,reg.size=8;

#define MANIFEST_ENTRY0_FILE_VERS_OFF(reg)        reg.adr=0x00008,reg.size=4;
#define MANIFEST_ENTRY0_FILE_TYPE_OFF(reg)        reg.adr=0x0000C,reg.size=4;
#define MANIFEST_ENTRY0_FILE_ADDRESS_OFF(reg)     reg.adr=0x00010,reg.size=8;
#define MANIFEST_ENTRY0_FILE_SIZE_OFF(reg)        reg.adr=0x00018,reg.size=8;
#define MANIFEST_ENTRY0_SHA1HASH_OFF(reg)         reg.adr=0x00020,reg.size=20;
#define MANIFEST_ENTRY0_RESERVED_OFF(reg)         reg.adr=0x00034,reg.size=20;

#define MANIFEST_ENTRY1_FILE_VERS_OFF(reg)        reg.adr=0x00048,reg.size=4;
#define MANIFEST_ENTRY1_FILE_TYPE_OFF(reg)        reg.adr=0x0004C,reg.size=4;
#define MANIFEST_ENTRY1_FILE_ADDRESS_OFF(reg)     reg.adr=0x00050,reg.size=8;
#define MANIFEST_ENTRY1_FILE_SIZE_OFF(reg)        reg.adr=0x00058,reg.size=8;
#define MANIFEST_ENTRY1_SHA1HASH_OFF(reg)         reg.adr=0x00060,reg.size=20;
#define MANIFEST_ENTRY1_RESERVED_OFF(reg)         reg.adr=0x00074,reg.size=20;

//#define MANIFEST_ENTRY2_FILE_VERS_OFF        reg.adr=0x00088,reg.size=20;


#define PCO_WIDTH(reg)                            reg.adr=0x00030000,reg.size=4;
#define PCO_HEIGHT(reg)                           reg.adr=0x00030004,reg.size=4;
#define PCO_PIXEL_FORMAT(reg)                     reg.adr=0x00030008,reg.size=4;
/*kann ich leider so nicht hernehmen da unterschiedlich register addressen in flow und edge 
#define PCO_ACQUISITION_MODE(reg)                 reg.adr=0x00030010,reg.size=4;
#define PCO_ACQUISITION_START(reg)                reg.adr=0x00030014,reg.size=4;
#define PCO_ACQUISITION_STOP(reg)                 reg.adr=0x00030018,reg.size=4;
#define PCO_WIDTH_MAX(reg)                        reg.adr=0x0003001C,reg.size=4;
#define PCO_HEIGHT_MAX(reg)                       reg.adr=0x00030020,reg.size=4;
#define PCO_OFFSET_X(reg)                         reg.adr=0x00030024,reg.size=4;
#define PCO_OFFSET_Y(reg)                         reg.adr=0x00030028,reg.size=4;
#define PCO_LINE_ID(reg)                          reg.adr=0x0003002C,reg.size=4;

#define PCO_TESTIMAGE(reg)                        reg.adr=0x00040124,reg.size=4;
*/
#define PCO_CONTROL_COMMAND(reg)                  reg.adr=0x00040000;



typedef struct _CLHS_CONFIGREG {
  DWORD AvailPorts;
  DWORD NumConfig;
  DWORD ActualConfig;
  DWORD NextConfig;
  uint64_t config[32];
}CLHS_CONFIGREG;


typedef struct _CLHS_PORT_BOOTREG {
  char Vendor[64];
  char Model[64];
  char Serial[64];
  CLHS_CONFIGREG master;
}CLHS_PORT_BOOTREG;

#define PT_MONO8    0x01080001
#define PT_MONO8S   0x01080002
#define PT_MONO10   0x01100003
#define PT_MONO12   0x01100005
#define PT_MONO16   0x01100007
#define PT_MONO10P  0x010A0046
#define PT_MONO12P  0x010C0047

#define PT_BAYERGB8     0x0108000A
#define PT_BAYERGB10P   0x010A0054

#endif



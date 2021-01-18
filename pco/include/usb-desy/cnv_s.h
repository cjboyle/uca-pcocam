//-----------------------------------------------------------------//
// Name        | cnv_s.h                     | Type: ( ) source    //
//-------------------------------------------|       (*) header    //
// Project     | PCO                         |       ( ) others    //
//-----------------------------------------------------------------//
// Platform    | PC                                                //
//-----------------------------------------------------------------//
// Environment | Visual 'C++'                                      //
//-----------------------------------------------------------------//
// Purpose     | PCO - Convert DLL struct definitions              //
//-----------------------------------------------------------------//
// Author      | MBL, PCO AG                                       //
//-----------------------------------------------------------------//
// Revision    |  rev. 1.15 rel. 1.15                              //
//-----------------------------------------------------------------//
// Notes       |                                                   //
//-----------------------------------------------------------------//
// (c) 2002 PCO AG * Donaupark 11 *                                //
// D-93309      Kelheim / Germany * Phone: +49 (0)9441 / 2005-0 *  //
// Fax: +49 (0)9441 / 2005-20 * Email: info@pco.de                 //
//-----------------------------------------------------------------//

//-----------------------------------------------------------------//
// Revision History:                                               //
//-----------------------------------------------------------------//
// Rev.:     | Date:      | Changed:                               //
// --------- | ---------- | ---------------------------------------//
//  1.10     | 03.07.2003 |  gamma, alignement added, FRE          //
//-----------------------------------------------------------------//
//  1.13     | 16.03.2005 |  PCO_CNV_COL_SET added, FRE            //
//-----------------------------------------------------------------//
//  1.15     | 23.10.2007 |  PCO_CNV_COL_SET removed, FRE          //
//           |            |  Adapted all structures due to merging //
//           |            |  the data sets of the dialoges         //
//-----------------------------------------------------------------//
//  0.0      |   .  .2003 |                                        //
//           |            |                                        //
//-----------------------------------------------------------------//

// @ver1.000

// defines for Lut's ...
// local functions

//--------------------

#ifndef CNV_S_H
#define CNV_S_H

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#define MAX_LUT           60           // max. number of luts supported
#define MAX_OUTSIZE      256           // max. output value for 8bit
#define MAX_BITPIX        16           // max. input value supported
#define MAX_TYPES          2

#define MODE_FLIP_B       0x0001
#define MODE_MIRROR_B     0x0002
#define MODE_OUT32_B      0x0010
#define MODE_PSEUDO_B     0x0020


// eingefügt: 18.01.2007 - RFR
struct sRGB_color_correction_coefficients
{
	double da11, da12, da13;
	double da21, da22, da23;
	double da31, da32, da33;
};

typedef struct sRGB_color_correction_coefficients SRGBCOLCORRCOEFF;

struct bw_lut
{
  unsigned short wSize;               // size of bw_lut array
  int       min;
  int       max;
  int       typ;
  int       mid;
  int       min_out;
  int       max_out;
  int       size;
  void      *ptr;
  int       bitpix; // eingefügt: 30.06.2003 - FRE
  int       align; // align 0: rechtsbündig(wie SC, PCCam), 1: linksbündig(SC2); eingefügt: 30.06.2003 - FRE
  double    dgamma; // eingefügt: 30.06.2003 - FRE
  unsigned long dwFlags;                   // Flags: Bit0(1)->align upper, Bit1(2)->Set parameter active
                                       //        Bit2(4)->do sRGB,     Bit3(8)->Inverted table
                                       //        Bit4(16)->16bit out
};

typedef struct bw_lut BWLUT;


struct color_lut
{
  unsigned short wSize;                // size of color_lut array
  BWLUT     red;
  BWLUT     green;
  BWLUT     blue;
  double    dColorSaturation;          // Color saturation
  double    dRedGainColTempCorr;       // Contrast gain for color temp. correction red (camera spec. value)
  double    dGreenGainColTempCorr;     // Contrast gain for color temp. correction green (camera spec. value)
  double    dBlueGainColTempCorr;      // Contrast gain for color temp. correction blue (camera spec. value)
  unsigned short wDarkOffset;          // Offset of the image sensor, while shutter is closed (typically 30..100)
  SRGBCOLCORRCOEFF strColorCoeff;      // Color coefficients for sRGB color space
  unsigned long dwFlags;               // Flags: Bit0(1)->align upper, Bit1(2)->Set parameter active
                                       //        Bit2(4)->do sRGB,     Bit3(8)->Inverted table
                                       //        Bit4(16)->16bit out
};

struct pseudo_lut
{
  unsigned short wSize;                // size of pseudo_lut array
  BWLUT bw;
  int psize;
  unsigned char *red_ptr;
  unsigned char *green_ptr;
  unsigned char *blue_ptr;
  unsigned long dwFlags;               // Flags: Bit0(1)->align upper, Bit1(2)->Set parameter active
};

typedef struct color_lut COLORLUT;
typedef struct pseudo_lut PSEUDOLUT;

// PCO_CNV_COL_SET Flags
#define PCO_CNV_SET_ALIGNUPPER     0x0001
#define PCO_CNV_SET_SETISACTIVE    0x0002
#define PCO_CNV_SET_DOsRGB         0x0004
#define PCO_CNV_SET_INVERS         0x0008
#define PCO_CNV_SET_CREATEOUT16    0x0010

// PCO_LUT_DLG_INFO Flags
#define PCO_LUT_DLG_INFO_OPENCBSDLG 1  // Opens contrast-brightness-saturation dialog

typedef struct
{
  unsigned short wSize;                // size of this struct
  unsigned short wId;                  // Id of the calling class (use e.g. the camera class)
  unsigned long  dwFlags;              // Bit0: Open CollutCommon
}PCO_LUT_DLG_INFO;                     // This structure can be used to open a new sheet of a already open
                                       // dialog instead of opening as a complete new dialog.
                                       // To add only a sheet you have to set the wId before calling the
                                       // open function.



#endif                                 /* CNV_S_H */

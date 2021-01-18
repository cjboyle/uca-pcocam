//-----------------------------------------------------------------//
// Name        | cnvbw.h                     | Type: ( ) source    //
//-------------------------------------------|       (*) header    //
// Project     | PCO                         |       ( ) others    //
//-----------------------------------------------------------------//
// Platform    | PC                                                //
//-----------------------------------------------------------------//
// Environment | Visual 'C++'                                      //
//-----------------------------------------------------------------//
// Purpose     | PCO - Convert DLL function call definitions       //
//-----------------------------------------------------------------//
// Author      | MBL, PCO AG                                       //
//-----------------------------------------------------------------//
// Revision    |  rev. 1.15 rel. 1.15                              //
//-----------------------------------------------------------------//
// Notes       |                                                   //
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
//  1.10     | 03.07.2003 |  gamma, alignement added, FRE          //
//-----------------------------------------------------------------//
//  1.11     | 09.07.2004 |  RGGB sensitive, FRE                   //
//-----------------------------------------------------------------//
//  1.14     | 12.05.2006 |  added _to24, FRE                      //
//-----------------------------------------------------------------//
//  1.15     | 23.10.2007 |  PCO_CNV_COL_SET removed, FRE          //
//           |            |  Adapted all structures due to merging //
//           |            |  the data sets of the dialoges         //
//-----------------------------------------------------------------//
//  0.0      |   .  .2003 |                                        //
//           |            |                                        //
//-----------------------------------------------------------------//
#ifndef CNVBW_H
#define CNVBW_H


void convert_set_colset(BWLUT *lut, double dHell, double dKontr, double dCol, double dGamma, double dDarkOffset);
void convert_set(BWLUT *lut);

BWLUT *create_bwlut(int bitpix,int min_out,int max_out, int ialign);
int del_bwlut(BWLUT *lut);

COLORLUT *create_colorlut(int bitpix,int min_out,int max_out, int ialign);
int del_colorlut(COLORLUT *clut);

PSEUDOLUT *create_pseudolut(int bitpix,int min_out,int max_out, int ialign);
int del_pseudolut(PSEUDOLUT *plut);


void convert(int width,int height,unsigned short *b12,unsigned char *b8,BWLUT *bwlut);
void flip_convert(int width,int height,unsigned short *b12,unsigned char *b8,BWLUT *bwlut);
void mirror_convert(int width,int height,unsigned short *b12,unsigned char *b8,BWLUT *bwlut);
void flip_mirror_convert(int width,int height,unsigned short *b12,unsigned char *b8,BWLUT *bwlut);

void convert_to24(int width,int height,unsigned short *b12,unsigned char *b8,BWLUT *bwlut);
void flip_convert_to24(int width,int height,unsigned short *b12,unsigned char *b8,BWLUT *bwlut);
void mirror_convert_to24(int width,int height,unsigned short *b12,unsigned char *b8,BWLUT *bwlut);
void flip_mirror_convert_to24(int width,int height,unsigned short *b12,unsigned char *b8,BWLUT *bwlut);

void convert_to32(int width,int height,unsigned short *b12,void *b8,BWLUT *bwlut);
void flip_convert_to32(int width,int height,unsigned short *b12,void *b8,BWLUT *bwlut);
void mirror_convert_to32(int width,int height,unsigned short *b12,void *b8,BWLUT *bwlut);
void flip_mirror_convert_to32(int width,int height,unsigned short *b12,void *b8,BWLUT *bwlut);



#endif /* CNVBW_H */

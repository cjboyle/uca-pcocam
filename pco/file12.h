//-----------------------------------------------------------------//
// Name        | file12.h                    | Type: ( ) source    //
//-------------------------------------------|       (*) header    //
// Project     | pco.camera                  |       ( ) others    //
//-----------------------------------------------------------------//
// Platform    | Linux                                             //
//-----------------------------------------------------------------//
// Environment |                                                   //
//             |                                                   //
//-----------------------------------------------------------------//
// Purpose     | pco.camera - file functions                       //
//-----------------------------------------------------------------//
// Author      | MBL, PCO AG                                       //
//-----------------------------------------------------------------//
// Revision    | rev. 1.03                                         //
//-----------------------------------------------------------------//
// Notes       | functions exported from pcofile library           //
//             |                                                   //
//             |                                                   //
//-----------------------------------------------------------------//
// (c) 2010 - 2012 PCO AG                                          //
// Donaupark 11 D-93309  Kelheim / Germany                         //
// Phone: +49 (0)9441 / 2005-0   Fax: +49 (0)9441 / 2005-20        //
// Email: info@pco.de                                              //
//-----------------------------------------------------------------//

#ifndef FILE12_H
#define FILE12_H

#ifdef __cplusplus
extern "C" {
#endif
int get_b16_fileparams(char *filename,int *width,int *height,int *colormode);
int read_b16(char *filename,void *bufadr);
int store_b16(char *filename,int width,int height,int colormode,void *bufadr);
int store_tif(char *filename,int width,int height,int colormode,void *bufadr);
int store_bmp(char *filename,int width,int height,int colormode,void *bufadr);
int store_tif8bw(char *filename,int width,int height,int colormode,void *bufadr);
int store_fits(char *filename,int width,int height,int colormode,void *bufadr);
int store_fits_exp(char *filename,int W, int H, void *_img_data,int _exp_time_ms);
#ifdef __cplusplus
} //end C
#endif

/*
int store_bmp24(char *filename,int width,int height,int colormode,void *bufadr);
int store_tif8rgb(char *filename,int width,int height,int colormode,void *bufadr);
*/
#endif


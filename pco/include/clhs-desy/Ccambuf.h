//-----------------------------------------------------------------//
// Name        | Ccambuf.h                   | Type: ( ) source    //
//-------------------------------------------|       (*) header    //
// Project     | pco.camera                  |       ( ) others    //
//-----------------------------------------------------------------//
// Platform    | Linux                                             //
//-----------------------------------------------------------------//
// Environment |                                                   //
//             |                                                   //
//-----------------------------------------------------------------//
// Purpose     | pco.camera - buffer class functions               //
//-----------------------------------------------------------------//
// Author      | MBL, PCO AG                                       //
//-----------------------------------------------------------------//
// Revision    | rev. 1.03                                         //
//-----------------------------------------------------------------//
// Notes       | class functions exported from Ccambuf library     //
//             |                                                   //
//             |                                                   //
//-----------------------------------------------------------------//
// (c) 2010 - 2012 PCO AG                                          //
// Donaupark 11 D-93309  Kelheim / Germany                         //
// Phone: +49 (0)9441 / 2005-0   Fax: +49 (0)9441 / 2005-20        //
// Email: info@pco.de                                              //
//-----------------------------------------------------------------//


#ifndef Ccambuf_h
#define Ccambuf_h


#define OUT_BW     0x00
#define OUT_BGR    0x01
#define OUT_BGR0   0x02
#define OUT_PSEUDO 0x03

#define IN_BW     0x00 //input buffer is from BW-Camera
#define IN_COLOR  0x10 //input buffer is from COLOR-Camera

#define MASK_RGB  0x20
#define IN_RGB    0x20 //input buffer is from RGB-Camera
#define IN_RGB0   0x21 //input buffer is from RGB0-Camera
#define IN_BGR    0x22 //input buffer is from BGR-Camera
#define IN_BGR0   0x23 //input buffer is from BGR0-Camera

#define STATUS_INCONVERT 0x01

class Cambuf;


class CCambuf
{
//variables
public:
  int     status;

public:
  CCambuf();
  ~CCambuf();

  int Allocate(int width, int height,int bitpix,int align,int color_in);
  int Allocate(int width, int height,int bitpix,int align);
  int Allocate(int width, int height,int bitpix);
  int Allocate(int size);
  int Allocate();
  int FreeBuffer();

  int Get_Size();
  int Get_actwidth();
  int Get_actheight();
  int Get_actbitpix();
  int Get_actalign();
  void* Get_actadr();

  int SetCalculationRectangle(int firstline,int lastline,int firstrow,int lastrow);
  void GetMinMax(int *min,int *max);
  void GetMinMaxPos(int *min,int *min_pos_x,int *min_pos_y,int *max,int *max_pos_x,int *max_pos_y);

  int CreateHistogramTable();
  int CreateHistogramTable(int *min,int *max,double *mean,double *sigma);
  int CreateHistogramTable(DWORD *histtab,int size,int *min,int *max,double *mean,double *sigma);
  int CreateHistogramTables();

  void Clearbuffer();

  void Set_output_buffer(int mode);
  void Set_output_buffer(int mode,void *bufadr);
  void Set_output_lut(void *lut);

  int GetColorinmode();

  int convert();
  int convert(void *out);
  int convert(void *out,void *lut);
  int convert_bw(void *out,void *lut);
  int convert_col(void *out,void *lut);

  void copy8to24(unsigned char *b8,unsigned char *out);
  void copy8to32(unsigned char *b8,unsigned char *out);


  int subtract(CCambuf *pic);
  int subtract(CCambuf *pic,int offset);

  int reduce(CCambuf *pic,int factor);


protected:
  Cambuf *cambuf;
};

#endif

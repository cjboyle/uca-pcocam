#ifndef CONVERTLIB_H
#define CONVERTLIB_H

#include "cnv_s.h"

#ifdef __cplusplus
extern "C" {
#endif

 BWLUT* pcocnv_create_bwlut(int bitpix, int min_out, int max_out);
 void pcocnv_convert_set(BWLUT* lut,int min,int max,int typ);
 int pcocnv_delete_bwlut(BWLUT* lut);

 int pcocnv_conv_buf_12to8(int mode,int width,int height,unsigned short *b12, unsigned char *b8,BWLUT *lut);
 int pcocnv_conv_buf_12to24(int mode,int width,int height,unsigned short *b12, unsigned char *b8,BWLUT *lut);
 int pcocnv_conv_buf_12to32(int mode,int width,int height,unsigned short *b12, void *b32,BWLUT *lut);

 BWLUT* pcocnv_create_bwlut_ex(int bitpix, int min_out, int max_out,int ialign);
 int pcocnv_delete_bwlut_ex(BWLUT* lut);

 COLORLUT* pcocnv_create_colorlut(int bitpix, int min_out, int max_out);
 int pcocnv_delete_pseudolut(COLORLUT* lut);


 COLORLUT* pcocnv_create_colorlut_ex(int bitpix, int min_out, int max_out,int icreateflags);
 int pcocnv_delete_colorlut(COLORLUT* lut);
 int pcocnv_delete_colorlut_ex(COLORLUT* lut);

 PSEUDOLUT* pcocnv_create_pseudolut_ex(int bitpix, int min_out, int max_out,int ialign);
 int pcocnv_delete_pseudolut_ex(PSEUDOLUT* lut);

 void pcocnv_convert_set_ex(BWLUT* lut,int min,int max,int typ,double gamma);
 void pcocnv_convert_set_col_ex(COLORLUT* lut,int rmin,int gmin,int bmin,int rmax,int gmax,int bmax,
                                   int typ,double gamma,double dsaturation);

 int pcocnv_load_pseudo_lut_ex(PSEUDOLUT* plut, int format, char *filename);
 int pcocnv_conv_buf_12to8_ex(int mode,int width,int height,unsigned short *b12, unsigned char *b8,BWLUT *lut);
 int pcocnv_conv_buf_12to24_ex(int mode,int width,int height,unsigned short *b12, unsigned char *b8,BWLUT *lut);

 int pcocnv_conv_buf_12tocol_ex(int mode, int width, int height, unsigned short *gb12, unsigned char *gb8,COLORLUT *clut);
 int pcocnv_conv_buf_12topseudo_ex(int mode, int width, int height, unsigned short *gb12, unsigned char *gb8,PSEUDOLUT *plut);

 int pcocnv_whitebalance(int mode, int iwidth, int iheight,
                 unsigned short *gb16in, COLORLUT* pcollut,
                 int ilowerlimitprz, int iupperlimitprz, int idarkoffset,
                 short* sredgain, short* sgreengain, short* sbluegain);



#ifdef __cplusplus
}
#endif

#endif

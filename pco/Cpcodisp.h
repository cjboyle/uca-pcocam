//-----------------------------------------------------------------//
// Name        | Cpcodisp.h                   | Type: ( ) source    //
//-------------------------------------------|       (*) header    //
// Project     | pco.camera                  |       ( ) others    //
//-----------------------------------------------------------------//
// Platform    | Linux                                             //
//-----------------------------------------------------------------//
// Environment |                                                   //
//             |                                                   //
//-----------------------------------------------------------------//
// Purpose     | pco.camera - display class functions              //
//-----------------------------------------------------------------//
// Author      | MBL, PCO AG                                       //
//-----------------------------------------------------------------//
// Revision    | rev. 1.03                                         //
//-----------------------------------------------------------------//
// Notes       | class functions exported from Cpcodisp library    //
//             |                                                   //
//             |                                                   //
//-----------------------------------------------------------------//
// (c) 2010 - 2012 PCO AG                                          //
// Donaupark 11 D-93309  Kelheim / Germany                         //
// Phone: +49 (0)9441 / 2005-0   Fax: +49 (0)9441 / 2005-20        //
// Email: info@pco.de                                              //
//-----------------------------------------------------------------//


#ifndef CPCODISP_H
#define CPCODISP_H

#include <X11/Xlib.h>
#include <X11/X.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/extensions/XShm.h>

#include "defs.h"
#include "PCO_err.h"

#include "Ccambuf.h"
#include "Cpco_log.h"


class PCODisp;

class CPCODisp
{
    public:
        CPCODisp();
        ~CPCODisp();
        void start_log();
        void start_log(DWORD level);
        void stop_log();

        int initialize();
        int initialize(char *title);
        int initialize(char *title,CCambuf *actpic);
//        int open();
//        int close();

        void convert(void);
        void Set_Actual_pic(CCambuf* actpic);

        int SetConvert(int typ);
        int SetConvert(double gamma);

        int SetConvert(int min,int max);
        int SetConvert(int min, int max,int typ,double gamma);

        int SetConvert_Green(int min, int max);
        int SetConvert_Red(int min, int max);
        int SetConvert_Blue(int min, int max);
        int SetConvert_Col(int rmin,int gmin,int bmin,int rmax,int gmax,int bmax);

        int whitebalance(int low,int high);


        void SetBitAlign(int align);

//        void SetSize(int width,int height);
//        void SetPosition(int xpos,int ypos);
//        void SetZoom(double zoom);
//        void GetZoom(double *zoom);
        void displayfunc(void);

  protected:
        PCODisp *pcodisp;
  
};

#endif // CPCODISP_H

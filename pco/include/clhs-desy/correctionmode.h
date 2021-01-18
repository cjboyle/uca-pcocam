#ifndef EDGE_CORRECTIONMODE_H
#define EDGE_CORRECTIONMODE_H


//---------command code

#define GET_CORRECTION_MODE    0x2A11
#define SET_CORRECTION_MODE    0x2B11


//-------- telegram structure

typedef struct
{
  WORD        wCode;                   // telegram code 
  WORD        wSize;                   // telegram length
  WORD        wMode;                   // see defines
  WORD        wOffs;                   
  WORD        wRsrvd1;
  WORD        wRsrvd2;
  BYTE        bCks;                    // checksum byte
} SC2_Set_Correction_Mode;

#define SC2_Set_Correction_Mode_Response SC2_Set_Correction_Mode
#define SC2_Get_Correction_Mode_Response SC2_Set_Correction_Mode

//-------- correction mode settings

#define CORRMODE_COMPLETELY_OFF          0x0000
#define CORRMODE_DSNU_CORRECTION         0x0001
#define CORRMODE_LINEARIZATION           0x0004


#endif // ifndef EDGE_CORRECTIONMODE_H

// ------------------------------< end of file >--------------------------- //


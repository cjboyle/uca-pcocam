//-----------------------------------------------------------------//
// Name        | pco_clhs_dc_cam.h           | Type: ( ) source    //
//-------------------------------------------|       (*) header    //
// Project     | pco clhs                    |       ( ) others    //
//-----------------------------------------------------------------//
// Platform    |                                                   //
//-----------------------------------------------------------------//
// Environment | gcc                                               //
//             |                                                   //
//-----------------------------------------------------------------//
// Purpose     | pco CLHS camera class Header                      //
//-----------------------------------------------------------------//
// Author      | MBL, PCO AG                                       //
//-----------------------------------------------------------------//
// Revision    | rev. 0.01 rel. 0.00                               //
//-----------------------------------------------------------------//
// Notes       | public                                            //
//             |                                                   //
//             |                                                   // 
//-----------------------------------------------------------------//
// (c) 2016 PCO AG * Donaupark 11 *                                //
// D-93309      Kelheim / Germany * Phone: +49 (0)9441 / 2005-0 *  //
// Fax: +49 (0)9441 / 2005-20 * Email: info@pco.de                 //
//-----------------------------------------------------------------//


//-----------------------------------------------------------------//
// Revision History:                                               //
//-----------------------------------------------------------------//
// Rev.:     | Date:      | Changed:                               //
// --------- | ---------- | ---------------------------------------//
//  0.01     | 22.07.2016 |  new file                              //
//           |            |                                        //
//-----------------------------------------------------------------//
//  0.0x     | xx.xx.200x |                                        //
//-----------------------------------------------------------------//

#ifndef PCO_CLHS_CAM_H
#define PCO_CLHS_CAM_H

#include "clhs_register.h"
#include "portable_endian.h"


#ifndef PCO_CLHS_DC_CAM
#define PCO_CLHS_DC_CAM

#define MAX_PORT_NUM 4
#define MAX_CAMERA_NUM MAX_GRABBER_NUM*MAX_PORT_NUM

typedef struct _PCO_CLHS_DC_PORTASSIGNMENT
{
 int grabber_num;
 int port_num;
}PCO_CLHS_DC_PORTASSIGNMENT;

typedef struct _PCO_CLHS_DC_CAMERA
{
 DWORD cam_flags;
 DWORD pco_number;
 DWORD process_id;
 DWORD selected_config;
 DWORD connected_ports;
 DWORD acquire_ports;
 DWORD sensor_width;
 DWORD sensor_height;
 DWORD sensor_bitpix;
 PCO_CLHS_DC_PORTASSIGNMENT grabber_port_assign[MAX_PORT_NUM];
}PCO_CLHS_DC_CAMERA;
#endif

class Cpco_clhs_dc;
class Cpco_clhs_acq;

class CPco_clhs_cam
{
 public:
  CPco_clhs_cam();
  ~CPco_clhs_cam();

  void SetLog(CPco_Log *elog);

  DWORD Open(DWORD num);
  DWORD Close();
  DWORD opencount();
  DWORD is_master();

  PCO_CLHS_DC_CAMERA* get_camera(DWORD cam_num);
  DWORD assign_camera_ports(DWORD cam_num);
  DWORD release_assigned_ports(DWORD connected_ports,PCO_CLHS_DC_PORTASSIGNMENT *grabber_port_assign);

  DWORD read_cam(uint64_t adr,uint32_t* length,void *ret_buf);
  DWORD write_cam(uint64_t  adr,uint32_t length,void *in_buf);

  DWORD Start_acquisition();
  DWORD Stop_acquisition();

  DWORD Set_alignment(int align);
  DWORD Set_acquire_param(int bitpix,int bufnr,int type);
  DWORD Set_acquire_mem(int maxmem,int minbuf,int maxbuf);
  DWORD Set_acquire_size(int width,int height);
  DWORD Get_acquire_status();

  DWORD Set_acquire_buffer(void *bufadr);
  DWORD Wait_acquire_buffer(int waittime);
  DWORD Cancel_acquire_buffer();

 protected:
  Cpco_clhs_dc *Cclhs_dc;
  Cpco_clhs_acq *Cclhs_acq;
  CPco_Log *clog;

  DWORD status;
  DWORD acquire_ports;
  PCO_CLHS_DC_PORTASSIGNMENT camera_assign[MAX_PORT_NUM];
  PCO_CLHS_DC_CAMERA *camera;

  void writelog(DWORD lev,const char *str,...);
};

#endif

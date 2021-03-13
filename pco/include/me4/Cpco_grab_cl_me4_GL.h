//-----------------------------------------------------------------//
// Name        | Cpco_me4_GL.h               | Type: ( ) source    //
//-------------------------------------------|       (*) header    //
// Project     | pco.camera                  |       ( ) others    //
//-----------------------------------------------------------------//
// Platform    | Linux                                             //
//-----------------------------------------------------------------//
// Environment |                                                   //
//             |                                                   //
//-----------------------------------------------------------------//
// Purpose     | pco.edge - CameraLink ME4 API                     //
//-----------------------------------------------------------------//
// Author      | MBL, PCO AG                                       //
//-----------------------------------------------------------------//
// Revision    | rev. 1.03                                         //
//-----------------------------------------------------------------//
// Notes       | In this file are all functions and definitions,   //
//             | for communication with CameraLink grabbers        //
//             |                                                   //
//             |                                                   //
//-----------------------------------------------------------------//
// (c) 2010 - 2012 PCO AG                                          //
// Donaupark 11 D-93309  Kelheim / Germany                         //
// Phone: +49 (0)9441 / 2005-0   Fax: +49 (0)9441 / 2005-20        //
// Email: info@pco.de                                              //
//-----------------------------------------------------------------//

//-----------------------------------------------------------------//
// Revision History:                                               //
//  see Cpco_cl_me4_GL.cpp                                         //
//-----------------------------------------------------------------//


#include "Cpco_grab_cl_me4.h"

#define HAP_VERSION_NOISE 12


///
/// \brief The class for a pco.edge global shutter
///
class CPco_grab_cl_me4_edge_GL : public CPco_grab_cl_me4
{

public:
  CPco_grab_cl_me4_edge_GL(CPco_com_cl_me4* camera);
  ///
  /// \brief Opens the grabber and retrieves the neccessary variables from the camera object.
  ///
  /// \param board Set to zero if there is only one camera connected.
  /// Open_Cam() on the appropriate class object \b must be called first or this will fail!
  /// \return Error or 0 in case of success
  ///
  DWORD Open_Grabber(int board);
  ///
  /// \brief Sets the grabber size.
  ///
  /// It is extremely important to set this before any images are transferred! If any of the settings are changed that influence the image size
  /// Set_Grabber_Size \b must be called again before any images are transferred! If this is not done, memory or segmentation faults will occur!
  ///
  /// \param width Actual width of the picture
  /// \param height Actual height of the picture
  /// \return Error or 0 in case of success
  ///
  DWORD Set_Grabber_Size(DWORD width,DWORD height);
  ///
  /// \brief Sets the data format for the grabber
  /// \anchor Set_DataFormat
  /// \param format The new data format.
  /// \return always 0
  ///
  DWORD Set_DataFormat(DWORD format);
  ///
  /// \brief Reorders the image lines and copies the reordered image to the output buffer.
  /// \anchor Extract_Image
  /// \param bufout Output buffer. Must be large enough to hold the image!
  /// \param bufin Input buffer.
  /// \param width Image width.
  /// \param height Image height.
  /// \param line_width Unused.
  ///
  void Extract_Image(void *bufout, void *bufin, int width, int height,int line_width ATTRIBUTE_UNUSED);
  ///
  /// \overload Extract_Image(void*,void*,int,int,int)
  ///
  void Extract_Image(void *bufout,void *bufin,int width,int height);
  ///
  /// \brief Get a single image line
  ///
  /// \param bufout Output buffer. Must be large enough to hold a single image line!
  /// \param bufin Input buffer.
  /// \param linenumber Line number to get.
  /// \param width Image width.
  /// \param height Image height.
  ///
  void Get_Image_Line(void *bufout,void *bufin,int linenumber,int width,int height);

  DWORD PostArm(int userset=0); 
  ~CPco_grab_cl_me4_edge_GL(){Close_Grabber();}

protected:
  DWORD set_actual_size_global_shutter(int width,int height,int xlen,int ylen,int xoff,int yoff,int doublemode);
  DWORD set_actual_noisefilter(int noisefilter,int color);
  DWORD set_actual_timestamp(int timestamp);

  int act_sccmos_version;
};

//-----------------------------------------------------------------//
// Name        | CPco_grab_clhs.h             | Type: ( ) source    //
//-------------------------------------------|       (*) header    //
// Project     | pco.camera                  |       ( ) others    //
//-----------------------------------------------------------------//
// Platform    | Linux                                             //
//-----------------------------------------------------------------//
// Environment | gcc                                               //
//             |                                                   //
//-----------------------------------------------------------------//
// Purpose     | pco.camera - CLHS                                 //
//-----------------------------------------------------------------//
// Author      | MBL, PCO AG                                       //
//-----------------------------------------------------------------//
// Revision    | rev. 1.01 rel. 0.00                               //
//-----------------------------------------------------------------//
// Notes       | In this file are all functions and definitions,   //
//             | for grabbing from clhscamera                      //
//             |                                                   //
//             |                                                   //
//-----------------------------------------------------------------//
// (c) 2014 PCO AG * Donaupark 11 *                                //
// D-93309      Kelheim / Germany * Phone: +49 (0)9441 / 2005-0 *  //
// Fax: +49 (0)9441 / 2005-20 * Email: info@pco.de                 //
//-----------------------------------------------------------------//

#ifndef CPCO_GRAB_CLHS_H
#define CPCO_GRAB_CLHS_H

//#include <vector>

#include "pco_includes.h"
#include "Cpco_com_clhs.h"

///
/// \brief The CPco_grab_clhs class
///
/// This is the grabber class for CLHS. It it responsible for image transfers by allocating and submitting CLHS transfers.
///
///\nosubgrouping

class CPco_grab_clhs
{

public:


protected:
  PCO_HANDLE hgrabber;
  CPco_com_clhs *cam;
  CPco_clhs_cam* Cclhs_cam;
  CPco_Log *clog;

  int ImageTimeout;
  int aquire_flag;

  WORD act_align;
  WORD act_bitpix;
  int act_width,act_height;
  int DataFormat;

  SC2_Camera_Description_Response description;

  WORD  camtype;
  DWORD serialnumber;
  DWORD cam_pixelrate;
  WORD cam_timestampmode;
  WORD cam_doublemode;
  WORD cam_align;
  WORD cam_noisefilter;
  WORD cam_colorsensor;
  DWORD cam_width,cam_height;

public:
  ///
  /// Constructor for the class. It is possible (though not very useful) to create a class object without passing it a camera class object pointer as parameter.
  /// \anchor CPco_grab_usb
  /// \param camera A CPco_com_usb object with a previously opened pco.camera
  /// \return
  ///
  CPco_grab_clhs(CPco_com_clhs *camera=NULL);
  ~CPco_grab_clhs();

  ///
  /// \brief Opens the grabber and retrieves the neccessary variables from the camera object.
  /// \anchor Open_Grabber
  /// \param board Set to zero if there is only one camera connected.
  /// Open_Cam() on the appropriate class object \b must be called first or this will fail!
  /// \return Error or 0 in case of success
  ///
  DWORD Open_Grabber(int board);

  ///
  /// \brief Opens the grabber and retrieves the neccessary variables from the camera object.
  /// \param board Set to zero if there is only one camera connected.
  /// Open_Cam() on the appropriate class object \b must be called first or this will fail!
  /// \return Error or 0 in case of success
  ///
  DWORD Open_Grabber(int board,int initmode ATTRIBUTE_UNUSED);

  ///
  /// Closes the grabber. This should be done before calling Close_Cam().
  /// \anchor Close_Grabber
  /// \return Error or 0 in case of success
  ///
  DWORD Close_Grabber();

  ///
  /// \brief Check if grabber is opened..
  /// \return openstatus
  /// \retval TRUE grabber is opened
  /// \retval FALSE grabber is closed
  ///
  BOOL IsOpen();

  ///
  /// \brief Sets the logging behaviour for the grabber class.
  /// \n
  /// If this function is not called no logging is performed.
  /// Logging might be useful to follow the program flow of the application.
  /// \param elog Pointer to a CPco_Log logging class object
  /// \return
  ///
  void SetLog(CPco_Log *elog);


  /// @name Image Acquire Functions
  ///
  /// These functions are used to acquire images from the camera.

  ///
  /// \brief A simple image acquisition function.
  /// \anchor Acquire_Image
  /// This function is synchronous. It does not return until either an image is grabbed
  /// and completely transferred to the given address or the timeout has been reached.
  /// \n Start_Acquire() is called, when Grabber is not already started.
  /// Then the function is waiting until a single image is in the grabber buffer.
  /// Stop_Acquire is called when grabber was not started.
  /// \n Internal timeout setting is used, see \ref Set_Grabber_Timeout.
  /// \param adr Pointer to address where the image gets stored
  /// \return Error or 0 in case of success
  ///
  DWORD Acquire_Image(void *adr);

  ///
  /// \brief A simple image acquisition function.
  /// This function is synchronous. It does not return until either an image is grabbed
  /// and completely transferred to the given address or the timeout has been reached.
  /// \n Start_Acquire() is called, when Grabber is not already started.
  /// Then the function is waiting until a single image is in the grabber buffer.
  /// Stop_Acquire is called when grabber was not started.
  /// \n Custom timeout setting is used.
  /// \param adr Pointer to address where the image gets stored
  /// \param timeout time to wait for image in ms
  /// \return Error or 0 in case of success
  ///
  DWORD Acquire_Image(void *adr,int timeout);

  ///
  /// \brief Transfers an image from the recorder buffer of the camera. (Blocking)
  /// \anchor Get_Image
  /// This function is synchronous. It does not return until either an image is grabbed
  /// and completely transferred to the given address or the timeout has been reached.
  /// \n Start_Acquire() is called, when Grabber is not already started.
  /// Then the function does request an image from the camera and is waiting until this image is in the grabber buffer.
  /// Stop_Acquire is called when grabber was not started.
  /// \n Internal timeout setting is used, see \ref Set_Grabber_Timeout.
  /// \param adr Pointer to address where the image gets stored
  /// \n Internal timeout is used, see \ref Set_Grabber_Timeout.
  /// \param adr Pointer to address where the image gets stored
  /// \param Segment select segment of recorder buffer
  /// \param ImageNr select image in recorder buffer
  /// \return Error or 0 in case of success
  ///
  DWORD Get_Image(WORD Segment,DWORD ImageNr,void *adr);

  ///
  /// \brief Set the general timeout for all image acquire functions without timeout parameter.
  /// \anchor Set_Grabber_Timeout
  /// \param timeout new timeout in ms
  /// \return always 0
  ///
  DWORD Set_Grabber_Timeout(int timeout);

  ///
  /// \brief Get the general timeout for all image acquire functions without timeout parameter.  /// \anchor Get_Grabber_Timeout
  /// \anchor Get_Grabber_Timeout
  /// \param timeout pointer to variable which receives the actual timeout value in ms
  /// \return always 0
  ///
  DWORD Get_Grabber_Timeout(int *timeout);
  ///
  /// \brief Starts image acquisition.
  /// This should be the first function called when starting a new image acquisition series.
  /// After that, only calls to Wait_For_Image are neccessary.
  ///
  /// \return Error code, or or 0 in case of success
  ///
  DWORD Start_Acquire();

  ///
  /// \brief Stops image acquisition.
  /// \anchor Stop_Acquire
  /// \return Error code or 0 in case of success
  ///
  DWORD Stop_Acquire();

  ///
  /// \brief Returns the status of the image acquisition.
  /// \return Acquisition status: TRUE = running, FALSE = stopped.
  ///
  BOOL started();

  ///
  /// \brief Waits until the next image is completely transferred to the given address.
  /// \param adr Pointer to address where the image gets stored
  /// \param timeout Sets a timeout in milliseconds. After this the function returns with an error.
  /// \return Error code or 0 in case of success
  ///
  DWORD Wait_For_Next_Image(void* adr,int timeout);


  /// @name Class Control Functions
  ///
  /// These functions are used to control and retrieve some internal variables of the class.
  ///


  ///
  /// \brief Get camera settings and set internal parameters
  /// \n
  /// This function call should be called after Arm_Camera is called and is an overall replacement for the following functions.
  /// Parameter userset is used to determine if the grabber parameters are changed (recommended) or not.
  /// \param userset If set to 0 (default), this function does setup the grabber class correctly for following image transfers.
  /// If set to any other value grabber class \b must be setup with the following functions.  
  /// \return Error or 0 in case of success
  ///
  DWORD PostArm(int userset=0);

  ///
  /// \brief Sets the grabber size.
  /// \anchor Set_Grabber_Size
  /// It is extremely important to set this before any images are transferred! If any of the settings are changed that influence the image size
  /// Set_Grabber_Size \b must be called again before any images are transferred! If this is not done, memory or segmentation faults will occur!
  /// \param width Actual width of the picture
  /// \param height Actual height of the picture
  /// \param bitpix Actual number of bits per pixel. This value is rounded up to a multiple of 8.
  /// \return Error or 0 in case of success
  ///
  DWORD Set_Grabber_Size(int width,int height,int bitpix);

  ///
  /// \overload
  ///
  /// \return
  ///
  DWORD Set_Grabber_Size(int width,int height);

  ///
  /// \brief Returns the current grabber sizes
  /// \param width pointer to variable 
  /// \param height pointer to variable
  /// \param bitpix pointer to variabl
  /// \return Returns always 0.
  /// \retval *width Current width
  /// \retval *height Current height
  /// \retval *bitpix current bits per pixel
  ///
  int Get_actual_size(unsigned int *width,unsigned int *height,unsigned int *bitpix);

  ///
  /// \brief Set the dataformat for the following image transfers
  /// \n
  /// At the moment this call is only a dummy function without really usefulness
  /// \param dataformat New Dataformat
  /// \return
  ///
  DWORD Set_DataFormat(DWORD dataformat);

  ///
  /// \brief Returns the current grabber format
  /// \return The actual DataFormat
  ///
  DWORD Get_DataFormat(){return DataFormat;}

  ///
  /// \brief Set BitAlignment parameter to the grabber class, which is needed for correct handling of image data
  /// \param act_align value retrieved from camera after last PCO_ArmCamera() call
  /// \return
  ///
  void SetBitAlignment(int act_align);

  ///
  /// \brief Returns the current grabber height
  /// \return Current height.
  ///
  int Get_Height();

  ///
  /// \brief Returns the current grabber width
  /// \return Current height.
  ///
  int Get_Width();


  /// \cond

  int Get_Line_Width();

  DWORD Get_Camera_Settings();
  DWORD Allocate_Framebuffer(int nr_of_buffer ATTRIBUTE_UNUSED);
  DWORD Free_Framebuffer();

protected:
  ///
  /// \implements writelog
  ///
  void writelog(DWORD lev,PCO_HANDLE hdriver,const char *str,...);


  /// \endcond

};

#endif

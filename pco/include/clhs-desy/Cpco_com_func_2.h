//-----------------------------------------------------------------//
// Name        | Cpco_com_func_2.h           | Type: ( ) source    //
//-------------------------------------------|       (*) header    //
// Project     | pco.camera                  |       ( ) others    //
//-----------------------------------------------------------------//
// Platform    | Linux                                             //
//-----------------------------------------------------------------//
// Environment |                                                   //
//             |                                                   //
//-----------------------------------------------------------------//
// Purpose     | pco.camera - Communication                        //
//-----------------------------------------------------------------//
// Author      | MBL, PCO AG                                       //
//-----------------------------------------------------------------//
// Revision    |                                                   //
//-----------------------------------------------------------------//
// Notes       | Telegram functions                                //
//             |                                                   //
//             |                                                   //
//-----------------------------------------------------------------//
// (c) 2010 - 2015 PCO AG                                          //
// Donaupark 11 D-93309  Kelheim / Germany                         //
// Phone: +49 (0)9441 / 2005-0   Fax: +49 (0)9441 / 2005-20        //
// Email: info@pco.de                                              //
//-----------------------------------------------------------------//

//-----------------------------------------------------------------//
// Revision History:                                               //
//  see Cpco_com_func_2.cpp                                        //
//-----------------------------------------------------------------//

#ifndef CPCO_COM_FUNC_2_H
#define CPCO_COM_FUNC_2_H

///
/// \file Cpco_com_func_2.h
///
/// \brief Advanced camera communication
///
/// \author PCO AG
///


/// @name Storage Control and Status
///
/// This section contains functions to change and retrieve actual format settings

///
/// \brief Gets the ram and page size of the camera.
///
/// One page is the smallest unit for RAM segmentation as well as for storing images. Segment
/// sizes can only configured as multiples of pages. The size reserved for one image is also
/// calculated as multiples of whole pages. Therefore, there may be some unused RAM memory
/// if the page size is not exactly a multiple of the image size. The number of pages needed for
/// one image depends on the image size (Xres x Yres) divided by the pixels per page (page size).
/// Every page size that has been started must be considered, so if 50.6 pages are used for an
/// image 51 pages are actually needed for this image. With this value of 'pages per image',the
/// user can calculate the number of images fitting into the segment.
/// \param dwRamSize Pointer to a DWORD variable to receive the total camera RAM.
/// \param wPageSize Pointer to a DWORD variable to receive the pagesize as a multiple of pixels.
/// \return Error message, 0 in case of success else less than 0.
///
DWORD PCO_GetCameraRamSize(DWORD* dwRamSize, WORD* wPageSize);

///
/// \brief Gets the segment sizes of the camera.
///
/// \b Note:
/// - the sum of all segment sizes must not be larger than the total size of the RAM (as multiples of pages)
/// - \b size = [0] indicates that the segment will not be used
/// - using only one segment is possible by assigning the total RAM size to segment 1 and 0x0000 to all other segments.
/// - The segment number is 1 based, while the array dwRamSegSize is zero based, e.g. ram size of segment 1 is stored in dwRamSegSize[0]!
///
/// \param dwRamSegSize Pointer to a DWORD array to receive the ramsegmentsizes in pages.
/// \return Error message, 0 in case of success else less than 0.
///
DWORD PCO_GetCameraRamSegmentSize(DWORD* dwRamSegSize);

///
/// \brief Set Camera RAM Segment Size. The segment size has to be big enough to hold at least two images.
///
/// \b Note:
/// - the sum of all segment sizes must not be larger than the total size of the RAM (as multiples of pages)
/// - a single segment size can have the value 0x0000, but the sum of all four segments must be bigger than the size of two images.
/// - the command will be rejected, if Recording State is [run]
/// - The segment number is 1 based, while the array dwRamSegSize is zero based, e.g. ram size of segment 1 is stored in dwRamSegSize[0]!
/// - This function will result in \b all segments being \b cleared. All previously recorded images will be \b lost!}
///
/// \param dwRamSegSize
/// Pointer to a DWORD array to receive the ramsegmentsize in pages.
/// \return Error message, 0 in case of success else less than 0.
///
DWORD PCO_SetCameraRamSegmentSize(DWORD* dwRamSegSize);

///
/// \brief Get the active camera RAM segment.
///
/// The active segment is where images are stored.
///
/// \param wActSeg Pointer to a WORD variable to receive the actual segment. (1 - 4)
/// \return Error message, 0 in case of success else less than 0.
///
DWORD PCO_GetActiveRamSegment(WORD* wActSeg);

///
/// \brief Set the active camera RAM segment.
///
/// The active segment is where images are stored.
///
/// \param wActSeg WORD variable to hold the actual segment.
/// \return Error message, 0 in case of success else less than 0.
///
DWORD PCO_SetActiveRamSegment(WORD wActSeg);

///
/// \brief Clear active camera RAM segment, delete all image info and prepare segment for new images.
///
/// \return Error message, 0 in case of success else less than 0.
///
DWORD PCO_ClearRamSegment();

///
/// \brief Get the number of valid images within the segment.
///
/// This function is not applicable with cameras
///without internal recorder memory. The operation is slightly different due to the selected storage mode:
///
/// In [recorder mode], if recording is not stopped and in [FIFO buffer mode] the number of images is
/// dynamic due to read and write accesses to the camera RAM. If the \b camera \b storage \b mode is in
/// [recorder mode] and recording is stopped, the number is fixed.
///
/// In [FIFO buffer] mode the ratio of valid number of images to the maximum number of images is some sort of filling indicator.
/// \param wSegment WORD variable that holds the segment to query.
/// \param dwValid Pointer to a DWORD varibale to receive the valid image count.
/// \param dwMax Pointer to a DWORD varibale to receive the max image count which may be saved to this segment.
/// \return Error message, 0 in case of success else less than 0.
///
DWORD PCO_GetNumberOfImagesInSegment(WORD wSegment,DWORD* dwValid,DWORD* dwMax);

///
/// \brief Get the image settings for images stored into one of the four segments. This function is not applicable with cameras without internal recorder memory.
///
/// Gets the sizes information for one segment.
///
/// X0, Y0 start at 1. X1, Y1 end with max. sensor size.
///
/// \latexonly \begin{tabular}{| l c r | } \hline x0,y0 & & \\ & ROI & \\ & & x1,y1 \\ \hline \end{tabular} \endlatexonly
///
/// \param wSegment WORD variable that holds the segment to query.
/// \param wRes_hor Pointer to a WORD variable to receive the x resolution of the image in segment
/// \param wRes_ver Pointer to a WORD variable to receive the y resolution of the image in segment
/// \param wBin_x Pointer to a WORD variable to receive the horizontal binning of the image in segment
/// \param wBin_y Pointer to a WORD variable to receive the vertical binning of the image in segment
/// \param wRoi_x0 Pointer to a WORD variable to receive the left x offset of the image in segment
/// \param wRoi_y0 Pointer to a WORD variable to receive the upper y offset of the image in segment
/// \param wRoi_x1 Pointer to a WORD variable to receive the right x offset of the image in segment
/// \param wRoi_y1 Pointer to a WORD variable to receive the lower y offset of the image in segment
/// \return Error message, 0 in case of success else less than 0.
///
DWORD PCO_GetSegmentImageSettings(WORD wSegment,WORD* wRes_hor,WORD* wRes_ver,WORD* wBin_x,WORD* wBin_y,WORD* wRoi_x0,WORD* wRoi_y0,WORD* wRoi_x1,WORD* wRoi_y1);


/// @name Image transfer
///
/// This section contains functions to invoke image transfers from the camera

///
/// \brief  Reads the specified images from segment.
///
/// \param wSegment WORD variable that holds the segment to query.
/// \param dwStartImage DWORD variable that holds the first image to receive.
/// \param dwLastImage DWORD variable that holds the last image to recieve.
/// \return Error message, 0 in case of success else less than 0.
///
DWORD PCO_ReadImagesFromSegment(WORD wSegment,DWORD dwStartImage,DWORD dwLastImage);


///
/// \brief Requests a single image from the camera.
/// \return Error message, 0 in case of success else less than 0
///
DWORD PCO_RequestImage();

///
/// \brief Repeats the last image.
///
/// \return Error message, 0 in case of success else less than 0.
///
DWORD PCO_RepeatImage();

///
/// \brief PCO_CancelImage
///
/// Cancels the image transfer.
///
/// \return Error message, 0 in case of success else less than 0.
///
DWORD PCO_CancelImage();


///
/// \brief Cancels the image processing.
///
/// \return Error message, 0 in case of success else less than 0.
///
DWORD PCO_CancelImageTransfer();

///
/// \brief Gets the image transfer mode
/// Get the current active transfer mode and the additional parameters for this mode
///
/// \param wMode Pointer to WORD variable to receive the image mode. (e.g. full, scaled, cutout etc.)
/// \param wImageWidth Pointer to WORD variable to receive the original image width
/// \param wImageHeight Pointer to WORD variable to receive the original image height
/// \param wTxWidth Pointer to WORD variable to receive the transferred image width
/// \param wTxHeight Pointer to WORD variable to receive the transferred image height
/// \param wTxLineWordCnt Meaning depends on selected mode
/// \param wParam Meaning depends on selected mode
/// \param wParamLen Pointer to WORD variable to receive wParam length.
/// \return Error message, 0 in case of success else less than 0.
///
DWORD PCO_GetImageTransferMode(WORD* wMode,WORD* wImageWidth,WORD* wImageHeight,WORD* wTxWidth,WORD* wTxHeight,WORD* wTxLineWordCnt,WORD* wParam,WORD* wParamLen);

///
/// \brief Sets the image transfer mode
/// Set the current transfer mode and the additional parameters for this mode
/// This function offers the ability to reduce the amount of image data, which is transferred through the interface.
/// It can be used to offer an quick preview of all images stored in the camera.
///
/// \param wMode WORD variable to set the image mode. (e.g. full, scaled, cutout etc.)
/// \param wImageWidth WORD variable to set the original image width
/// \param wImageHeight WORD variable to set the original image height
/// \param wTxWidth WORD variable to set the scaled/cutout image width
/// \param wTxHeight WORD variable to set the scaled/cutout image height
/// \param wTxLineWordCnt Meaning depends on selected mode
/// \param wParam Meaning depends on selected mode
/// \param wParamLen WORD variable to hold the wParam length.
/// \return Error message, 0 in case of success else less than 0.
///
DWORD PCO_SetImageTransferMode(WORD wMode,WORD wImageWidth,WORD wImageHeight,WORD wTxWidth,WORD wTxHeight,WORD wTxLineWordCnt,WORD* wParam,WORD wParamLen);




/// @name Image data options
///
/// This section contains functions to change and retrieve actual settings for the transferred image data


///
/// \brief Gets infos about lookup tables in the camera, if available.
///
/// Only available with a pco.edge.
///
/// \param wLUTNum WORD variable to hold the number of LUT to query.
/// \param wNumberOfLuts Pointer to WORD variable to recieve the number of LUTs which can be queried
/// \param Description Pointer to string to recieve the description, e.g. "HD/SDI 12 to 10".
/// \param wDescLen Pointer to WORD variable to recieve the string length.
/// \param wIdentifier Pointer to WORD variable to recieve the loadable LUTs. Range from 0x0001 to 0xFFFF
/// \param bInputWidth Pointer to BYTE variable to recieve the maximum input in bits.
/// \param bOutputWidth Pointer to BYTE variable to recieve the maximum output in bits.
/// \param wFormat Pointer to WORD variable to recieve the accepted data structures (see defines!)
/// \return Error message, 0 in case of success else less than 0.
///
DWORD PCO_GetLookupableInfo(WORD wLUTNum, WORD* wNumberOfLuts, char* Description,WORD wDescLen,WORD* wIdentifier, BYTE* bInputWidth,BYTE* bOutputWidth, WORD* wFormat);


///
/// \brief Gets the active lookup table in the camera, if available.
/// 
/// Only available with a pco.edge
/// \param Identifier Currently active LUT, 0x0000 for no LUT
/// \param Parameter Offset: 11 Bit value for fixed offset subtraction before transferring the data via the lookup table
/// \return Error message, 0 in case of success else less than 0
///
DWORD PCO_GetLut(WORD *Identifier,WORD *Parameter);


///
/// \brief Sets the active lookup table in the camera, if available.
/// 
/// Only available with a pco.edge
/// \param Identifier define LUT to be activated, 0x0000 for no LUT, see PCO_GetLookupTableInfo() for available LUTs
/// \param Parameter Offset: 11 Bit value for fixed offset subtraction before transferring the data via the lookup table
/// \return Error message, 0 in case of success else less than 0
///
DWORD PCO_SetLut(WORD Identifier,WORD Parameter);

///
/// \brief Gets the actual bit alignment of the raw image data.
///
/// Since the image data is less than a WORD, which is 16 bit, the data can be placed in two reasonable ways.
/// Either you set the LSB of the image data to the LSB of the transferred data or
/// you set the MSB of the image data to the MSB of the transferred data.
///
/// \param align Pointer to a WORD variable to receive the bit alignment.
/// \return Error message, 0 in case of success else less than 0
///
DWORD PCO_GetBitAlignment(WORD *align);

///
/// \brief Sets the actual bit alignment of the raw image data. See PCO_GetBitAlignment() for details.
/// 
/// Set the following parameter:
/// - wBitAlignment:
///   - 0x0000 = [MSB aligned]; all raw image data will be aligned to the MSB. This is the default setting.
///   - 0x0001 = [LSB aligned]; all raw image data will be aligned to the LSB.
///
/// \param align WORD variable which holds the bit alignment.
/// \return Error message, 0 in case of success else less than 0
///
DWORD PCO_SetBitAlignment(WORD align);

///
/// Get mode of the timestamp function.
///
/// The input pointer will be filled with the following parameter:
/// - 0x0000 = no stamp in image
/// - 0x0001 = BCD coded stamp in the first 14 pixel
/// - 0x0002 = BCD coded stamp in the first 14 pixel + ASCII text
/// - 0x0003 = ASCII text only (see descriptor for availability)
///
/// \param mode Pointer to a WORD variable to receive the time stamp mode.
/// See PCO_SetTimestampMode() for a detailed explanation.
/// \return Error code
///
DWORD PCO_GetTimestampMode(WORD *mode);

///
/// Set mode of the timestamp function.
///
/// To obtain information about the recording time of images this command can be useful. It writes a
/// continuous image number and date / time information with a resolution of 10 \f$\mu\f$s direct into the
/// raw image data. The first 14 pixels (top left corner) are used to hold this information. The
/// numbers are coded in BCD with one byte per pixel, which means that every pixel can hold 2
/// digits. If the pixels have more resolution as 8 bits, then the BCD digits are left bound adjusted and
/// the lower bits are zero. Additionally to this 14 pixels, the information can be written in ASCII text
/// for direct inspection. An 8 by 8 pixel array is used per ASCII digit. The digits are displayed below the BCD coded line.
///
/// The input data should be filled with the following parameter:
/// - 0x0000 = no stamp in image
/// - 0x0001 = BCD coded stamp in the first 14 pixel
/// - 0x0002 = BCD coded stamp in the first 14 pixel + ASCII text
/// - 0x0003 = ASCII text only (see descriptor for availability)
///
/// \b Note:
/// - the image number is set to value = [1], when an arm command is performed
/// - using this command without setting the [date] / [time] results in an error message
///
/// Format of BCD coded pixels:
/// Pixel 1 | Pixel 2 | Pixel 3 | Pixel 4 | Pixel 5 | Pixel 6 | Pixel 7 |
/// --------|---------|---------|---------|---------|---------|---------|
/// image counter (MSB) (00...99)  | image counter (00...99)  | image counter (00...99) | image counter (LSB) (00...99) | year (MSB) (20) | year (LSB) (15...99) | month (01...12) |
///
/// Pixel 8 | Pixel 9 | Pixel 10 | Pixel 11 | Pixel 12 | Pixel 13 | Pixel 14 |
/// --------|---------|---------|---------|---------|---------|---------|
/// day (01...31) | h (00...23) | min (00...59) | s (00...59) | \f$\mu\f$s * 10000 (00...99) | \f$\mu\f$s * 100 (00...99) | \f$\mu\f$s (00...99) |
/// \param mode WORD variable to hold the time stamp mode.
/// \return Error value or 0 in case of success
///
DWORD PCO_SetTimestampMode(WORD mode);

///
/// \brief Get the Hot Pixel correction mode.
///
/// This command is optional and depends on the hardware and firmware. Check the availability according to the camera descriptor (HOT_PIXEL_CORRECTION).
/// Mode:
/// - 0x0000 = [OFF]
/// - 0x0001 = [ON]
///
/// \param wHotPixelCorrectionMode Pointer to a WORD variable to receive the hot pixel correction mode.
/// \return Error message, 0 in case of success else less than 0.
///
DWORD PCO_GetHotPixelCorrectionMode (WORD* wHotPixelCorrectionMode);

///
/// \brief Get the Hot Pixel correction mode.
///
/// This command is optional and depends on the hardware and firmware. Check the availability according to the camera descriptor (HOT_PIXEL_CORRECTION).
/// Mode:
/// - 0x0000 = [OFF]
/// - 0x0001 = [ON]
///
/// \param wHotPixelCorrectionMode Pointer to a WORD variable to receive the hot pixel correction mode.
/// \return Error message, 0 in case of success else less than 0.
///
DWORD PCO_SetHotPixelCorrectionMode (WORD wHotPixelCorrectionMode);

///
/// \brief Gets the metadata mode
///
/// his command is optional and depends on the hardware and firmware. Check the availability
/// according to the camera descriptor (METADATA). Gets the mode for meta data. See
/// PCO_GetMetaData() (dimax only) for more information.
///
/// When wMode is set to 1, the user is responsible to add further
/// line(s) to the buffers, where the number of lines depends on x-resolution and needed wMetaDataSize.
///
/// \param wMode Pointer to a WORD variable receiving the meta data mode.
///  - 0x0000: [OFF]
///  - 0x0001: [ON]
/// \param wMetadataSize Pointer to a WORD variable receiving the meta data block size in additional pixels.
/// \param wMetadataVersion Pointer to a WORD variable receiving the meta data version information.
/// \return Error message, 0 in case of success else less than 0.
///
DWORD PCO_GetMetadataMode(WORD* wMode,WORD* wMetadataSize,WORD* wMetadataVersion);

///
/// \brief Sets the meta data mode
///
/// This command is optional and depends on the hardware and firmware. Check the availability
/// according to the camera descriptor (METADATA). Sets the mode for meta data. See
/// PCO_GetMetaData() (dimax only) for more information.
/// When wMetaDataMode is set to 1, the user is responsible to add further
/// line(s) to the buffers, where the number of lines depends on x-resolution and needed wMetaDataSize.
///
/// This option is only available with pco.dimax
///
/// \param wMode WORD variable to set the meta data mode.
///  - 0x0000: [OFF]
///  - 0x0001: [ON]
/// \param wMetadataSize Pointer to a WORD variable receiving the meta data block size in additional pixels.
/// \param wMetadataVersion Pointer to a WORD variable receiving the meta data version information.
/// \return Error message, 0 in case of success else less than 0.
///
DWORD PCO_SetMetadataMode(WORD wMode,WORD* wMetadataSize,WORD* wMetadataVersion);


/// @name Hardware Input/Output options
///
/// This section contains functions to change and retrieve actual settings for Input and output connectors


///
/// \brief Gets the number of available HW signals. Not applicable to all cameras.
/// 
/// Get the number of hardware IO signals, which are available with the camera. To set and get the
/// single signals use PCO_GetHWIOSignal() (dimax, edge only) and PCO_SetHWIOSignal() (dimax,
/// edge only). This functions is not available with all cameras. Actually it is implemented in the pco.dimax.
/// \param numSignals WORD variable to get the number of signals
/// \return Error message, 0 in case of success else less than 0
///
DWORD PCO_GetHWIOSignalCount(WORD *numSignals);

///
/// \brief Gets the signal descriptor of the requested hardware IO signal. Not applicable to all cameras.
/// 
/// To get the number of available hardware IO signals, please call PCO_GetHWIOSignalCount() (dimax edge only). To set and get the single
/// signals use PCO_GetHWIOSignal() (dimax, edge only) and PCO_SetHWIOSignal() (dimax, edge only).
/// This functions is not available with all cameras. Actually it is implemented in the pco.dimax.
///
/// The output structure has the following parameters:
/// - wSignalDefinitions: Flags showing signal options:
///   - 0x01: Signal can be enabled/disabled
///   - 0x02: Signal is a status output
///   - 0x10: Signal function 1 has got parameter value
///   - 0x20: Signal function 2 has got parameter value
///   - 0x40: Signal function 3 has got parameter value
///   - 0x80: Signal function 4 has got parameter value
/// - wSignalTypes: Flags showing which signal type is available:
///   - 0x01: TTL
///   - 0x02: High Level TTL
///   - 0x04: Contact Mode
///   - 0x08: RS485 differential
/// - wSignalPolarity: Flags showing which signal polarity can be selected:
///   - 0x01: Low level active
///   - 0x02: High Level active
///   - 0x04: Rising edge active
///   - 0x08: Falling edge active
/// - wSignalFilter: Flags showing the filter option:
///   - 0x01: Filter can be switched off (t > ~65ns)
///   - 0x02: Filter can be switched to medium (t > ~1\f$\mu\f$s)
///   - 0x04: Filter can be switched to high (t > ~100ms)
///
/// \param SignalNum WORD variable to query the signal
/// \param SignalDesc Pointer to a SIGNAL_DESC structure to get the signal description
/// \return Error message, 0 in case of success else less than 0
///
DWORD PCO_GetHWIOSignalDescriptor(WORD SignalNum,SC2_Get_HW_IO_Signal_Descriptor_Response *SignalDesc);

///
/// \brief Gets the signal descriptor of the requested signal number as a string for console output.
/// \param SignalNum WORD variable to query the signal
/// \param outbuf Pointer to string to hold the signal description.
/// \param size Pointer to size of input string
/// \return Error message, 0 in case of success else less than 0
///
DWORD PCO_GetHWIOSignalDescriptor(WORD SignalNum,char *outbuf,int *size);

///
/// \brief Gets the signal options of the requested signal number.
/// 
/// Gets the settings of the requested hardware IO signal. This functions is not available with all cameras. Actually it is implemented in the pco.dimax.
/// \param SignalNum Index of the signal
/// \param Enabled Flags showing enable state of the signal
///                - 0x00: Signal is off
///                - 0x01: Signal is active
/// \param Type Flags showing which signal type is selected
///             - 0x01: TTL
///             - 0x02: High Level TTL
///             - 0x04: Contact Mode
///             - 0x08: RS485 differential
/// \param Polarity Flags showing which signal polarity is selected
///                 - 0x01: High level active
///                 - 0x02: Low level active
///                 - 0x04: Rising edge active
///                 - 0x08: Falling edge active
/// \param FilterSetting Flags showing the filter option which is selected
///                      - 0x01: Filter can be switched off (t > ~65ns)
///                      - 0x02: Filter can be switched to medium (t > ~1\f$\mu\f$)
///                      - 0x04: Filter can be switched to high (t > ~100ms)
/// \param Selected In case the HWIOSignaldescription shows more than one SignalNames, this parameter can be used to select a different signal, e.g. 'Status Busy' or 'Status Exposure'.
/// \return Error message, 0 in case of success else less than 0
///
DWORD PCO_GetHWIOSignal(WORD SignalNum,WORD *Enabled,WORD *Type,WORD *Polarity,WORD *FilterSetting,WORD *Selected);

///
/// \brief Sets the signal options of the requested signal number.
/// 
/// Sets the settings of the requested hardware IO signal. This functions is not available with all cameras. Actually it is implemented in the pco.dimax.
/// \param SignalNum Index of the signal
/// \param Enabled Flags showing enable state of the signal
///                - 0x00: Signal is off
///                - 0x01: Signal is active
/// \param Type Flags showing which signal type is selected
///             - 0x01: TTL
///             - 0x02: High Level TTL
///             - 0x04: Contact Mode
///             - 0x08: RS485 differential
/// \param Polarity Flags showing which signal polarity is selected
///                 - 0x01: High level active
///                 - 0x02: Low level active
///                 - 0x04: Rising edge active
///                 - 0x08: Falling edge active
/// \param FilterSetting Flags showing the filter option which is selected
///                      - 0x01: Filter can be switched off (t > ~65ns)
///                      - 0x02: Filter can be switched to medium (t > ~1\f$\mu\f$s)
///                      - 0x04: Filter can be switched to high (t > ~100ms)
/// \param Selected In case the HWIOSignaldescription shows more than one SignalNames, this parameter can be used to select a different signal, e.g. 'Status Busy' or 'Status Exposure'.
/// \return Error message, 0 in case of success else less than 0
///
DWORD PCO_SetHWIOSignal(WORD SignalNum,WORD Enabled,WORD Type,WORD Polarity,WORD FilterSetting,WORD Selected);

///
/// \brief Gets the signal timing parameter  of the requested signal number.
/// 
DWORD PCO_GetHWIOSignalTiming(WORD SignalNum,WORD Selection,DWORD *type,DWORD *Parameter);

///
/// \brief Sets the signal timing parameter  of the requested signal number.
/// 
DWORD PCO_SetHWIOSignalTiming(WORD SignalNum,WORD Selection,DWORD Parameter);


/// @name Enhanced Timing control status
///
/// This section contains functions to change and retrieve further timing settings

///
/// \brief Get mode for CCD or CMOS power down mode (see camera manual).
///
/// Mode:
/// - 0x0000 = [AUTO]
/// - 0x0001 = [USER]
///
/// \param wPowerDownMode Pointer to a WORD variable to receive the power down mode.
/// \return Error message, 0 in case of success else less than 0.
///
DWORD PCO_GetPowerDownMode(WORD* wPowerDownMode);

///
/// \brief Set mode for CCD or CMOS power down threshold time control.
///
/// Power down functions are controllable when power down mode = [user] is selected (see camera manual).
///
/// Mode:
/// - 0x0000 = [AUTO]
/// - 0x0001 = [USER]
///
/// \param wPowerDownMode WORD variable to hold the power down mode.
/// \return Error message, 0 in case of success else less than 0.
///
DWORD PCO_SetPowerDownMode(WORD wPowerDownMode);


///
/// \brief Get user values for CCD or CMOS power down threshold time (see camera manual).
///
/// \param dwPdnTime Pointer to a DWORD variable to receive the power down time.
///                  Current CCD power down threshold time as multiples of ms (0ms .. 49.7 days)
/// \return Error message, 0 in case of success else less than 0.
///
DWORD PCO_GetUserPowerDownTime(DWORD* dwPdnTime);

///
/// \brief Set user values for CCD or CMOS power down threshold time (see camera manual).
///
/// If the exposure time is greater than the selected Power Down Time, then the CCD or CMOS sensor is
/// switched (electrically) into a special power down mode to reduce dark current effects. If power
/// down mode = [user] is selected, the power down threshold time set by this function will become
/// effective. The default Power Down Time is one second.
/// \param dwPdnTime DWORD variable to set the power down time.
///                  Current CCD power down threshold time as multiples of ms (0ms .. 49.7 days)
/// \return Error message, 0 in case of success else less than 0.
///
DWORD PCO_SetUserPowerDownTime(DWORD dwPdnTime);

///
/// \brief Get delay / exposure time table.
///
/// \b General \b note:
///
/// For some camera types it is possible to define a table with delay / exposure times (defined in the
/// camera description). After the exposure is started, the camera will take a series of consecutive
/// images with delay and exposure times, as defined in the table. Therefore, a flexible message
/// format has been defined. The table consists of a maximum of 16 delay / exposure time pairs. If an
/// exposure time entry is set to the value zero, then at execution time this delay / exposure pair is
/// disregarded and the sequence is started automatically with the first valid entry in the table. This
/// results in a sequence of 1 to 16 images with different delay and exposure time settings. External oautomatic image triggering is fully functional for every image in the sequence. If the user wants
/// maximum speed (at CCDs overlapping exposure and read out is taken), [auto trigger] should be
/// selected and the sequence should be controlled with the \<acq enbl\> input.
/// \b Note:
///
/// The commands set delay / exposure time and set delay / exposure time table can only be
/// used alternatively. Using set delay / exposure time has the same effect as using the table
/// command and setting all but the first delay / exposure entry to zero.
/// Despite the same parameter set, this function is different to PCO_GetDelayExposureTime() because the corresponding pointers are used as an array of 16 values each.
///
/// Timebase:
/// - 0: ns
/// - 1: \f$\mu\f$s;
/// - 2: ms
///
/// \param dwDelay Pointer to a DWORD array to receive the exposure times.
/// \param dwExposure Pointer to a DWORD array to receive the delay times.
/// \param wTimeBaseDelay Pointer to a WORD variable to receive the exp. timebase.
/// \param wTimebaseExposure Pointer to a WORD variable to receive the del. timebase.
/// \param wCount Maximum count of delay and exposure pairs, not more than 16 DWORDS.
/// \return Error message, 0 in case of success else less than 0.
///
DWORD PCO_GetDelayExposureTimeTable(DWORD* dwDelay,DWORD* dwExposure,WORD* wTimeBaseDelay,WORD* wTimebaseExposure,WORD wCount);

///
/// \brief Sets the exposure and delay time table and the time bases of the camera.
///
/// Timebase:
/// - 0: ns
/// - 1: \f$\mu\f$s;
/// - 2: ms
///
/// \param dwDelay Pointer to a DWORD array to hold the exposure times.
/// \param dwExposure Pointer to a DWORD array to hold the delay times.
/// \param wTimeBaseDelay WORD variable to hold the exp. timebase.
/// \param wTimebaseExposure WORD variable to hold the del. timebase.
/// \param wCount Count of delay and exposure pairs.
/// \return Error message, 0 in case of success else less than 0.
/// \see PCO_GetDelayExposureTimeTable
///
DWORD PCO_SetDelayExposureTimeTable(DWORD* dwDelay,DWORD* dwExposure,WORD wTimeBaseDelay,WORD wTimebaseExposure,WORD wCount);

///
/// \brief Gets the modulation mode and necessary parameters.
///
/// The Modulation Mode is an optional feature which is not available for all camera models. See the descriptors of the camera.
///
/// - Current modulation mode:
///   - 0x0000 = [modulation mode off]
///   - 0x0001 = [modulation mode on]
/// - Periodical time as a multiple of the timebase unit: The periodical time, delay and exposure time must meet the following condition : tp - (te + td) \> 'Min Per Condition'
/// - Timebase for periodical time
///   - 0x0000 => timebase = [ns]
///   - 0x0001 => timebase = [\f$\mu\f$s]
///   - 0x0002 => timebase = [ms]
/// - Number of exposures: number of exposures done for one frame
/// - Monitor signal offset [ns]: controls the offset for the \<status out\> signal. The possible range is
/// limited in a very special way. See tm in the above timing diagrams. The minimum range is -tstd...0.
/// The negative limit can be enlarged by adding a delay. The maximum negative
/// monitor offset is limited to -20\f$\mu\f$s, no matter how long the delay will be set. The positive
/// limit can be enlarged by longer exposure times than the minimum exposure time. The
/// maximum positive monitor offset is limited to 20us, no matter how long the exposure will be set.
///
/// \param wModulationMode Pointer to a WORD variable to receive the modulation mode
/// \param dwPeriodicalTime Pointer to a DWORD variable to receive the periodical time
/// \param wTimebasePeriodical Pointer to a WORD variable to receive the time base of pt
/// \param dwNumberOfExposures Pointer to a DWORD variable to receive the number of exposures
/// \param lMonitorOffset Pointer to a signed DWORD variable to receive the monitor offset
/// \return Error message, 0 in case of success else less than 0.
///
DWORD PCO_GetModulationMode(WORD *wModulationMode, DWORD* dwPeriodicalTime, WORD *wTimebasePeriodical, DWORD *dwNumberOfExposures, LONG *lMonitorOffset);

///
/// \brief Set the modulation mode and necessary parameters.
///
/// The Modulation Mode is an optional feature which is not available for all camera models. See the descriptors of the camera.
///
/// - Current modulation mode:
///   - 0x0000 = [modulation mode off]
///   - 0x0001 = [modulation mode on]
/// - Periodical time as a multiple of the timebase unit: The periodical time, delay and exposure time must meet the following condition : tp - (te + td) \> 'Min Per Condition'
/// - Timebase for periodical time
///   - 0x0000 => timebase = [ns]
///   - 0x0001 => timebase = [\f$\mu\f$s]
///   - 0x0002 => timebase = [ms]
/// - Number of exposures: number of exposures done for one frame
/// - Monitor signal offset [ns]: controls the offset for the \<status out\> signal. The possible range is
/// limited in a very special way. See tm in the above timing diagrams. The minimum range is -tstd...0.
/// The negative limit can be enlarged by adding a delay. The maximum negative
/// monitor offset is limited to -20\f$\mu\f$s, no matter how long the delay will be set. The positive
/// limit can be enlarged by longer exposure times than the minimum exposure time. The
/// maximum positive monitor offset is limited to 20us, no matter how long the exposure will be set.
///
/// \param wModulationMode WORD variable to hold the modulation mode
/// \param dwPeriodicalTime DWORD variable to hold the periodical time
/// \param wTimebasePeriodical WORD variable to hold the time base of pt
/// \param dwNumberOfExposures DWORD variable to hold the number of exposures
/// \param lMonitorOffset DWORD variable to hold the monitor offset
/// \return Error message, 0 in case of success else less than 0.
///
DWORD PCO_SetModulationMode(WORD wModulationMode, DWORD dwPeriodicalTime, WORD wTimebasePeriodical, DWORD dwNumberOfExposures, LONG lMonitorOffset);


DWORD PCO_GetCMOSLinetiming(WORD* wParameter,WORD* wTimebase,DWORD* dwLineTime);
DWORD PCO_SetCMOSLinetiming(WORD wParameter,WORD wTimebase,DWORD dwLineTime);
DWORD PCO_GetCMOSLineExposureDelay(DWORD* dwExposureLines,DWORD* dwDelayLines);
DWORD PCO_SetCMOSLineExposureDelay(DWORD dwExposureLines,DWORD dwDelayLines);




///
/// \brief Gets the camera synchronisation mode for a dimax.
///
/// Dimax cameras can be cascaded in order to synchronize the timing of a camera chain. It is mandatory to set one of the cameras in the chain to
/// master mode. Usually this is the first camera connected to the chain. All output side connected
/// cameras should be set to slave mode. Those cameras will follow the timing of the master camera, thus all timing settings are disabled at the slave cameras.
/// \param wCameraSynchMode Pointer to a WORD variable to receive the synch mode.
/// - 0x0000 = [off]
/// - 0x0001 = [master]
/// - 0x0002 = [slave]
/// \return Error message, 0 in case of success else less than 0.
///
DWORD PCO_GetCameraSynchMode(WORD* wCameraSynchMode);

///
/// \brief Sets the camera synchronisation mode for a dimax.
///
/// Dimax cameras can be cascaded in order to synchronize the timing of a camera chain. It is mandatory to set one of the cameras in the chain to
/// master mode. Usually this is the first camera connected to the chain. All output side connected
/// cameras should be set to slave mode. Those cameras will follow the timing of the master camera,
/// thus all timing settings are disabled at the slave cameras.
/// \param wCameraSynchMode WORD variable to set the synch mode.
/// - 0x0000 = [off]
/// - 0x0001 = [master]
/// - 0x0002 = [slave]
/// \return Error message, 0 in case of success else less than 0.
///
DWORD PCO_SetCameraSynchMode(WORD wCameraSynchMode);

///
/// \brief Gets the camera fast timing mode for a dimax.
///
/// To increase the possible exposure time with high frame rates it is possible to enable the 'Fast Timing' mode. This means that the maximum possible
/// exposure time can be longer than in normal mode, while getting stronger offset drops. In case,
/// especially in PIV applications, image quality is less important, but exposure time is, this mode
/// reduces the gap between exposure end and start of the next exposure from ~75\f$\mu\f$S to 3.5\f$\mu\f$S.
/// \param wFastTimingMode Pointer to a WORD variable to receive the fast timing mode.
/// - 0x0000 = [OFF]
/// - 0x0001 = [ON]
/// \return Error message, 0 in case of success else less than 0.
///
DWORD PCO_GetFastTimingMode(WORD* wFastTimingMode);

///
/// \brief Set the camera fast timing mode for a dimax.
///
/// To increase the possible exposure time with high frame rates it is possible to enable the 'Fast Timing' mode. This means that the maximum possible
/// exposure time can be longer than in normal mode, while getting stronger offset drops. In case,
/// especially in PIV applications, image quality is less important, but exposure time is, this mode
/// reduces the gap between exposure end and start of the next exposure from ~75\f$\mu\f$S to 3.5\f$\mu\f$S.
/// \param wFastTimingMode WORD variable to set the fast timing mode.
/// - 0x0000 = [OFF]
/// - 0x0001 = [ON]
/// \return Error message, 0 in case of success else less than 0.
///
DWORD PCO_SetFastTimingMode(WORD wFastTimingMode);


/// @name Enhanced Information
///
/// This section contains functions to retrieve further information

///
/// \brief Gets the firmware versions as string for console output.
/// 
/// If the string size is not large enough, not all firmware strings will be shown.
/// \param outbuf Pointer to string to hold the firmware version output.
/// \param size Pointer int that holds the size of the string.
/// \return Error message, 0 in case of success else less than 0
///
DWORD PCO_GetFirmwareVersion(char *outbuf,int *size);

///
/// \brief Gets the hardware versions as string for console output.
/// 
/// If the string size is not large enough, not all hardware strings will be shown.
/// \param outbuf Pointer to string to hold the firmware version output.
/// \param size Pointer to size of the string.
/// \return Error message, 0 in case of success else less than 0
///
DWORD PCO_GetHardwareVersion(char *outbuf,int *size);

///
/// \brief Gets the firmware versions of first 10 devices in the camera.
/// \anchor PCO_GetFirmwareVersion
///
/// Get Firmware version of first 10 devices and the number of installed devices
/// If number of devices exceeds 10 PCO_GetFirmwareVersionExt must be called with increased block counter
/// \param response Pointer to a the firmware version structure
/// \return Error message, 0 in case of success else less than 0.
///
DWORD PCO_GetFirmwareVersion(SC2_Firmware_Versions_Response* response);


///
/// \brief Gets the firmware versions of the devices in the camera
/// \anchor PCO_GetFirmwareVersionExt
///
/// not applicable to all cameras.
/// \param blocknumber Pointer to a number, which contains the actual blocknumber
/// \return Error message, 0 in case of success else less than 0.
///
DWORD PCO_GetFirmwareVersionExt(BYTE bNum,SC2_Firmware_Versions_Response* response);

/// @name IEEE1394 interface parameters
///
/// This section contains functions to change and retrieve parameters for the IEEE1394 interface

///
/// \brief Gets the IEEE1394 interface parameters.
///
/// Gets the FireWire transfer parameters.
///
/// \param wMasterNode Pointer to WORD variable to receive the master node address.
/// \param wIsochChannel Pointer to WORD variable to recieve the used ISO channel.
/// \param wIsochPacketLen Pointer to WORD variable to receive the ISO packet length.
/// \param wIsochPacketNum Pointer to WORD variable to receive the ISO packet count.
/// \return Error message, 0 in case of success else less than 0.
/// \see PCO_SetIEEE1394InterfaceParams
///
DWORD PCO_GetIEEE1394InterfaceParams(WORD* wMasterNode,WORD* wIsochChannel,WORD* wIsochPacketLen,WORD* wIsochPacketNum);

///
/// \brief PCO_SetIEEE1394InterfaceParams Sets the IEEE1394 interface parameters.
///
/// The user can instantiate a structure _PCO1394_ISO_PARAMS, which is defined in SC2_SDKAddendum.h.
/// - bandwidth_bytes: set to a bandwidth size which is a fraction of 4096 / (num of cameras). e.g. 2
/// - cameras connected: bandwidth_bytes = 2048.
/// - speed_of_isotransfer: 1,2,4, whereas 1 is 100MBit/s, 2=200 and 4=400; default is 4.
/// - number_of_isochannel: Channel numbers are 32-bit encoded and the highest bit equals the lowest channel. (e.g. 0x80000000 = channel 0).
/// - number_of_isobuffers: 16...256; default is 128
/// - byte_per_isoframe: set to the same value as bandwidth_bytes.
///
/// Remarks for number_of_isochannel: Usually it is not necessary to change this parameter (Open_Grabber() does it automatically), but in
/// case the user wants to transfer images from more than one camera, the iso channel must be unique
/// for each camera.
/// Only one bit may be set at a time.
///
/// In case the user wants to establish a connection, this has to be the first command sent or the camera won't know how to respond to commands.
///
/// \param wMasterNode WORD variable to set the master node address.
/// \param wIsochChannel WORD variable to set the ISO channel.
/// \param wIsochPacketLen WORD variable to set the ISO packet length.
/// \param wIsochPacketNum WORD variable to set the ISO packet count.
/// \return Error message, 0 in case of success else less than 0.
///
DWORD PCO_SetIEEE1394InterfaceParams(WORD wMasterNode,WORD wIsochChannel,WORD wIsochPacketLen,WORD wIsochPacketNum);

///
/// \brief Gets the IEEE1394 byte order.
///
/// \param wMode Pointer to WORD variable to receive the IEEE1394 byte order.
/// \return Error message, 0 in case of success else less than 0.
///
DWORD PCO_GetIEEE1394ISOByteorder(WORD* wMode);

///
/// \brief Sets the IEEE1394 byte order.
///
/// \param wMode WORD variable to set the IEEE1394 byte order.
/// \return Error message, 0 in case of success else less than 0.
///
DWORD PCO_SetIEEE1394ISOByteorder(WORD wMode);


/// @name HD/SDI interface parameters and image transfer control
///
/// This section contains functions to control output to the HD/SDI interface

///
/// \brief Gets the actual interface output format.
///
/// This is only valid with a dimax with a built in HD/SDI interface.
/// This command can be used to determine the image streaming interface, which is active.
/// If the addressed interface is set to [off], then the standard interface, e.g. GigE or USB, is used to
/// stream the data. If the addressed interface is activated, the standard interface is only for camera
/// control, thus streaming to this interface is disabled.
///
/// \param wInterface WORD variable to get the desired interface.
///   - 0: reserved
///   - 1: HD/SDI
///   - 2: DVI
/// \param wFormat Pointer to WORD variable to get the interface format
///   - 0: Output is disabled
///   - 1: HD/SDI, 1080p25, RGB
///   - 2: HD/SDI, 1080p25, arbitrary raw mode
/// \return Error message, 0 in case of success else less than 0.
///
DWORD PCO_GetInterfaceOutputFormat(WORD wInterface,WORD* wFormat);


///
/// \brief Sets the actual interface output format.
///
/// This option is only available with pco.dimax and HD/SDI interface.
/// This command can be used to set the image streaming interface, which is active. If the
/// addressed interface is set to [off], then the standard interface, e.g. GigE or USB, is used to stream
/// the data. If the addressed interface is activated, the standard interface is only for camera control,
/// thus streaming to this interface is disabled.
///
/// \param wInterface WORD variable to set the desired interface.
///   - 0: reserved
///   - 1: HD/SDI
///   - 4: DVI
/// \param wFormat WORD variable to set the interface format
///   - 0: Output is disabled
///   - 1: HD/SDI, 1080p25, RGB
///   - 2: HD/SDI, 1080p25, arbitrary raw mode
/// \return Error message, 0 in case of success else less than 0.
///
DWORD PCO_SetInterfaceOutputFormat(WORD wInterface,WORD wFormat);

///
/// \brief Get interface status messages.
/// This option is only available with pco.dimax and HD/SDI interface.
///
/// \param wInterface WORD variable holding the interface to be queried.
/// \param dwWarnings Pointer to WORD variable to receive the warnings.
/// \param dwErrors Pointer to WORD variable to receive the errors.
/// \param dwStatus Pointer to WORD variable to receive the status.
/// \return Error message, 0 in case of success else less than 0.
///
DWORD PCO_GetInterfaceStatus(WORD wInterface,DWORD* dwWarnings,DWORD* dwErrors,DWORD* dwStatus);



/// @name HD/SDI interface image transfer control


///
/// \brief Plays the images recorded to the camera RAM.
///
/// This option is only available with pco.dimax and HD/SDI interface.
//  This interface does not request images, but it has to be supplied with a continuous data stream.
///
/// \b Note: Command is only valid, if \b storage \b mode is set to [recorder] and recording to the camera RAM segment is stopped!
///
/// The play speed is defined by the Speed parameter together with the Mode parameter:
///
/// - Fast forward: The play position is increased by [Speed], i.e. [Speed - 1] images are leaped.
/// - Fast rewind: The play position is decreased by [Speed], i.e. [Speed - 1] images are leaped.
/// - Slow forward: The current image is sent [Speed] times before the position is increased
/// - Slow rewind: The current image is sent [Speed] times before the position is decreased
///
/// The play command can also be sent to change parameters (e.g. speed) while a play is active. The
/// new parameters will be changed immediately. It is possible to change parameters like play speed
/// or play direction without changing the current position by setting Start No. to -1 or
/// 0xFFFFFFFFH in the DWORD format.
///
/// \param wSegment WORD variable with the segment to read from
/// \param wInterface WORD variable to set the interface (0x0001 for HDSDI)
/// \param wMode WORD variable to set the play mode
///  - 0: Stop play,
///  - 1: Fast forward (step 'wSpeed' images),
///  - 2: Fast rewind (step 'wSpeed' images),
///  - 3: Slow forward (show each image 'wSpeed'-times)
///  - 4: Slow rewind (show each image 'wSpeed'-times)
///  - Additional flags: 0x0100-> - 0: Repeat last image
///                               - 1: Repeat sequence
/// \param wSpeed WORD variable to set the stepping or repeat count
/// \param dwRangeLow Lowest image number to be played
/// \param dwRangeHigh Highest image number to be played
/// \param dwStartPos Set position to image number #, -1: unchanged
/// \return Error message, 0 in case of success else less than 0.
///
DWORD PCO_PlayImagesFromSegment(WORD wSegment,WORD wInterface,WORD wMode,WORD wSpeed,DWORD dwRangeLow,DWORD dwRangeHigh,DWORD dwStartPos);

///
/// \brief The "Get Play Position" command requests at which position the play pointer of the currently started sequence is.
///
/// When the command "Play Images from Segment" was called, the sequence is started and the
/// response message is sent immediately, whereas it may take seconds or up to minutes, until the sequence transmission is finished.
///
/// \b Note: Due to time necessary for communication and processing the command, the actual pointer may be 1 or 2 images ahead at the time, when the response is sent completely.
///
/// \param wStatus WORD variable to get the status of image play state machine.
///  - 0: no play is active, or play has already stopped
///  - 1: play is active, position is valid
/// \param dwPosition: Number of the image currently sent to the interface. It is between Range Low and Range High, as set by "Play Images from Segment". Only valid when sequence play is still active.
/// \return Error message, 0 in case of success else less than 0.
///
DWORD PCO_GetPlayPosition(WORD* wStatus,DWORD *dwPosition);


/// cond

//seems to be a non operational command
DWORD PCO_SetVideoPayloadIdentifier(WORD wSegment,WORD wMode1,WORD wMode2,DWORD dwSetPos1,DWORD dwClrPos1,DWORD dwSetPos2,DWORD dwClrPos2,DWORD dwSetPos3,DWORD dwClrPos3,DWORD dwSetPos4,DWORD dwClrPos4);
/// endcond


///
/// \brief Gets the color convert settings inside the camera.
///
/// This option is only available with pco.dimax and HD/SDI interface.
///
/// \param ColSetResp Pointer to a SC2_Set_Color_Settings structure to receive the color set data.
/// \return Error message, 0 in case of success else less than 0.
///
DWORD PCO_GetColorSettings(SC2_Get_Color_Settings_Response* ColSetResp);

///
/// \brief Sets the color convert settings inside the camera.
///
/// This option is only available with pco.dimax and HD/SDI interface.
///
/// \param SetColSet Pointer to a SC2_Set_Color_Settings structure to set the color set data.
/// \return Error message, 0 in case of success else less than 0.
///
DWORD PCO_SetColorSettings(SC2_Set_Color_Settings* SetColSet);

///
/// \brief  Starts a white balancing calculation.
///
/// This option is only available with pco.dimax and HD/SDI interface.
///
/// \param wMode WORD variable to set the meta data mode. Set to 1.
/// \return Error message, 0 in case of success else less than 0.
///
DWORD PCO_DoWhiteBalance(WORD wMode);


///
/// \brief Gets the white balancing status.
///
/// This option is only available with pco.dimax and HD/SDI interface.
///
/// \param wStatus Pointer to WORD variable to receive the status.
/// \param wColorTemp Pointer to WORD variable to recieve the color temperature.
/// \param sTint Pointer to SHORT variable to recieve the tint.
/// \return Error message, 0 in case of success else less than 0.
///
DWORD PCO_GetWhiteBalanceStatus(WORD* wStatus,WORD* wColorTemp,SHORT* sTint);


/// @name Special control status
///
/// This section contains functions to change and retrieve special camera settings

///
/// \brief Starts a self test procedure.
///
/// See PCO_GetCameraHealthStatus().
/// \param dwWarn Pointer to DWORD variable to receive warnings.
/// \param dwErr Pointer to DWORD variable to receive errors.
/// \return Error message, 0 in case of success else less than 0.
///
DWORD PCO_InitiateSelftestProcedure(DWORD* dwWarn, DWORD* dwErr);


///
/// \brief Writes a hot pixel list to the camera.
///
/// This command is optional and depends on the hardware and firmware. Check the availability
/// according to the camera descriptor (HOT_PIXEL_CORRECTION). To change the hot pixel list
/// inside the camera, please call PCO_ReadHotPixelList() first, then modify the list and write it back
/// with this command. We recommend doing a backup of the list after readout. An invalid list will break the hot pixel correction!
///
/// The x and y coordinates have to be consistent, that means corresponding coordinate pairs must have the same index!
/// \param wListNo WORD variable which holds the number of the list (zero based).
/// \param wNumValid WORD variable which holds the number of valid members
/// \param wHotPixX WORD array which holds the x coordinates of a hotpixel list
/// \param wHotPixY WORD array which holds the y coordinates of a hotpixel list
/// \return Error message, 0 in case of success else less than 0.
///
DWORD PCO_WriteHotPixelList(WORD wListNo, WORD wNumValid,WORD* wHotPixX, WORD* wHotPixY);

///
/// \brief Reads a hot pixel list from the camera.
///
/// This command is optional and depends on the hardware and firmware. Check the availability
/// according to the camera descriptor (HOT_PIXEL_CORRECTION). To change the hot pixel list
/// inside the camera, please call this command first, then modify the list and write it back
/// with PCO_WriteHotPixelList(). We recommend doing a backup of the list after readout. An invalid list will break the hot pixel correction!
/// \param wListNo WORD variable which holds the number of the list (zero based).
/// \param wArraySize WORD variable which holds the number of members, which can be transferred to the list
/// \param wNumValid Pointer to a WORD variable to receive the number of valid hotpixel.
/// \param wNumMax Pointer to a WORD variable to receive the max. possible number of hotpixel.
/// \param wHotPixX WORD array which gets the x coordinates of a hotpixel list
///                 This ptr can be set to ZERO if only the valid and max number have to be read.
/// \param wHotPixY WORD array which gets the y coordinates of a hotpixel list
///                 This ptr can be set to ZERO if only the valid and max number have to be read.
/// \return Error message, 0 in case of success else less than 0.
///
DWORD PCO_ReadHotPixelList(WORD wListNo, WORD wArraySize,WORD* wNumValid, WORD* wNumMax, WORD* wHotPixX, WORD* wHotPixY);

///
/// \brief Clears a hot pixel list in the camera.
///
/// This command is optional and depends on the hardware and firmware. Check the availability
/// according to the camera descriptor (HOT_PIXEL_CORRECTION). To change the hot pixel list
/// inside the camera, please first call PCO_ReadHotPixelList(). Then modify the list and write it back
/// with PCO_WriteHotPixelList(). We recommend doing a backup of the list after readout. An invalid
/// list will break the hot pixel correction! This command clears the list addressed completely. Use with caution!
///
/// Set the following parameter:
/// - wListNo: Number of the list to modify (0 ...).
/// - dwMagic1: Unlock code, set to 0x1000AFFE
/// - dwMagic2: Unlock code, set to 0x2000ABBA
///
/// \param wListNo WORD variable which holds the number of the list (zero based).
/// \param dwMagic1 DWORD variable which holds the unlock code 1.
/// \param dwMagic2 DWORD variable which holds the unlock code 2.
/// \return Error message, 0 in case of success else less than 0.
///
DWORD PCO_ClearHotPixelList(WORD wListNo, DWORD dwMagic1,DWORD dwMagic2);

///
/// \overload
///
/// \return
///
DWORD PCO_ClearHotPixelList(WORD wListNo);

///
/// \brief Loads a lookup table to the camera, if available.
///
/// Only available with a pco.edge.
///
/// \param wIdentifier WORD variable to hold the LUT to be loaded
/// \param wPacketNum WORD variable to hold the packet number to load the LUT in several steps.
/// \param wFormat WORD variable to hold the data structure in bData (see defnines)
/// \param wLength WORD variable to hold the length of the data structure
/// \param bData Pointer to BYTE to hold the actual data
/// \return Error message, 0 in case of success else less than 0.
///
DWORD PCO_LoadLookuptable(WORD wIdentifier,WORD wPacketNum,WORD wFormat,WORD wLength,BYTE* bData);

///
/// \brief Reads a lookup table from the camera, if available.
///
/// Only available with a pco.edge.
///
/// \param wIdentifier WORD variable to hold the LUT to be read
/// \param wPacketNum WORD variable to hold the packet number to read the LUT in several steps.
/// \param wFormat Pointer to WORD variable to receive the data structure in bData (see defnines)
/// \param wLength Pointer to WORD variable to receive the length of the data structure
/// \param bData Pointer to BYTE array to receive the data
/// \param buflen WORD variable to hold the length of the BYTE array (bData)
/// \return Error message, 0 in case of success else less than 0.
///
DWORD PCO_ReadLookuptable(WORD wIdentifier,WORD wPacketNum,WORD* wFormat,WORD* wLength,BYTE* bData,WORD buflen);

///
/// \brief Gets the color multiplier matrix to normalize the color values of a color camera to 6500k.
///
/// Only available with a dimax
/// \param szCCM Pointer to a string to hold the values.
/// \param len Pointer to a WORD that holds the string length.
/// \return Error message, 0 in case of success else less than 0.
///
DWORD PCO_GetColorCorrectionMatrix(char* szCCM,WORD* len);

///
/// \brief Requests the Bayer multipliers.
///
/// The Bayer multipliers are used by cameras with color sensor in order to compensate the color response of the sensor and the optical setup. Thus when exposed to
/// white light the R Gr Gb B pixels will ideally show the same amplitude. This option is only available with a pco.dimax
/// \param wMode
/// - 0x0001: The returned values are changed, but not yet saved.
/// - 0x0002: The returned values are saved.
/// \param wMul Pointer to an array of four WORD;
///             Red/GreenRed/GreenBlue/Blue Multiplier: Number from 0 to 3999, where 1000 corresponds to multiplier of 1.0 (leave values unchanged).
///             Element 0 is the same as in the color descriptor. See wColorPatternDESC in PCO_Description.
/// \return Error message, 0 in case of success else less than 0.
///
DWORD PCO_GetBayerMultiplier(WORD* wMode,WORD* wMul);

///
/// \brief Sets the Bayer multipliers.
///
/// The Bayer multipliers are used by cameras with color sensor in order to compensate the color response of the sensor and the optical setup. Thus when exposed to
/// white light the R Gr Gb B pixels will ideally show the same amplitude. This option is only available with a pco.dimax
/// \param wMode
/// - 0x0001: Set new values immediately but do not save.
/// - 0x0002: Save values and set immediately.
/// \param wMul Pointer to an array of four WORD;
///             Red/GreenRed/GreenBlue/Blue Multiplier: Number from 0 to 3999, where 1000 corresponds to multiplier of 1.0 (leave values unchanged).
///             Element 0 is the same as in the color descriptor. See wColorPatternDESC in PCO_Description.
/// \return Error message, 0 in case of success else less than 0.
///
DWORD PCO_SetBayerMultiplier(WORD wMode,WORD* wMul);

///
/// \brief Get parameters for fan operation control
///
/// Only supported from cameras which have GENERALCAPS1_FAN_CONTROL Bit set in the descriptor.
/// Get possible Parameter values which can be set with the PCO_SetFanControlStatus() function in UserMode.
///
/// Mode can be set to 
///  - 0x0000 = Automatic: Fan speed is controlled depending on camera temperature.
///  - 0x0001 = UserMode: 
///
/// \param wFanMode Pointer to WORD variable to get the actual fan operation control mode
/// \param wFanMin  Pointer to WORD variable which holds the minimal setting for fan speed
/// \param wFanMax  Pointer to WORD variable which holds the maximal setting for fan speed
/// \param wStepSize Pointer to WORD variable which holds the steps for fan speed setting
/// \param wSetValue Pointer to WORD variable to get the fan speed setting, from last set call
/// \param wActualValue Pointer to WORD variable to get the actual fan speed
/// \return Error message, 0 in case of success else less than 0.
///
DWORD PCO_GetFanControlStatus(WORD* wFanMode,WORD* wFanMin,WORD* wFanMax,WORD* wStepSize,WORD* wSetValue,WORD* wActualValue);


///
/// \brief Set parameters to control Fan operation
///
/// Only supported from cameras which have GENERALCAPS1_FAN_CONTROL Bit set in the descriptor.
/// Mode can be set to 
///  - 0x0000 = Automatic: Fan speed is controlled depending on camera temperature.
///  - 0x0001 = User: 
///
/// \param wFanMode switch fan control mode
/// \param wSetValue 
/// \return Error message, 0 in case of success else less than 0.
///
DWORD PCO_SetFanControlStatus(WORD wFanMode,WORD wSetValue);


///
/// \brief Get operating state of the backpanel LEDs.
///
/// \param dwParameter DWORD variable to get operating state of LED's.
/// - 0x00000000 = [OFF]
/// - 0xFFFFFFFF = [ON]
/// \return Error message, 0 in case of success else less than 0.
///
DWORD PCO_GetHWLEDSignal(DWORD* dwParameter);

///
/// \brief Switch the operating state of the back panel LEDs.
///
/// To ensure, that light of the camera LED's doesn't spoil lowlight level exposures, the state of the LED can be changed.
/// With this command LED's could be switched OFF and on again to get minimal status information of the camera.
/// \param dwParameter DWORD variable to set the Led Signals.
/// - 0x00000000 = [OFF]
/// - 0xFFFFFFFF = [ON]
/// \return Error message, 0 in case of success else less than 0.
///
DWORD PCO_SetHWLEDSignal(DWORD dwParameter);

///
/// \brief Gets the camera internal DSNU adjustment mode.
///
/// Only available with a dimax.
///
/// \param wMode
/// - 0: no DSNU correction.
/// - 1: automatic DSNU correction.
/// - 2: manual DSNU correction.
/// \return Error message, 0 in case of success else less than 0.
///
DWORD PCO_GetDSNUAdjustMode(WORD *wMode);

///
/// \brief Sets the camera internal DSNU adjustment mode.
///
/// Only available with a dimax.
///
/// \param wMode
/// - 0: no DSNU correction.
/// - 1: automatic DSNU correction.
/// - 2: manual DSNU correction.
/// \return Error message, 0 in case of success else less than 0.
///
DWORD PCO_SetDSNUAdjustMode(WORD wMode);

///
/// \brief Starts the camera internal DSNU adjustment in case it is set to manual.
///
/// Only available with a dimax.
///
/// \param wMode
/// - 0: no DSNU correction.
/// - 1: automatic DSNU correction.
/// - 2: manual DSNU correction.
/// \return Error message, 0 in case of success else less than 0.
///
DWORD PCO_InitDSNUAdjustment(WORD *wMode);

///
/// \brief Gets the correlated double image mode of the camera.
///
/// Only available with a dimax.
///
/// \param wMode Pointer to a WORD variable to receive the correlated double image mode.
/// \return Error message, 0 in case of success else less than 0.
///
DWORD PCO_GetCDIMode(WORD* wMode);

///
/// \brief Sets the correlated double image mode of the camera.
///
/// Only available with a dimax.
///
///
/// \param wMode WORD variable to set the correlated double image mode.
/// \return Error message, 0 in case of success else less than 0.
///
DWORD PCO_SetCDIMode(WORD wMode);

///
/// \brief Gets the camera power save mode.
///
/// Not applicable to all cameras.
/// \param wMode WORD pointer to get the actual power save mode. (0-off,default; 1-on)
/// \param wDelayMinutes WORD pointer to get the delay until the camera enters power save mode after main power loss. The actual switching delay is between wDelayMinutes and wDelayMinutes + 1. Possible range is 1 - 60.
/// \return Error message, 0 in case of success else less than 0.
///
DWORD PCO_GetPowersaveMode(WORD* wMode,WORD* wDelayMinutes);

///
/// \brief Sets the camera power save mode.
///
/// Not applicable to all cameras.
/// \param wMode WORD to set the actual power save mode. (0-off,default; 1-on)
/// \param wDelayMinutes WORD to set the delay until the camera enters power save mode after main power loss. The actual switching delay is between wDelayMinutes and wDelayMinutes + 1. Possible range is 1 - 60.
/// \return Error message, 0 in case of success else less than 0.
///
DWORD PCO_SetPowersaveMode(WORD wMode,WORD wDelayMinutes);

///
/// \brief Gets the camera battery status.
///
/// Not applicable to all cameras.
/// \param wBatteryType WORD pointer to get the battery type.
/// - 0x0000 = no battery mounted
/// - 0x0001 = nickel metal hydride type
/// - 0x0002 = lithium ion type
/// - 0x0003 = lithium iron phosphate type
/// - 0xFFFF = unknown battery type
/// \param wBatteryLevel WORD pointer to get the battery level in percent.
/// \param wPowerStatus WORD pointer to get the power status.
/// - 0x0001 = power supply is available
/// - 0x0002 = battery mounted and detected
/// - 0x0004 = battery is charged
///
/// Bits can be combined e.g. 0x0003 means that camera has a battery and is running on external power, 0x0002: camera runs on battery.
/// \return Error message, 0 in case of success else less than 0.
///
DWORD PCO_GetBatteryStatus(WORD* wBatteryType,WORD* wBatteryLevel,WORD* wPowerStatus);


/// \cond

///
/// \brief Gets the value of an external register.
///
/// \param wID WORD pointer to hold the number of the register
/// \param bData Pointer to a BYTE variable to recieve the value of the register.
/// \param len Pointer to a WORD variable to recieve the length of the register.
/// \return Error message, 0 in case of success else less than 0.
///
DWORD PCO_GetExternalRegister(WORD* wID,BYTE* bData,WORD *len);

///
/// \brief Sets the value of an external register.
///
/// Values: 0x0000 - 0x0003
///
/// \param wID WORD pointer to hold the number of the register.
/// \param bData Pointer to a BYTE variable to hold the value of the register.
/// \param wDataSize Pointer to a WORD variable to hold the length of the value to be written.
/// \return Error message, 0 in case of success else less than 0.
///
DWORD PCO_SetExternalRegister(WORD* wID,BYTE* bData,WORD* wDataSize);


DWORD PCO_WriteMailbox(WORD wMailboxNo,BYTE* bData,WORD len);

DWORD PCO_ReadMailbox(WORD wMailboxNo,BYTE* bData,WORD* len);

DWORD PCO_GetMailboxStatus(WORD* wNumberOfMailboxes,WORD* wMailboxStatus,WORD *len);

DWORD PCO_GetFlimModulationParams(WORD* wSourceSelect,WORD* wOutputWaveform);
DWORD PCO_SetFlimModulationParams(WORD wSourceSelect,WORD wOutputWaveform);
DWORD PCO_GetFlimPhaseSequenceParams(WORD* wPhaseNumber,WORD* wPhaseSymmetry,WORD* wPhaseOrder,WORD* wTapSelect);
DWORD PCO_SetFlimPhaseSequenceParams(WORD wPhaseNumber,WORD wPhaseSymmetry,WORD wPhaseOrder,WORD wTapSelect);
DWORD PCO_GetFlimImageProcessingFlow(WORD* wAsymmetryCorrection);
DWORD PCO_SetFlimImageProcessingFlow(WORD wAsymmetryCorrection);
DWORD PCO_GetFlimMasterModulationFrequency(DWORD* dwFrequency);
DWORD PCO_SetFlimMasterModulationFrequency(DWORD dwFrequency);
DWORD PCO_GetFlimRelativePhase(DWORD* dwPhaseMilliDeg);
DWORD PCO_SetFlimRelativePhase(DWORD dwPhaseMilliDeg);

// implementation dependent on actual class 
virtual DWORD PCO_GetTransferParameter(void* buf ATTRIBUTE_UNUSED,int length ATTRIBUTE_UNUSED){ return PCO_NOERROR;}
virtual DWORD PCO_SetTransferParameter(void* buf ATTRIBUTE_UNUSED,int length ATTRIBUTE_UNUSED){ return PCO_NOERROR;}
//  void  PCO_SetVerbose(int iNewState);
//  DWORD PCO_GetImageTiming(void);

/// \endcond

#endif

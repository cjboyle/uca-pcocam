//-----------------------------------------------------------------//
// Name        | Cpco_com_func.h             | Type: ( ) source    //
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
//  see Cpco_com_func.cpp                                          //
//-----------------------------------------------------------------//

#ifndef CPCO_COM_FUNC_H
#define CPCO_COM_FUNC_H

///
/// \file Cpco_com_func.h
///
/// \brief Basic camera communication
///
/// \author PCO AG
///

/// @name Camera Control Functions
///
/// These functions are used to communicate with the camera.
/// The functions are arranged in order of importance and similar tasks are grouped together.
/// All functions return an Error code. In case of success PCO_NOERROR=0 else one of the error codes defined in PCO_Err.h.
///

/// @name General Control and Status
///
/// This section contains general functions to control the camera and to request status information about the camera.
///

/// @{
///
/// \brief Sets the current recording state and waits until the status is valid. If the state cannot be set within one second
/// (+ current frametime for state [stop]), the function will return an error.
///
/// The recording state controls the run state of the camera. If the Recording State is [run], sensor exposure and
/// readout sequences are started depending on current camera settings (trigger mode,acquire mode, external signals...).\n
/// The Recording State has the highest priority compared to functions like \<acq enbl\> or exposure trigger.\n
/// When the Recording State is set to [stop], sensor exposure and readout sequences are stopped.
/// If the camera is currently in [sensor_readout] state, this readout is finished, before camera run
/// state is changed to [sensor_idle]. If the camera is currently in [sensor_exposing] state, the
/// exposure is cancelled and camera run state is changed immediately to [sensor_idle].
/// In run state [sensor_idle] the camera is running a special idle mode to prevent dark charge accumulation.\n
/// \n
/// If any camera parameter was changed: before setting the Recording State to [run], the function
/// \ref PCO_ArmCamera must be called. This is to ensure that all settings were correctly and are
/// accepted by the camera.
/// If a successful Recording State [run] command is sent and recording is started, the images from
/// a previous record to the active segment are lost.
///
/// The recording status has the highest priority compared to functions like \<acq enbl\> or \<exp trig\>.
/// The recording state can be [stop]'ped at any time.
/// The recording state can be set to [run] only if the camera was successfully armed before.
///
/// \param recstate Variable to set the active recording state.
/// - 0x0000 = stop camera and wait until recording state = [stop]
/// - 0x0001 = start camera and wait until recording state = [run]
/// \return Error code or PCO_NOERROR on success
///
DWORD PCO_SetRecordingState(WORD recstate);


///
/// \brief Requests the current recording state.
/// 
/// This function returns the current Recording State of the camera.
/// The Recording State can change from [run] to [stop] through:
/// - Call to function PCO_SetRecordingState [stop]
/// - PCO_SetStorageMode is [recorder], PCO_SetRecorderSubmode is [sequence] and active segment is full
/// - PCO_SetStorageMode is [recorder], PCO_SetRecorderSubmode is [ring buffer],
/// - PCO_SetRecordStopEvent is [on] and the given number of images is recorded.
///
/// \param recstate Pointer to a WORD variable to get the current recording state.
/// - 0x0000 = camera is stopped, recording state [stop]
/// - 0x0001 = camera is running, recording state [run]
/// \return Error code or PCO_NOERROR on success
///
DWORD PCO_GetRecordingState(WORD* recstate);


///
/// \brief Arms the camera and validates the settings
/// \anchor PCO_ArmCamera
/// This function does arm, this means prepare the camera for a following recording. All
/// configurations and settings made up to this moment are accepted, validated and the internal
/// settings of the camera are prepared. If the arm was successful the camera state is changed to
/// [armed] and the camera is able to start image recording immediately, when recording state is
/// set to [run].\n
/// The command will be rejected, if Recording State is [run], see \ref PCO_GetRecordingState.\n
/// On power up the camera is in state [not armed] and Recording State [stop].
/// Camera arm state is changed to [not armed], when settings are changed, with the following
/// exception. Camera arm state is not changed, when settings related to exposure time will be done
/// during Recording State [run].\n
///
/// \return Error code or PCO_NOERROR on success
///
DWORD PCO_ArmCamera();

///
/// \brief Gets the actual armed image size of the camera.
/// This accounts for binning and ROI.
/// If the user recently changed size influencing values without issuing an ARM, the GetSizes function will return
/// the sizes from the last recording. If no recording occurred, it will return the last ROI settings.
/// 
/// \param width Pointer to an DWORD to get the current width in pixel
/// \param height Pointer to an DWORD to get the current height in pixel
/// \return Error code or PCO_NOERROR on success
///
DWORD PCO_GetActualSize(DWORD* width,DWORD* height);

///
/// \brief Resets all camera settings to default values.
/// This function can be used to reset all camera settings to its default values. This function is also
/// executed during a power-up sequence. The camera must be stopped before calling this
/// command. Default settings are slightly different for all cameras.\n
/// The following are the default settings:
/// \latexonly \vspace{0.4cm} \endlatexonly
///
/// | Setting: | Default: |
/// |----------|----------|
/// | Sensor Format | Standard |
/// | ROI | Full resolution |
/// | Binning | No binning |
/// | Pixel Rate | Depending on camera type |
/// | Gain | Normal gain (if setting available due to sensor) |
/// | Double Image Mode | Off |
/// | IR sensitivity | Off (if setting available due to sensor) |
/// | Cooling Set point | Depending on camera type |
/// | ADC mode | Using one ADC (if setting available |
/// | Exposure Time | Depending on camera type (10-20ms) |
/// | Delay Time | 0ms |
/// | Trigger Mode | Auto Trigger |
/// | Recording state | Stopped |
/// | Memory Segmentation | Total memory allocated to first segment |
/// | Storage Mode | Recorder Ring Buffer |
/// | Acquire Mode | Auto |
/// 
/// \return Error code or PCO_NOERROR on success
///
DWORD PCO_ResetSettingsToDefault();

///
/// \brief Sets the camera time to current system time.
/// 
/// The date and time is updated automatically, as long as the camera is supplied with power.
/// Camera time is used for the timestamp and metadata.
/// When powering up the camera the camera clock is reset and all date and time information is set to zero.
/// Therefore this command or \ref PCO_SetDateTime should be called once.
/// It might be necessary to call the function again in distinct time intervals, because some deviation between
/// PC time and camera time might occur after some time.
///
/// \return Error code or PCO_NOERROR on success
///
DWORD PCO_SetCameraToCurrentTime();

///
/// \brief Sets the time and date to the camera.
/// The date and time is updated automatically, as long as the camera is supplied with power.
/// Camera time is used for the timestamp and metadata.
/// When powering up the camera the camera clock is reset and all date and time information is set to zero.
/// Therefore this command or \ref PCO_SetDateTime should be called once.
/// It might be necessary to call the function again in distinct time intervals, because some deviation between
/// PC time and camera time might occur after some time.
/// Note:
/// - [ms] and [\f$\mu\f$s] values of the camera clock are set to zero, when this command is executed
///
/// \param strTime Pointer to a tm structure.
/// \return Error code or PCO_NOERROR on success
///
DWORD PCO_SetDateTime(struct tm* strTime);

/// \cond

/// obsolet, because of misleading declaration, please use PCO_GetCameraSetup(WORD setup_id, DWORD *setup_flag, WORD *length);
DWORD PCO_GetCameraSetup(WORD *setup_id, DWORD *setup_flag, WORD *length);
/// \return Error code or PCO_NOERROR on success
/// \endcond

///
/// \brief Request the current camera setup
/// \anchor PCO_GetCameraSetup
/// 
/// This function is used to query the current operation mode of the camera. Some cameras can work at different
/// operation modes with different descriptor settings.\n
/// pco.edge:\n
/// To get the current shutter mode input index setup_id must be set to 0.\n
/// current shutter mode is returned in setup_flag[0]
///  - 0x00000001 = Rolling Shutter
///  - 0x00000002 = Global Shutter
///  - 0x00000004 = Global Reset 
///
/// \param setup_id Identification code for selected setup type. 
/// \param setup_flag Pointer to a DWORD array to get the current setup flags. If set to NULL in input only 
/// the array length is returned.
/// - On input this variable can be set to NULL, then only array length is filled with correct value. 
/// - On output the array is filled with the available information for the selected setup_id
/// \param length Pointer to a WORD variable
/// - On input to indicate the length of the Setup_flag array in DWORDs.
/// - On output the length of the setup_flag array in DWORDS
/// \return Error code or PCO_NOERROR on success
///
DWORD PCO_GetCameraSetup(WORD setup_id, DWORD *setup_flag, WORD *length);

///
/// \brief Sets the camera setup structure (see camera specific structures)
/// \anchor PCO_SetCameraSetup
/// 
/// pco.edge:\n
/// To get the current shutter mode input index setup_id must be set to 0.\n
/// current shutter mode is returned in setup_flag[0]
///  - 0x00000001 = Rolling Shutter
///  - 0x00000002 = Global Shutter
///  - 0x00000004 = Global Reset 
/// When camera is set to a new shuttermode uit must be reinitialized by calling one of the reboot functions.
/// After rebooting, camera description must be read again see \ref  PCO_GetCameraDescription.
///
/// \param setup_id Identification code for selected setup type. 
/// \param setup_flag Flags to be set for the selected setup type.
/// \param length Number of valid DWORDs in setup_flag array.
/// \return Error code or PCO_NOERROR on success
///
DWORD PCO_SetCameraSetup(WORD setup_id, DWORD *setup_flag,WORD length);

///
/// \brief Reboot the camera
/// 
/// Camera does a reinitialisation. The function will return immediately and the reboot process in the camera is started.
/// When reboot is finished (approximately after 6 to 10 seconds, up to 40 seconds for cameras with GigE interface) one can try to
/// reconnect again with a simple command like \ref PCO_GetCameraType. The reboot command is used during firmware update and is
/// necessary when camera setup is changed.
/// \return Error code or PCO_NOERROR on success
///
DWORD PCO_RebootCamera();


//an interface specific function must be used, because depending on used interface some constraints must be fullfilled for reconnecting
//
// \brief Reboot the camera and wait until camera is reconnected.
// Camera does a reinitialisation. This function tries to reconnect to the camera after waiting the reboot wait time.
// With parameter connect_time the maximum wait time can be set.
//   
// \param connect_time Time in ms while trying to reconnect the camera 
// \return Error code or PCO_NOERROR on success
//
//DWORD PCO_RebootCamera_Wait(DWORD connect_time);

/// @}


/// @name Camera Description
///
/// Functions and comments for Camera description 

/// \par PCO_Description structure
/// Because different sensors (CCD, CMOS, sCMOS) are used in the different camera models, each
/// camera has its own description. This description should be readout shortly after access to the
/// camera is established. In the description general margins for all sensor related settings and
/// bitfields for available options of the camera are given. This set of information can be used to
/// validate the input parameter for commands, which change camera settings, before they are sent
/// to the camera. The dwGeneralCapsDESC1 and dwGeneralCapsDESC2 bitfields in the
/// PCO_Description structure can be used to see what options are supported from the connected
/// camera. Supported options may vary with different camera types and also between different
/// firmware versions of one camera type.

///
/// \brief Gets the cached camera description data structure.
/// This is a cached value that is retrieved once when Open_Cam is called and with every \ref PCO_GetCameraDescription call.
/// See \ref PCO_GetCameraDescription for a more detailed version of the retrieved camera description structure.
/// Because parameters inside the structure are fixed values, this is the recommended function to work with.
///
/// \param description Pointer to a PCO description structure.
/// - on output structure is filled with cached camera settings
/// \return Error code or PCO_NOERROR on success
///
DWORD PCO_GetCameraDescriptor(SC2_Camera_Description_Response *description);


/// \brief Read the camera description data structure from the camera.

/// \param response Pointer to a SC2_Camera_Description_Response structure.
/// - on output structure is filled with the camera settings
/// \return Error code or PCO_NOERROR on success
///
DWORD PCO_GetCameraDescription(SC2_Camera_Description_Response* response);

///
/// \brief Gets the additional camera descriptions
/// See sc2_telegram.h for more information.
///
/// \param response Pointer to a SC2_Camera_Description_2_Response structure.
/// - on output structure is filled with the camera settings
/// \return Error code or PCO_NOERROR on success
///
DWORD PCO_GetCameraDescription(SC2_Camera_Description_2_Response* response);


/// @name General Camera StatusInformation
///
/// This section contains general functions to request general information about the camera and the actual camera state.
///

///
/// \brief Gets the camera type, serial number and interface type.
///
/// \param wCamType Pointer to WORD variable to receive the camera type.
/// \param dwSerialNumber Pointer to DWORD variable to receive the serial number.
/// \param wIfType Pointer to WORD variable to receive connected Interface type
///
/// \return Error message, 0 in case of success else less than 0.
///
DWORD PCO_GetCameraType(WORD* wCamType,DWORD* dwSerialNumber,WORD* wIfType=NULL);





///
/// \brief Gets the name of the camera.
/// 
/// The string buf has to be long enough to get the camera name. Maximum length will be 40 characters including a terminating zero.
///
/// The input pointers will be filled with the following parameters:
/// - Camera name as it is stored inside the camera (e.g. "pco.4000").
///
/// \param buf Pointer to a string to receive the camera name.
/// \param length WORD variable which holds the maximum length of the string.
/// \retval buf Null terminated string with camera name
/// \return Error code
///
DWORD PCO_GetCameraName(void* buf,int length);

///
/// \brief Gets the basic information of the camera.
/// 
/// The string buf has to be long enough to get the information.
///
/// The input pointer will be filled with one of the following parameters:
/// - Camera name as it is stored inside the camera (e.g. "pco.4000").
/// - Sensor name as it is stored inside the camera
///
/// \param typ selector for information
///            - 1: Camera name
///            - 2: Sensor name
/// \param buf Pointer to a string, to receive the requested information
/// \param length int variable which holds the length of buf.
/// \retval buf Null terminated string with requested information
/// \return Error message, 0 in case of success else less than 0
///
DWORD PCO_GetInfo(DWORD typ,void* buf,int length);


///
/// \brief Request the current camera and power supply temperatures.
/// 
/// Power supply temperature is not available with all cameras. If it is not available, the temperature will show 0. In case the sensor
/// temperature is not available it will show 0x8000.
///
/// The input pointers will be filled with the following parameters:
/// - CCD temperature as signed word in \f$^\circ\f$C*10.
/// - Camera temperature as signed word in \f$^\circ\f$C.
/// - Power Supply temperature as signed word in \f$^\circ\f$C.
///
/// \param CCDTemp Pointer to a SHORT variable, to receive the CCD temp. value.
/// \param CAMTemp Pointer to a SHORT variable, to receive the camera temp. value.
/// \param ExtTemp Pointer to a SHORT variable, to receive the power device temp. value.
/// \retval CCDTemp CCD temperature value.
/// \retval CAMTemp Camera temperature value.
/// \retval ExtTemp Extended device temperature value (i.E. Power device). Not supported from all cameras.
/// \return Error code
///
DWORD PCO_GetTemperature(SHORT *CCDTemp,SHORT *CAMTemp,SHORT *ExtTemp);


///
/// \brief Request the current camera health status: warnings, errors.
/// 
/// It is recommended to call this function frequently (e.g. every 5s, or after calling ARM) in order to recognize camera internal problems,
/// like electronics temperature error. This will enable users to prevent the camera hardware from damage.
/// - Warnings are encoded as bits of a long word. Bit set indicates warning, bit cleared indicates that the corresponding parameter is OK.
/// - System errors encoded as bits of a long word. Bit set indicates error, bit cleared indicates that the corresponding status is OK.
/// - System Status encoded as bits of a long word.
///
/// \param warnings Pointer to a DWORD variable, to receive the warning value.
/// \param errors Pointer to a DWORD variable, to receive the error value.
/// \param status Pointer to a DWORD variable, to receive the error value.
/// \retval warnings Actual warning state
/// \retval errors Actual error state
/// \retval status Actual system state
/// \return Error code
///
DWORD PCO_GetHealthStatus(unsigned int *warnings,unsigned int *errors,unsigned int *status);    


///
/// \brief Gets the signal state of the camera sensor. Edge only!
/// 
/// The signals must not be deemed to be a real time response of the sensor, since the command path adds a system dependent delay. Sending a command
/// and getting the camera response lasts about 2ms (+/- 1ms; for 'simple' commands). In case you need a closer synchronization use hardware signals.
/// \param status DWORD pointer to receive the status flags of the sensor (can be NULL).
///  - Bit0: SIGNAL_STATE_BUSY  0x0001
///  - Bit1: SIGNAL_STATE_IDLE  0x0002
///  - Bit2: SIGNAL_STATE_EXP   0x0004
///  - Bit3: SIGNAL_STATE_READ  0x0008
/// \param imagecount DWORD pointer to receive the # of the last finished image(can be NULL).
/// \retval status Actual camera state
/// \retval imagecount number of last finished image inside camera
/// \return Error code
///
DWORD PCO_GetSensorSignalStatus(DWORD *status,DWORD *imagecount);


///
/// \brief Gets the busy state of the camera.
///
/// Get camera busy status: a trigger is ignored if the camera is still busy ([exposure] or [readout]). In
/// case of force trigger command, the user may request the camera busy status in order to be able to
/// start a valid force trigger command. Please do not use this function for image synchronization.
///
/// \b Note: The busy status is according to the hardware signal \<busy\> at the \<status output\> at the
/// rear of pco.power or the camera connectors. Due to response and processing times, e.g., caused by the interface and/or
/// the operating system, the delay between the delivered status and the actual status may be
/// several 10 ms up to 100 ms. If timing is critical, it is strongly recommended that the hardware
/// signal (\<busy\>) be used.
///
/// \param camera_busy Pointer to a WORD variable to receive the busy state.
/// \retval camera_busy Actual camera busy state
///                     - 0x0000 = camera is [not busy], ready for a new trigger command
///                     - 0x0001 = camera is [busy], not ready for a new trigger command
/// \return Error code
///
DWORD PCO_GetCameraBusyStatus(WORD* camera_busy);


///
/// \brief Get the current status of the \<exp trig\> user input (one of the \<control in\> inputs at the rear of pco.power or the camera connectors).
///
/// See camera manual for more information about hardware signals.
/// \param exptrgsignal Pointer to a WORD variable to receive the exposure trigger signal state.
/// \retval exptrgsignal Actual exposure trigger signal state.
///                      - 0x0000 = exposure trigger signal is off
///                      - 0x0001 = exposure trigger signal is on
/// \return Error code.
///
DWORD PCO_GetExpTrigSignalStatus(WORD* exptrgsignal);


///
/// \brief Gets the frametime for one image of the camera.
///
/// Get and split the 'camera operation code' (COC) runtime into two DWORD. One will hold the longer
/// part, in seconds, and the other will hold the shorter part, in nanoseconds. This function can be
/// used to calculate the FPS. The sum of dwTime_s and dwTime_ns covers the delay, exposure and
/// readout time.
///
/// \param time_s Pointer to a DWORD variable to receive the time part in seconds.
/// \param time_ns Pointer to a DWORD variable to receive the time part in nanoseconds.
/// \retval time_s Time part in seconds of the COC.
/// \retval time_ns Time part in nanoseconds of the COC (range: 0ns-999.999.999ns).
/// \return Error code
///
DWORD PCO_GetCOCRuntime(DWORD *time_s,DWORD *time_ns);


///
/// \brief Gets the actual exposuretime for one image of the camera.
///
/// Get and split the actual exposuretime into two DWORD. One will hold the longer
/// part, in seconds, and the other will hold the shorter part, in nanoseconds.
//  The sum of dwTime_s and dwTime_ns covers the delay and exposure time.
///
/// \param time_s Pointer to a DWORD variable to receive the time part in seconds.
/// \param time_ns Pointer to a DWORD variable to receive the time part in nanoseconds.
/// \retval time_s Time part in seconds of the exposuretime.
/// \retval time_ns Time part in nanoseconds of the exposuretime (range: 0ns-999.999.999ns).
/// \return Error code
///
DWORD PCO_GetCOCExptime(DWORD* time_s,DWORD* time_ns);


///
/// \brief Gets the timing of one image, including trigger delay, trigger jitter, etc.
///
/// The input structure will be filled with the following parameters:
/// - FrameTime_ns: Nanoseconds part of the time to expose and readout one image.
/// - FrameTime_s: Seconds part of the time to expose and readout one image.
/// - ExposureTime_ns: Nanoseconds part of the exposure time.
/// - ExposureTime_s: Seconds part of the exposure time.
/// - TriggerSystemDelay_ns: System internal minimum trigger delay, till a trigger is recognized and executed by the system.
/// - TriggerSystemJitter_ns: Maximum possible trigger jitter, which influences the real trigger delay. Real trigger delay=TriggerDelay_ns +/-TriggerSystemJitter
/// - TriggerDelay_ns: Total trigger delay part in ns, till a trigger is recognized and executed by the system.
/// - TriggerDelay_ns: Total trigger delay part in s, till a trigger is recognized and executed by the system.
///
/// \param image_timing Pointer to a SC2_Get_Image_Timing_Response structure
/// \retval image_timing SC2_Get_Image_Timing_Response structure filled with camera settings
/// \return Error code
///
DWORD PCO_GetImageTiming (SC2_Get_Image_Timing_Response *image_timing);




/// @name Timing Control and Status
///
/// This section contains functions to change and retrieve actual timing settings
/// Setting of delay and exposure times can be done also when recording state is RUN.
/// When recording state is STOP or for any other settings an additional \ref PCO_ArmCamera has to be done.

///
/// \brief Get image trigger mode (for further explanation see camera manual).
/// 
/// Trigger modes:
/// - 0x0000 = [auto trigger]
///
///   An exposure of a new image is started automatically best possible compared to the
///   readout of an image. If a CCD is used, and images are taken in a sequence, then exposures
///   and sensor readout are started simultaneously. Signals at the trigger input (\<exp trig\>) are irrelevant.
/// - 0x0001 = [software trigger]:
///
///   An exposure can only be started by a force trigger command.
/// - 0x0002 = [extern exposure & software trigger]:
///
///   A delay / exposure sequence is started at the RISING or FALLING edge (depending on
///   the DIP switch setting) of the trigger input (\<exp trig\>).
///
/// - 0x0003 = [extern exposure control]:
///   The exposure time is defined by the pulse length at the trigger input(\<exp trig\>). The
///   delay and exposure time values defined by the set/request delay and exposure command
///   are ineffective. (Exposure time length control is also possible for double image mode; the
///   exposure time of the second image is given by the readout time of the first image.)
///
/// \b Note: In the [extern exposure & software trigger] and [extern exposure control] modes, it also
/// depends on the selected acquire mode, if a trigger edge at the trigger input (\<exp trig\>)
/// will be effective or not (see also PCO_GetAcquireMode() (Auto / External)). A software
/// trigger however will always be effective independent of the state of the \<acq enbl\> input
/// (concerned trigger modes are: [software trigger] and [extern exposure & software trigger].
///
/// \param mode Pointer to a WORD variable to receive the triggermode.
/// \retval mode Actual trigger mode
/// \return Error code
///
DWORD PCO_GetTriggerMode(WORD *mode);


///
/// \brief Sets the trigger mode of the camera.
/// 
/// See PCO_GetTriggerMode for a detailed explanation.
///
/// The command will be rejected, if Recording State is [run].
/// \param mode WORD variable to hold the triggermode.
/// \return Error code
///
DWORD PCO_SetTriggerMode(WORD mode);


///
/// \brief Forces a software trigger to the camera.
/// 
/// This software command starts an exposure if the trigger mode is in the [software trigger]
/// (0x0001) state or in the [extern exposure & software trigger] (0x0002) state. If the trigger mode is
/// in the [extern exposure control] (0x0003) state, nothing happens. A ForceTrigger should not be used to generate a distinct timing.
/// To accept a force trigger command the camera must be recording and ready: (recording = [start])
/// and [not busy]. If a trigger fails it will not trigger future exposures.
///
/// Result:
///
/// \param trigger Pointer to a WORD variable to receive whether a trigger occurred or not.
/// \retval trigger trigger occurence state
///                 - 0x0000 = trigger command was unsuccessful because the camera is busy
///                 - 0x0001 = a new image exposure has been triggered by the command
///
/// \return Error code
///
DWORD PCO_ForceTrigger(WORD *trigger);


///
/// \brief Gets the pixelrate for reading images from the image sensor.
/// 
/// \param pixelrate Pointer to a DWORD variable to receive the pixelrate.
/// \retval pixelrate Actual pixelrate of image sensor
/// \return Error code
///
DWORD PCO_GetPixelRate(DWORD *pixelrate);

///
/// \brief Sets the pixelrate of the camera.
/// 
/// For the pco.edge the higher pixelrate needs also execution of PCO_SetTransferParameter() and PCO_SetLut() with appropriate parameters.
///
/// \param PixelRate DWORD variable to hold the pixelrate.
/// \return Error code
///
DWORD PCO_SetPixelRate(DWORD PixelRate);

///
/// \brief Gets the exposure and delay time and the time bases of the camera.
/// 
/// Timebase:
/// - 0 -> value is in ns: exp. time of 100 means 0.0000001s.
/// - 1 -> value is in \f$\mu\f$s: exp. time of 100 means 0.0001s.
/// - 2 -> value is in ms: exp. time of 100 means 0.1s.
///
/// Note:
/// - delay and exposure values are multiplied with the configured timebase unit values
/// - the range of possible values can be checked with the values defined in the camera description.
///
/// \param delay Pointer to a DWORD variable to receive the delay time.
/// \param expos Pointer to a DWORD variable to receive the exposure time.
/// \param tb_delay Pointer to a WORD variable to receive the delay timebase.
/// \param tb_expos Pointer to a WORD variable to receive the exposure timebase.
/// \return Error message, 0 in case of success else less than 0
///
DWORD PCO_GetDelayExposureTime(DWORD* delay,DWORD* expos,WORD* tb_delay,WORD* tb_expos);

///
/// \brief Sets the exposure and delay time and the time bases of the camera.
/// 
/// If the recording state is on, it is possible to change the timing without calling PCO_ArmCamera().
///
/// Timebase:
/// - 0 -> value is in ns: exp. time of 100 means 0.0000001s.
/// - 1 -> value is in \f$\mu\f$s: exp. time of 100 means 0.0001s.
/// - 2 -> value is in ms: exp. time of 100 means 0.1s.
///
/// Note: - delay and exposure values are multiplied with the configured timebase unit values
///       - the range of possible values can be checked with the values defined in the camera description.
///       - can be used to alter the timing in case the recording state is on. In this case it is not necessary to call PCO_ArmCamera().
///       - If the recording state is off calling PCO_ArmCamera() is mandatory.
///
/// \param delay DWORD variable to hold the delay time.
/// \param expos DWORD variable to hold the exposure time.
/// \param tb_delay WORD variable to hold the delay timebase.
/// \param tb_expos WORD variable to hold the exp. timebase.
/// \return Error code
///
DWORD PCO_SetDelayExposureTime(DWORD delay,DWORD expos,WORD tb_delay,WORD tb_expos);


///
/// \brief Gets the exposure and delay time table of the camera.
/// See PCO_SetDelayExposureTime() for a detailed description.
/// 
/// \param delay Pointer to a DWORD array to receive the delay time.
/// \param expos Pointer to a DWORD array to receive the exposure time.
/// \retval delay Actual delay time of camera
/// \retval expos Actual exposure time of camera
/// \return Error message, 0 in case of success else less than 0
///
DWORD PCO_GetDelayExposure(DWORD *delay,DWORD *expos);


///
/// \brief  Sets the exposure and delay time of the camera without changing the timebase.
///
/// If the recording state is on, it is possible to change the timing without calling PCO_ArmCamera.
///
/// See PCO_SetDelayExposureTime() for a detailed description.
/// \param delay DWORD variable to hold the delay time.
/// \param expos DWORD variable to hold the exposure time.
/// \return Error message, 0 in case of success else less than 0
///
DWORD PCO_SetDelayExposure(DWORD delay,DWORD expos);


///
/// \brief Gets the exposure and delay time bases of the camera.
/// See PCO_SetDelayExposureTime() for a detailed description
/// \param delay Pointer to WORD variable to receive the delay timebase.
/// \param expos Pointer to WORD variable to receive the exposure timebase.
/// \return Error message, 0 in case of success else less than 0
///
DWORD PCO_GetTimebase(WORD *delay,WORD *expos);


///
/// \brief Sets the exposure and delay time bases of the camera.
/// See PCO_SetDelayExposureTime() for a detailed description.
/// \param delay WORD variable to hold the delay time base.
/// \param expos WORD variable to hold the exposure time base.
/// \return Error message, 0 in case of success else less than 0
///
DWORD PCO_SetTimebase(WORD delay,WORD expos);



///
/// \brief Get frame rate / exposure time.
///
/// \b Note:
/// - Frame rate and exposure time are also affected by the "Set Delay/Exposure Time" command.
/// It is strongly recommend to use either the "Set Framerate" or the "Set Delay/Exposure
/// Time" command! The last issued command will determine the timing before calling the ARM command.
/// - Function is not supported by all cameras, at that moment only by the pco.dimax!
///
/// \param wFrameRateStatus Pointer to WORD variable to receive the status
/// - 0x0000: Settings consistent, all conditions met
/// - 0x0001: Framerate trimmed, framerate was limited by readout time
/// - 0x0002: Framerate trimmed, framerate was limited by exposure time
/// - 0x0004: Exposure time trimmed, exposure time cut to frame time
/// - 0x8000: The return values dwFrameRate and dwFrameRateExposure are not yet validated. The values returned are the values which were passed with the most recent call of PCO_SetFramerate() function.
/// \param dwFrameRate DWORD variable to receive the actual frame rate in mHz
/// \param dwFrameRateExposure DWORD variable to receive the actual exposure time (in ns)
/// \return Error message, 0 in case of success else less than 0.
///
DWORD PCO_GetFrameRate (WORD* wFrameRateStatus, DWORD* dwFrameRate, DWORD* dwFrameRateExposure);

///
/// \brief Sets the frame rate mode, rate and exposure time
///
/// Set frame rate and exposure time. This command is intended to set directly the frame rate and the
/// exposure time of the camera. The frame rate is limited by the readout time and the exposure time:
/// \f[
/// Framerate \leq \frac{1}{t_{readout}}
/// \f]
/// \f[
/// Framerate \leq \frac{1}{t_{expos}}
/// \f]
/// Please note that there are some overhead times, therefore the real values can differ slightly, e.g.
/// the maximum frame rate will be a little bit less than 1 / exposure time. The mode parameter of the
/// function call defines how the function works if these conditions are not met.
/// The function differs, if the camera is recording (recording state = 1) or if recording is off:
///
/// Camera is recording:
/// The frame rate / exposure time is changed immediately. The function returns the actually configured frame rate and exposure time.
///
/// Record is off:
/// The frame rate / exposure time is stored. The function does not change the input values for frame
/// rate and exposure time. A succeeding "Arm Camera" command (PCO_ArmCamera()) validates the
/// input parameters together with other settings, e.g. The status returned indicates, if the input
/// arameters are validated. The following procedure is recommended:
/// - Set frame rate and exposure time using the PCO_SetFrameRate() function.
/// - Do other settings, before or after the PCO_SetFrameRate() function.
/// - Call the PCO_ArmCamera() function in order to validate the settings.
/// - Retrieve the actually set frame rate and exposure time using PCO_GetFrameRate.
///
/// \param wFrameRateStatus Pointer to WORD variable to receive the status
/// - 0x0000: Settings consistent, all conditions met
/// - 0x0001: Framerate trimmed, framerate was limited by readout time
/// - 0x0002: Framerate trimmed, framerate was limited by exposure time
/// - 0x0004: Exposure time trimmed, exposure time cut to frame time
/// \param wFramerateMode Pointer to WORD variable to set the frame rate mode
/// - 0x0000: auto mode (camera decides which parameter will be trimmed)
/// - 0x0001: Framerate has priority, (exposure time will be trimmed)
/// - 0x0002: Exposure time has priority, (framerate will be trimmed)
/// - 0x0003: Strict, function shall return with error if values are not possible.
/// \param dwFrameRate DWORD variable to receive the actual frame rate in mHz (milli!)
/// \param dwFrameRateExposure DWORD variable to receive the actual exposure time (in ns)
/// \return Error message, 0 in case of success else less than 0.
///
DWORD PCO_SetFrameRate (WORD* wFrameRateStatus, WORD wFramerateMode, DWORD* dwFrameRate, DWORD* dwFrameRateExposure);

///
/// \brief The FPS Exposure Mode is available for the pco.1200hs camera model only!
///
/// The FPS exposure mode is useful if the user wants to get the maximum exposure time for the maximum frame rate.
/// The maximum image frame rate (FPS = Frames Per Second) depends on the pixelrate, the vertical ROI and the exposure time.
/// \param wFPSExposureMode Pointer to a WORD variable to receive the FPS-exposure-mode.
///   - 0: FPS Exposure Mode off, exposure time set by PCO_SetDelay/Exposure Time command
///   - 1: FPS Exposure Mode on, exposure time set automatically to 1 / FPS max. PCO_SetDelay/Exposure Time commands are ignored.
/// \param dwFPSExposureTime Pointer to a DWORD variable to receive the FPS exposure time in nanoseconds.
/// \return Error message, 0 in case of success else less than 0.
///
DWORD PCO_GetFPSExposureMode(WORD* wFPSExposureMode,DWORD* dwFPSExposureTime);


///
/// \brief The FPS Exposure Mode is available for the pco.1200hs camera model only!
///
/// The FPS exposure mode is useful if the user wants to get the maximum exposure time for the maximum frame rate.
/// The maximum image frame rate (FPS = Frames Per Second) depends on the pixelrate, the vertical ROI and the exposure time.
/// \param wFPSExposureMode WORD variable to hold the FPS-exposure-mode.
/// - 0: FPS Exposure Mode off, exposure time set by PCO_SetDelay/Exposure Time command
/// - 1: FPS Exposure Mode on, exposure time set automatically to 1 / FPS max. PCO_SetDelay/Exposure Time commands are ignored.
/// \param dwFPSExposureTime Pointer to a DWORD variable to receive the FPS exposure time in nanoseconds.
/// \return Error message, 0 in case of success else less than 0.
///
DWORD PCO_SetFPSExposureMode(WORD wFPSExposureMode,DWORD* dwFPSExposureTime);


/// @name Sensor Control and Status
///
/// This section contains functions to change and retrieve actual format settings

///
/// \brief Gets the sensor format.
/// 
/// The [standard] format uses only effective pixels, while the [extended] format shows all pixels inclusive effective, dark, reference and dummy.
///
/// - 0x0000 = [standard]
/// - 0x0001 = [extended]
///
/// \param wSensor Pointer to a WORD variable to receive the sensor format.
/// \return Error message, 0 in case of success else less than 0
///
DWORD PCO_GetSensorFormat(WORD *wSensor);


///
/// \brief Sets the sensor format.
/// 
/// The [standard] format uses only effective pixels, while the [extended] format shows all pixels inclusive effective, dark, reference and dummy.
///
/// - 0x0000 = [standard]
/// - 0x0001 = [extended]
///
/// \param wSensor WORD variable which holds the sensor format.
/// \return Error message, 0 in case of success else less than 0
///
DWORD PCO_SetSensorFormat(WORD wSensor);


///
/// \brief Gets the region of interest of the camera.
/// 
/// Get ROI (region or area of interest) window. The ROI is equal to or smaller than the absolute
/// image area, which is defined by the settings of format and binning.
///
/// Some sensors have a ROI stepping. See the camera description and check the parameters
/// wRoiHorStepsDESC and/or wRoiVertStepsDESC. In case stepping is zero ROI setting other than x0=1, x1=max/bin, y0=1, y1=max/bin it not possible.
///
/// For dual ADC mode the horizontal ROI must be symmetrical. For a pco.dimax the horizontal and
/// vertical ROI must be symmetrical. For a pco.edge the vertical ROI must be symmetrical.
///
/// X0, Y0 start at 1. X1, Y1 end with max. sensor size.
///
/// \latexonly \begin{tabular}{| l c r | } \hline x0,y0 & & \\ & ROI & \\ & & x1,y1 \\ \hline \end{tabular} \endlatexonly
///
/// \param RoiX0
/// Pointer to a WORD variable to receive the x value for the upper left corner.
/// \param RoiY0
/// Pointer to a WORD variable to receive the y value for the upper left corner.
/// \param RoiX1
/// Pointer to a WORD variable to receive the x value for the lower right corner.
/// \param RoiY1
/// Pointer to a WORD variable to receive the y value for the lower right corner.
/// \return Error message, 0 in case of success else less than 0
///
DWORD PCO_GetROI(WORD *RoiX0,WORD *RoiY0,WORD *RoiX1,WORD *RoiY1);


///
/// \brief Sets the region of interest of the camera.
///
/// Set ROI (region or area of interest) window. The ROI must be equal to or smaller than the
/// absolute image area, which is defined by the settings of format and binning. If the binning
/// settings are changed, the user must adapt the ROI, before PCO_ArmCamera() is accessed. The
/// binning setting sets the limits for the ROI. For example, a sensor with 1600x1200 and binning 2x2
/// will result in a maximum ROI of 800x600.
///
/// Some sensors have a ROI stepping. See the camera description and check the parameters
/// wRoiHorStepsDESC and/or wRoiVertStepsDESC. In case stepping is zero ROI setting other than
/// x0=1, x1=max/bin, y0=1, y1=max/bin it not possible (max depends on the selected sensor format;
/// bin depends on the current binning settings).
///
/// For dual ADC mode the horizontal ROI must be symmetrical. For a pco.dimax the horizontal and
/// vertical ROI must be symmetrical. For a pco.edge the vertical ROI must be symmetrical.
///
/// X0, Y0 start at 1. X1, Y1 end with max. sensor size.
///
/// \latexonly \begin{tabular}{| l c r | } \hline x0,y0 & & \\ & ROI & \\ & & x1,y1 \\ \hline \end{tabular} \endlatexonly
///
/// \param RoiX0
/// WORD variable to hold the x value for the upper left corner.
/// \param RoiY0
/// WORD variable to hold the y value for the upper left corner.
/// \param RoiX1
/// WORD variable to hold the x value for the lower right corner.
/// \param RoiY1
/// WORD variable to hold the y value for the lower right corner.
/// \return Error message, 0 in case of success else less than 0
///
DWORD PCO_SetROI(WORD RoiX0,WORD RoiY0,WORD RoiX1,WORD RoiY1);


///
/// \brief Gets the binning values of the camera.
/// 
/// \param BinHorz Pointer to a WORD variable to hold the horizontal binning value.
/// \param BinVert Pointer to a WORD variable to hold the vertikal binning value.
/// \return Error message, 0 in case of success else less than 0
///
DWORD PCO_GetBinning(WORD *BinHorz,WORD *BinVert);


///
/// \brief Sets the binning values of the camera.
/// 
/// Set binning. If the binning settings are changed, the user must adapt the ROI, before
/// PCO_ArmCamera() is accessed. The binning setting sets the limits for the ROI. E.g. a sensor with
/// 1600x1200 and binning 2x2 will result in a maximum ROI of 800x600.
/// \param BinHorz WORD variable to hold the horizontal binning value.
/// \param BinVert WORD variable to hold the vertikal binning value.
/// \return Error message, 0 in case of success else less than 0
///
DWORD PCO_SetBinning(WORD BinHorz,WORD BinVert);



///
/// \brief Get analog-digital-converter (ADC) operation for reading the image sensor data.
///
/// Pixel data can be read out using one ADC (better linearity), or in parallel using two ADCs (faster). This option is
/// only available for some camera models (defined in the camera description). If the user sets 2ADCs he must center and
/// adapt the ROI to symmetrical values, e.g. pco.1600: x1,y1,x2,y2=701,1,900,500 (100,1,200,500 is not possible).
/// \param wADCOperation Pointer to a WORD variable to receive the adc operation mode.
/// \return Error message, 0 in case of success else less than 0.
///
DWORD PCO_GetADCOperation(WORD* wADCOperation);


///
/// \brief Sets the adc operation mode of the camera, if available.
/// 
/// Set analog-digital-converter (ADC) operation for reading the image sensor data. Pixel data can be
/// read out using one ADC (better linearity) or in parallel using two ADCs (faster). This option is
/// only available for some camera models. If the user sets 2ADCs he must center and adapt the ROI
/// to symmetrical values, e.g. pco.1600: x1,y1,x2,y2=701,1,900,500 (100,1,200,500 is not possible).
///
/// The input data has to be filled with the following parameter:
/// - operation to be set:
///   - 0x0001 = 1 ADC or
///   - 0x0002 = 2 ADCs should be used...
/// - the existence of the number of ADCs can be checked with the values defined in the camera description
///
/// \param num
/// WORD variable to hold the adc operation mode.
/// \return Error message, 0 in case of success else less than 0
///
DWORD PCO_SetADCOperation(WORD num);

///
/// \brief Gets the double image mode of the camera.
///
/// Not applicable to all cameras.
/// \param wDoubleImage Pointer to a WORD variable to receive the double image mode.
/// - 0x0001 = double image mode ON
/// - 0x0000 = double image mode OFF
/// \return Error message, 0 in case of success else less than 0.
///
DWORD PCO_GetDoubleImageMode(WORD* wDoubleImage);


///
/// \brief Sets the double image mode of the camera.
///
/// Some cameras (defined in the camera description) allow the user to make
/// a double image with two exposures separated by a short interleaving time. A double image is
/// transferred as one frame, that is the two images resulting from the two/double exposures are
/// stitched together as one and are counted as one. Thus the buffer size has to be doubled. The first
/// half of the buffer will be filled with image 'A', the first exposed frame. The second exposure
/// (image 'B') will be transferred to the second half of the buffer.
///
/// Not applicable to all cameras.
/// \param wDoubleImage WORD variable to hold the double image mode.
/// - 0x0001 = double image mode ON
/// - 0x0000 = double image mode OFF
/// \return Error message, 0 in case of success else less than 0.
///
DWORD PCO_SetDoubleImageMode(WORD wDoubleImage);


///
/// \brief Get the actual noise filter mode. See the camera descriptor for availability of this feature.
///
/// Parameter:
/// - 0x0000 = [OFF]
/// - 0x0001 = [ON]
/// - 0x0101 = [ON + Hot Pixel correction]
///
/// \param wNoiseFilterMode Pointer to a WORD variable to receive the noise filter mode.
/// \return Error message, 0 in case of success else less than 0.
///
DWORD PCO_GetNoiseFilterMode (WORD* wNoiseFilterMode);


///
/// \brief Sets the actual noise filter mode. See the camera descriptor for availability of this feature.
///
/// Parameter:
/// - 0x0000 = [OFF]
/// - 0x0001 = [ON]
/// - 0x0101 = [ON + Hot Pixel correction]
///
/// \param wNoiseFilterMode WORD variable to hold the noise filter mode.
/// \return Error message, 0 in case of success else less than 0.
///
DWORD PCO_SetNoiseFilterMode (WORD wNoiseFilterMode);


///
/// \brief Get image sensor gain setting
///
/// Current conversion factor in electrons/count (the variable must be divided by 100 to get the real value)
///
/// i.e. 0x01B3 (hex) = 435 (decimal) = 4.35 electrons/count conversion factor must be valid as defined in the camera description
/// \param wConvFact Pointer to a WORD variable to receive the conversion factor.
/// \return Error message, 0 in case of success else less than 0.
///
DWORD PCO_GetConversionFactor(WORD* wConvFact);


///
/// \brief Set image sensor gain
///
/// Conversion factor to be set in electrons/count (the variable must be divided by 100 to get the real value)
///
/// i.e. 0x01B3 (hex) = 435 (decimal) = 4.35 electrons/count
///
/// Conversion factor must be valid as defined in the camera description.
/// \param wConvFact WORD to set the conversion factor.
/// \return Error message, 0 in case of success else less than 0.
///
DWORD PCO_SetConversionFactor(WORD wConvFact);


///
/// \brief Gets the IR sensitivity mode of the camera.
///
/// This option is only available for special camera models with image sensors that have improved IR sensitivity.
/// \param wIR Pointer to a WORD variable to receive the IR sensitivity mode.
/// - 0x0000 IR sensitivity OFF
/// - 0x0001 IR sensitivity ON
/// \return Error message, 0 in case of success else less than 0.
///
DWORD PCO_GetIRSensitivity(WORD* wIR);


///
/// \brief Gets the IR sensitivity mode of the camera.
///
/// Set IR sensitivity for the image sensor. This option is only available for special camera models with image sensors that have improved IR sensitivity.
/// \param wIR WORD variable to set the IR sensitivity mode.
/// - 0x0000 IR sensitivity OFF
/// - 0x0001 IR sensitivity ON
/// \return Error message, 0 in case of success else less than 0.
///
DWORD PCO_SetIRSensitivity(WORD wIR);

/// \cond

///
/// \brief Get the mode for the offset regulation with reference pixels (see camera manual for further explanations).
///
/// Mode:
/// - 0x0000 = [auto]
/// - 0x0001 = [OFF]
///
/// \param wOffsetRegulation Pointer to a WORD variable to receive the offset mode.
/// \return Error message, 0 in case of success else less than 0.
///
DWORD PCO_GetOffsetMode (WORD* wOffsetRegulation);

///
/// \brief Set the mode for the offset regulation with reference pixels (see the camera manual for further explanations).
///
/// Mode:
/// - 0x0000 = [auto]
/// - 0x0001 = [OFF]
///
/// \param wOffsetRegulation WORD variable to hold the offset mode.
/// \return Error message, 0 in case of success else less than 0.
///
DWORD PCO_SetOffsetMode (WORD wOffsetRegulation);

/// \endcond  

///
/// \brief Get the temperature set point for cooling the image sensor (only available for cooled cameras).
///
/// If min. cooling set point (in \f$^\circ\f$C) and max. cooling set point (in \f$^\circ\f$C) are zero, then cooling is not available.
/// \param sCoolSet Pointer to a SHORT variable to receive the cooling setpoint temperature.
/// \return Error message, 0 in case of success else less than 0.
///
DWORD PCO_GetCoolingSetpointTemperature(SHORT* sCoolSet);

///
/// \brief Set the temperature set point for cooling the image sensor (only available for cooled cameras).
///
/// If min. cooling set point (in \f$^\circ\f$C) and max. cooling set point (in \f$^\circ\f$C) are zero, then cooling is not available.
/// \param sCoolSet SHORT variable to hold the cooling setpoint temperature in \f$^\circ\f$C units.
/// \return Error message, 0 in case of success else less than 0.
///
DWORD PCO_SetCoolingSetpointTemperature(SHORT sCoolSet);

///
/// \brief Gets the cooling setpoints of the camera.
///
/// This is used when there is no min max range available.
///
/// \param wBlockID Number of the block to query (currently 0)
/// \param sSetPoints Pointer to a SHORT array to receive the possible cooling setpoint temperatures.
/// \param wValidSetPoints WORD Pointer to set the max number of setpoints to query and to get the valid number of set points inside the camera. In case more than COOLING_SETPOINTS_BLOCKSIZE set points are valid they can be queried by incrementing the wBlockID till wNumSetPoints is reached.
/// \return Error message, 0 in case of success else less than 0.
///
DWORD PCO_GetCoolingSetpoints(WORD wBlockID,SHORT* sSetPoints,WORD* wValidSetPoints);


/// @name Recording Control and Status
///
/// This section contains functions to change and retrieve actual recorder settings

///
/// \brief Get storage mode [recorder] or [FIFO buffer].
///
/// \f[
/// \begin{tabular}{| p{7cm} | p{7cm} |}
/// \hline
/// \textbf{Recorder Mode} & \textbf{FIFO Buffer mode} \\ \hline
/// \begin{itemize}
/// \item images are recorded and stored within the internal camera memory camRAM
/// \item Live View transfers the most recent image to the PC (for viewing/monitoring)
/// \item indexed or total image readout after the recording has been stopped
/// \end{itemize}
/// &
/// \begin{itemize}
/// \item all images taken are transferred to the PC in chronological order
/// \item camera memory (camRAM) is used as a huge FIFO buffer to bypass short data transmission bottlenecks
/// \item if buffer overflows, the oldest images are overwritten
/// \end{itemize} \\ \hline
/// \end{tabular}
/// \f]
/// \param wStorageMode Pointer to a WORD variable to receive the storage mode.
/// - 0: Recorder
/// - 1: FIFO
/// \return Error message, 0 in case of success else less than 0.
///
DWORD PCO_GetStorageMode(WORD* wStorageMode);

///
/// \brief Set storage mode [recorder] or [FIFO buffer].
///
/// \f[
/// \begin{tabular}{| p{7cm} | p{7cm} |}
/// \hline
/// \textbf{Recorder Mode} & \textbf{FIFO Buffer mode} \\ \hline
/// \begin{itemize}
/// \item images are recorded and stored within the internal camera memory camRAM
/// \item Live View transfers the most recent image to the PC (for viewing/monitoring)
/// \item indexed or total image readout after the recording has been stopped
/// \end{itemize}
/// &
/// \begin{itemize}
/// \item all images taken are transferred to the PC in chronological order
/// \item camera memory (camRAM) is used as a huge FIFO buffer to bypass short data transmission bottlenecks
/// \item if buffer overflows, the oldest images are overwritten
/// \item if set recorder = [stop] is sent, recording is stopped and the transfer of the current image to the PC is finished. Images not read are stored within the segment and can be read with the ReadImageFromSegment command.
/// \end{itemize} \\ \hline
/// \end{tabular}
/// \f]
/// \param wStorageMode WORD variable to hold the storage mode.
/// - 0: Recorder
/// - 1: FIFO
/// \return Error message, 0 in case of success else less than 0.
///
DWORD PCO_SetStorageMode(WORD wStorageMode);

///
/// \brief Get recorder sub mode: [sequence] or [ring buffer] (see explanation boxes below).
///
/// Recorder submode is only available if the storage mode is set to [recorder].
/// \f[
/// \begin{tabular}{| p{7cm} | p{7cm} |}
/// \hline
/// \textbf{recorder sub mode = [sequence]} & \textbf{recorder sub mode = [ring buffer]} \\ \hline
/// \begin{itemize}
/// \item recording is stopped when the allocated buffer is full
/// \end{itemize}
/// &
/// \begin{itemize}
/// \item camera records continuously into ring buffer
/// \item if the allocated buffer overflows, the oldest images are overwritten
/// \item recording is stopped by software or disabling acquire signal (<acq enbl>)
/// \end{itemize} \\ \hline
/// \end{tabular}
/// \f]
/// \param wRecSubmode Pointer to a WORD variable to receive the recorder sub mode.
/// - 0: Sequence
/// - 1: Ring buffer
/// \return Error message, 0 in case of success else less than 0.
///
DWORD PCO_GetRecorderSubmode(WORD* wRecSubmode);



///
/// \brief Set recorder sub mode: [sequence] or [ring buffer] (see explanation boxes below).
///
/// Recorder sub mode is only available if the storage mode is set to [recorder].
/// \f[
/// \begin{tabular}{| p{7cm} | p{7cm} |}
/// \hline
/// \textbf{recorder sub mode = [sequence]} & \textbf{recorder sub mode = [ring buffer]} \\ \hline
/// \begin{itemize}
/// \item recording is stopped when the allocated buffer is full
/// \end{itemize}
/// &
/// \begin{itemize}
/// \item camera records continuously into ring buffer
/// \item if the allocated buffer overflows, the oldest images are overwritten
/// \item recording is stopped by software or disabling acquire signal (<acq enbl>)
/// \end{itemize} \\ \hline
/// \end{tabular}
/// \f]
/// \param wRecSubmode WORD variable to hold the recorder sub mode.
/// - 0: Sequence
/// - 1: Ring buffer
/// \return Error message, 0 in case of success else less than 0.
///
DWORD PCO_SetRecorderSubmode(WORD wRecSubmode);

///
/// \brief Get acquire mode: [auto] or [external] (see camera manual for explanation)
///
/// Acquire modes:
/// - 0x0000 = [auto] - all images taken are stored
/// - 0x0001 = [external] - the external control input \<acq enbl\> is a static enable signal of
/// images. If this input is TRUE (level depending on the DIP switch), exposure triggers are
/// accepted and images are taken. If this signal is set FALSE, all exposure triggers are
/// ignored and the sensor readout is stopped.
/// - 0x0002 = [external] - the external control input \<acq enbl\> is a dynamic frame start
/// signal. If this input has got a rising edge TRUE (level depending on the DIP switch), a
/// frame will be started with modulation mode. This is only available with modulation mode
/// enabled (see camera description).
///
/// \param wAcquMode Pointer to a WORD variable to receive the acquire mode.
/// \return Error message, 0 in case of success else less than 0.
///
DWORD PCO_GetAcquireMode(WORD* wAcquMode);



///
/// \brief Set acquire mode: [auto] or [external] (see camera manual for explanation).
///
/// Acquire modes:
/// - 0x0000 = [auto] - all images taken are stored
/// - 0x0001 = [external] - the external control input \<acq enbl\> is a static enable signal of
/// images. If this input is TRUE (level depending on the DIP switch), exposure triggers are
/// accepted and images are taken. If this signal is set FALSE, all exposure triggers are
/// ignored and the sensor readout is stopped.
/// - 0x0002 = [external] - the external control input \<acq enbl\> is a dynamic frame start
/// signal. If this input has got a rising edge TRUE (level depending on the DIP switch), a
/// frame will be started with modulation mode. This is only available with modulation mode
/// enabled (see camera description).
///
/// \param wAcquMode WORD variable to hold the acquire mode.
/// \return Error message, 0 in case of success else less than 0.
///
DWORD PCO_SetAcquireMode(WORD wAcquMode);


///
/// \brief Set acquire mode: [auto] or [external] (see camera manual for explanation).
///
/// Acquire modes:
/// - 0x0000 = [auto] - all images taken are stored
/// - 0x0001 = [external] - the external control input \<acq enbl\> is a static enable signal of
/// images. If this input is TRUE (level depending on the DIP switch), exposure triggers are
/// accepted and images are taken. If this signal is set FALSE, all exposure triggers are
/// ignored and the sensor readout is stopped.
/// - 0x0002 = [external] - the external control input \<acq enbl\> is a dynamic frame start
/// signal. If this input has got a rising edge TRUE (level depending on the DIP switch), a
/// frame will be started with modulation mode. This is only available with modulation mode
/// enabled (see camera description).
///
/// \param wAcquMode Pointer to a WORD variable to receive the acquire mode.
/// \param dwNumberImages Pointer to a DWORD variable to receive the number of images (for mode sequence).
/// \return Error message, 0 in case of success else less than 0.
///
DWORD PCO_GetAcquireModeEx(WORD* wAcquMode, DWORD* dwNumberImages);

///
/// \brief Get acquire mode: [auto] or [external] (see camera manual for explanation)
///
/// Acquire modes:
/// - 0x0000 = [auto] - all images taken are stored
/// - 0x0001 = [external] - the external control input \<acq enbl\> is a static enable signal of
/// images. If this input is TRUE (level depending on the DIP switch), exposure triggers are
/// accepted and images are taken. If this signal is set FALSE, all exposure triggers are
/// ignored and the sensor readout is stopped.
/// - 0x0002 = [external] - the external control input \<acq enbl\> is a dynamic frame start
/// signal. If this input has got a rising edge TRUE (level depending on the DIP switch), a
/// frame will be started with modulation mode. This is only available with modulation mode
/// enabled (see camera description).
///
/// \param wAcquMode WORD variable to hold the acquire mode.
/// \param dwNumberImages DWORD variable to hold the number of images (for mode sequence).
/// \return Error message, 0 in case of success else less than 0.
///
DWORD PCO_SetAcquireModeEx(WORD wAcquMode, DWORD dwNumberImages);


///
/// \brief Get the current status of the \<acq enbl\> user input (one of the \<control in\> inputs at the rear of pco.power or the camera). See camera manual for more information.
///
/// \b Note:
/// Due to response and processing times e.g. caused by the interface and/or the operating
/// system, the delay between the delivered status and the actual status may be several 10 ms up
/// to 100 ms. If timing is critical it is strongly recommended to use other trigger modes.
/// \param wAcquEnableState Pointer to a WORD variable to receive the acquire enable signal status.
/// - 0x0000 = [FALSE]
/// - 0x0001 = [TRUE]
/// \return Error message, 0 in case of success else less than 0.
///
DWORD PCO_GetAcqEnblSignalStatus(WORD* wAcquEnableState);

/// \cond


///
/// \brief This command can be used for setting up the record stop event.
///
/// After a stop event the camera records the configured number of images and stops after that. The command is useful to record a
/// series of images to see what happens before and after the stop event.
///
/// A record stop event can be either a software command or an edge at the \<acq enbl\> input (at the
/// power unit). The edge detection depends on the DIP switch setting at the power unit. If the DIP switch shows \f$\rfloor\lfloor\f$
/// then a rising edge is the stop event. If the DIP switch shows \f$\bigsqcup\f$ then a falling edge is the stop event.
///
/// The software command is the command "Stop Record" described below.
///
/// Use the record stop even function only when Storage Mode = [Recorder] and Recorder Sub mode = [Ring buffer]!
///
/// \b Note:
/// - Use the record stop event function only when Storage Mode = [Recorder] and Recorder Sub mode = [Ring buffer]!
/// - Due to internal timing issues the actual number of images taken after the event may differ by +/- 1 from the configured number.
/// - The command is not available for all cameras. It is currently only available on the pco.1200hs. See the descriptor for availability.
///
/// \param wRecordStopEventMode Pointer to a WORD variable to receive the record stop event mode.
///  - 0x0000 = no record stop event is accepted
///  - 0x0001 = record stop by software command
///  - 0x0002 = record stop by edge at the \<acq enbl\> input or by software
/// \param dwRecordStopDelayImages Pointer to a DWORD variable to receive the number of images which are taken after the record stop event.
/// If the number of images is taken, record will be stopped automatically.
/// \return Error message, 0 in case of success else less than 0.
///
DWORD PCO_GetRecordStopEvent(WORD* wRecordStopEventMode,DWORD* dwRecordStopDelayImages);

///
/// \brief This command can be used for setting up the record stop event.
///
/// After a stop event the camera records the configured number of images and stops after that. The command is useful to record a
/// series of images to see what happens before and after the stop event.
///
/// A record stop event can be either a software command or an edge at the \<acq enbl\> input (at the
/// power unit). The edge detection depends on the DIP switch setting at the power unit. If the DIP switch shows \f$\rfloor\lfloor\f$
/// then a rising edge is the stop event. If the DIP switch shows \f$\bigsqcup\f$ then a falling edge is the stop event.
///
/// The software command is the command PCO_StopRecord().
///
/// Use the record stop even function only when Storage Mode = [Recorder] and Recorder Sub mode = [Ring buffer]!
///
/// \b Note:
/// - Use the record stop event function only when Storage Mode = [Recorder] and Recorder Sub mode = [Ring buffer]!
/// - Due to internal timing issues the actual number of images taken after the event may differ by +/- 1 from the configured number.
/// - The command is not available for all cameras. It is currently only available on the pco.1200hs. See the descriptor for availability.
///
/// \param wRecordStopEventMode WORD variable to hold the record stop event mode.
///  - 0x0000 = no record stop event is accepted
///  - 0x0001 = record stop by software command
///  - 0x0002 = record stop by edge at the \<acq enbl\> input or by software
/// \param dwRecordStopDelayImages DWORD variable to hold the number of images which are taken after the record stop event. If the number of images is taken, record will be stopped automatically.
/// \return Error message, 0 in case of success else less than 0.
///
DWORD PCO_SetRecordStopEvent(WORD wRecordStopEventMode,DWORD dwRecordStopDelayImages);


///
/// \brief This command is useful to generate a stop event by software for the record stop event mode. See also PCO_GetRecordStopEvent() and PCO_SetRecordStopEvent().
///
/// If you want to stop immediately please use PCO_SetRecordingState(0).
///
/// \b Note:
/// - Use the record stop event function only when Storage Mode = [Recorder] and Recorder Sub mode = [Ring buffer]!
/// - Due to internal timing issues the actual number of images taken after the event may differ by +/- 1 from the configured number.
/// - The command is not available for all cameras. It is currently only available on the pco.1200hs. See the descriptor for availability.
///
/// \param wReserved0 Pointer to a WORD variable (Set to zero!).
/// \param dwReserved1 Pointer to a DWORD variable (Set to zero!).
/// \return Error message, 0 in case of success else less than 0.
///
DWORD PCO_StopRecord(WORD* wReserved0, DWORD *dwReserved1);

/// \endcond


/// \cond

DWORD	PCO_GetAcquireControl(WORD* wMode);
DWORD	PCO_SetAcquireControl(WORD wMode);

DWORD PCO_GetEventMonConfiguration(WORD* wConfig);
DWORD PCO_SetEventMonConfiguration(WORD wConfig);
DWORD PCO_GetEventList(WORD wIndex,WORD *wMaxEvents,WORD* wValidEvents,WORD* wValidEventsInTelegram,SC2_EVENT_LIST_ENTRY* list);
/// \endcond

#endif


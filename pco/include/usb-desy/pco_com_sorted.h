//sorted functions


//com.h
//open/close

///////////////////////////////////////////////////////////////////////
//general control and status
//com_func.h
DWORD PCO_GetRecordingState(WORD *recstate);
DWORD PCO_SetRecordingState(WORD recstate);
DWORD PCO_ArmCamera();
DWORD PCO_GetActualSize(DWORD *width,DWORD *height);

DWORD PCO_ResetSettingsToDefault();
DWORD PCO_SetCameraToCurrentTime();
DWORD PCO_SetDateTime(struct tm *st);

DWORD PCO_GetCameraSetup(WORD *Type,DWORD *Setup,WORD *Len);
DWORD PCO_SetCameraSetup(WORD Type, DWORD *Setup,WORD Len);

///////////////////////////////////////////////////////////////////////
//information1
2 DWORD PCO_GetCameraType(WORD* wCamType,DWORD* dwSerialNumber,WORD* wIfType=NULL);

DWORD PCO_GetCameraDescriptor(SC2_Camera_Description_Response *description);
2 DWORD PCO_GetCameraDescription(SC2_Camera_Description_Response* resp);
2 DWORD PCO_GetCameraDescriptionEx(SC2_Camera_Description_Response* descript1,SC2_Camera_Description_2_Response* descript2,WORD wType);

DWORD PCO_GetCameraName(void* buf,int length);
DWORD PCO_GetInfo(DWORD typ,void* buf,int length);

DWORD PCO_GetTemperature(SHORT *sCCDTemp,SHORT *sCAMTemp,SHORT *sExtTemp);
DWORD PCO_GetHealthStatus(unsigned int *dwWarnings,unsigned int *dwErrors,unsigned int *dwStatus);

DWORD PCO_GetSensorSignalStatus(DWORD *status,DWORD *imagecount);
2 DWORD PCO_GetCameraBusyStatus(WORD* wCameraBusyState);
2 DWORD PCO_GetExpTrigSignalStatus(WORD* wExpTrgSignal);

DWORD PCO_GetCOCRuntime(DWORD *s,DWORD *ns);
2 DWORD PCO_GetCOCExptime(DWORD* dwtime_s,DWORD* dwtime_ns);
2 DWORD PCO_GetImageTiming (SC2_Get_Image_Timing_Response *imageresp);


///////////////////////////////////////////////////////////////////////
//timing control status

DWORD PCO_GetTriggerMode(WORD *mode);
DWORD PCO_SetTriggerMode(WORD mode);

DWORD PCO_ForceTrigger(WORD *trigger);

DWORD PCO_GetPixelRate(DWORD *PixelRate);
DWORD PCO_SetPixelRate(DWORD PixelRate);

DWORD PCO_GetDelayExposureTime(DWORD *delay,DWORD *expos,WORD *tb_delay,WORD *tb_expos);
DWORD PCO_SetDelayExposureTime(DWORD delay,DWORD expos,WORD tb_delay,WORD tb_expos);

DWORD PCO_GetDelayExposure(DWORD *delay,DWORD *expos);
DWORD PCO_SetDelayExposure(DWORD delay,DWORD expos);

DWORD PCO_GetTimebase(WORD *delay,WORD *expos);
DWORD PCO_SetTimebase(WORD delay,WORD expos);


2 DWORD PCO_GetFrameRate (WORD* wFrameRateStatus, DWORD* dwFrameRate, DWORD* dwFrameRateExposure);
2 DWORD PCO_SetFrameRate (WORD* wFrameRateStatus, WORD wFramerateMode, DWORD* dwFrameRate, DWORD* dwFrameRateExposure);

2 DWORD PCO_GetFPSExposureMode(WORD* wFPSExposureMode,DWORD* dwFPSExposureTime);
2 DWORD PCO_SetFPSExposureMode(WORD wFPSExposureMode,DWORD* dwFPSExposureTime);

///////////////////////////////////////////////////////////////////////
//sensor control status
//com_func.h

DWORD PCO_GetSensorFormat(WORD *wSensor);
DWORD PCO_SetSensorFormat(WORD wSensor);

2 DWORD PCO_GetDoubleImageMode(WORD* wDoubleImage);
2 DWORD PCO_SetDoubleImageMode(WORD wDoubleImage);

DWORD PCO_GetROI(WORD *RoiX0,WORD *RoiY0,WORD *RoiX1,WORD *RoiY1);
DWORD PCO_SetROI(WORD RoiX0,WORD RoiY0,WORD RoiX1,WORD RoiY1);

DWORD PCO_GetBinning(WORD *BinHorz,WORD *BinVert);
DWORD PCO_SetBinning(WORD BinHorz,WORD BinVert);

2 DWORD PCO_GetADCOperation(WORD* wADCOperation);
DWORD PCO_SetADCOperation(WORD num);

2 DWORD PCO_GetNoiseFilterMode (WORD* wNoiseFilterMode);
2 DWORD PCO_SetNoiseFilterMode (WORD wNoiseFilterMode);

2 DWORD PCO_GetConversionFactor(WORD* wConvFact);
2 DWORD PCO_SetConversionFactor(WORD wConvFact);

2 DWORD PCO_GetIRSensitivity(WORD* wIR);
2 DWORD PCO_SetIRSensitivity(WORD wIR);

2 DWORD PCO_GetOffsetMode (WORD* wOffsetRegulation);
2 DWORD PCO_SetOffsetMode (WORD wOffsetRegulation);

2 DWORD PCO_GetCoolingSetpointTemperature(SHORT* sCoolSet);
2 DWORD PCO_SetCoolingSetpointTemperature(SHORT sCoolSet);
2 DWORD PCO_GetCoolingSetpoints(WORD wBlockID,SHORT* sSetPoints,WORD* wValidSetPoints);


///////////////////////////////////////////////////////////////////////
//recording control

2 DWORD PCO_GetStorageMode(WORD* wStorageMode);
2 DWORD PCO_SetStorageMode(WORD wStorageMode);

2 DWORD PCO_GetRecorderSubmode(WORD* wRecSubmode);
2 DWORD PCO_SetRecorderSubmode(WORD wRecSubmode);

2 DWORD PCO_GetAcquireMode(WORD* wAcquMode);
2 DWORD PCO_SetAcquireMode(WORD wAcquMode);

2 DWORD PCO_GetAcquireModeEx(WORD* wAcquMode, DWORD* dwNumberImages);
2 DWORD PCO_SetAcquireModeEx(WORD wAcquMode, DWORD dwNumberImages);

2 DWORD	PCO_GetAcquireControl(WORD* wMode);
2 DWORD	PCO_SetAcquireControl(WORD wMode);

2 DWORD PCO_GetAcqEnblSignalStatus(WORD* wAcquEnableState);

2 DWORD PCO_GetRecordStopEvent(WORD* wRecordStopEventMode,DWORD* dwRecordStopDelayImages);
2 DWORD PCO_SetRecordStopEvent(WORD wRecordStopEventMode,DWORD dwRecordStopDelayImages);

2 DWORD PCO_StopRecord(WORD* wReserved0, DWORD *dwReserved1);

2 DWORD PCO_GetEventMonConfiguration(WORD* wConfig);
2 DWORD PCO_SetEventMonConfiguration(WORD wConfig);
2 DWORD PCO_GetEventList(WORD wIndex,WORD *wMaxEvents,WORD* wValidEvents,WORD* wValidEventsInTelegram,SC2_EVENT_LIST_ENTRY* list);


///////////////////////////////////////////////////////////////////////
//storage _control status
2 DWORD PCO_GetCameraRamSize(DWORD* dwRamSize, WORD* wPageSize);

2 DWORD PCO_GetCameraRamSegmentSize(DWORD* dwRamSegSize);
2 DWORD PCO_SetCameraRamSegmentSize(DWORD* dwRamSegSize);

2 DWORD PCO_GetActiveRamSegment(WORD* wActSeg);
2 DWORD PCO_SetActiveRamSegment(WORD wActSeg);

2 DWORD PCO_GetNumberOfImagesInSegment(WORD wSegment,DWORD* dwValid,DWORD* dwMax);
2 DWORD PCO_ClearRamSegment();

2 DWORD PCO_GetSegmentImageSettings(WORD wSegment,WORD* wRes_hor,WORD* wRes_ver,WORD* wBin_x,WORD* wBin_y,WORD* wRoi_x0,WORD* wRoi_y0,WORD* wRoi_x1,WORD* wRoi_y1);


///////////////////////////////////////////////////////////////////////
//image transfer 
DWORD PCO_RequestImage();
2 DWORD PCO_RepeatImage();
2 DWORD PCO_ReadImagesFromSegment(WORD wSegment,DWORD dwStartImage,DWORD dwLastImage);

2 DWORD PCO_CancelImageTransfer();
2 DWORD PCO_CancelImage();

2 DWORD PCO_PlayImagesFromSegment(WORD wSegment,WORD wInterface,WORD wMode,WORD wSpeed,DWORD dwRangeLow,DWORD dwRangeHigh,DWORD dwStartPos);
2 DWORD PCO_GetPlayPosition(WORD* wStatus,DWORD *dwPosition);
2 DWORD PCO_SetVideoPayloadIdentifier(WORD wSegment,WORD wMode1,WORD wMode2,DWORD dwSetPos1,DWORD dwClrPos1,DWORD dwSetPos2,DWORD dwClrPos2,DWORD dwSetPos3,DWORD dwClrPos3,DWORD dwSetPos4,DWORD dwClrPos4);

2 DWORD PCO_GetColorSettings(SC2_Get_Color_Settings_Response* ColSetResp);
2 DWORD PCO_SetColorSettings(SC2_Set_Color_Settings* SetColSet);
2 DWORD PCO_DoWhiteBalance(WORD wMode);
2 DWORD PCO_GetWhiteBalanceStatus(WORD* wStatus,WORD* wColorTemp,SHORT* sTint);

2 DWORD PCO_GetImageTransferMode(WORD* wMode,WORD* wImageWidth,WORD* wImageHeight,WORD* wTxWidth,WORD* wTxHeight,WORD* wTxLineWordCnt,WORD* wParam,WORD* wParamLen);
2 DWORD PCO_SetImageTransferMode(WORD wMode,WORD wImageWidth,WORD wImageHeight,WORD wTxWidth,WORD wTxHeight,WORD wTxLineWordCnt,WORD* wParam,WORD wParamLen);

///////////////////////////////////////////////////////////////////////
//image data options

2 DWORD PCO_GetLookupableInfo(WORD wLUTNum, WORD* wNumberOfLuts, char* Description,WORD wDescLen,WORD* wIdentifier, BYTE* bInputWidth,BYTE* bOutputWidth, WORD* wFormat);

DWORD PCO_GetLut(WORD *Identifier,WORD *Parameter);  //?? buffer data
DWORD PCO_SetLut(WORD Identifier,WORD Parameter);    //?? buffer data

DWORD PCO_GetBitAlignment(WORD *align);
DWORD PCO_SetBitAlignment(WORD align);

DWORD PCO_GetTimestampMode(WORD *mode);
DWORD PCO_SetTimestampMode(WORD mode);

2 DWORD PCO_GetHotPixelCorrectionMode (WORD* wHotPixelCorrectionMode);
2 DWORD PCO_SetHotPixelCorrectionMode (WORD wHotPixelCorrectionMode);

2 DWORD PCO_GetMetadataMode(WORD* wMode,WORD* wMetadataSize,WORD* wMetadataVersion);
2 DWORD PCO_SetMetadataMode(WORD wMode,WORD* wMetadataSize,WORD* wMetadataVersion);

///////////////////////////////////////////////////////////////////////
//input output control status

DWORD PCO_GetHWIOSignalCount(WORD *numSignals);
DWORD PCO_GetHWIOSignalDescriptor(WORD SignalNum,SC2_Get_HW_IO_Signal_Descriptor_Response *SignalDesc);
DWORD PCO_GetHWIOSignalDescriptor(WORD SignalNum,char *outbuf,int *size);

DWORD PCO_GetHWIOSignal(WORD SignalNum,WORD *Enabled,WORD *Type,WORD *Polarity,WORD *FilterSetting,WORD *Selected);
DWORD PCO_SetHWIOSignal(WORD SignalNum,WORD Enabled,WORD Type,WORD Polarity,WORD FilterSetting,WORD Selected);

DWORD PCO_GetHWIOSignalTiming(WORD SignalNum,WORD Selection,DWORD *type,DWORD *Parameter);
DWORD PCO_SetHWIOSignalTiming(WORD SignalNum,WORD Selection,DWORD Parameter);


///////////////////////////////////////////////////////////////////////
//enhanced timing control status

2 DWORD PCO_GetPowerDownMode(WORD* wPowerDownMode);
2 DWORD PCO_SetPowerDownMode(WORD wPowerDownMode);

2 DWORD PCO_GetUserPowerDownTime(DWORD* dwPdnTime);
2 DWORD PCO_SetUserPowerDownTime(DWORD dwPdnTime);

2 DWORD PCO_GetDelayExposureTimeTable(DWORD* dwDelay,DWORD* dwExposure,WORD* wTimeBaseDelay,WORD* wTimebaseExposure,WORD wCount);
2 DWORD PCO_SetDelayExposureTimeTable(DWORD* dwDelay,DWORD* dwExposure,WORD wTimeBaseDelay,WORD wTimebaseExposure,WORD wCount);

2 DWORD PCO_GetModulationMode(WORD *wModulationMode, DWORD* dwPeriodicalTime, WORD *wTimebasePeriodical, DWORD *dwNumberOfExposures, LONG *lMonitorOffset);
2 DWORD PCO_SetModulationMode(WORD wModulationMode, DWORD dwPeriodicalTime, WORD wTimebasePeriodical, DWORD dwNumberOfExposures, LONG lMonitorOffset);

2 DWORD PCO_GetCameraSynchMode(WORD* wCameraSynchMode);
2 DWORD PCO_SetCameraSynchMode(WORD wCameraSynchMode);

2 DWORD PCO_GetCMOSLinetiming(WORD* wParameter,WORD* wTimebase,DWORD* dwLineTime);
2 DWORD PCO_SetCMOSLinetiming(WORD wParameter,WORD wTimebase,DWORD dwLineTime);
2 DWORD PCO_GetCMOSLineExposureDelay(DWORD* dwExposureLines,DWORD* dwDelayLines);
2 DWORD PCO_SetCMOSLineExposureDelay(DWORD dwExposureLines,DWORD dwDelayLines);

2 DWORD PCO_GetFastTimingMode(WORD* wFastTimingMode);
2 DWORD PCO_SetFastTimingMode(WORD wFastTimingMode);


///////////////////////////////////////////////////////////////////////
//information2
DWORD PCO_GetFirmwareVersion(char *outbuf,int *size);
DWORD PCO_GetHardwareVersion(char *outbuf,int *size);

DWORD PCO_GetFirmwareVersion(SC2_Firmware_Versions_Response* response);

DWORD PCO_GetFirmwareVersionExt(BYTE* bNum);


///////////////////////////////////////////////////////////////////////
//interface specific hidden PCO:internal only??

2 DWORD PCO_GetIEEE1394InterfaceParams(WORD* wMasterNode,WORD* wIsochChannel,WORD* wIsochPacketLen,WORD* wIsochPacketNum);
2 DWORD PCO_SetIEEE1394InterfaceParams(WORD wMasterNode,WORD wIsochChannel,WORD wIsochPacketLen,WORD wIsochPacketNum);

2 DWORD PCO_GetIEEE1394ISOByteorder(WORD* wMode);
2 DWORD PCO_SetIEEE1394ISOByteorder(WORD wMode);

///////////////////////////////////////////////////////////////////////
//interface specific HD/SDI hidden PCO:internal only??

2 DWORD PCO_GetInterfaceOutputFormat(WORD wInterface,WORD* wFormat);
2 DWORD PCO_SetInterfaceOutputFormat(WORD wInterface,WORD wFormat);

2 DWORD PCO_GetInterfaceStatus(WORD wInterface,DWORD* dwWarnings,DWORD* dwErrors,DWORD* dwStatus);


///////////////////////////////////////////////////////////////////////
//special control hidden PCO:internal only??

2 DWORD PCO_InitiateSelftestProcedure(DWORD* dwWarn, DWORD* dwErr);

2 DWORD PCO_WriteHotPixelList(WORD wListNo, WORD wNumValid,WORD* wHotPixX, WORD* wHotPixY);
2 DWORD PCO_ReadHotPixelList(WORD wListNo, WORD wArraySize,WORD* wNumValid, WORD* wNumMax, WORD* wHotPixX, WORD* wHotPixY);

2 DWORD PCO_ClearHotPixelList(WORD wListNo, DWORD dwMagic1,DWORD dwMagic2);
2 DWORD PCO_ClearHotPixelList(WORD wListNo);

2 DWORD PCO_LoadLookuptable(WORD wIdentifier,WORD wPacketNum,WORD wFormat,WORD wLength,BYTE* bData);
2 DWORD PCO_ReadLookuptable(WORD wIdentifier,WORD wPacketNum,WORD* wFormat,WORD* wLength,BYTE* bData,WORD buflen);

2 DWORD PCO_GetLookuptableInfoExt(WORD wIndex);


2 DWORD PCO_GetColorCorrectionMatrix(char* szCCM,WORD* len);
2 DWORD PCO_GetBayerMultiplier(WORD* wMode,WORD* wMul);
2 DWORD PCO_SetBayerMultiplier(WORD wMode,WORD* wMul);


//special control edge
2 DWORD PCO_GetHWLEDSignal(DWORD* dwParameter);
2 DWORD PCO_SetHWLEDSignal(DWORD dwParameter);

2 DWORD PCO_GetFanControlStatus(WORD* wFanMode,WORD* wFanMin,WORD* wFanMax,WORD* wStepSize,WORD* wSetValue,WORD* wActualValue);
2 DWORD PCO_SetFanControlStatus(WORD wFanMode,WORD wSetValue);


//special control dimax
2 DWORD PCO_GetDSNUAdjustMode(WORD *wMode);
2 DWORD PCO_SetDSNUAdjustMode(WORD wMode);
2 DWORD PCO_InitDSNUAdjustment(WORD *wMode);

2 DWORD PCO_GetCDIMode(WORD* wMode);
2 DWORD PCO_SetCDIMode(WORD wMode);

2 DWORD PCO_GetPowersaveMode(WORD* wMode,WORD* wDelayMinutes);
2 DWORD PCO_SetPowersaveMode(WORD wMode,WORD wDelayMinutes);
2 DWORD PCO_GetBatteryStatus(WORD* wBatteryType,WORD* wBatteryLevel,WORD* wPowerStatus);

2 DWORD PCO_GetExternalRegister(WORD* wID,BYTE* bData,WORD *len);
2 DWORD PCO_SetExternalRegister(WORD* wID,BYTE* bData,WORD* wDataSize);

2 DWORD PCO_WriteMailbox(WORD wMailboxNo,BYTE* bData,WORD len);
2 DWORD PCO_ReadMailbox(WORD wMailboxNo,BYTE* bData,WORD* len);
2 DWORD PCO_GetMailboxStatus(WORD* wNumberOfMailboxes,WORD* wMailboxStatus,WORD *len);


//special control flim
2 DWORD PCO_GetFlimModulationParams(WORD* wSourceSelect,WORD* wOutputWaveform);
2 DWORD PCO_SetFlimModulationParams(WORD wSourceSelect,WORD wOutputWaveform);
2 DWORD PCO_GetFlimPhaseSequenceParams(WORD* wPhaseNumber,WORD* wPhaseSymmetry,WORD* wPhaseOrder,WORD* wTapSelect);
2 DWORD PCO_SetFlimPhaseSequenceParams(WORD wPhaseNumber,WORD wPhaseSymmetry,WORD wPhaseOrder,WORD wTapSelect);
2 DWORD PCO_GetFlimImageProcessingFlow(WORD* wAsymmetryCorrection);
2 DWORD PCO_SetFlimImageProcessingFlow(WORD wAsymmetryCorrection);
2 DWORD PCO_GetFlimMasterModulationFrequency(DWORD* dwFrequency);
2 DWORD PCO_SetFlimMasterModulationFrequency(DWORD dwFrequency);
2 DWORD PCO_GetFlimRelativePhase(DWORD* dwPhaseMilliDeg);
2 DWORD PCO_SetFlimRelativePhase(DWORD dwPhaseMilliDeg);















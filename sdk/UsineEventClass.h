//-----------------------------------------------------------------------------
///@file UsineEventClass.h
///
///@author
///	BrainModular team
///
///@brief  
///	All includes for making a user module
///
///@HISTORIC 
///	2023/11/28
///    first release for Hollyhock CPP SDK : 7.10.07 
///
///@IMPORTANT
///	This file is part of the Usine Hollyhock CPP SDK
///
///  Please, report bugs and patch to Usine forum :
///  support@brainModular.com 
///  
/// All dependencies are under there own licence.
///
///@LICENSE
/// Copyright (C) 2023 BrainModular, BeSpline, Adamson
/// 
///-----------------------------------------------------------------------------

// Include once, no more
#pragma once

#include "UsineDefinitions.h"

//-----------------------------------------------------------------------------
// former module functions
//-----------------------------------------------------------------------------

/// Accessors to m_masterInfo
inline const TMasterInfo* sdkGetMasterInfo() { return UserModuleBase::sdkGetMasterInfo(); };
inline void sdkSetMasterInfo(TMasterInfo* m_masterInfo) { UserModuleBase::sdkSetMasterInfo(m_masterInfo); }

//----------------------------------------------------------------------------
/// Utils functions to help manipulate events
/// @name Events manipulation
/// @{

/// Compare Events, returns TRUE if e1 and e2 are equal.
[[deprecated("Deprecated: use UsineEventClass::compare instead")]]
inline bool sdkCompareEvt( UsineEventPtr e1, UsineEventPtr e2 ) {
    return UserModuleBase::m_masterInfo->CompareEvt( e1, e2 ); };

/// Copy Events, copies src to dest.
[[deprecated("Deprecated: use UsineEventClass::copy instead")]]
inline void sdkCopyEvt( UsineEventPtr scr, UsineEventPtr dest) {
    UserModuleBase::m_masterInfo->CopyEvt( scr, dest ); };

/// Event Concatenation dest is the concatenation of e1 and e2.
inline void sdkConcatEvt( UsineEventPtr e1, UsineEventPtr e2, UsineEventPtr dest ) {
    UserModuleBase::m_masterInfo->ConcatEvt( e1, e2, dest ); };

/// Shift the values of src and store the result into dest.
/// Right shift for n>0, left shift for n<0.
inline void sdkShiftEvt( int n, UsineEventPtr src, UsineEventPtr dest ) {
    UserModuleBase::m_masterInfo->ShiftEvt( n, src, dest ); };

/// Set the size of the event (number of elements). Typically the size of audio events is given by sdkGetBlocSize.
/// @see sdkGetBlocSize
[[deprecated("Deprecated: use UsineEventClass::setSize instead")]]
inline void sdkSetEvtSize( UsineEventPtr e, int size ) {
    UserModuleBase::m_masterInfo->SetEvtSize( e, size ); };

/// Get the size of the event (number of elements).
[[deprecated("Deprecated: use UsineEventClass::getSize instead")]]
inline int  sdkGetEvtSize( UsineEventPtr e)                                {
    return UserModuleBase::m_masterInfo->GetEvtSize( e ); };

/// Set the maximum size allowed by the event. Can be greater than the event size.
[[deprecated("Deprecated: use UsineEventClass::setMaxSize instead")]]
inline void sdkSetEvtMaxSize( UsineEventPtr e, int size )                  {
    UserModuleBase::m_masterInfo->SetEvtMaxSize( e, size ); };

/// Destroy the event.
[[deprecated("Deprecated: use UsineEventClass::destroy instead")]]
inline void sdkDestroyEvt( UsineEventPtr &e )                              {
    UserModuleBase::m_masterInfo->DestroyEvt( e ); };

/// Create an event with an original size.
[[deprecated("Deprecated: use UsineEventClass::create instead")]]
inline void sdkCreateEvt( UsineEventPtr &e, int originalSize )             {
    UserModuleBase::m_masterInfo->CreateEvt( e, originalSize ); };

/// Copy a 32Bits bloc of memory. The size is the number of 32bits elements.
inline void sdkMoveBLOC32( TPrecision* pointerSrc, TPrecision* pointerDest, int size )      {
    UserModuleBase::m_masterInfo->MoveBLOC32( pointerSrc, pointerDest, size ); };

/// Add values of two events and store the result into dest.
inline void  sdkAddEvt3(UsineEventPtr e1, UsineEventPtr e2, UsineEventPtr dest) {
    UserModuleBase::m_masterInfo->AddEvt3(e1, e2, dest); }

/// Add values of two events and store the result into e2.
[[deprecated("Deprecated: use UsineEventClass::add instead")]]
inline void  sdkAddEvt2(UsineEventPtr e1, UsineEventPtr e2) {
    UserModuleBase::m_masterInfo->AddEvt2(e1, e2); }

/// Subtract values of two events and store the result into dest.
inline void  sdkSubEvt3(UsineEventPtr e1, UsineEventPtr e2, UsineEventPtr dest) {
    UserModuleBase::m_masterInfo->SubEvt3(e1, e2, dest); }

/// Subtract values of two events and store the result into e2.
[[deprecated("Deprecated: use UsineEventClass::sub instead")]]
inline void  sdkSubEvt2(UsineEventPtr e1, UsineEventPtr e2) {
    UserModuleBase::m_masterInfo->SubEvt2(e1, e2); }

/// Divide values of e1 by values of e2 and store the result into dest.
 inline void  sdkDivEvt3(UsineEventPtr e1, UsineEventPtr e2, UsineEventPtr dest) {
    UserModuleBase::m_masterInfo->DivEvt3(e1, e2, dest); }


/// Multiply values of e1 by values of e2 and store the result into dest.
inline void  sdkMultEvt3(UsineEventPtr e1, UsineEventPtr e2, UsineEventPtr dest) {
    UserModuleBase::m_masterInfo->MultEvt3(e1, e2, dest); }

/// Multiply values of e1 by values of e2 and store the result into e2.
[[deprecated("Deprecated: use UsineEventClass::mult instead")]]
inline void  sdkMultEvt2(UsineEventPtr e1, UsineEventPtr e2) {
    UserModuleBase::m_masterInfo->MultEvt2(e1, e2); }

/// Multiply values of e1 by values of e2 and store the result into e2. Same as sdkMultEvt2.
[[deprecated("Deprecated: use UsineEventClass::multAudio instead")]]
inline void  sdkMultEvt2Audio(UsineEventPtr e1, UsineEventPtr e2) {
    UserModuleBase::m_masterInfo->MultEvt2(e1, e2); }

/// Multiply values of e by a float value inplace.
[[deprecated("Deprecated: use UsineEventClass::mult instead")]]
inline void  sdkMultEvt1(TPrecision m, UsineEventPtr e) {
    UserModuleBase::m_masterInfo->MultEvt1(m, e); }

/// Modulo operation of e1 by values of e2 and store the result into dest.
inline void  sdkModEvt3(UsineEventPtr e1, UsineEventPtr e2, UsineEventPtr dest) {
    UserModuleBase::m_masterInfo->ModEvt3(e1, e2, dest); }

/// Power values of e1 by values of e2 and store the result into dest.
inline void  sdkPowerEvt3(UsineEventPtr e1, UsineEventPtr e2, UsineEventPtr dest) {
    UserModuleBase::m_masterInfo->PowerEvt3(e1, e2, dest); }

/// Exponential of e inplace.
[[deprecated("Deprecated: use UsineEventClass::exp instead")]]
inline void  sdkExpEvt1(UsineEventPtr e) {
    UserModuleBase::m_masterInfo->ExpEvt1(e); }

/// Square root of e inplace.
[[deprecated("Deprecated: use UsineEventClass::sqrt instead")]]
inline void  sdkSqrtEvt1(UsineEventPtr in1) {
    UserModuleBase::m_masterInfo->SqrtEvt1(in1); }

/// Returns the maximum value of e.
[[deprecated("Deprecated: use UsineEventClass::max instead")]]
inline TPrecision sdkMaxEvt1(UsineEventPtr e) {
    return UserModuleBase::m_masterInfo->MaxEvt1(e); };

/// Returns the minimum value of e.
[[deprecated("Deprecated: use UsineEventClass::min instead")]]
inline TPrecision    sdkMinEvt1(UsineEventPtr e) {
    return UserModuleBase::m_masterInfo->MinEvt1(e); };

/// calculates the maximum of each element and store the result into dest. dest[i] = max(e1[i],e2[i])
inline void    sdkMaxEvt3(UsineEventPtr e1, UsineEventPtr e2, UsineEventPtr dest) {
    UserModuleBase::m_masterInfo->MaxEvt3(e1, e2, dest); };

/// calculates the minimum of each element and store the result into dest. dest[i] = min(e1[i],e2[i])
inline void    sdkMinEvt3(UsineEventPtr in1, UsineEventPtr in2, UsineEventPtr out) {
    UserModuleBase::m_masterInfo->MinEvt3(in1, in2, out); };

/// Clip values of e in the range [min..max] and store the result into dest.
inline void  sdkThreshEvt2(const UsineEventPtr e, UsineEventPtr dest, TPrecision min, TPrecision max) {
    UserModuleBase::m_masterInfo->ThreshEvt2(e, dest, min, max); };

/// Clip values of e in the range [min..max] inplace.
[[deprecated("Deprecated: use UsineEventClass::tresh instead")]]
inline void  sdkThreshEvt1(UsineEventPtr e, TPrecision min, TPrecision max) {
    UserModuleBase::m_masterInfo->ThreshEvt1(e, min, max); };

/// Mix MIDI values of e1 and e2 and store the result in dest. For the MIDI, this operation equivalent to sdkConcatEvt
inline void  sdkMixMidiEvt3(UsineEventPtr e1, UsineEventPtr e2, UsineEventPtr dest) {
    UserModuleBase::m_masterInfo->MixMidiEvt3(e1, e2, dest); };

/// Mix MIDI values of e1 and e2 and store the result in e2. For the MIDI, this operation is equivalent to sdkConcatEvt
[[deprecated("Deprecated: use UsineEventClass::mixMidi instead")]]
inline void  sdkMixMidiEvt2(UsineEventPtr e1, UsineEventPtr e2) {
    UserModuleBase::m_masterInfo->MixMidiEvt2(e1, e2); };

/// Apply a short fadein to the audio event. The duration of the fade is given by sdkGetBlocSize.
[[deprecated("Deprecated: use UsineEventClass::fadeIn instead")]]
inline void  sdkFadeInAudioEvt(UsineEventPtr e) {
    UserModuleBase::m_masterInfo->FadeInAudioEvt(e); };

/// Apply a short fadeout to the audio event. The duration of the fade is given by sdkGetBlocSize.
[[deprecated("Deprecated: use UsineEventClass::fadeOut instead")]]
inline void  sdkFadeOutAudioEvt(UsineEventPtr e) {
    UserModuleBase::m_masterInfo->FadeOutAudioEvt(e); };

/// Clear an audio event. Set values to zero and add a small denormalization value.
[[deprecated("Deprecated: use UsineEventClass::clearAudio instead")]]
inline void  sdkClearAudioEvt(UsineEventPtr e) {
    UserModuleBase::m_masterInfo->ClearAudioEvt(e); };

/// Add a small denormalization value to an audio event.
[[deprecated("Deprecated: use UsineEventClass::denormalizeAudio instead")]]
inline void  sdkDenormalizeAudioEvt(UsineEventPtr e) {
    UserModuleBase::m_masterInfo->DenormalizeAudioEvt(e); };

/// Smooth event.
[[deprecated("Deprecated: use UsineEventClass::smooth instead")]]
inline void sdkSmoothPrecision(TPrecision& oldValue, UsineEventPtr e, TPrecision factor) {
    return UserModuleBase::m_masterInfo->SmoothPrecision(oldValue, e, factor);};

/// Smooth Event with a target value.
[[deprecated("Deprecated: use UsineEventClass::smooth instead")]]
inline void sdkSmoothEvent(TPrecision& oldValue, UsineEventPtr e, TPrecision target, TPrecision factor){
    return UserModuleBase::m_masterInfo->SmoothEvent(oldValue, e, target, factor);}

/// @}

/// @name Access to Event values
/// @{

/// Set a Text value.
[[deprecated("Deprecated: use UsineEventClass::setPChar instead")]]
inline void sdkSetEvtPChar( UsineEventPtr e,  AnsiCharPtr textValue ) {
    UserModuleBase::m_masterInfo->SetEvtPChar( e, textValue ); };

/// Get a Text value.
[[deprecated("Deprecated: use UsineEventClass::getPChar instead")]]
inline AnsiCharPtr sdkGetEvtPChar( UsineEventPtr e ){
    return UserModuleBase::m_masterInfo->GetEvtPChar( e ); };

/// Get the address of the first data in the Event
[[deprecated("Deprecated: use UsineEventClass::getDataAddr instead")]]
inline TPrecision* sdkGetEvtDataAddr( UsineEventPtr ev ) {
    return UserModuleBase::m_masterInfo->GetEvtDataAddr( ev ); };

/// Set a float value
[[deprecated("Deprecated: use UsineEventClass::setData instead")]]
inline void sdkSetEvtData( UsineEventPtr e, TPrecision floatValue ) {
    UserModuleBase::m_masterInfo->SetEvtData( e, floatValue ); };

/// Get a float Value
[[deprecated("Deprecated: use UsineEventClass::getData instead")]]
inline TPrecision sdkGetEvtData( UsineEventPtr e ) {
    return UserModuleBase::m_masterInfo->GetEvtData( e ); };

/// Set a value to an Array at an index position.
[[deprecated("Deprecated: use UsineEventClass::setArrayData instead")]]
inline void sdkSetEvtArrayData( UsineEventPtr e, int index, TPrecision floatValue ) {
    UserModuleBase::m_masterInfo->SetEvtArrayData( e, index, floatValue ); };

/// Push (append) a value to an Array.
[[deprecated("Deprecated: use UsineEventClass::pushArrayData instead")]]
inline void	sdkPushEvtArrayData(UsineEventPtr e, TPrecision floatValue) {
    int index = UserModuleBase::m_masterInfo->GetEvtSize(e);
    if (index >= UserModuleBase::m_masterInfo->GetEvtMaxSize(e))
    {
        UserModuleBase::m_masterInfo->SetEvtMaxSize(e, index + PUSH_GRANULOSITY);
    }
    UserModuleBase::m_masterInfo->SetEvtSize(e, index + 1);
    UserModuleBase::m_masterInfo->SetEvtArrayData(e, index, floatValue);
};

/// Get a value from an Array at an index position.
[[deprecated("Deprecated: use UsineEventClass::getArrayData instead")]]
inline TPrecision sdkGetEvtArrayData( UsineEventPtr e, int index ) {
    return UserModuleBase::m_masterInfo->GetEvtArrayData( e, index ); };

/// Set a pointer value.
[[deprecated("Deprecated: use UsineEventClass::setEventPointer instead")]]
inline void sdkSetEvtPointer( UsineEventPtr e, void* pointerValue ) {
    UserModuleBase::m_masterInfo->SetEvtPointer( e, pointerValue ); };

/// Get a pointer value.
[[deprecated("Deprecated: use UsineEventClass::getEventPointer instead")]]
inline void* sdkGetEvtPointer( UsineEventPtr e ){
    return UserModuleBase::m_masterInfo->GetEvtPointer( e ); };

 /// Set a color value.
[[deprecated("Deprecated: use UsineEventClass::setColor instead")]]
 inline void sdkSetEvtColor( UsineEventPtr e, TUsineColor colorValue ) {
    UserModuleBase::m_masterInfo->SetEvtColor( e, colorValue ); };

/// Get a color value.
 [[deprecated("Deprecated: use UsineEventClass::getColor instead")]]
inline TUsineColor    sdkGetEvtColor( UsineEventPtr e ) {
    return UserModuleBase::m_masterInfo->GetEvtColor( e ); };

/// Set a color value to an array at index position.
[[deprecated("Deprecated: use UsineEventClass::setArrayColor instead")]]
inline void sdkSetEvtArrayColor(UsineEventPtr e, int index, TUsineColor colorValue) {
    // sanity check
    int idx = std::max(0, std::min(index, UserModuleBase::m_masterInfo->GetEvtSize(e)));
    TUsineColor* data = (TUsineColor*)UserModuleBase::m_masterInfo->GetEvtDataAddr(e);
    data += idx;
    *data = colorValue; };

/// Get a color value to an array at index position.
[[deprecated("Deprecated: use UsineEventClass::getArrayColor instead")]]
inline TUsineColor sdkGetEvtArrayColor( UsineEventPtr e, int index ) {
    // sanity check
    int idx = std::max(0, std::min(index, UserModuleBase::m_masterInfo->GetEvtSize(e)));
    TUsineColor* data =  (TUsineColor*)UserModuleBase::m_masterInfo->GetEvtDataAddr (e);
    TUsineColor result = *(data + idx);
    return result; };

/// Set a Midi value to an array at index position.
[[deprecated("Deprecated: use UsineEventClass::setArrayMidi instead")]]
inline void    sdkSetEvtArrayMidi( UsineEventPtr e, int index, TUsineMidiCode midiValue ) {
    UserModuleBase::m_masterInfo->SetEvtArrayMidi( e, index, midiValue ); };

/// Get a Midi value to an array at index position.
[[deprecated("Deprecated: use UsineEventClass::getArrayMidi instead")]]
inline TUsineMidiCode sdkGetEvtArrayMidi( UsineEventPtr e, int index )                    {
    return UserModuleBase::m_masterInfo->GetEvtArrayMidi( e, index ); };

/// Get a 3D point.
/// the event must have a line number of 3 to work,
/// @param pointIndex is the index of the point you want to get
[[deprecated("Deprecated: use UsineEventClass::get3DPoint instead")]]
inline T3DPointF sdkGetEvt3DPoint (UsineEventPtr e, int pointIndex){
    T3DPointF result;
    if (pointIndex < (UserModuleBase::m_masterInfo->GetEvtSize (e)/3))
    {
        TPrecision* pointer3DPoint = UserModuleBase::m_masterInfo->GetEvtDataAddr (e) + (pointIndex*3);
        result.x = *(pointer3DPoint);
        result.y = *(pointer3DPoint + 1);
        result.z = *(pointer3DPoint + 2);
    }
    else
    {
        result.x = 0.0f;
        result.y = 0.0f;
        result.z = 0.0f;
    }

    return result;
};
/// @}

//----------------------------------------------------------------------------
/// Synchronization Objects.
/// @name SyncObjects
/// @{

/// Create SyncObject.
inline TSyncObjectPtr sdkSyncObjectCreate() {
    return UserModuleBase::m_masterInfo->SyncObjectCreate();
}

/// Destroy SyncObject.
inline void sdkSyncObjectDestroy(TSyncObjectPtr pSyncObject) {
    UserModuleBase::m_masterInfo->SyncObjectDestroy(pSyncObject);
}

/// Set SyncObject.
inline void sdkSyncObjectSet(TSyncObjectPtr pSyncObject) {
    UserModuleBase::m_masterInfo->SyncObjectSet(pSyncObject);
}

/// Reset SyncObject.
inline void sdkSyncObjectReset(TSyncObjectPtr pSyncObject) {
    UserModuleBase::m_masterInfo->SyncObjectReset(pSyncObject);
}

/// Wait SyncObject.
inline void sdkSyncObjectWait(TSyncObjectPtr pSyncObject, UINT32 timeout) {
    UserModuleBase::m_masterInfo->SyncObjectWait(pSyncObject,timeout);
}

/// @}

//----------------------------------------------------------------------------
/// To use with the audio multi channels query.
/// @name Audio Channels query
/// @{

/// Get the title string for the popup.
inline AnsiCharPtr  sdkGetAudioQueryTitle () { return UserModuleBase::m_masterInfo->AudioQueryTitle; };

/// Get the query string list for an multi channels query.
inline AnsiCharPtr  sdkGetAudioQueryChannelList () { return UserModuleBase::m_masterInfo->AudioQueryChannelList; };

/// Get the number of channels from the query index choice.
inline int  sdkGetAudioQueryToNbChannels (int qQueryIndex) { return UserModuleBase::m_masterInfo->AudioQueryToNbChannels (qQueryIndex); };

/// Get the name of a channel.
inline AnsiCharPtr  sdkGetAudioQueryChannelNames (AnsiCharPtr prefix, int index, int queryIndex)
    { return UserModuleBase::m_masterInfo->AudioQueryChannelNames (prefix, index, queryIndex); };
/// @}


//----------------------------------------------------------------------------
/// @name Global Array functions
/// @{

/// Get the Hash (internal ID) of the Global Array needed for function bellow
inline UINT64 sdkGlobalArrayGetHash(AnsiCharPtr name) { return UserModuleBase::m_masterInfo->GlobalArrayGetHash(name); };

/// Set a float value of a data Global Array at the index
inline void  sdkGlobalArraySetValueFloat(UINT64 hash, int index, TPrecision value, AnsiCharPtr errorMsg) {
    UserModuleBase::m_masterInfo->GlobalArraySetValueFloat(hash, index, value, errorMsg); };
/// Set a color value of a color Global Array at the index
inline void  sdkGlobalArraySetValueColor(UINT64 hash, int index, TUsineColor value, AnsiCharPtr errorMsg) {
    UserModuleBase::m_masterInfo->GlobalArraySetValueColor(hash, index, value, errorMsg); };
/// Set a string value of a string Global Array at the index
inline void  sdkGlobalArraySetValueAnsiChar(UINT64 hash, int index, AnsiCharPtr value, AnsiCharPtr errorMsg) {
    UserModuleBase::m_masterInfo->GlobalArraySetValueAnsiChar(hash, index, value, errorMsg); };
/// Fill an entire global array with the content of an Event
inline void  sdkGlobalArraySetValueEvt(UINT64 hash, UsineEventPtr value, AnsiCharPtr errorMsg) {
    UserModuleBase::m_masterInfo->GlobalArraySetValueEvt(hash, value, errorMsg); };

/// Get a float value from a data Global Array at the index
inline TPrecision sdkGlobalArrayGetValueFloat(UINT64 hash, int index, AnsiCharPtr errorMsg) {
    return UserModuleBase::m_masterInfo->GlobalArrayGetValueFloat(hash, index, errorMsg); };
/// Get a color value from a color Global Array at the index
inline TUsineColor sdkGlobalArrayGetValueColor(UINT64 hash, int index, AnsiCharPtr errorMsg) {
    return UserModuleBase::m_masterInfo->GlobalArrayGetValueColor(hash, index, errorMsg); };
/// get a string value from a string Global Array at the index
inline AnsiCharPtr sdkGlobalArrayGetValueAnsiChar(UINT64 hash, int index, AnsiCharPtr errorMsg) {
    return UserModuleBase::m_masterInfo->GlobalArrayGetValueAnsiChar(hash, index, errorMsg); };
/// Get an entire global array and fill its content in an Event
inline void sdkGlobalArrayGetValueEvt(UINT64 hash, AnsiCharPtr errorMsg, UsineEventPtr result) {
    return UserModuleBase::m_masterInfo->GlobalArrayGetValueEvt(hash, errorMsg, result); };
/// Get the list of all available global-array's names as a comma-text. This function can be slow.
inline AnsiCharPtr sdkGlobalArrayGetAllNames() {
    return UserModuleBase::m_masterInfo->GlobalArrayGetAllNames(); };


/// @}

//----------------------------------------------------------------------------
/// @name Usine Core infos and services
/// @{

/// Get the list of possible usine block size.
inline AnsiCharPtr  sdkGetUsineBlockSizeList() {
    return UserModuleBase::m_masterInfo->BlockSizeList; };

/// Get the default usine block size.
inline int  sdkGetUsineDefaultBlockSizeIndex() {
    return UserModuleBase::m_masterInfo->BlockSizeDefaultIndex; };

/// Low level , not for public use
/// Usine audio callback
inline void    sdkUsineAudioDeviceIOCallback(const float** inputChannelData, int numInputChannels, float** outputChannelData, int numOutputChannels, int numSamples ){
    UserModuleBase::m_masterInfo->AudioDeviceIOCallback( inputChannelData, numInputChannels, outputChannelData, numOutputChannels, numSamples );};

/// Low level , not for public use
/// Usine  midi callback
inline void sdkUsineMidiDeviceCallback (int deviceID, double TimeStamp, TUsineMidiCode midiData)    {
    UserModuleBase::m_masterInfo->MidiDeviceCallback (deviceID, TimeStamp, midiData);};

/// Low level , not for public use
/// Usine midi sysex callback
inline void sdkUsineMidiSysexDeviceCallback (int deviceID, double TimeStamp, char** sysEx, int size){
    UserModuleBase::m_masterInfo->MidiSysexDeviceCallback (deviceID, TimeStamp, sysEx, size);};

/// To get the current Usine block size.
inline int        sdkGetBlocSize()              {
    return UserModuleBase::m_masterInfo->BlocSize; };

/// To get the current Usine sample rate
inline double    sdkGetSampleRate()              {
    return UserModuleBase::m_masterInfo->GetSampleRate(); };
    
/// PC only, Usine mainform handle (THandle)
inline NativeInt sdkGetUsineMainWindow()          {
    return UserModuleBase::m_masterInfo->GetUsineMainWindow(); };

/// MAC only, Usine NSView
inline void* sdkGetUsineNSView() {
    return UserModuleBase::m_masterInfo->GetMacNSView();
};

/// MAC only, Usine NSApplication
inline void* sdkGetUsineNSApplication()       {
    return (void*)UserModuleBase::m_masterInfo->NSApplication; };

/// To get a named color from the Usine ColorSet.
inline TUsineColor sdkGetUsineColor(UINT32 color){
    TUsineColor result = 0;
    if (UserModuleBase::m_masterInfo != 0)
        result = UserModuleBase::m_masterInfo->GetUsineColor (color-1);
    return result;};

///Translate a StringID to the current Usine language.
inline AnsiCharPtr sdkGetTrans(AnsiCharPtr StringID) {
    return UserModuleBase::m_masterInfo->GetTranslation(StringID);
};

/// To get the version type of Usine @see TUsineVersionType.
[[deprecated("Usine Version is no longer used in the SDK,\nUsine will send always the value 0")]]
inline int sdkGetUsineVersionType() {
    return 0;
};

/// To get the current language used in Usine. 
/// @return a string naming the current language (En, Fr...)
inline AnsiCharPtr sdkGetUsineLanguage() {
    return UserModuleBase::m_masterInfo->UsineLanguage;
};

/// To get the of the save format of Usine. 
inline int sdkGetUsineSaveVersion() {
    return UserModuleBase::m_masterInfo->UsineSaveVersion;
};

/// To get the maximum audio inputs Usine can accept. 
inline int sdkGetUsineMaxAudioInputs() {
    return USINE_MAX_AUDIO_INPUTS;
};

/// To get the maximum audio outputs Usine can accept. 
inline int sdkGetUsineMaxAudioOutputs() {
    return USINE_MAX_AUDIO_OUTPUTS;
};

/// To get the maximum Midi device Usine can accept. 
inline int sdkGetUsineMaxMidiDevices() {
    return USINE_MAX_MIDI_DEVICES;
};

/// To get the current polyphony of Usine.  
inline int sdkGetUsineMaxPolyphony() {
    return USINE_MULTIPHONY_MAX;
};

/// @}

//----------------------------------------------------------------------------
/// To get the path to access some important folders of Usine.
/// @name Usine Paths getters
/// @{


/// The Sound folder as configured in the Global setting Tab.
inline AnsiCharPtr    sdkGetUsineSoundPath  ()            {
    return UserModuleBase::m_masterInfo->SoundPath; };

/// The Patch library folder as configured in the Global setting Tab.
inline AnsiCharPtr    sdkGetUserLibPath ()                {
    return UserModuleBase::m_masterInfo->UserLibPath; };

/// The Record Out folder as configured in the Global setting Tab.
inline AnsiCharPtr    sdkGetUsineRecordPath ()            {
    return UserModuleBase::m_masterInfo->RecordPath; };

/// The install folder of Usine.
inline AnsiCharPtr    sdkGetGlobalApplicationPath ()   {
    return UserModuleBase::m_masterInfo->GlobalApplicationPath; };

/// The path of the current workspace.
inline AnsiCharPtr    sdkGetCurrentWorkspacePath ()    {
    return UserModuleBase::m_masterInfo->CurrentWorkspacePath; };

/// The path of the patch containing the module.
inline AnsiCharPtr    sdkGetCurrentPatchPath ()        {
    return UserModuleBase::m_masterInfo->CurrentPatchPath; };

/// The Temp folder Used by Usine.
inline AnsiCharPtr    sdkGetUsineTempPath ()            {
    return UserModuleBase::m_masterInfo->UsineTempPath; };

/// Not for public use.
inline AnsiCharPtr    sdkGetUsinePlugInsPath ()        {
    return UserModuleBase::m_masterInfo->PlugInsPath; };
/// @}

//----------------------------------------------------------------------------
/// @name Files utils
/// @{

/// Show a dialog window to open a file.
inline LongBool    sdkProcessOpenDialog ( AnsiCharPtr* filename, AnsiCharPtr initialDir, AnsiCharPtr filter ) {
    return UserModuleBase::m_masterInfo->ProcessOpenDialog    ( filename, initialDir, filter ); };

/// Show a dialog window to save a file.
inline LongBool    sdkProcessSaveDialog ( AnsiCharPtr* filename, AnsiCharPtr initialDir, AnsiCharPtr filter ) {
    return UserModuleBase::m_masterInfo->ProcessSaveDialog    ( filename, initialDir, filter ); };

/// Ask Usine to search if a file exist.
inline AnsiCharPtr    sdkFindFile          ( AnsiCharPtr* filename, AnsiCharPtr initialDir )               {
    return UserModuleBase::m_masterInfo->FindFile            ( filename, initialDir ); };
/// @}

//----------------------------------------------------------------------------
/// @name Audio Files manipulation
/// @{

/// Create an audio file handle
inline TAudioFilePtr    sdkCreateAudioFile          (){
    return UserModuleBase::m_masterInfo->CreateAudioFile(); };

/// Destroy an audio file handle
inline void            sdkDestroyAudioFile         (TAudioFilePtr audioFile){
    UserModuleBase::m_masterInfo->DestroyAudioFile( audioFile); };

/// Get a sample value of an audio file at the position pos of the audio channel
inline TPrecision   sdkGetSampleAudioFile       (TAudioFilePtr audioFile, int channel, int pos)                      {
    return UserModuleBase::m_masterInfo->GetSampleAudioFile(audioFile, channel, pos); };

/// Get all samples values (as an array) of an audio file for an audio channel
inline TPrecision*  sdkGetSampleArrayAudioFile  (TAudioFilePtr audioFile, int channel)                               {
    return UserModuleBase::m_masterInfo->GetSampleArrayAudioFile(audioFile, channel); };

/// Get a bloc of samples values of an audio file at position pos for an audio channel and store it in an Event
inline void            sdkGetBlocSampleAudioFile   (TAudioFilePtr audioFile, int channel, int pos, UsineEventPtr evt)    {
    UserModuleBase::m_masterInfo->GetBlocSampleAudioFile            (audioFile, channel, pos, evt); };

/// Get the numbers of samples of an audio file
inline int            sdkGetSizeAudioFile         (TAudioFilePtr audioFile)                                            {
    return UserModuleBase::m_masterInfo->GetSizeAudioFile            (audioFile); };

/// Get the the number of channels of an audio file
inline int            sdkGetChannelAudioFile      (TAudioFilePtr audioFile)                                            {
    return UserModuleBase::m_masterInfo->GetChannelAudioFile        ( audioFile); };

///Get the samplerate of an audio file
inline int            sdkGetSampleRateAudioFile   (TAudioFilePtr audioFile)                                            {
    return UserModuleBase::m_masterInfo->GetSampleRateAudioFile    ( audioFile); };

/// Get the bit per sample of an audio file
inline int            sdkGetBitPerSampleAudioFile (TAudioFilePtr audioFile)                                            {
    return UserModuleBase::m_masterInfo->GetBitPerSampleAudioFile    ( audioFile); };

/// Load an audio file in memory
inline void            sdkLoadInMemoryAudioFile    (TAudioFilePtr audioFile, AnsiCharPtr name)                            {
    UserModuleBase::m_masterInfo->LoadInMemoryAudioFile    ( audioFile, name); };

/// Load an audio file but keep it on disk
inline void            sdkLoadStayOnDiskAudioFile  (TAudioFilePtr audioFile, AnsiCharPtr name)                            {
    UserModuleBase::m_masterInfo->LoadStayOnDiskAudioFile    ( audioFile, name); };

/// Get the peak of the audio files (to draw the wave form) and store it in an Event
inline void            sdkGetPeaksAudioFile        (TAudioFilePtr audioFile, UsineEventPtr evt)                            {
    UserModuleBase::m_masterInfo->GetPeaksAudioFile        ( audioFile, evt); };

/// Save an audio file to disk
inline void            sdkSaveToDiskAudioFile      (TAudioFilePtr audioFile, AnsiCharPtr name, int nbChannels)            {
    UserModuleBase::m_masterInfo->SaveToDiskAudioFile        ( audioFile, name, nbChannels); };

/// Set the number of channels of an audio file
inline void            sdkSetChannelsAudioFile     (TAudioFilePtr audioFile, int nbChannels)                            {
    UserModuleBase::m_masterInfo->SetChannelsAudioFile        ( audioFile, nbChannels); };

/// Set the number of samples of an audio file
inline void            sdkSetSizeAudioFile         (TAudioFilePtr audioFile, int size)                                    {
    UserModuleBase::m_masterInfo->SetSizeAudioFile            ( audioFile, size); };

/// Set a sample value of an audio file at the position pos of the audio channel
inline void            sdkSetSampleAudioFile       (TAudioFilePtr audioFile, int channel, int pos, TPrecision sample)    {
    UserModuleBase::m_masterInfo->SetSampleAudioFile        ( audioFile, channel, pos, sample); };

/// Clear an audio file
inline void            sdkClearAudioFile           (TAudioFilePtr audioFile)                                            {
    UserModuleBase::m_masterInfo->ClearAudioFile(audioFile); };

/// Resample (stretch) an audio file by a factor
inline void            sdkResampleAudioFile        (TAudioFilePtr audioFile, TPrecision factor)                         {
    UserModuleBase::m_masterInfo->ResampleAudioFile(audioFile,factor); };
/// @}

//----------------------------------------------------------------------------
/// @name Math utils
/// @{

/// Linear interpolation  on a TPrecision value.
/// @return The interpolated value.
inline TPrecision sdkLinearInterpolation ( TPrecision f,  TPrecision a, TPrecision b )                                            {
    return UserModuleBase::m_masterInfo->LinearInterpolation    ( f, a, b ); };

/// Cubic interpolation  on a TPrecision value.
/// @return The interpolated value.
inline TPrecision sdkCubicInterpolation  ( TPrecision fr, TPrecision inm1, TPrecision inp, TPrecision inp1, TPrecision inp2 )    {
    return UserModuleBase::m_masterInfo->CubicInterpolation    ( fr, inm1, inp, inp1, inp2 ); };

/// Spline interpolation on a TPrecision value.
/// @return The interpolated value.
inline TPrecision sdkSplineInterpolation ( TPrecision fr, TPrecision inm1, TPrecision inp, TPrecision inp1, TPrecision inp2 )    {
    return UserModuleBase::m_masterInfo->SplineInterpolation    ( fr, inm1, inp, inp1, inp2 ); };
/// @}

//----------------------------------------------------------------------------
/// @name Trace functions
/// @{

/// Show a string in the Trace Panel.
inline void sdkTraceChar (const AnsiCharPtr s) {
    UserModuleBase::m_masterInfo->TraceChar ( s ); };
inline void sdkTraceChar(const std::string& s) {
    UserModuleBase::m_masterInfo->TraceChar(s.c_str()); };

/// Show an integer in the Trace Panel.
inline void sdkTraceInt (const int i) {
    UserModuleBase::m_masterInfo->TraceInteger ( i ); };

/// Show a decimal number in the Trace Panel.
inline void sdkTracePrecision (const TPrecision f)                     {
    UserModuleBase::m_masterInfo->TracePrecision ( f ); };

/// Show a string in the Trace Panel with the error tag.
inline void sdkTraceErrorChar   (const AnsiCharPtr s) 
{
    if (UserModuleBase::m_masterInfo != nullptr)
    {
        std::stringstream traceString;
        traceString
            << "[" << UserModuleBase::MODULE_NAME << "]"
            << " " << s;
        UserModuleBase::m_masterInfo->TraceErrorChar(static_cast<AnsiCharPtr>(traceString.str().c_str()));
    }
};
inline void sdkTraceErrorChar(const std::string& s)
{
    if (UserModuleBase::m_masterInfo != nullptr)
    {
        std::stringstream traceString;
        traceString
            << "[" << UserModuleBase::MODULE_NAME << "]"
            << " " << s;
        UserModuleBase::m_masterInfo->TraceErrorChar(static_cast<AnsiCharPtr>(traceString.str().c_str()));
    }
};

/// Show a string in the Trace Panel with the warning tag.
inline void sdkTraceWarningChar (const AnsiCharPtr s) {
    if (UserModuleBase::m_masterInfo != nullptr)
    {
        std::stringstream traceString;
        traceString
            << "[" << UserModuleBase::MODULE_NAME << "]"
            << " " << s;
        UserModuleBase::m_masterInfo->TraceWarningChar(static_cast<AnsiCharPtr>(traceString.str().c_str()));
    }
};
inline void sdkTraceWarningChar(const std::string& s) {
    if (UserModuleBase::m_masterInfo != nullptr)
    {
        std::stringstream traceString;
        traceString
            << "[" << UserModuleBase::MODULE_NAME << "]"
            << " " << s;
        UserModuleBase::m_masterInfo->TraceWarningChar(static_cast<AnsiCharPtr>(traceString.str().c_str()));
    }
};


/// Show a string in the SplashForm.
inline void sdkTraceSplashChar  (const AnsiCharPtr s, const int autoClose)  {
    UserModuleBase::m_masterInfo->TraceSplashChar   ( s, autoClose ); };
inline void sdkTraceSplashChar(const std::string& s, const int autoClose) {
    UserModuleBase::m_masterInfo->TraceSplashChar(s.c_str(), autoClose);
};


/// Print a string in the LogUsine.log file. Optionally show it in the SplashForm.
inline void sdkTraceLogChar     (const AnsiCharPtr s, const LongBool showInSplashForm = FALSE){
    if (UserModuleBase::m_masterInfo != nullptr)
    {
        std::stringstream traceString;
        traceString
            << "  [" << UserModuleBase::MODULE_NAME << "]"
            << " " << s;
        UserModuleBase::m_masterInfo->TraceLogChar((AnsiCharPtr)traceString.str().c_str(), showInSplashForm);
    }

};

/// Print a string and a value in the LogUsine.log file. Optionally show it in the SplashForm.
inline void sdkTraceLogValueChar(const std::string& traceMsg, const std::string& value, const LongBool showInSplashForm = FALSE)
{
    if (UserModuleBase::m_masterInfo != nullptr)
    {
        std::stringstream traceString;
        traceString
            << "  [" << UserModuleBase::MODULE_NAME << "]"
            << " " << traceMsg << " " << value;
        UserModuleBase::m_masterInfo->TraceLogChar((AnsiCharPtr)traceString.str().c_str(), showInSplashForm);
    }
}

/// @}


//-----------------------------------------------------------------------------
/// @name Dialog popups
/// @{

/// A dialog popup with Yes, No and Cancel buttons.
/// @param msg The message to show on the popup popup.
/// @return The choice of the user.
/// @see TDialogsResults
inline int sdkDialogConfirmationYesNoCancel    ( AnsiCharPtr msg ) {
    return UserModuleBase::m_masterInfo->DialogConfirmationYesNoCancel(msg);};

/// A dialog popup with Yes and No buttons.
/// @param msg The message to show on the popup popup.
/// @return The choice of the user.
/// @see TDialogsResults
inline int sdkDialogConfirmationYesNo            ( AnsiCharPtr msg ) {
    return UserModuleBase::m_masterInfo->DialogConfirmationYesNo(msg);};

/// A dialog popup with OK button.
/// @param msg The message to show on the popup popup.
/// @return The choice of the user.
/// @see TDialogsResults
inline int sdkDialogInformationOk                ( AnsiCharPtr msg ) {
    return UserModuleBase::m_masterInfo->DialogInformationOk(msg);};

/// A dialog popup with OK and Cancel buttons.
/// @param msg The message to show on the popup popup.
/// @return The choice of the user.
/// @see TDialogsResults
inline int sdkDialogConfirmationOKCancel        ( AnsiCharPtr msg ) {
    return UserModuleBase::m_masterInfo->DialogConfirmationOKCancel(msg);};

/// A dialog popup with a string input box .
/// @return the edited string.
/// @see TDialogsResults
inline AnsiCharPtr sdkDialogInputBox(AnsiCharPtr caption, AnsiCharPtr prompt, AnsiCharPtr defaultValue) {
    return UserModuleBase::m_masterInfo->DialogInputBox(caption,prompt,defaultValue); };

/// @}

//----------------------------------------------------------------------------
/// Usine event class, wrapper around UsineEventPtr
/// @name UsineEventClass
/// @{
class UsineEventClass
{

public:
    /// Constructor.
	UsineEventClass() = default;
    UsineEventClass(UsineEventPtr e) 
        : p_event_(e)
    {}
    
    /// Disable implicit copy.
    UsineEventClass& operator=(const UsineEventClass&) = delete;
    
    /// Destructor.
    ~UsineEventClass() {};

public:
    /// Get a reference to the contained event.
    /// Not for public use.
    inline UsineEventPtr& event() { return p_event_; };
    
    /// Destroy the contained event.
    /// @see sdkDestroyEvent.
    inline void destroyEvent() { UserModuleBase::m_masterInfo->DestroyEvt(p_event_); };
    /// Allocate a new event and point to it.
    /// The former contained event will not be deleted.
    /// @see sdkCreateEvent.
    inline void createEvent(int size) { UserModuleBase::m_masterInfo->CreateEvt(p_event_, size); };
    
    /// @name Events manipulation
    /// @{
    
    /// Compare events.

	inline const bool compare(const UsineEventClass& e) { return UserModuleBase::m_masterInfo->CompareEvt(p_event_, e.p_event_); };
    
    /// Copy events, copy the source event into this one.
    inline void copyfrom(const UsineEventClass& src) { UserModuleBase::m_masterInfo->CopyEvt(src.p_event_, p_event_); };

    /// Copy events, copy to the dest event.
    inline void copyto(const UsineEventClass& dest) { UserModuleBase::m_masterInfo->CopyEvt(p_event_, dest.p_event_); };

    /// Append the other event to this one.
    inline void concat(const UsineEventClass& e) { UserModuleBase::m_masterInfo->ConcatEvt(p_event_, e.p_event_, p_event_); };
    
    /// Shift the values of this event by n.
    /// Right shift for n > 0, left shift for n < 0.
    inline void shift(const int n) { UserModuleBase::m_masterInfo->ShiftEvt(n, p_event_, p_event_); };
    
    /// Set the size of this event (number of elements). Typically the size of audio events is given by UserModuleBase::m_masterInfo->GetBlocSize.
    /// @see UserModuleBase::m_masterInfo->GetBlocSize
    inline void setSize(const int size) { UserModuleBase::m_masterInfo->SetEvtSize(p_event_, size); };
    /// Get the size of this event (number of elements).
    inline const int getSize() const { return UserModuleBase::m_masterInfo->GetEvtSize(p_event_); };
    
    /// Set the maximum size allowed by this event. Can be greater than the event size.
    inline void setMaxSize(const int size) { UserModuleBase::m_masterInfo->SetEvtMaxSize(p_event_, size); };
    
    /// Add the ohter event to this one.
    inline void add(const UsineEventClass& e) { UserModuleBase::m_masterInfo->AddEvt2(e.p_event_, p_event_); };
    /// Substract the ohter event to this one.
    inline void sub(const UsineEventClass& e) { UserModuleBase::m_masterInfo->SubEvt2(e.p_event_, p_event_); };
    /// Divide this event by the other.
    inline void div(const UsineEventClass& e) { UserModuleBase::m_masterInfo->DivEvt3(p_event_, e.p_event_, p_event_); };
    /// Multiply this event by a float value.
    inline void mult(const TPrecision value) { UserModuleBase::m_masterInfo->MultEvt1(value, p_event_); };
    /// Multiply this event by the other.
    inline void mult(const UsineEventClass& e) { UserModuleBase::m_masterInfo->MultEvt2(e.p_event_, p_event_); };
	/// Compute the modulo of this event by the other one and store the result inplace.
	inline void mod(const UsineEventClass& e) { UserModuleBase::m_masterInfo->ModEvt3(p_event_, e.p_event_, p_event_); }
	/// Compute the power of this event by the other one and store the result inplace.
	inline void power(const UsineEventClass& e) { UserModuleBase::m_masterInfo->PowerEvt3(p_event_, e.p_event_, p_event_); }

	/// Multiply this event by the other but for audio.
	inline void multAudio(const UsineEventClass& e) { UserModuleBase::m_masterInfo->MultEvt2Audio(e.p_event_, p_event_); };
    
    /// Exponential of this event inplace.
    inline void exp() { UserModuleBase::m_masterInfo->ExpEvt1(p_event_); };
    /// Square root of this event inplace.
    inline  void sqrt() { UserModuleBase::m_masterInfo->SqrtEvt1(p_event_); };
    /// Get the maximum value of this event.
    inline const TPrecision max() { return UserModuleBase::m_masterInfo->MaxEvt1(p_event_); };
    /// Get the minimum value of this event.
    inline const TPrecision min() { return UserModuleBase::m_masterInfo->MinEvt1(p_event_); };
    
    /// Clip values of this event in the range [min..max] and store the result into dest.
    inline void tresh(const TPrecision min, const TPrecision max) { UserModuleBase::m_masterInfo->ThreshEvt1(p_event_, min, max); };
    
    /// Mix MIDI values of the other event and this one and store the result inplace. For MIDI, this operation is equivalent to sdkConcatEvt
    inline void mixMidi(const UsineEventClass& e) { UserModuleBase::m_masterInfo->MixMidiEvt2(e, p_event_); };
    /// Apply a short fadein to this audio event. The duration of the fade is given by UserModuleBase::m_masterInfo->GetBlocSize.
    inline void fadeIn() { UserModuleBase::m_masterInfo->FadeInAudioEvt(p_event_); };
    /// Apply a short fadeout to this audio event. The duration of the fade is given by UserModuleBase::m_masterInfo->GetBlocSize.
	inline void fadeOut() { UserModuleBase::m_masterInfo->FadeOutAudioEvt(p_event_); };
    /// Clear this audio event. Set values to zero and add a small denormalization value.
    inline void clearAudio() { UserModuleBase::m_masterInfo->ClearAudioEvt(p_event_); };
    /// Add a small denormalization value to this audio event.
    inline void denormalizeAudio() { UserModuleBase::m_masterInfo->DenormalizeAudioEvt(p_event_); };
    /// Smooth this event.
    inline void smooth(TPrecision& oldValue, const TPrecision factor) { UserModuleBase::m_masterInfo->SmoothPrecision(oldValue, p_event_, factor); };
    /// Smooth this event with a target value.
    inline void smooth(TPrecision& oldValue, const TPrecision target, const TPrecision factor) { UserModuleBase::m_masterInfo->SmoothEvent(oldValue, p_event_, target, factor); };
    

    /// @}
    
    /// @name Access to Event values
    /// @{
    
    /// Set a Text value.
    inline  void setPChar(const AnsiCharPtr s) { UserModuleBase::m_masterInfo->SetEvtPChar(p_event_, s); };
    /// Get a Text value.
    inline  const AnsiCharPtr getPChar() const { return UserModuleBase::m_masterInfo->GetEvtPChar(p_event_); };
    
    /// Get the address of the first data in this Event
    inline TPrecision* getDataAddr() const { return UserModuleBase::m_masterInfo->GetEvtDataAddr(p_event_); };
    
    /// Set a float value
    inline void setData(const TPrecision value) { UserModuleBase::m_masterInfo->SetEvtData(p_event_, value); };
    /// Get a float value
    inline const TPrecision getData() const { return UserModuleBase::m_masterInfo->GetEvtData(p_event_); };
    
    /// Set a value at an index position.
    inline void setArrayData(const int index, const TPrecision value) { UserModuleBase::m_masterInfo->SetEvtArrayData(p_event_, index, value); };
    
    /// Push (append) a value to this event.
    inline void pushArrayData(const TPrecision value) {
        int index = UserModuleBase::m_masterInfo->GetEvtSize(p_event_);
        if (index >= UserModuleBase::m_masterInfo->GetEvtMaxSize(p_event_))
        {
            UserModuleBase::m_masterInfo->SetEvtMaxSize(p_event_, index + PUSH_GRANULOSITY);
        }
        UserModuleBase::m_masterInfo->SetEvtSize(p_event_, index + 1);
        UserModuleBase::m_masterInfo->SetEvtArrayData(p_event_, index, value);
    };
    
    /// Get a value at an index position.
    inline const TPrecision getArrayData(const int index) { return UserModuleBase::m_masterInfo->GetEvtArrayData(p_event_, index); };
    
    /// Set the pointer of the contained Event Pointer.
    inline void setEventPointer(void* pointerValue) { UserModuleBase::m_masterInfo->SetEvtPointer(p_event_, pointerValue); };
    /// Get the pointer of the contained Event Pointer .
    inline const void* getEventPointer() const { return UserModuleBase::m_masterInfo->GetEvtPointer(p_event_); };
    
    /// Set a color value.
    inline void setColor(const TUsineColor colorValue) { UserModuleBase::m_masterInfo->SetEvtColor(p_event_, colorValue); };
    /// Get a color value.
    inline const TUsineColor getColor() const { return UserModuleBase::m_masterInfo->GetEvtColor(p_event_); };
    
    /// Set a color value at an index position.
    inline void setArrayColor(const int index, const TUsineColor colorValue) {     
        // sanity check
        int idx = std::max(0, std::min(index, UserModuleBase::m_masterInfo->GetEvtSize(p_event_)));
        TUsineColor* data = (TUsineColor*)this->getDataAddr();
        data += idx;
        *data = colorValue;
    };
    /// Get a color value at an index position.
    inline const TUsineColor getArrayColor(const int index) const {     
        // sanity check
        int idx = std::max(0, std::min(index, UserModuleBase::m_masterInfo->GetEvtSize(p_event_)));
        TUsineColor* data = (TUsineColor*)UserModuleBase::m_masterInfo->GetEvtDataAddr(p_event_);
        TUsineColor result = *(data + idx);
        return result;
    };
    
    /// Get a Midi value at an index position.
    inline void setArrayMidi(const int index, const TUsineMidiCode midiValue) { UserModuleBase::m_masterInfo->SetEvtArrayMidi(p_event_, index, midiValue); };
    /// Set a Midi value at an index position.
    inline const TUsineMidiCode getArrayMidi(const int index) const { return UserModuleBase::m_masterInfo->GetEvtArrayMidi(p_event_, index); };
    
    /// Get a 3D point.
    /// this event must have a line number of 3 to work,
    /// @param pointIndex is the index of the point you want to get
    inline const T3DPointF get3DPoint(const int pointIndex) const {
        T3DPointF result = { 0.0f,0.0f,0.0f };
        if (pointIndex < (UserModuleBase::m_masterInfo->GetEvtSize(p_event_) / 3))
        {
            TPrecision* pointer3DPoint = 
                UserModuleBase::m_masterInfo->GetEvtDataAddr(p_event_) + (pointIndex * 3);
            result.x = *(pointer3DPoint);
            result.y = *(pointer3DPoint + 1);
            result.z = *(pointer3DPoint + 2);
        }
        else
        {
            result.x = 0.0f;
            result.y = 0.0f;
            result.z = 0.0f;
        }

        return result;
    };
    
    /// @}
    
    // FIXME: Overload the UserModuleBase::m_masterInfo-> functions instead or declare explicit ?
    inline operator UsineEventPtr() const { return p_event_; };

    /// Used in onGetParamInfo, declared explicit to avoid conflict 
    explicit inline operator UsineEventPtr* () { return &p_event_; };


    
protected:
    /// The contained UsineEventPtr.
	UsineEvent* p_event_;

};

/// @}

/// <summary>
/// setter for the TParamInfo struct, must come after UsineEventClass definition, hence defined here.
/// ignores deprecation.
/// </summary>
/// <param name="e">event</param>
#if defined(__GNUC__)
#pragma GCC diagnostic push 
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
inline void TParamInfo::setEventClass(UsineEventClass& e)
{
    // disable deprecation warning for this line only.
    this->EventPtr = &e.event();
}
#pragma GCC diagnostic pop

#elif(_WIN32)

inline void TParamInfo::setEventClass(UsineEventClass& e)
{
    // disable deprecation warning for this line only.
#pragma warning(suppress : 4996)
    this->EventPtr = &e.event();
}

#endif // defined(__GNUC__)


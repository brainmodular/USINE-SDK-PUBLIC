///-----------------------------------------------------------------------------
///@file  
///	UsineFunctions.h
///
///@author
///	BrainModular team
///
///@brief  
///	Datas and functions declarations exposed by Usine.
///
///@HISTORIC
///	creation 2025/05/28
///
///@IMPORTANT
///	This file is part of the Usine Hollyhock CPP SDK
///
///  Please, report bugs and patch to Usine forum :
///  support@brainmodular.com 
///
/// All dependencies are under there own licence.
///
///@LICENSE
/// Copyright (C) 2023 BrainModular, BeSpline, Adamson
/// 
///-----------------------------------------------------------------------------
#pragma once

#include "UsineDefinitions.h"
#include "UsineEventClass.h"

//-----------------------------------------------------------------------------
/// @addtogroup Functions
/// @{
/// this file contains the Usine SDK functions.
/// 


//----------------------------------------------------------------------------
/// Threads synchronization Objects.
/// This is a set of functions to create and manipulate synchronization objects.
/// 
/// @name SyncObjects
/// @{

/// Create SyncObject.
[[maybe_unused]]
inline TSyncObjectPtr sdkSyncObjectCreate()
{
    return UserModuleBase::m_masterInfo->SyncObjectCreate();
}

/// Destroy SyncObject.
[[maybe_unused]]
inline void sdkSyncObjectDestroy(TSyncObjectPtr pSyncObject)
{
    UserModuleBase::m_masterInfo->SyncObjectDestroy(pSyncObject);
}

/// Set SyncObject.
[[maybe_unused]]
inline void sdkSyncObjectSet(TSyncObjectPtr pSyncObject)
{
    UserModuleBase::m_masterInfo->SyncObjectSet(pSyncObject);
}

/// Reset SyncObject.
[[maybe_unused]]
inline void sdkSyncObjectReset(TSyncObjectPtr pSyncObject)
{
    UserModuleBase::m_masterInfo->SyncObjectReset(pSyncObject);
}

/// Wait SyncObject.
[[maybe_unused]]
inline void sdkSyncObjectWait(TSyncObjectPtr pSyncObject, const UINT32 timeout)
{
    UserModuleBase::m_masterInfo->SyncObjectWait(pSyncObject, timeout);
}

/// @}

//----------------------------------------------------------------------------
/// To use with the audio multi channels query.
/// @name Audio Channels query
/// @{

/// Get the title string for the popup.
[[maybe_unused]] 
inline AnsiCharPtr sdkGetAudioQueryTitle() { return UserModuleBase::m_masterInfo->AudioQueryTitle; }

/// Get the query string list for an multi channels query.
[[maybe_unused]] 
inline AnsiCharPtr sdkGetAudioQueryChannelList() { return UserModuleBase::m_masterInfo->AudioQueryChannelList; }

/// Get the number of channels from the query index choice.
[[maybe_unused]] 
inline int sdkGetAudioQueryToNbChannels(const int qQueryIndex)
{
    return UserModuleBase::m_masterInfo->AudioQueryToNbChannels(qQueryIndex);
}

/// Get the name of a channel.
[[maybe_unused]] 
inline AnsiCharPtr sdkGetAudioQueryChannelNames(const AnsiCharPtr prefix, const int index, const int queryIndex)
{
    return UserModuleBase::m_masterInfo->AudioQueryChannelNames(prefix, index, queryIndex);
}

/// @}


//----------------------------------------------------------------------------
/// @name Global Array functions
/// @{

/// Get the Hash (internal ID) of the Global Array needed for function bellow
[[maybe_unused]] 
inline UINT64 sdkGlobalArrayGetHash(const AnsiCharPtr name)
{
    return UserModuleBase::m_masterInfo->GlobalArrayGetHash(name);
}

/// Set a float value of a data Global Array at the index
[[maybe_unused]]
inline void sdkGlobalArraySetValueFloat(const UINT64 hash, const int index, const TPrecision value,
                                        const AnsiCharPtr errorMsg)
{
    UserModuleBase::m_masterInfo->GlobalArraySetValueFloat(hash, index, value, errorMsg);
}

/// Set a color value of a color Global Array at the index
[[maybe_unused]]
inline void sdkGlobalArraySetValueColor(const UINT64 hash, const int index, const TUsineColor value,
                                        const AnsiCharPtr errorMsg)
{
    UserModuleBase::m_masterInfo->GlobalArraySetValueColor(hash, index, value, errorMsg);
}

/// Set a string value of a string Global Array at the index
[[maybe_unused]]
inline void sdkGlobalArraySetValueAnsiChar(const UINT64 hash, const int index, const AnsiCharPtr value,
                                           const AnsiCharPtr errorMsg)
{
    UserModuleBase::m_masterInfo->GlobalArraySetValueAnsiChar(hash, index, value, errorMsg);
}

/// Fill an entire global array with the content of an Event
[[maybe_unused]]
inline void sdkGlobalArraySetValueEvt(const UINT64 hash, UsineEventPtr value, const AnsiCharPtr errorMsg)
{
    UserModuleBase::m_masterInfo->GlobalArraySetValueEvt(hash, value, errorMsg);
}

/// Get a float value from a data Global Array at the index
[[maybe_unused]] 
inline TPrecision sdkGlobalArrayGetValueFloat(const UINT64 hash, const int index, const AnsiCharPtr errorMsg)
{
    return UserModuleBase::m_masterInfo->GlobalArrayGetValueFloat(hash, index, errorMsg);
}

/// Get the size of the Global Array
[[maybe_unused]] 
inline TPrecision sdkGlobalArrayGetSize(const UINT64 hash, const AnsiCharPtr errorMsg)
{
    return UserModuleBase::m_masterInfo->GlobalArrayGetSize(hash, errorMsg);
}

/// Get a color value from a color Global Array at the index
[[maybe_unused]] 
inline TUsineColor sdkGlobalArrayGetValueColor(const UINT64 hash, const int index, const AnsiCharPtr errorMsg)
{
    return UserModuleBase::m_masterInfo->GlobalArrayGetValueColor(hash, index, errorMsg);
}

/// get a string value from a string Global Array at the index
[[maybe_unused]] 
inline AnsiCharPtr sdkGlobalArrayGetValueAnsiChar(const UINT64 hash, const int index, const AnsiCharPtr errorMsg)
{
    return UserModuleBase::m_masterInfo->GlobalArrayGetValueAnsiChar(hash, index, errorMsg);
}

/// Get an entire global array and fill its content in an Event
[[maybe_unused]]
inline void sdkGlobalArrayGetValueEvt(const UINT64 hash, const AnsiCharPtr errorMsg, UsineEventPtr result)
{
    return UserModuleBase::m_masterInfo->GlobalArrayGetValueEvt(hash, errorMsg, result);
}

/// Get the list of all available global-array's names as a comma-text. This function can be slow.
[[maybe_unused]] 
inline AnsiCharPtr sdkGlobalArrayGetAllNames()
{
    return UserModuleBase::m_masterInfo->GlobalArrayGetAllNames();
}

/// @}

//----------------------------------------------------------------------------
/// @name Usine Core infos and services
/// @{

/// Get the list of possible usine block size.
[[maybe_unused]] 
inline AnsiCharPtr sdkGetUsineBlockSizeList()
{
    return UserModuleBase::m_masterInfo->BlockSizeList;
}

/// Get the default usine block size.
[[maybe_unused]] 
inline int sdkGetUsineDefaultBlockSizeIndex()
{
    return UserModuleBase::m_masterInfo->BlockSizeDefaultIndex;
}

/// Low level , not for public use
/// Usine audio callback
[[maybe_unused]]
inline void sdkUsineAudioDeviceIOCallback(const float *const *inputChannelData, const int numInputChannels,
                                          float *const *outputChannelData, const int numOutputChannels,
                                          const int numSamples)
{
    UserModuleBase::m_masterInfo->AudioDeviceIOCallback(inputChannelData, numInputChannels, outputChannelData,
                                                        numOutputChannels, numSamples);
}

/// Low level , not for public use
/// Usine  midi callback
[[maybe_unused]]
inline void sdkUsineMidiDeviceCallback(const int deviceID, const double TimeStamp, const TUsineMidiCode midiData)
{
    UserModuleBase::m_masterInfo->MidiDeviceCallback(deviceID, TimeStamp, midiData);
}

/// Low level , not for public use
/// Usine midi sysex callback
[[maybe_unused]]
inline void sdkUsineMidiSysexDeviceCallback(const int deviceID, const double TimeStamp, char **sysEx, const int size)
{
    UserModuleBase::m_masterInfo->MidiSysexDeviceCallback(deviceID, TimeStamp, sysEx, size);
}

/// To get the current Usine block size.
[[maybe_unused]]
inline int sdkGetBlocSize()
{
    return UserModuleBase::m_masterInfo->BlocSize;
}

/// To get the current Usine sample rate
[[maybe_unused]]
inline double sdkGetSampleRate()
{
    return UserModuleBase::m_masterInfo->GetSampleRate();
}

#if defined(USINE_WIN64)

/// PC only, Usine mainform handle (THandle)
[[maybe_unused]]
inline NativeInt sdkGetUsineMainWindow()
{
    return UserModuleBase::m_masterInfo->GetUsineMainWindow();
}


#elif defined(USINE_OSX64)

/// MAC only, Usine NSView
[[maybe_unused]]
inline void *sdkGetUsineNSView()
{
    return UserModuleBase::m_masterInfo->GetMacNSView();
}

/// MAC only, Usine NSApplication
[[maybe_unused]]
inline void *sdkGetUsineNSApplication()
{
    return (void *) UserModuleBase::m_masterInfo->NSApplication;
}

#endif

/// To get a named color from the Usine ColorSet.
/// @param color The color index, starting at 1.
[[maybe_unused]]
inline TUsineColor sdkGetUsineColor(const UINT32 color)
{
    TUsineColor result = 0;
    if (UserModuleBase::m_masterInfo)
        result = UserModuleBase::m_masterInfo->GetUsineColor(color - 1);
    return result;
}

///Translate a StringID to the current Usine language.
/// @param StringID The ID of the string to translate.
/// @return A pointer to the translated string.
[[maybe_unused]]
inline AnsiCharPtr sdkGetTrans(const AnsiCharPtr StringID)
{
    return UserModuleBase::m_masterInfo->GetTranslation(StringID);
}

/// To get the version type of Usine, no longer used in the SDK
[[deprecated("Usine Version is no longer used in the SDK, always returns 0")]]
inline int sdkGetUsineVersionType()
{
    return 0;
}

/// To get the current language used in Usine.
/// @return a string naming the current language (En, Fr...)
[[maybe_unused]]
inline AnsiCharPtr sdkGetUsineLanguage()
{
    return UserModuleBase::m_masterInfo->UsineLanguage;
}

/// To get the of the save format version of Usine.
[[maybe_unused]]
inline int sdkGetUsineSaveVersion()
{
    return UserModuleBase::m_masterInfo->UsineSaveVersion;
}

/// To get the maximum audio inputs Usine can accept.
/// This is a constant defined in UsineDefinitions.h as USINE_MAX_SOUNDCARD_AUDIO_INPUTS.
[[deprecated("Deprecated: use the constant USINE_MAX_SOUNDCARD_AUDIO_INPUTS instead")]]
inline int sdkGetUsineMaxSoundCardAudioInputs()
{
    return USINE_MAX_SOUNDCARD_AUDIO_INPUTS;
}

/// To get the maximum audio outputs Usine can accept.
/// This is a constant defined in UsineDefinitions.h as USINE_MAX_SOUNDCARD_AUDIO_OUTPUTS.
[[deprecated("Deprecated: use the constant USINE_MAX_SOUNDCARD_AUDIO_OUTPUTS instead")]]
inline int sdkGetUsineMaxSoundCardAudioOutputs()
{
    return USINE_MAX_SOUNDCARD_AUDIO_OUTPUTS;
}

/// To get the maximum Midi device Usine can accept.
/// This is a constant defined in UsineDefinitions.h as USINE_MAX_MIDI_DEVICES.
[[deprecated("Deprecated: use the constant USINE_MAX_MIDI_DEVICES instead")]]
inline int sdkGetUsineMaxMidiDevices()
{
    return USINE_MAX_MIDI_DEVICES;
}

/// To get the current polyphony of Usine.
/// This is a constant defined in UsineDefinitions.h as USINE_AUDIO_CHANNEL_MODULE_MAX.
[[deprecated("Deprecated: use the constant USINE_AUDIO_CHANNEL_MODULE_MAX instead")]]
inline int sdkGetUsineMaxPolyphony()
{
    return USINE_AUDIO_CHANNEL_MODULE_MAX;
}

/// @}

//----------------------------------------------------------------------------
/// To get the path to access some important folders of Usine.
/// @name Usine Paths getters
/// @{


/// The Sound folder as configured in the Global setting Tab.
[[maybe_unused]]
inline AnsiCharPtr sdkGetUsineSoundPath()
{
    return UserModuleBase::m_masterInfo->SoundPath;
}

/// The Patch library folder as configured in the Global setting Tab.
[[maybe_unused]]
inline AnsiCharPtr sdkGetUserLibPath()
{
    return UserModuleBase::m_masterInfo->UserLibPath;
}

/// The Record Out folder as configured in the Global setting Tab.
[[maybe_unused]]
inline AnsiCharPtr sdkGetUsineRecordPath()
{
    return UserModuleBase::m_masterInfo->RecordPath;
}

/// The install folder of Usine.
[[maybe_unused]]
inline AnsiCharPtr sdkGetGlobalApplicationPath()
{
    return UserModuleBase::m_masterInfo->GlobalApplicationPath;
}

/// The path of the current workspace.
[[maybe_unused]]
inline AnsiCharPtr sdkGetCurrentWorkspacePath()
{
    return UserModuleBase::m_masterInfo->CurrentWorkspacePath;
}

/// The path of the patch containing the module.
[[maybe_unused]]
inline AnsiCharPtr sdkGetCurrentPatchPath()
{
    return UserModuleBase::m_masterInfo->CurrentPatchPath;
}

/// The Temp folder Used by Usine.
[[maybe_unused]]
inline AnsiCharPtr sdkGetUsineTempPath()
{
    return UserModuleBase::m_masterInfo->UsineTempPath;
}

/// Not for public use.
[[maybe_unused]]
inline AnsiCharPtr sdkGetUsinePlugInsPath()
{
    return UserModuleBase::m_masterInfo->PlugInsPath;
}
/// @}

    //----------------------------------------------------------------------------
/// @name Files utils
/// @{

/// Show a dialog window to open a file.
/// @param filename A pointer to a string that will be filled with the selected file name.
/// @param initialDir The initial directory to show in the dialog.
/// @param filter The filter to apply to the files shown in the dialog (e.g., "*.wav;*.mp3").
/// @return True if the user selected a file, false otherwise.
/// This function is used to open files in Usine, such as audio files, presets, or other data.
[[maybe_unused]]
inline LongBool sdkProcessOpenDialog(AnsiCharPtr *filename, const AnsiCharPtr initialDir, const AnsiCharPtr filter)
{
    return UserModuleBase::m_masterInfo->ProcessOpenDialog(filename, initialDir, filter);
}

/// Show a dialog window to save a file.
/// @param filename A pointer to a string that will be filled with the selected file name.
/// @param initialDir The initial directory to show in the dialog.
/// @param filter The filter to apply to the files shown in the dialog (e.g., "*.wav;*.mp3").
/// @return True if the user selected a file, false otherwise.
/// This function is used to save files in Usine, such as audio files, presets, or other data.
[[maybe_unused]]
inline LongBool sdkProcessSaveDialog(AnsiCharPtr *filename, const AnsiCharPtr initialDir, const AnsiCharPtr filter)
{
    return UserModuleBase::m_masterInfo->ProcessSaveDialog(filename, initialDir, filter);
}

/// Ask Usine to search if a file exist.
/// @param filename A pointer to a string that will be filled with the found file name.
/// @param initialDir The initial directory to start the search.
/// @return A pointer to the found file name, or NULL if not found.
[[maybe_unused]]
inline AnsiCharPtr sdkFindFile(AnsiCharPtr *filename, const AnsiCharPtr initialDir)
{
    return UserModuleBase::m_masterInfo->FindFile(filename, initialDir);
}
/// @}

    //----------------------------------------------------------------------------
/// @name Audio Files manipulation
/// @{

/// Create an audio file handle
/// This function creates a new audio file handle that can be used to manipulate audio data in Usine.
/// It returns a pointer to the created audio file handle.
[[maybe_unused]]
inline TAudioFilePtr sdkCreateAudioFile()
{
    return UserModuleBase::m_masterInfo->CreateAudioFile();
}

/// Destroy an audio file handle
/// This function destroys the audio file handle and frees the associated memory.
/// It should be called when the audio file is no longer needed to avoid memory leaks.
[[maybe_unused]]
inline void sdkDestroyAudioFile(TAudioFilePtr audioFile)
{
    UserModuleBase::m_masterInfo->DestroyAudioFile(audioFile);
}

/// Get a sample value of an audio file at the position pos of the audio channel
/// @return The sample value at the specified position and channel.
/// @param channel The channel number, starting from 1.
/// @param pos The position of the sample in the audio file, starting from 0.
/// This function retrieves a specific sample value from an audio file.
[[maybe_unused]]
inline TPrecision sdkGetSampleAudioFile(TAudioFilePtr audioFile, const int channel, const int pos)
{
    return UserModuleBase::m_masterInfo->GetSampleAudioFile(audioFile, channel, pos);
}

/// Get all samples values (as an array) of an audio file for an audio channel
/// @return A pointer to the array of samples values.
/// @channel The channel number, starting from 1.
/// This function retrieves all sample values from an audio file for a specific audio channel.
[[maybe_unused]]
inline TPrecision *sdkGetSampleArrayAudioFile(TAudioFilePtr audioFile, const int channel)
{
    return UserModuleBase::m_masterInfo->GetSampleArrayAudioFile(audioFile, channel);
}

/// Get a bloc of samples values of an audio file at position pos for an audio channel and store it in an Event
/// @param audioFile The audio file handle.
/// @param channel The channel number, starting from 1.
/// @param pos The position of the sample in the audio file, starting from 0.
/// @param evt The event where the samples will be stored.
/// This function retrieves a bloc of sample values from an audio file for a specific audio channel
[[maybe_unused]]
inline void sdkGetBlocSampleAudioFile(TAudioFilePtr audioFile, const int channel, const int pos,
                                      UsineEventPtr evt)
{
    UserModuleBase::m_masterInfo->GetBlocSampleAudioFile(audioFile, channel, pos, evt);
}

/// Get the numbers of samples of an audio file
/// @return The number of samples in the audio file.
/// This function retrieves the total number of samples in an audio file.
[[maybe_unused]]
inline int sdkGetSizeAudioFile(TAudioFilePtr audioFile)
{
    return UserModuleBase::m_masterInfo->GetSizeAudioFile(audioFile);
}

/// Get the the number of channels of an audio file
/// @return The number of channels in the audio file.
/// This function retrieves the total number of channels in an audio file.
[[maybe_unused]]
inline int sdkGetChannelAudioFile(TAudioFilePtr audioFile)
{
    return UserModuleBase::m_masterInfo->GetChannelAudioFile(audioFile);
}

///Get the samplerate of an audio file
/// @return The sample rate of the audio file.
/// This function retrieves the sample rate of an audio file, which is the number of samples per second.
[[maybe_unused]]
inline int sdkGetSampleRateAudioFile(TAudioFilePtr audioFile)
{
    return UserModuleBase::m_masterInfo->GetSampleRateAudioFile(audioFile);
}

/// Get the bit per sample of an audio file
/// @return The bit depth of the audio file, which indicates the number of bits used to represent each sample.
/// This function retrieves the bit depth of an audio file, which is the number of bits used to represent each sample.
[[maybe_unused]]
inline int sdkGetBitPerSampleAudioFile(TAudioFilePtr audioFile)
{
    return UserModuleBase::m_masterInfo->GetBitPerSampleAudioFile(audioFile);
}

/// Load an audio file in memory
/// @param audioFile The audio file handle.
/// This function loads an audio file into memory, allowing for fast access to its samples.
[[maybe_unused]]
inline void sdkLoadInMemoryAudioFile(TAudioFilePtr audioFile, const AnsiCharPtr name)
{
    UserModuleBase::m_masterInfo->LoadInMemoryAudioFile(audioFile, name);
}

/// Load an audio file but keep it on disk
/// @param audioFile The audio file handle.
/// This function loads an audio file but keeps it on disk, which can be useful for large files that do not need to be fully loaded into memory.
[[maybe_unused]]
inline void sdkLoadStayOnDiskAudioFile(TAudioFilePtr audioFile, const AnsiCharPtr name)
{
    UserModuleBase::m_masterInfo->LoadStayOnDiskAudioFile(audioFile, name);
}

/// Get the peak of the audio files (to draw the wave form) and store it in an Event
/// @param audioFile The audio file handle.
/// @param evt The event where the peaks will be stored.
/// This function retrieves the peaks of an audio file and stores them in an event for further processing or visualization.
[[maybe_unused]]
inline void sdkGetPeaksAudioFile(TAudioFilePtr audioFile, UsineEventPtr evt)
{
    UserModuleBase::m_masterInfo->GetPeaksAudioFile(audioFile, evt);
}

/// Save an audio file to disk
/// @param audioFile The audio file handle.
/// @param name The name of the audio file to save.
/// @param nbChannels The number of channels to save the audio file with.
/// This function saves an audio file to disk, allowing it to be stored and accessed later.
/// It is used to persist audio data in a file format that can be read by Usine or other audio applications.
[[maybe_unused]]
inline void sdkSaveToDiskAudioFile(TAudioFilePtr audioFile, const AnsiCharPtr name, const int nbChannels)
{
    UserModuleBase::m_masterInfo->SaveToDiskAudioFile(audioFile, name, nbChannels);
}

/// Set the number of channels of an audio file
/// @param audioFile The audio file handle.
/// @param nbChannels The number of channels to set for the audio file.
/// This function sets the number of channels for an audio file, allowing it to be configured for mono, stereo, or multi-channel audio.
[[maybe_unused]]
inline void sdkSetChannelsAudioFile(TAudioFilePtr audioFile, const int nbChannels)
{
   UserModuleBase::m_masterInfo->SetChannelsAudioFile(audioFile, nbChannels);
}

/// Set the number of samples of an audio file
/// @param audioFile The audio file handle.
/// @param size The number of samples to set for the audio file.
/// This function sets the size of an audio file, allowing it to be resized or truncated as needed.
[[maybe_unused]]
inline void sdkSetSizeAudioFile(TAudioFilePtr audioFile, const int size)
{
    UserModuleBase::m_masterInfo->SetSizeAudioFile(audioFile, size);
}

/// Set a sample value of an audio file at the position pos of the audio channel
/// @param audioFile The audio file handle.
/// @param channel The channel number, starting from 1.
/// @param pos The position of the sample in the audio file, starting from 0.
/// @param sample The sample value to set at the specified position and channel.
/// This function sets a specific sample value in an audio file, allowing for precise manipulation of audio data.
[[maybe_unused]]
inline void sdkSetSampleAudioFile(TAudioFilePtr audioFile, const int channel, const int pos,
                                  const TPrecision sample)
{
    UserModuleBase::m_masterInfo->SetSampleAudioFile(audioFile, channel, pos, sample);
}

/// Clear an audio file
/// This function clears the audio file, effectively resetting its content.
/// It is used to remove all audio data from the file, allowing it to be reused or repopulated with new audio samples.
/// @param audioFile The audio file handle.
/// This function clears the audio file, effectively resetting its content.
[[maybe_unused]]
inline void sdkClearAudioFile(TAudioFilePtr audioFile)
{
    UserModuleBase::m_masterInfo->ClearAudioFile(audioFile);
}

/// Resample (stretch) an audio file by a factor
/// @param audioFile The audio file handle.
/// @param factor The resampling factor, where 1.0 is the original speed, less than 1.0 is slower, and greater than 1.0 is faster.
/// This function resamples an audio file by a specified factor, allowing for time-stretching or compression of the audio data.
[[maybe_unused]]
inline void sdkResampleAudioFile(TAudioFilePtr audioFile, const TPrecision factor)
{
    UserModuleBase::m_masterInfo->ResampleAudioFile(audioFile, factor);
}
/// @}

//----------------------------------------------------------------------------
/// @name Math utils
/// @{

/// Linear interpolation  on a TPrecision value.
/// @param f The interpolation factor, typically between 0.0 and 1.0.
/// @param a The value at the start of the interpolation range.
/// @param b The value at the end of the interpolation range.
/// @return The interpolated value.
/// This function performs linear interpolation between two values a and b based on the factor f.
[[maybe_unused]]
inline TPrecision sdkLinearInterpolation(const TPrecision f, const TPrecision a, const TPrecision b)
{
    return UserModuleBase::m_masterInfo->LinearInterpolation(f, a, b);
}

/// Cubic interpolation  on a TPrecision value.
/// @param fr The interpolation factor, typically between 0.0 and 1.0.
/// @param inm1 The value before the start of the interpolation range.
/// @param inp The value at the start of the interpolation range.
/// @param inp1 The value at the end of the interpolation range.
/// @param inp2 The value after the end of the interpolation range.
/// @return The interpolated value.
/// This function performs cubic interpolation between five values: inm1, inp, inp1, and inp2 based on the factor fr.
[[maybe_unused]]
inline TPrecision
sdkCubicInterpolation(const TPrecision fr, const TPrecision inm1, const TPrecision inp, const TPrecision inp1,
                      const TPrecision inp2)
{
    return UserModuleBase::m_masterInfo->CubicInterpolation(fr, inm1, inp, inp1, inp2);
}

/// Spline interpolation on a TPrecision value.
/// @param fr The interpolation factor, typically between 0.0 and 1.0.
/// @param inm1 The value before the start of the interpolation range.
/// @param inp The value at the start of the interpolation range.
/// @param inp1 The value at the end of the interpolation range.
/// @param inp2 The value after the end of the interpolation range.
/// This function performs spline interpolation between five values: inm1, inp, inp1, and inp2 based on the factor fr.
/// It is used to create smooth transitions between values in a way that is more flexible than linear or cubic interpolation.
/// @return The interpolated value.
[[maybe_unused]]
inline TPrecision
sdkSplineInterpolation(const TPrecision fr, const TPrecision inm1, const TPrecision inp, const TPrecision inp1,
                       const TPrecision inp2)
{
    return UserModuleBase::m_masterInfo->SplineInterpolation(fr, inm1, inp, inp1, inp2);
}
/// @}

//----------------------------------------------------------------------------
/// @name Trace functions
/// @{

/// Show a string in the Trace Panel.
/// @param s The string to show in the Trace Panel.
/// This function is used to display debug information, messages, or other text in the Usine Trace Panel.
[[maybe_unused]]
inline void sdkTraceChar(const AnsiCharPtr s)
{
    UserModuleBase::m_masterInfo->TraceChar(s);
}

[[maybe_unused]]
/// Show a string in the Trace Panel.
/// @param s The string to show in the Trace Panel.
/// This function is used to display debug information, messages, or other text in the Usine Trace Panel.
inline void sdkTraceChar(const std::string &s)
{
    UserModuleBase::m_masterInfo->TraceChar(s.c_str());
}

/// Show an integer in the Trace Panel.
/// @param i The integer to show in the Trace Panel.
/// This function is used to display integer values in the Usine Trace Panel, which can be useful for debugging or monitoring purposes.
/// It is typically used to log numerical data, counters, or other integer-based information during the execution of a Usine module.
[[maybe_unused]]
inline void sdkTraceInt(const int i)
{
    UserModuleBase::m_masterInfo->TraceInteger(i);
}

/// Show a decimal number in the Trace Panel.
/// @param f The decimal number to show in the Trace Panel.
/// This function is used to display floating-point values in the Usine Trace Panel, which can be useful for debugging or monitoring purposes.
[[maybe_unused]]
inline void sdkTracePrecision(const TPrecision f)
{
    UserModuleBase::m_masterInfo->TracePrecision(f);
}

/// Show a string in the Trace Panel with the error tag.
/// @param s The string to show in the Trace Panel.
/// This function is used to display error messages or critical information in the Usine Trace Panel.
[[maybe_unused]]
inline void sdkTraceErrorChar(const AnsiCharPtr s, const LongBool namePrefix = TRUE)
{
    std::stringstream traceString;
    if (UserModuleBase::m_masterInfo)
    {
        if (namePrefix)
        {
            traceString << "[" << UserModuleBase::MODULE_NAME << "]" << " " << s;
        }
        else
        {
            traceString << s;
        }
        UserModuleBase::m_masterInfo->TraceErrorChar(static_cast<AnsiCharPtr>(traceString.str().c_str()));
    }
}

[[maybe_unused]]
/// Show a string in the Trace Panel with the error tag.
/// @param s The string to show in the Trace Panel.
/// This function is used to display error messages or critical information in the Usine Trace Panel.
inline void sdkTraceErrorChar(const std::string& s)
{
    if (UserModuleBase::m_masterInfo) {
        std::stringstream traceString;
        traceString
            << "[" << UserModuleBase::MODULE_NAME << "]"
            << " " << s;
        UserModuleBase::m_masterInfo->TraceErrorChar(static_cast<AnsiCharPtr>(traceString.str().c_str()));
    }
}
    

/// Show a string in the Trace Panel with the warning tag.
/// @param s The string to show in the Trace Panel.
/// This function is used to display warning messages or non-critical information in the Usine Trace Panel.
[[maybe_unused]]
inline void sdkTraceWarningChar(const AnsiCharPtr s)
{
    if (UserModuleBase::m_masterInfo) {
        std::stringstream traceString;
        traceString
        << "[" << UserModuleBase::MODULE_NAME << "]"
        << " " << s;
        UserModuleBase::m_masterInfo->TraceWarningChar(static_cast<AnsiCharPtr>(traceString.str().c_str()));
    }
}

[[maybe_unused]]
/// Show a string in the Trace Panel with the warning tag.
/// @param s The string to show in the Trace Panel.
/// This function is used to display warning messages or non-critical information in the Usine Trace Panel.
inline void sdkTraceWarningChar(const std::string &s)
{
    if (UserModuleBase::m_masterInfo) {
        std::stringstream traceString;
        traceString
        << "[" << UserModuleBase::MODULE_NAME << "]"
        << " " << s;
        UserModuleBase::m_masterInfo->TraceWarningChar(static_cast<AnsiCharPtr>(traceString.str().c_str()));
    }
}


/// Show a string in the SplashForm.
/// @param s The string to show in the SplashForm.
/// This function is used to display messages in the Usine SplashForm, which is typically shown during the startup of Usine or when loading patches.
[[maybe_unused]]
inline void sdkTraceSplashChar(const AnsiCharPtr s, const LongBool autoClose)
{
    UserModuleBase::m_masterInfo->TraceSplashChar(s, autoClose);
}

[[maybe_unused]]
/// Show a string in the SplashForm.
/// @param s The string to show in the SplashForm.
/// This function is used to display messages in the Usine SplashForm, which is typically shown during the startup of Usine or when loading patches.
inline void sdkTraceSplashChar(const std::string &s, const LongBool autoClose)
{
    UserModuleBase::m_masterInfo->TraceSplashChar(s.c_str(), autoClose);
}

/// Print a string in the LogUsine.log file. Optionally show it in the SplashForm.
/// @param s The string to print in the LogUsine.log file.
/// @param showInSplashForm If TRUE, the string will also be shown in the SplashForm.
/// This function is used to log messages in the LogUsine.log file, which is a log file used by Usine to record various events, errors, and information during its operation.
/// It can be useful for debugging, monitoring, or auditing purposes.
[[maybe_unused]]
inline void sdkTraceLogChar(const AnsiCharPtr s, const LongBool showInSplashForm = FALSE)
{
    if (UserModuleBase::m_masterInfo) {
        std::stringstream traceString;
        traceString
        << "  [" << UserModuleBase::MODULE_NAME << "]"
        << " " << s;
        UserModuleBase::m_masterInfo->TraceLogChar((AnsiCharPtr) traceString.str().c_str(), showInSplashForm);
    }
}

/// Print a string in the LogUsine.log file. Optionally show it in the SplashForm.
/// @param s The string to print in the LogUsine.log file.
/// @param showInSplashForm If TRUE, the string will also be shown in the SplashForm.
/// This function is used to log messages in the LogUsine.log file, which is a log file used by Usine to record various events, errors, and information during its operation.
/// It can be useful for debugging, monitoring, or auditing purposes.
[[maybe_unused]]
inline void sdkTraceLogChar(const std::string &s, const LongBool showInSplashForm = FALSE)
{
    if (UserModuleBase::m_masterInfo) {
        std::stringstream traceString;
        traceString
        << "  [" << UserModuleBase::MODULE_NAME << "]"
        << " " << s;
        UserModuleBase::m_masterInfo->TraceLogChar((AnsiCharPtr) traceString.str().c_str(), showInSplashForm);
    }
}

/// Print a string and a value in the LogUsine.log file. Optionally show it in the SplashForm.
/// @param traceMsg The message to print in the LogUsine.log file.
/// @param value The value to print in the LogUsine.log file.
/// @param showInSplashForm If TRUE, the string will also be shown in the SplashForm.
/// This function is used to log messages in the LogUsine.log file, which is a log file used by Usine to record various events, errors, and information during its operation.
/// It can be useful for debugging, monitoring, or auditing purposes.
[[maybe_unused]]
inline void
sdkTraceLogValueChar(const std::string &traceMsg, const std::string &value, const LongBool showInSplashForm = FALSE)
{
    if (UserModuleBase::m_masterInfo) {
        std::stringstream traceString;
        traceString
        << "  [" << UserModuleBase::MODULE_NAME << "]"
        << " " << traceMsg << " " << value;
        UserModuleBase::m_masterInfo->TraceLogChar((AnsiCharPtr) traceString.str().c_str(), showInSplashForm);
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

[[maybe_unused]]
inline int32_t sdkDialogConfirmationYesNoCancel(const AnsiCharPtr msg)
{
    return UserModuleBase::m_masterInfo->DialogConfirmationYesNoCancel(msg);
}

/// A dialog popup with Yes and No buttons.
/// @param msg The message to show on the popup popup.
/// @return The choice of the user.
/// @see TDialogsResults
[[maybe_unused]]
inline int32_t sdkDialogConfirmationYesNo(const AnsiCharPtr msg)
{
    return UserModuleBase::m_masterInfo->DialogConfirmationYesNo(msg);
}

/// A dialog popup with OK button.
/// @param msg The message to show on the popup popup.
/// @return The choice of the user.
/// @see TDialogsResults
[[maybe_unused]]
inline int32_t sdkDialogInformationOk(const AnsiCharPtr msg)
{
    return UserModuleBase::m_masterInfo->DialogInformationOk(msg);
}

/// A dialog popup with OK and Cancel buttons.
/// @param msg The message to show on the popup popup.
/// @return The choice of the user.
/// @see TDialogsResults
[[maybe_unused]]
inline int32_t sdkDialogConfirmationOKCancel(const AnsiCharPtr msg)
{
    return UserModuleBase::m_masterInfo->DialogConfirmationOKCancel(msg);
}

/// A dialog popup with a string input box .
/// @return the edited string.
/// @see TDialogsResults
[[maybe_unused]]
inline AnsiCharPtr sdkDialogInputBox(const AnsiCharPtr caption, const AnsiCharPtr prompt,
                                     const AnsiCharPtr defaultValue)
{
    return UserModuleBase::m_masterInfo->DialogInputBox(caption, prompt, defaultValue);
}

/// @}

//----------------------------------------------------------------------------
/// @name Deprecated Event manipulation. Use UsineEvent class instead
/// @{


/// Compare Events, returns TRUE if e1 and e2 are equal.
/// @deprecated Deprecated: use @ref UsineEventClass::compare instead
[[maybe_unused]] [[deprecated("Deprecated: use UsineEventClass::compare instead")]]
inline bool sdkCompareEvt(UsineEventPtr e1, UsineEventPtr e2)
{
    return UserModuleBase::m_masterInfo->CompareEvt(e1, e2);
}

/// Copy Events, copies src to dest.
/// @deprecated Deprecated: use @ref UsineEventClass functions instead
[[maybe_unused]] [[deprecated("Deprecated: use UsineEventClass::copy instead")]]
inline void sdkCopyEvt(UsineEventPtr scr, UsineEventPtr dest)
{
    UserModuleBase::m_masterInfo->CopyEvt(scr, dest);
}

/// Event Concatenation dest is the concatenation of e1 and e2.
/// @deprecated Deprecated: use @ref UsineEventClass functions instead
[[maybe_unused]]
inline void sdkConcatEvt(UsineEventPtr e1, UsineEventPtr e2, UsineEventPtr dest)
{
    UserModuleBase::m_masterInfo->ConcatEvt(e1, e2, dest);
}

/// Shift the values of src and store the result into dest.
/// Right shift for n>0, left shift for n<0.
/// @deprecated Deprecated: use @ref UsineEventClass functions instead
[[maybe_unused]]
inline void sdkShiftEvt(const int n, UsineEventPtr src, UsineEventPtr dest)
{
    UserModuleBase::m_masterInfo->ShiftEvt(n, src, dest);
}

/// Set the size of the event (number of elements). Typically the size of audio events is given by sdkGetBlocSize.
/// @see sdkGetBlocSize
/// @deprecated Deprecated: use @ref UsineEventClass functions instead
[[maybe_unused]] [[deprecated("Deprecated: use UsineEventClass::setSize instead")]]
inline void sdkSetEvtSize(UsineEventPtr e, const int size)
{
    UserModuleBase::m_masterInfo->SetEvtSize(e, size);
}

/// Get the size of the event (number of elements).
/// @deprecated Deprecated: use @ref UsineEventClass functions instead
[[maybe_unused]] [[deprecated("Deprecated: use UsineEventClass::getSize instead")]]
inline int sdkGetEvtSize(UsineEventPtr e)
{
    return UserModuleBase::m_masterInfo->GetEvtSize(e);
}

/// Set the maximum size allowed by the event. Can be greater than the event size.
/// @deprecated Deprecated: use @ref UsineEventClass functions instead
[[maybe_unused]] [[deprecated("Deprecated: use UsineEventClass::setMaxSize instead")]]
inline void sdkSetEvtMaxSize(UsineEventPtr e, const int size)
{
    UserModuleBase::m_masterInfo->SetEvtMaxSize(e, size);
}

/// Destroy the event.
/// @deprecated Deprecated: use @ref UsineEventClass functions instead
[[maybe_unused]] [[deprecated("Deprecated: use UsineEventClass::destroy instead")]]
inline void sdkDestroyEvt(UsineEventPtr &e)
{
    UserModuleBase::m_masterInfo->DestroyEvt(e);
}

/// Create an event with an original size.
/// @deprecated Deprecated: use @ref UsineEventClass functions instead
[[maybe_unused]] [[deprecated("Deprecated: use UsineEventClass::create instead")]]
inline void sdkCreateEvt(UsineEventPtr &e, const int originalSize)
{
    UserModuleBase::m_masterInfo->CreateEvt(e, originalSize);
}

/// Copy a 32Bits bloc of memory. The size is the number of 32bits elements.
/// @param pointerSrc The source pointer to the bloc of memory.
/// @param pointerDest The destination pointer to the bloc of memory.
/// @param size The size of the bloc in 32bits elements.
/// This function is used to copy a bloc of memory from one location to another.
[[maybe_unused]]
inline void sdkMoveBLOC32(TPrecision *pointerSrc, TPrecision *pointerDest, const int size)
{
    UserModuleBase::m_masterInfo->MoveBLOC32(pointerSrc, pointerDest, size);
}

/// Add values of two events and store the result into dest.
/// @deprecated Deprecated: use UsineEventClass::add instead
/// This function adds the values of two events (e1 and e2) and stores the result in a third event (dest).
[[maybe_unused]]
inline void sdkAddEvt3(UsineEventPtr e1, UsineEventPtr e2, UsineEventPtr dest)
{
    UserModuleBase::m_masterInfo->AddEvt3(e1, e2, dest);
}

/// Add values of two events and store the result into e2.
/// @deprecated Deprecated: use UsineEventClass::add instead
/// This function adds the values of e1 and e2 and stores the result in e2.
[[maybe_unused]]
[[deprecated("Deprecated: use UsineEventClass::add instead")]]
inline void sdkAddEvt2(UsineEventPtr e1, UsineEventPtr e2)
{
    UserModuleBase::m_masterInfo->AddEvt2(e1, e2);
}

/// Subtract values of two events and store the result into dest.
/// @deprecated Deprecated: use UsineEventClass::sub instead
/// This function subtracts the values of e2 from e1 and stores the result in a third event (dest).
[[maybe_unused]]
inline void sdkSubEvt3(UsineEventPtr e1, UsineEventPtr e2, UsineEventPtr dest)
{
    UserModuleBase::m_masterInfo->SubEvt3(e1, e2, dest);
}

/// Subtract values of two events and store the result into e2.
/// @deprecated Deprecated: use UsineEventClass::sub instead
/// This function subtracts the values of e2 from e1 and stores the result in e2.
[[maybe_unused]]
[[deprecated("Deprecated: use UsineEventClass::sub instead")]]
inline void sdkSubEvt2(UsineEventPtr e1, UsineEventPtr e2)
{
    UserModuleBase::m_masterInfo->SubEvt2(e1, e2);
}

/// Divide values of e1 by values of e2 and store the result into dest.
/// @deprecated Deprecated: use UsineEventClass::div instead
/// This function divides the values of two events (e1 and e2) and stores the result in a third event (dest).
[[maybe_unused]]
inline void sdkDivEvt3(UsineEventPtr e1, UsineEventPtr e2,
                       UsineEventPtr dest)
{
    UserModuleBase::m_masterInfo->DivEvt3(e1, e2, dest);
}


/// Multiply values of e1 by values of e2 and store the result into dest.
/// @deprecated Deprecated: use UsineEventClass::mult instead
/// This function multiplies the values of two events (e1 and e2) and stores the result in a third event (dest).
[[maybe_unused]]
inline void sdkMultEvt3(UsineEventPtr e1, UsineEventPtr e2, UsineEventPtr dest)
{
    UserModuleBase::m_masterInfo->MultEvt3(e1, e2, dest);
}

/// Multiply values of e1 by values of e2 and store the result into e2.
/// @deprecated Deprecated: use UsineEventClass::mult instead
/// This function multiplies the values of e1 by the values of e2 and stores the result in e2.
[[maybe_unused]] [[deprecated("Deprecated: use UsineEventClass::mult instead")]]
inline void sdkMultEvt2(UsineEventPtr e1, UsineEventPtr e2)
{
    UserModuleBase::m_masterInfo->MultEvt2(e1, e2);
}

/// Multiply values of e1 by values of e2 and store the result into e2. Same as sdkMultEvt2.
/// @deprecated Deprecated: use UsineEventClass::multAudio instead
/// This function multiplies the values of e1 by the values of e2 and stores the result in e2. It is specifically used for audio events.
[[maybe_unused]] [[deprecated("Deprecated: use UsineEventClass::multAudio instead")]]
inline void sdkMultEvt2Audio(UsineEventPtr e1, UsineEventPtr e2)
{
    UserModuleBase::m_masterInfo->MultEvt2(e1, e2);
}

/// Multiply values of e by a float value inplace.
/// @param m The multiplier value.
/// @param e The event to multiply.
/// @deprecated Deprecated: use UsineEventClass::mult instead
/// This function multiplies the values of an event (e) by a float value (m) and modifies the event in place.
[[maybe_unused]] [[deprecated("Deprecated: use UsineEventClass::mult instead")]]
inline void sdkMultEvt1(const TPrecision m, UsineEventPtr e)
{
    UserModuleBase::m_masterInfo->MultEvt1(m, e);
}

/// Modulo operation of e1 by values of e2 and store the result into dest.
/// @deprecated Deprecated: use UsineEventClass::mod instead
/// This function performs a modulo operation on the values of two events (e1 and e2) and stores the result in a third event (dest).
[[maybe_unused]]
inline void sdkModEvt3(UsineEventPtr e1, UsineEventPtr e2, UsineEventPtr dest)
{
    UserModuleBase::m_masterInfo->ModEvt3(e1, e2, dest);
}

/// Power values of e1 by values of e2 and store the result into dest.
/// @deprecated Deprecated: use UsineEventClass::power instead
/// This function raises the values of e1 to the power of the corresponding values in e2 and stores the result in a third event (dest).
[[maybe_unused]]
inline void sdkPowerEvt3(UsineEventPtr e1, UsineEventPtr e2, UsineEventPtr dest)
{
    UserModuleBase::m_masterInfo->PowerEvt3(e1, e2, dest);
}

/// Exponential of e inplace.
/// @deprecated Deprecated: use UsineEventClass::exp instead
/// This function calculates the exponential of each value in the event (e) and modifies the event in place.
[[maybe_unused]] [[deprecated("Deprecated: use UsineEventClass::exp instead")]]
[[deprecated("Deprecated: use UsineEventClass::exp instead")]]
inline void sdkExpEvt1(UsineEventPtr e)
{
    UserModuleBase::m_masterInfo->ExpEvt1(e);
}

/// Square root of e inplace.
/// @deprecated Deprecated: use UsineEventClass::sqrt instead
/// This function calculates the square root of each value in the event (e) and modifies the event in place.
[[maybe_unused]] [[deprecated("Deprecated: use UsineEventClass::sqrt instead")]]
[[deprecated("Deprecated: use UsineEventClass::sqrt instead")]]
inline void sdkSqrtEvt1(UsineEventPtr in1)
{
    UserModuleBase::m_masterInfo->SqrtEvt1(in1);
}

/// Returns the maximum value of e.
/// @deprecated Deprecated: use UsineEventClass::max instead
/// This function retrieves the maximum value from an event (e).
[[deprecated("Deprecated: use UsineEventClass::max instead")]]
inline TPrecision sdkMaxEvt1(UsineEventPtr e)
{
    return UserModuleBase::m_masterInfo->MaxEvt1(e);
}

/// Returns the minimum value of e.
/// @deprecated Deprecated: use UsineEventClass::min instead
/// This function retrieves the minimum value from an event (e).
[[deprecated("Deprecated: use UsineEventClass::min instead")]]
inline TPrecision sdkMinEvt1(UsineEventPtr e)
{
    return UserModuleBase::m_masterInfo->MinEvt1(e);
}

/// calculates the maximum of each element and store the result into dest. dest[i] = max(e1[i],e2[i])
/// @deprecated Deprecated: use UsineEventClass::max instead
/// This function calculates the maximum of corresponding elements from two events (e1 and e2) and stores the result in a third event (dest).
[[maybe_unused]]
inline void sdkMaxEvt3(UsineEventPtr e1, UsineEventPtr e2, UsineEventPtr dest)
{
    UserModuleBase::m_masterInfo->MaxEvt3(e1, e2, dest);
}

/// calculates the minimum of each element and store the result into dest. dest[i] = min(e1[i],e2[i])
/// @deprecated Deprecated: use UsineEventClass::min instead
/// This function calculates the minimum of corresponding elements from two events (e1 and e2) and stores the result in a third event (dest).
[[maybe_unused]]
inline void sdkMinEvt3(UsineEventPtr in1, UsineEventPtr in2, UsineEventPtr out)
{
    UserModuleBase::m_masterInfo->MinEvt3(in1, in2, out);
}

/// Clip values of e in the range [min..max] and store the result into dest.
/// @deprecated Deprecated: use UsineEventClass::tresh instead
/// This function clips the values of an event (e) to a specified range [min, max] and stores the result in a destination event (dest).
[[maybe_unused]] [[deprecated("Deprecated: use UsineEventClass::tresh instead")]]
inline void sdkThreshEvt2(UsineEventPtr e, UsineEventPtr dest, TPrecision min, TPrecision max)
{
    UserModuleBase::m_masterInfo->ThreshEvt2(e, dest, min, max);
}

/// Clip values of e in the range [min..max] inplace.
/// @deprecated Deprecated: use UsineEventClass::tresh instead
/// This function clips the values of an event (e) to a specified range [min, max] and modifies the event in place.
[[maybe_unused]] [[deprecated("Deprecated: use UsineEventClass::tresh instead")]]
inline void sdkThreshEvt1(UsineEventPtr e, TPrecision min, TPrecision max)
{
    UserModuleBase::m_masterInfo->ThreshEvt1(e, min, max);
}

/// Mix MIDI values of e1 and e2 and store the result in dest. For the MIDI, this operation equivalent to sdkConcatEvt
/// @deprecated Deprecated: use UsineEventClass::mixMidi instead
/// Mix MIDI values of e1 and e2 and store the result in dest. For the MIDI, this operation equivalent to sdkConcatEvt
[[maybe_unused]] [[deprecated("Deprecated: use UsineEventClass::mixMidi instead")]]
inline void sdkMixMidiEvt3(UsineEventPtr e1, UsineEventPtr e2, UsineEventPtr dest)
{
    UserModuleBase::m_masterInfo->MixMidiEvt3(e1, e2, dest);
}

/// Mix MIDI values of e1 and e2 and store the result in e2. For the MIDI, this operation is equivalent to sdkConcatEvt
/// @deprecated Deprecated: use UsineEventClass::mixMidi instead
[[maybe_unused]] [[deprecated("Deprecated: use UsineEventClass::mixMidi instead")]]
inline void sdkMixMidiEvt2(UsineEventPtr e1, UsineEventPtr e2)
{
    UserModuleBase::m_masterInfo->MixMidiEvt2(e1, e2);
}

/// Apply a short fadein to the audio event. The duration of the fade is given by sdkGetBlocSize.
/// @deprecated Deprecated: use UsineEventClass::fadeIn instead
/// @param e The audio event to fade in.
/// This function applies a short fade-in effect to an audio event, allowing for a smooth transition from silence to the audio content.
[[maybe_unused]] [[deprecated("Deprecated: use UsineEventClass::fadeIn instead")]]
inline void sdkFadeInAudioEvt(UsineEventPtr e)
{
    UserModuleBase::m_masterInfo->FadeInAudioEvt(e);
}

/// Apply a short fadeout to the audio event. The duration of the fade is given by sdkGetBlocSize.
/// @deprecated Deprecated: use UsineEventClass::fadeOut instead
/// @param e The audio event to fade out.
/// This function applies a short fade-out effect to an audio event, allowing for a smooth transition from the audio content to silence.
[[maybe_unused]] [[deprecated("Deprecated: use UsineEventClass::fadeOut instead")]]
inline void sdkFadeOutAudioEvt(UsineEventPtr e)
{
    UserModuleBase::m_masterInfo->FadeOutAudioEvt(e);
}

/// Clear an audio event. Set values to zero and add a small denormalization value.
/// @deprecated Deprecated: use UsineEventClass::clearAudio
/// @param e The audio event to clear.
/// This function clears the audio event by setting its values to zero and adding a small denormalization value to prevent denormalization issues in audio processing.
[[maybe_unused]] [[deprecated("Deprecated: use UsineEventClass::clearAudio instead")]]
inline void sdkClearAudioEvt(UsineEventPtr e)
{
    UserModuleBase::m_masterInfo->ClearAudioEvt(e);
}

/// Add a small denormalization value to an audio event.
/// @deprecated Deprecated: use UsineEventClass::denormalizeAudio instead
/// @param e The audio event to denormalize.
/// This function adds a small denormalization value to an audio event to prevent denormalization issues in audio processing. Denormalization can occur when processing very small floating-point numbers, leading to performance issues in audio applications.
[[maybe_unused]] [[deprecated("Deprecated: use UsineEventClass::denormalizeAudio instead")]]
inline void sdkDenormalizeAudioEvt(UsineEventPtr e)
{
    UserModuleBase::m_masterInfo->DenormalizeAudioEvt(e);
}

/// Smooth event.
/// @deprecated Deprecated: use UsineEventClass::smooth instead
/// Smooth the values of an event with a factor.
/// @param oldValue The value to smooth.
/// @param e The event to smooth.
/// @param factor The smoothing factor, typically between 0.0 and 1.0. @see SmoothFactor
/// This function smooths the values of an event (e) by applying a smoothing factor to the old value. It is typically used to create a gradual transition between values in an event, such as audio or control data.    
[[maybe_unused]] [[deprecated("Deprecated: use UsineEventClass::smooth instead")]]
inline void sdkSmoothPrecision(TPrecision &oldValue, UsineEventPtr e, const TPrecision factor)
{
    return UserModuleBase::m_masterInfo->SmoothPrecision(oldValue, e, factor);
}

/// Smooth Event with a target value.
/// @deprecated Deprecated: use UsineEventClass::smooth instead
/// Smooth the values of an event with a target value and a factor.
/// @param oldValue The value to smooth.
/// @param e The event to smooth.
/// @param target The target value to smooth towards.
/// @param factor The smoothing factor, typically between 0.0 and 1.0. @see SmoothFactor
/// This function smooths the values of an event (e) by applying a smoothing factor to the old value and moving towards a target value. It is typically used to create a gradual transition between values in an event, such as audio or control data.
[[maybe_unused]] [[deprecated("Deprecated: use UsineEventClass::smooth instead")]]
inline void sdkSmoothEvent(TPrecision &oldValue, UsineEventPtr e, const TPrecision target,
                           const TPrecision factor)
{
    return UserModuleBase::m_masterInfo->SmoothEvent(oldValue, e, target, factor);
}


/// Set a Text value.
/// @deprecated Deprecated: use UsineEventClass::setPChar instead
/// @param e The event to set the text value.
/// @param textValue The text value to set in the event.
[[maybe_unused]] [[deprecated("Deprecated: use UsineEventClass::setPChar instead")]]
inline void sdkSetEvtPChar(UsineEventPtr e, AnsiCharPtr textValue)
{
    UserModuleBase::m_masterInfo->SetEvtPChar(e, textValue);
}

/// Get a Text value.
/// @deprecated Deprecated: use UsineEventClass::getPChar instead
/// @param e The event to get the text value from.
/// @return The text value of the event.
/// This function retrieves the text value from an event (e). It is typically used to access string data stored in an event, such as labels, names, or other textual information.
[[maybe_unused]] [[deprecated("Deprecated: use UsineEventClass::getPChar instead")]]
inline AnsiCharPtr sdkGetEvtPChar(UsineEventPtr e)
{
    return UserModuleBase::m_masterInfo->GetEvtPChar(e);
}

/// Get the address of the first data in the Event
/// @deprecated Deprecated: use UsineEventClass::getDataAddr instead
/// @param ev The event to get the data address from.
/// @return A pointer to the first data in the event.
[[maybe_unused]] [[deprecated("Deprecated: use UsineEventClass::getDataAddr instead")]]
inline TPrecision *sdkGetEvtDataAddr(UsineEventPtr ev)
{
    return UserModuleBase::m_masterInfo->GetEvtDataAddr(ev);
}

/// Set a float value
/// @deprecated Deprecated: use UsineEventClass::setData instead
/// @param e The event to set the float value.
/// @param floatValue The float value to set in the event.
/// This function sets a float value in an event (e). It is typically used to store numerical data in an event, such as control values, parameters, or other floating-point information.
[[maybe_unused]] [[deprecated("Deprecated: use UsineEventClass::setData instead")]]
inline void sdkSetEvtData(UsineEventPtr e, const TPrecision floatValue)
{
    UserModuleBase::m_masterInfo->SetEvtData(e, floatValue);
}

/// Get a float Value
/// @deprecated Deprecated: use UsineEventClass::getData instead
/// @param e The event to get the float value from.
/// @return The float value of the event.
/// This function retrieves a float value from an event (e). It is typically used to access numerical data stored in an event, such as control values, parameters, or other floating-point information.
[[maybe_unused]] [[deprecated("Deprecated: use UsineEventClass::getData instead")]]
inline TPrecision sdkGetEvtData(UsineEventPtr e)
{
    return UserModuleBase::m_masterInfo->GetEvtData(e);
}

/// Set a value to an Array at an index position.
/// @deprecated Deprecated: use UsineEventClass::setArrayData instead
/// @param e The event to set the array data.
/// @param index The index position in the array where the value will be set.
/// @param floatValue The float value to set in the array at the specified index.
/// This function sets a value in an array stored within an event (e) at a specific index position. It is typically used to manipulate array data within an event, such as control values, parameters, or other floating-point information stored in an array format.
[[maybe_unused]] [[deprecated("Deprecated: use UsineEventClass::setArrayData instead")]]
inline void sdkSetEvtArrayData(UsineEventPtr e, const int index, const TPrecision floatValue)
{
    UserModuleBase::m_masterInfo->SetEvtArrayData(e, index, floatValue);
}

/// Push (append) a value to an Array.
/// @deprecated Deprecated: use UsineEventClass::pushArrayData instead
/// @param e The event to which the value will be pushed.
/// @param floatValue The float value to push to the array.
/// This function appends a value to an array stored within an event (e). It is typically used to dynamically add values to an array, such as control values, parameters, or other floating-point information stored in an array format.
[[maybe_unused]] [[deprecated("Deprecated: use UsineEventClass::pushArrayData instead")]]
inline void sdkPushEvtArrayData(UsineEventPtr e, const TPrecision floatValue)
{
    int index = UserModuleBase::m_masterInfo->GetEvtSize(e);
    if (index >= UserModuleBase::m_masterInfo->GetEvtMaxSize(e)) {
        UserModuleBase::m_masterInfo->SetEvtMaxSize(e, index + PUSH_GRANULARITY);
    }
    UserModuleBase::m_masterInfo->SetEvtSize(e, index + 1);
    UserModuleBase::m_masterInfo->SetEvtArrayData(e, index, floatValue);
}

/// Get a value from an Array at an index position.
/// @deprecated Deprecated: use UsineEventClass::getArrayData instead
/// @param e The event to get the array data from.
/// @param index The index position in the array from which the value will be retrieved.
/// @return The float value at the specified index in the array.
/// This function retrieves a value from an array stored within an event (e) at a specific index position. It is typically used to access array data within an event, such as control values, parameters, or other floating-point information stored in an array format.
[[maybe_unused]] [[deprecated("Deprecated: use UsineEventClass::getArrayData instead")]]
inline TPrecision sdkGetEvtArrayData(UsineEventPtr e, const int index)
{
    return UserModuleBase::m_masterInfo->GetEvtArrayData(e, index);
}

/// Set a pointer value.
/// @deprecated Deprecated: use UsineEventClass::setEventPointer instead
/// @param e The event to set the pointer value.
/// @param pointerValue The pointer value to set in the event.
/// This function sets a pointer value in an event (e). It is typically used to store references to data structures or objects that can be accessed later, such as custom data types, buffers, or other resources.
[[maybe_unused]] [[deprecated("Deprecated: use UsineEventClass::setEventPointer instead")]]
inline void sdkSetEvtPointer(UsineEventPtr e, void *pointerValue)
{
    UserModuleBase::m_masterInfo->SetEvtPointer(e, pointerValue);
}

/// Get a pointer value.
/// @deprecated Deprecated: use UsineEventClass::getEventPointer instead
/// @param e The event to get the pointer value from.
/// @return The pointer value of the event.
/// This function retrieves a pointer value from an event (e). It is typically used to access references to data structures or objects that were previously stored in the event, such as custom data types, buffers, or other resources.
[[maybe_unused]] [[deprecated("Deprecated: use UsineEventClass::getEventPointer instead")]]
inline void *sdkGetEvtPointer(UsineEventPtr e)
{
    return UserModuleBase::m_masterInfo->GetEvtPointer(e);
}

/// Set a color value.
/// @deprecated Deprecated: use UsineEventClass::setColor instead
/// @param e The event to set the color value.
/// @param colorValue The color value to set in the event.
/// This function sets a color value in an event (e). It is typically used to store color information, such as RGB or RGBA values, which can be used for visual representation or styling within the Usine environment.
[[maybe_unused]] [[deprecated("Deprecated: use UsineEventClass::setColor instead")]]
inline void sdkSetEvtColor(UsineEventPtr e, const TUsineColor colorValue)
{
    UserModuleBase::m_masterInfo->SetEvtColor(e, colorValue);
}

/// Get a color value.
/// @deprecated Deprecated: use UsineEventClass::getColor instead
/// @param e The event to get the color value from.
/// @return The color value of the event.
/// This function retrieves a color value from an event (e). It is typically used to access color information, such as RGB or RGBA values, which can be used for visual representation or styling within the Usine environment.
[[maybe_unused]] [[deprecated("Deprecated: use UsineEventClass::getColor instead")]]
inline TUsineColor sdkGetEvtColor(UsineEventPtr e)
{
    return UserModuleBase::m_masterInfo->GetEvtColor(e);
}

/// Set a color value to an array at index position.
/// @deprecated Deprecated: use UsineEventClass::setArrayColor instead
/// @param e The event to set the color value.
/// @param index The index position in the array where the color value will be set.
/// @param colorValue The color value to set in the array at the specified index.
/// This function sets a color value in an array stored within an event (e) at a specific index position. It is typically used to manipulate color data within an event, such as RGB or RGBA values, which can be used for visual representation or styling within the Usine environment.
[[maybe_unused]] [[deprecated("Deprecated: use UsineEventClass::setArrayColor instead")]]
inline void sdkSetEvtArrayColor(UsineEventPtr e, const int index, const TUsineColor colorValue)
{
        // sanity check
    const int idx = std::max(0, std::min(index, UserModuleBase::m_masterInfo->GetEvtSize(e)));
    auto *data = reinterpret_cast<TUsineColor *>(UserModuleBase::m_masterInfo->GetEvtDataAddr(e));
    data += idx;
    *data = colorValue;
}

/// Get a color value to an array at index position.
/// @deprecated Deprecated: use UsineEventClass::getArrayColor instead
/// @param e The event to get the color value from.
/// @param index The index position in the array from which the color value will be retrieved.
/// @return The color value at the specified index in the array.
/// This function retrieves a color value from an array stored within an event (e) at a specific index position. It is typically used to access color data within an event, such as RGB or RGBA values, which can be used for visual representation or styling within the Usine environment.
[[maybe_unused]] [[deprecated("Deprecated: use UsineEventClass::getArrayColor instead")]]
inline TUsineColor sdkGetEvtArrayColor(UsineEventPtr e, const int index)
{
        // sanity check
    int idx = std::max(0, std::min(index, UserModuleBase::m_masterInfo->GetEvtSize(e)));
    const auto *data = reinterpret_cast<TUsineColor *>(UserModuleBase::m_masterInfo->GetEvtDataAddr(e));
    const TUsineColor result = *(data + idx);
    return result;
}

/// Set a Midi value to an array at index position.
/// @deprecated Deprecated: use UsineEventClass::setArrayMidi instead
/// @param e The event to set the MIDI value.
/// @param index The index position in the array where the MIDI value will be set.
/// @param midiValue The MIDI value to set in the array at the specified index.
/// This function sets a MIDI value in an array stored within an event (e) at a specific index position. It is typically used to manipulate MIDI data within an event, such as note values, control changes, or other MIDI-related information.
[[maybe_unused]] [[deprecated("Deprecated: use UsineEventClass::setArrayMidi instead")]]
inline void sdkSetEvtArrayMidi(UsineEventPtr e, const int index, const TUsineMidiCode midiValue)
{
    UserModuleBase::m_masterInfo->SetEvtArrayMidi(e, index, midiValue);
}

/// Get a Midi value to an array at index position.
/// @deprecated Deprecated: use UsineEventClass::getArrayMidi instead
/// @param e The event to get the MIDI value from.
/// @param index The index position in the array from which the MIDI value will be retrieved.
/// @return The MIDI value at the specified index in the array.
/// This function retrieves a MIDI value from an array stored within an event (e) at a specific index position. It is typically used to access MIDI data within an event, such as note values, control changes, or other MIDI-related information.
[[maybe_unused]] [[deprecated("Deprecated: use UsineEventClass::getArrayMidi instead")]]
inline TUsineMidiCode sdkGetEvtArrayMidi(UsineEventPtr e, const int index)
{
    return UserModuleBase::m_masterInfo->GetEvtArrayMidi(e, index);
}

/// Get a 3D point.
/// @deprecated Deprecated: use UsineEventClass::get3DPoint
/// @param e the event
/// @param pointIndex is the index of the point you want to get
[[maybe_unused]] [[deprecated("Deprecated: use UsineEventClass::get3DPoint instead")]]
inline T3DPointF sdkGetEvt3DPoint(const UsineEventPtr e, const int pointIndex)
{
    T3DPointF result = { 0.0f, 0.0f, 0.0f };
    if (pointIndex < (UserModuleBase::m_masterInfo->GetEvtSize(e) / 3)) {
        TPrecision *pointer3DPoint = UserModuleBase::m_masterInfo->GetEvtDataAddr(e) + (pointIndex * 3);
        result.x = *(pointer3DPoint);
        result.y = *(pointer3DPoint + 1);
        result.z = *(pointer3DPoint + 2);
    } else {
        result.x = 0.0f;
        result.y = 0.0f;
        result.z = 0.0f;
    }
    
    return result;
}

/// @}


//----------------------------------------------------------------------------
/// FFT
/// @name FFT functions
/// @{


/// Create and initialize a FFT object
/// @param size from 32 to 65536
/// @return A pointer to the created FFT object.
/// This function creates and initializes a Fast Fourier Transform (FFT) object with the specified size. The size must be a power of two and can range from 32 to 65536. The created FFT object can be used for performing forward and inverse FFT operations on audio data.
[[maybe_unused]] inline TUsineFFTPtr sdkFFTCreate(const UINT32 size)
{
    return UserModuleBase::m_masterInfo->FFTCreate(size);
}
/// Destroy the FFT object
/// @param pFFT FFT object to destroy
/// This function destroys a previously created FFT object, releasing any resources associated with it. It is important to call this function when the FFT object is no longer needed to prevent memory leaks.
[[maybe_unused]] inline void sdkFFTDestroy(const TUsineFFTPtr pFFT)
{
    UserModuleBase::m_masterInfo->FFTDestroy(pFFT);
}

/// Calculate the forward FFT
/// @param pFFT FFT object relative to the FFT you want to perform
/// @param pAudio Audio input on which we perform the FFT
/// @param pRe Real part of the result of the FFT. The size of the pRe array is (size/2)+1
/// @param pIm Imaginary part of the result of the FFT. The size pIm array is (size/2) with pIm[0]=0
/// This function performs a forward Fast Fourier Transform (FFT) on the provided audio data.
inline void sdkFFTForward( TUsineFFTPtr pFFT, float* pAudio, float* pRe, float* pIm)
{
    UserModuleBase::m_masterInfo->FFTForward(pFFT,pAudio,pRe,pIm);
}

/// Calculate the inverse FFT
/// @param pFFT FFT object relative to the FFT you want to perform
/// @param pAudio Audio output of the inverse FFT
/// @param pRe Real part of the FFT. The size of the pRe array is (size/2)+1
/// @param pIm Imaginary part of the FFT. The size pIm array is (size/2) with pIm[0]=0
/// This function performs an inverse Fast Fourier Transform (FFT) on the provided real and imaginary parts of the FFT result, reconstructing the original audio signal.
inline void sdkFFTInverse(TUsineFFTPtr pFFT, float* pAudio, float* pRe, float* pIm)
{
    UserModuleBase::m_masterInfo->FFTInverse(pFFT,pAudio,pRe,pIm);
}
/// @}



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
/// 2023/11/28
///    added a more modern class system to the CPP SDK
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

constexpr int PUSH_GRANULARITY = 128;
//-----------------------------------------------------------------------------
// former module functions
//-----------------------------------------------------------------------------

/// Accessors to m_masterInfo
inline const TMasterInfo *sdkGetMasterInfo() { return UserModuleBase::sdkGetMasterInfo(); };

inline void sdkSetMasterInfo(TMasterInfo *m_masterInfo) { UserModuleBase::sdkSetMasterInfo(m_masterInfo); }

/// @addtogroup UsineEvents Exchanging data with Usine
/// @{

//----------------------------------------------------------------------------
/// @class UsineEventClass

/// @brief Usine event class, wrapper around UsineEventPtr. Commonly referred to as an event

/// The main data holding class of Usine's CPP SDK, its inner data is composed of a TPrecision (aka float 32)
/// pointer. In many ways this class behaves like an array but is also very often use with one only element as a regular
/// float.
/// This class also provide many event manipulation and getter functions.
/// @{
class UsineEventClass
{
public:
    /// Constructor.
    UsineEventClass() = default;

    explicit UsineEventClass(UsineEventPtr e)
        : p_event_(e)
    {
    }

    /// Disable implicit copy.
    UsineEventClass &operator=(const UsineEventClass &) = delete;

    /// Destructor.
    ~UsineEventClass() = default;

public:
    /// Get a reference to the contained event.
    /// Not for public use.
    UsineEventPtr &event() { return p_event_; }

    /// Destroy the contained event and deallocate the memory.
    [[maybe_unused]]
    void destroyEvent()
    {
        UserModuleBase::m_masterInfo->DestroyEvt(p_event_);
        p_event_ = nullptr;
    }

    /// Allocate a new event and point to it.
    /// The former contained event will not be deleted.
    /// @see @ref sdkCreateEvent.
    [[maybe_unused]]
    void createEvent(const int size)
    {
        UserModuleBase::m_masterInfo->CreateEvt(p_event_, size);
    }

    /// @name Events manipulation
    /// @{

    /// Compare events.

    [[maybe_unused]]
    [[nodiscard]] bool compare(const UsineEventClass &e) const
    {
        return UserModuleBase::m_masterInfo->CompareEvt(p_event_, e.p_event_);
    }

    /// Copy events, copy the source event into this one.
    [[maybe_unused]]
    void copyfrom(const UsineEventClass &src) const { UserModuleBase::m_masterInfo->CopyEvt(src.p_event_, p_event_); }

    /// Copy events, copy to the dest event.
    [[maybe_unused]]
    void copyto(const UsineEventClass &dest) const { UserModuleBase::m_masterInfo->CopyEvt(p_event_, dest.p_event_); }

    /// Append the other event to this one.
    [[maybe_unused]]
    void concat(const UsineEventClass &e) const
    {
        UserModuleBase::m_masterInfo->ConcatEvt(p_event_, e.p_event_, p_event_);
    }

    /// Shift the values of this event by n.
    /// Right shift for n > 0, left shift for n < 0.
    [[maybe_unused]]
    void shift(const int n) const { UserModuleBase::m_masterInfo->ShiftEvt(n, p_event_, p_event_); }

    /// Set the size of this event (number of elements). Typically the size of audio events is given by @ref sdkGetBlocSize .
    [[maybe_unused]]
    void setSize(const int size) const { UserModuleBase::m_masterInfo->SetEvtSize(p_event_, size); }

    /// Get the size of this event (number of elements).
    [[maybe_unused]] [[nodiscard]]
    int getSize() const { return UserModuleBase::m_masterInfo->GetEvtSize(p_event_); }

    /// Set the maximum size allowed by this event. Can be greater than the event size.
    [[maybe_unused]]
    void setMaxSize(const int size) const { UserModuleBase::m_masterInfo->SetEvtMaxSize(p_event_, size); }

    /// Add the other event to this one.
    [[maybe_unused]]
    void add(const UsineEventClass &e) const { UserModuleBase::m_masterInfo->AddEvt2(e.p_event_, p_event_); }

    /// Subtract the other event to this one.
    [[maybe_unused]]
    void sub(const UsineEventClass &e) const { UserModuleBase::m_masterInfo->SubEvt2(e.p_event_, p_event_); }

    /// Divide this event by the other.
    [[maybe_unused]]
    void div(const UsineEventClass &e) const { UserModuleBase::m_masterInfo->DivEvt3(p_event_, e.p_event_, p_event_); }

    /// Multiply this event by a float value.
    [[maybe_unused]]
    void mult(const TPrecision value) const { UserModuleBase::m_masterInfo->MultEvt1(value, p_event_); }

    /// Multiply this event by the other.
    [[maybe_unused]]
    void mult(const UsineEventClass &e) const { UserModuleBase::m_masterInfo->MultEvt2(e.p_event_, p_event_); }

    /// Compute the modulo of this event by the other one and store the result inplace.
    [[maybe_unused]]
    void mod(const UsineEventClass &e) const { UserModuleBase::m_masterInfo->ModEvt3(p_event_, e.p_event_, p_event_); }

    /// Compute the power of this event by the other one and store the result inplace.
    [[maybe_unused]]
    void power(const UsineEventClass &e) const
    {
        UserModuleBase::m_masterInfo->PowerEvt3(p_event_, e.p_event_, p_event_);
    }

    /// Multiply this event by the other but for audio.
    [[maybe_unused]]
    void multAudio(const UsineEventClass &e) const
    {
        UserModuleBase::m_masterInfo->MultEvt2Audio(e.p_event_, p_event_);
    }

    /// Exponential of this event inplace.
    [[maybe_unused]]
    void exp() const { UserModuleBase::m_masterInfo->ExpEvt1(p_event_); }

    /// Square root of this event inplace.
    [[maybe_unused]]
    void sqrt() const { UserModuleBase::m_masterInfo->SqrtEvt1(p_event_); }

    /// Get the maximum value of this event.
    [[maybe_unused]] [[nodiscard]]
    TPrecision max() const { return UserModuleBase::m_masterInfo->MaxEvt1(p_event_); }

    /// Get the minimum value of this event.
    [[maybe_unused]] [[nodiscard]]
    TPrecision min() const { return UserModuleBase::m_masterInfo->MinEvt1(p_event_); }

    /// Clip values of this event in the range [min..max] and store the result into dest.
    [[maybe_unused]]
    void tresh(const TPrecision min, const TPrecision max) const
    {
        UserModuleBase::m_masterInfo->ThreshEvt1(p_event_, min, max);
    }

    /// Mix MIDI values of the other event and this one and store the result inplace. For MIDI, this operation is equivalent to sdkConcatEvt
    [[maybe_unused]]
    void mixMidi(const UsineEventClass &e) const
    {
        UserModuleBase::m_masterInfo->MixMidiEvt2(static_cast<UsineEventPtr>(e), p_event_);
    }

    /// Apply a short fadein to this audio event. The duration of the fade is given by UserModuleBase::m_masterInfo->GetBlocSize.
    [[maybe_unused]]
    void fadeIn() const { UserModuleBase::m_masterInfo->FadeInAudioEvt(p_event_); }

    /// Apply a short fadeout to this audio event. The duration of the fade is given by UserModuleBase::m_masterInfo->GetBlocSize.
    [[maybe_unused]]
    void fadeOut() const { UserModuleBase::m_masterInfo->FadeOutAudioEvt(p_event_); }

    /// Clear this audio event. Set values to zero and add a small denormalization value.
    [[maybe_unused]]
    void clearAudio() const { UserModuleBase::m_masterInfo->ClearAudioEvt(p_event_); }

    /// Add a small denormalization value to this audio event.
    [[maybe_unused]]
    void denormalizeAudio() const { UserModuleBase::m_masterInfo->DenormalizeAudioEvt(p_event_); }

    /// Smooth this event.
    [[maybe_unused]]
    void smooth(TPrecision &oldValue, const TPrecision factor) const
    {
        UserModuleBase::m_masterInfo->SmoothPrecision(oldValue, p_event_, factor);
    }

    /// Smooth this event with a target value.
    [[maybe_unused]]
    void smooth(TPrecision &oldValue, const TPrecision target, const TPrecision factor) const
    {
        UserModuleBase::m_masterInfo->SmoothEvent(oldValue, p_event_, target, factor);
    }


    /// @}

    /// @name Access to Event values
    /// @{

    /// Set a Text value.
    [[maybe_unused]]
    void setPChar(const AnsiCharPtr s) const { UserModuleBase::m_masterInfo->SetEvtPChar(p_event_, s); }

    /// Get a Text value.
    [[maybe_unused]] [[nodiscard]]
    const char *getPChar() const { return UserModuleBase::m_masterInfo->GetEvtPChar(p_event_); }

    /// Get the address of the first data in this Event
    [[maybe_unused]] [[nodiscard]]
    TPrecision *getDataAddr() const
    {
        return UserModuleBase::m_masterInfo->GetEvtDataAddr(p_event_);
    }

    /// Set a float value
    [[maybe_unused]]
    void setData(const TPrecision value) const { UserModuleBase::m_masterInfo->SetEvtData(p_event_, value); }

    /// Get a float value
    [[maybe_unused]] [[nodiscard]]
    TPrecision getData() const { return UserModuleBase::m_masterInfo->GetEvtData(p_event_); }

    /// Set a value at an index position.
    [[maybe_unused]]
    void setArrayData(const int index, const TPrecision value) const
    {
        UserModuleBase::m_masterInfo->SetEvtArrayData(p_event_, index, value);
    }

    /// Push (append) a value to this event.
    [[maybe_unused]]
    void pushArrayData(const TPrecision value) const
    {
        const int index = UserModuleBase::m_masterInfo->GetEvtSize(p_event_);
        if (index >= UserModuleBase::m_masterInfo->GetEvtMaxSize(p_event_)) {
            UserModuleBase::m_masterInfo->SetEvtMaxSize(p_event_, index + PUSH_GRANULARITY);
        }
        UserModuleBase::m_masterInfo->SetEvtSize(p_event_, index + 1);
        UserModuleBase::m_masterInfo->SetEvtArrayData(p_event_, index, value);
    }

    /// Get a value at an index position.
    [[maybe_unused]] [[nodiscard]]
    TPrecision getArrayData(const int index) const
    {
        return UserModuleBase::m_masterInfo->GetEvtArrayData(p_event_, index);
    }

    /// Set the pointer of the contained Event Pointer.
    [[maybe_unused]]
    void setEventPointer(void *pointerValue) const
    {
        UserModuleBase::m_masterInfo->SetEvtPointer(p_event_, pointerValue);
    }

    /// Get the pointer of the contained Event Pointer .
    [[maybe_unused]] [[nodiscard]]
    const void *getEventPointer() const { return UserModuleBase::m_masterInfo->GetEvtPointer(p_event_); }

    /// Set a color value.
    [[maybe_unused]]
    void setColor(const TUsineColor colorValue) const
    {
        UserModuleBase::m_masterInfo->SetEvtColor(p_event_, colorValue);
    }

    /// Get a color value.
    [[maybe_unused]] [[nodiscard]]
    TUsineColor getColor() const { return UserModuleBase::m_masterInfo->GetEvtColor(p_event_); }

    /// Set a color value at an index position.
    [[maybe_unused]]
    void setArrayColor(const int index, const TUsineColor colorValue) const
    {
        // sanity check
        const int idx = std::max(0, std::min(index, UserModuleBase::m_masterInfo->GetEvtSize(p_event_)));
        auto *data = reinterpret_cast<TUsineColor *>(this->getDataAddr());
        data += idx;
        *data = colorValue;
    }

    /// Get a color value at an index position.
    [[maybe_unused]] [[nodiscard]]
    UINT32 getArrayColor(const int index) const
    {
        // sanity check
        const int idx = std::max(0, std::min(index, UserModuleBase::m_masterInfo->GetEvtSize(p_event_)));
        const auto *data = reinterpret_cast<TUsineColor *>(UserModuleBase::m_masterInfo->GetEvtDataAddr(p_event_));
        const TUsineColor result = *(data + idx);
        return result;
    }

    /// Get a Midi value at an index position.
    [[maybe_unused]]
    void setArrayMidi(const int index, const TUsineMidiCode midiValue) const
    {
        UserModuleBase::m_masterInfo->SetEvtArrayMidi(p_event_, index, midiValue);
    }

    /// Set a Midi value at an index position.
    [[maybe_unused]] [[nodiscard]]
    TUsineMidiCode getArrayMidi(const int index) const
    {
        return UserModuleBase::m_masterInfo->GetEvtArrayMidi(p_event_, index);
    }

    /// Get a 3D point.
    /// this event must have a line number of 3 to work,
    /// @param pointIndex is the index of the point you want to get
    [[maybe_unused]] [[nodiscard]]
    T3DPointF get3DPoint(const int pointIndex) const
    {
        T3DPointF result = {0.0f, 0.0f, 0.0f};
        if (pointIndex < (UserModuleBase::m_masterInfo->GetEvtSize(p_event_) / 3)) {
            const TPrecision *pointer3DPoint =
                    UserModuleBase::m_masterInfo->GetEvtDataAddr(p_event_) + (pointIndex * 3);
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

    operator UsineEventPtr() const { return p_event_; }

    /// Used in onGetParamInfo, declared explicit to avoid conflict
    explicit operator UsineEventPtr *() { return &p_event_; }

private:
    /// The contained UsineEventPtr.
    UsineEvent *p_event_;
};

/// @}


// The definition of this function must come after the declaration of UsineEventClass
#if defined(__GNUC__)
// Disable deprecation warning
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
inline void TParamInfo::setEventClass(UsineEventClass& e)
{
    this->EventPtr = &e.event();
}
#pragma GCC diagnostic pop

#elif(_WIN32)

inline void TParamInfo::setEventClass(UsineEventClass &e)
{
    // disable deprecation warning for this line only.
#pragma warning(suppress : 4996)
    this->EventPtr = &e.event();
}

#endif // defined(__GNUC__)

//----------------------------------------------------------------------------
/// Synchronization Objects.
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
[[maybe_unused]] [[nodiscard]]
inline AnsiCharPtr sdkGetAudioQueryTitle() { return UserModuleBase::m_masterInfo->AudioQueryTitle; }

/// Get the query string list for an multi channels query.
[[maybe_unused]] [[nodiscard]]
inline AnsiCharPtr sdkGetAudioQueryChannelList() { return UserModuleBase::m_masterInfo->AudioQueryChannelList; }

/// Get the number of channels from the query index choice.
[[maybe_unused]] [[nodiscard]]
inline int sdkGetAudioQueryToNbChannels(const int qQueryIndex)
{
    return UserModuleBase::m_masterInfo->AudioQueryToNbChannels(qQueryIndex);
}

/// Get the name of a channel.
[[maybe_unused]] [[nodiscard]]
inline AnsiCharPtr sdkGetAudioQueryChannelNames(const AnsiCharPtr prefix, const int index, const int queryIndex)
{
    return UserModuleBase::m_masterInfo->AudioQueryChannelNames(prefix, index, queryIndex);
}

/// @}


//----------------------------------------------------------------------------
/// @name Global Array functions
/// @{

/// Get the Hash (internal ID) of the Global Array needed for function bellow
[[maybe_unused]] [[nodiscard]]
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
[[maybe_unused]] [[nodiscard]]
inline TPrecision sdkGlobalArrayGetValueFloat(const UINT64 hash, const int index, const AnsiCharPtr errorMsg)
{
    return UserModuleBase::m_masterInfo->GlobalArrayGetValueFloat(hash, index, errorMsg);
}

/// Get the size of the Global Array
[[maybe_unused]] [[nodiscard]]
inline TPrecision sdkGlobalArrayGetSize(const UINT64 hash, const AnsiCharPtr errorMsg)
{
    return UserModuleBase::m_masterInfo->GlobalArrayGetSize(hash, errorMsg);
}

/// Get a color value from a color Global Array at the index
[[maybe_unused]] [[nodiscard]]
inline TUsineColor sdkGlobalArrayGetValueColor(const UINT64 hash, const int index, const AnsiCharPtr errorMsg)
{
    return UserModuleBase::m_masterInfo->GlobalArrayGetValueColor(hash, index, errorMsg);
}

/// get a string value from a string Global Array at the index
[[maybe_unused]] [[nodiscard]]
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
[[maybe_unused]] [[nodiscard]]
inline AnsiCharPtr sdkGlobalArrayGetAllNames()
{
    return UserModuleBase::m_masterInfo->GlobalArrayGetAllNames();
}

/// @}

//----------------------------------------------------------------------------
/// @name Usine Core infos and services
/// @{

/// Get the list of possible usine block size.
[[maybe_unused]] [[nodiscard]]
inline AnsiCharPtr sdkGetUsineBlockSizeList()
{
    return UserModuleBase::m_masterInfo->BlockSizeList;
}

/// Get the default usine block size.
[[maybe_unused]] [[nodiscard]]
inline int sdkGetUsineDefaultBlockSizeIndex()
{
    return UserModuleBase::m_masterInfo->BlockSizeDefaultIndex;
}

/// Low level , not for public use
/// Usine audio callback
// changed for linux testing by basile
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
[[maybe_unused]] [[nodiscard]]
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

#if defined(_WIN32)

/// PC only, Usine mainform handle (THandle)
[[maybe_unused]]
inline NativeInt sdkGetUsineMainWindow()
{
    return UserModuleBase::m_masterInfo->GetUsineMainWindow();
}

#elif defined(APPLE)

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
[[maybe_unused]]
inline TUsineColor sdkGetUsineColor(const UINT32 color)
{
    TUsineColor result = 0;
    if (UserModuleBase::m_masterInfo)
        result = UserModuleBase::m_masterInfo->GetUsineColor(color - 1);
    return result;
};

///Translate a StringID to the current Usine language.
[[maybe_unused]]
inline AnsiCharPtr sdkGetTrans(const AnsiCharPtr StringID)
{
    return UserModuleBase::m_masterInfo->GetTranslation(StringID);
};

/// To get the version type of Usine @see TUsineVersionType.
[[maybe_unused]] [[deprecated("Usine Version is no longer used in the SDK, always returns 0")]]
inline int sdkGetUsineVersionType()
{
    return 0;
};

/// To get the current language used in Usine.
/// @return a string naming the current language (En, Fr...)
[[maybe_unused]]
inline AnsiCharPtr sdkGetUsineLanguage()
{
    return UserModuleBase::m_masterInfo->UsineLanguage;
};

/// To get the of the save format of Usine.
[[maybe_unused]]
inline int sdkGetUsineSaveVersion()
{
    return UserModuleBase::m_masterInfo->UsineSaveVersion;
};

/// To get the maximum audio inputs Usine can accept.
[[maybe_unused]]
inline int sdkGetUsineMaxAudioInputs()
{
    return USINE_MAX_AUDIO_INPUTS;
};

/// To get the maximum audio outputs Usine can accept.
[[maybe_unused]]
inline int sdkGetUsineMaxAudioOutputs()
{
    return USINE_MAX_AUDIO_OUTPUTS;
};

/// To get the maximum Midi device Usine can accept.
[[maybe_unused]]
inline int sdkGetUsineMaxMidiDevices()
{
    return USINE_MAX_MIDI_DEVICES;
};

/// To get the current polyphony of Usine.
[[maybe_unused]]
inline int sdkGetUsineMaxPolyphony()
{
    return USINE_MULTIPHONY_MAX;
};

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
};

/// The Patch library folder as configured in the Global setting Tab.
[[maybe_unused]]
inline AnsiCharPtr sdkGetUserLibPath()
{
    return UserModuleBase::m_masterInfo->UserLibPath;
};

/// The Record Out folder as configured in the Global setting Tab.
[[maybe_unused]]
inline AnsiCharPtr sdkGetUsineRecordPath()
{
    return UserModuleBase::m_masterInfo->RecordPath;
};

/// The install folder of Usine.
[[maybe_unused]]
inline AnsiCharPtr sdkGetGlobalApplicationPath()
{
    return UserModuleBase::m_masterInfo->GlobalApplicationPath;
};

/// The path of the current workspace.
[[maybe_unused]]
inline AnsiCharPtr sdkGetCurrentWorkspacePath()
{
    return UserModuleBase::m_masterInfo->CurrentWorkspacePath;
};

/// The path of the patch containing the module.
[[maybe_unused]]
inline AnsiCharPtr sdkGetCurrentPatchPath()
{
    return UserModuleBase::m_masterInfo->CurrentPatchPath;
};

/// The Temp folder Used by Usine.
[[maybe_unused]]
inline AnsiCharPtr sdkGetUsineTempPath()
{
    return UserModuleBase::m_masterInfo->UsineTempPath;
};

/// Not for public use.
[[maybe_unused]]
inline AnsiCharPtr sdkGetUsinePlugInsPath()
{
    return UserModuleBase::m_masterInfo->PlugInsPath;
};
/// @}

//----------------------------------------------------------------------------
/// @name Files utils
/// @{

/// Show a dialog window to open a file.
[[maybe_unused]]
inline LongBool sdkProcessOpenDialog(AnsiCharPtr *filename, const AnsiCharPtr initialDir, const AnsiCharPtr filter)
{
    return UserModuleBase::m_masterInfo->ProcessOpenDialog(filename, initialDir, filter);
};

/// Show a dialog window to save a file.
[[maybe_unused]]
inline LongBool sdkProcessSaveDialog(AnsiCharPtr *filename, const AnsiCharPtr initialDir, const AnsiCharPtr filter)
{
    return UserModuleBase::m_masterInfo->ProcessSaveDialog(filename, initialDir, filter);
};

/// Ask Usine to search if a file exist.
[[maybe_unused]]
inline AnsiCharPtr sdkFindFile(AnsiCharPtr *filename, const AnsiCharPtr initialDir)
{
    return UserModuleBase::m_masterInfo->FindFile(filename, initialDir);
};
/// @}

//----------------------------------------------------------------------------
/// @name Audio Files manipulation
/// @{

/// Create an audio file handle
[[maybe_unused]]
inline TAudioFilePtr sdkCreateAudioFile()
{
    return UserModuleBase::m_masterInfo->CreateAudioFile();
};

/// Destroy an audio file handle
[[maybe_unused]]
inline void sdkDestroyAudioFile(TAudioFilePtr audioFile)
{
    UserModuleBase::m_masterInfo->DestroyAudioFile(audioFile);
};

/// Get a sample value of an audio file at the position pos of the audio channel
[[maybe_unused]]
inline TPrecision sdkGetSampleAudioFile(TAudioFilePtr audioFile, const int channel, const int pos)
{
    return UserModuleBase::m_masterInfo->GetSampleAudioFile(audioFile, channel, pos);
};

/// Get all samples values (as an array) of an audio file for an audio channel
[[maybe_unused]]
inline TPrecision *sdkGetSampleArrayAudioFile(TAudioFilePtr audioFile, const int channel)
{
    return UserModuleBase::m_masterInfo->GetSampleArrayAudioFile(audioFile, channel);
};

/// Get a bloc of samples values of an audio file at position pos for an audio channel and store it in an Event
[[maybe_unused]]
inline void sdkGetBlocSampleAudioFile(TAudioFilePtr audioFile, const int channel, const int pos,
                                      UsineEventPtr evt)
{
    UserModuleBase::m_masterInfo->GetBlocSampleAudioFile(audioFile, channel, pos, evt);
};

/// Get the numbers of samples of an audio file
[[maybe_unused]]
inline int sdkGetSizeAudioFile(TAudioFilePtr audioFile)
{
    return UserModuleBase::m_masterInfo->GetSizeAudioFile(audioFile);
};

/// Get the the number of channels of an audio file
[[maybe_unused]]
inline int sdkGetChannelAudioFile(TAudioFilePtr audioFile)
{
    return UserModuleBase::m_masterInfo->GetChannelAudioFile(audioFile);
};

///Get the samplerate of an audio file
[[maybe_unused]]
inline int sdkGetSampleRateAudioFile(TAudioFilePtr audioFile)
{
    return UserModuleBase::m_masterInfo->GetSampleRateAudioFile(audioFile);
};

/// Get the bit per sample of an audio file
[[maybe_unused]]
inline int sdkGetBitPerSampleAudioFile(TAudioFilePtr audioFile)
{
    return UserModuleBase::m_masterInfo->GetBitPerSampleAudioFile(audioFile);
};

/// Load an audio file in memory
[[maybe_unused]]
inline void sdkLoadInMemoryAudioFile(TAudioFilePtr audioFile, const AnsiCharPtr name)
{
    UserModuleBase::m_masterInfo->LoadInMemoryAudioFile(audioFile, name);
};

/// Load an audio file but keep it on disk
[[maybe_unused]]
inline void sdkLoadStayOnDiskAudioFile(TAudioFilePtr audioFile, const AnsiCharPtr name)
{
    UserModuleBase::m_masterInfo->LoadStayOnDiskAudioFile(audioFile, name);
};

/// Get the peak of the audio files (to draw the wave form) and store it in an Event
[[maybe_unused]]
inline void sdkGetPeaksAudioFile(TAudioFilePtr audioFile, UsineEventPtr evt)
{
    UserModuleBase::m_masterInfo->GetPeaksAudioFile(audioFile, evt);
};

/// Save an audio file to disk
[[maybe_unused]]
inline void sdkSaveToDiskAudioFile(TAudioFilePtr audioFile, const AnsiCharPtr name, const int nbChannels)
{
    UserModuleBase::m_masterInfo->SaveToDiskAudioFile(audioFile, name, nbChannels);
};

/// Set the number of channels of an audio file
[[maybe_unused]]
inline void sdkSetChannelsAudioFile(TAudioFilePtr audioFile, const int nbChannels)
{
    UserModuleBase::m_masterInfo->SetChannelsAudioFile(audioFile, nbChannels);
};

/// Set the number of samples of an audio file
[[maybe_unused]]
inline void sdkSetSizeAudioFile(TAudioFilePtr audioFile, const int size)
{
    UserModuleBase::m_masterInfo->SetSizeAudioFile(audioFile, size);
};

/// Set a sample value of an audio file at the position pos of the audio channel
[[maybe_unused]]
inline void sdkSetSampleAudioFile(TAudioFilePtr audioFile, const int channel, const int pos,
                                  const TPrecision sample)
{
    UserModuleBase::m_masterInfo->SetSampleAudioFile(audioFile, channel, pos, sample);
};

/// Clear an audio file
[[maybe_unused]]
inline void sdkClearAudioFile(TAudioFilePtr audioFile)
{
    UserModuleBase::m_masterInfo->ClearAudioFile(audioFile);
};

/// Resample (stretch) an audio file by a factor
[[maybe_unused]]
inline void sdkResampleAudioFile(TAudioFilePtr audioFile, const TPrecision factor)
{
    UserModuleBase::m_masterInfo->ResampleAudioFile(audioFile, factor);
};
/// @}

//----------------------------------------------------------------------------
/// @name Math utils
/// @{

/// Linear interpolation  on a TPrecision value.
/// @return The interpolated value.
[[maybe_unused]]
inline TPrecision sdkLinearInterpolation(const TPrecision f, const TPrecision a, const TPrecision b)
{
    return UserModuleBase::m_masterInfo->LinearInterpolation(f, a, b);
};

/// Cubic interpolation  on a TPrecision value.
/// @return The interpolated value.
[[maybe_unused]]
inline TPrecision
sdkCubicInterpolation(const TPrecision fr, const TPrecision inm1, const TPrecision inp, const TPrecision inp1,
                      const TPrecision inp2)
{
    return UserModuleBase::m_masterInfo->CubicInterpolation(fr, inm1, inp, inp1, inp2);
};

/// Spline interpolation on a TPrecision value.
/// @return The interpolated value.
[[maybe_unused]]
inline TPrecision
sdkSplineInterpolation(const TPrecision fr, const TPrecision inm1, const TPrecision inp, const TPrecision inp1,
                       const TPrecision inp2)
{
    return UserModuleBase::m_masterInfo->SplineInterpolation(fr, inm1, inp, inp1, inp2);
};
/// @}

//----------------------------------------------------------------------------
/// @name Trace functions
/// @{

/// Show a string in the Trace Panel.
[[maybe_unused]]
inline void sdkTraceChar(const AnsiCharPtr s)
{
    UserModuleBase::m_masterInfo->TraceChar(s);
};

[[maybe_unused]]
inline void sdkTraceChar(const std::string &s)
{
    UserModuleBase::m_masterInfo->TraceChar(s.c_str());
};

/// Show an integer in the Trace Panel.
[[maybe_unused]]
inline void sdkTraceInt(const int i)
{
    UserModuleBase::m_masterInfo->TraceInteger(i);
};

/// Show a decimal number in the Trace Panel.
[[maybe_unused]]
inline void sdkTracePrecision(const TPrecision f)
{
    UserModuleBase::m_masterInfo->TracePrecision(f);
};

/// Show a string in the Trace Panel with the error tag.
[[maybe_unused]]
inline void sdkTraceErrorChar(const AnsiCharPtr s, const LongBool namePrefix = TRUE)
{
    if (UserModuleBase::m_masterInfo && namePrefix) {
        std::stringstream traceString;
        traceString
                << "[" << UserModuleBase::MODULE_NAME << "]"
                << " " << s;
        UserModuleBase::m_masterInfo->TraceErrorChar(static_cast<AnsiCharPtr>(traceString.str().c_str()));
    }
};

[[maybe_unused]]
inline void sdkTraceErrorChar(const std::string &s)
{
    if (UserModuleBase::m_masterInfo) {
        std::stringstream traceString;
        traceString
                << "[" << UserModuleBase::MODULE_NAME << "]"
                << " " << s;
        UserModuleBase::m_masterInfo->TraceErrorChar(static_cast<AnsiCharPtr>(traceString.str().c_str()));
    }
};

/// Show a string in the Trace Panel with the warning tag.
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
};

[[maybe_unused]]
inline void sdkTraceWarningChar(const std::string &s)
{
    if (UserModuleBase::m_masterInfo) {
        std::stringstream traceString;
        traceString
                << "[" << UserModuleBase::MODULE_NAME << "]"
                << " " << s;
        UserModuleBase::m_masterInfo->TraceWarningChar(static_cast<AnsiCharPtr>(traceString.str().c_str()));
    }
};


/// Show a string in the SplashForm.
[[maybe_unused]]
inline void sdkTraceSplashChar(const AnsiCharPtr s, const int autoClose)
{
    UserModuleBase::m_masterInfo->TraceSplashChar(s, autoClose);
};

[[maybe_unused]]
inline void sdkTraceSplashChar(const std::string &s, const int autoClose)
{
    UserModuleBase::m_masterInfo->TraceSplashChar(s.c_str(), autoClose);
};

/// Print a string in the LogUsine.log file. Optionally show it in the SplashForm.
inline void sdkTraceLogChar(const AnsiCharPtr s, const LongBool showInSplashForm = FALSE)
{
    if (UserModuleBase::m_masterInfo) {
        std::stringstream traceString;
        traceString
                << "  [" << UserModuleBase::MODULE_NAME << "]"
                << " " << s;
        UserModuleBase::m_masterInfo->TraceLogChar((AnsiCharPtr) traceString.str().c_str(), showInSplashForm);
    }
};

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
inline int sdkDialogConfirmationYesNoCancel(const AnsiCharPtr msg)
{
    return UserModuleBase::m_masterInfo->DialogConfirmationYesNoCancel(msg);
};

/// A dialog popup with Yes and No buttons.
/// @param msg The message to show on the popup popup.
/// @return The choice of the user.
/// @see TDialogsResults
[[maybe_unused]]
inline int sdkDialogConfirmationYesNo(const AnsiCharPtr msg)
{
    return UserModuleBase::m_masterInfo->DialogConfirmationYesNo(msg);
};

/// A dialog popup with OK button.
/// @param msg The message to show on the popup popup.
/// @return The choice of the user.
/// @see TDialogsResults
[[maybe_unused]]
inline int sdkDialogInformationOk(const AnsiCharPtr msg)
{
    return UserModuleBase::m_masterInfo->DialogInformationOk(msg);
};

/// A dialog popup with OK and Cancel buttons.
/// @param msg The message to show on the popup popup.
/// @return The choice of the user.
/// @see TDialogsResults
[[maybe_unused]]
inline int sdkDialogConfirmationOKCancel(const AnsiCharPtr msg)
{
    return UserModuleBase::m_masterInfo->DialogConfirmationOKCancel(msg);
};

/// A dialog popup with a string input box .
/// @return the edited string.
/// @see TDialogsResults
[[maybe_unused]]
inline AnsiCharPtr sdkDialogInputBox(const AnsiCharPtr caption, const AnsiCharPtr prompt,
                                     const AnsiCharPtr defaultValue)
{
    return UserModuleBase::m_masterInfo->DialogInputBox(caption, prompt, defaultValue);
};

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
[[maybe_unused]] [[deprecated("Deprecated: use UsineEventClass::copy instead")]]
inline void sdkCopyEvt(UsineEventPtr scr, UsineEventPtr dest)
{
    UserModuleBase::m_masterInfo->CopyEvt(scr, dest);
}

/// Event Concatenation dest is the concatenation of e1 and e2.
[[maybe_unused]]
inline void sdkConcatEvt(UsineEventPtr e1, UsineEventPtr e2, UsineEventPtr dest)
{
    UserModuleBase::m_masterInfo->ConcatEvt(e1, e2, dest);
}

/// Shift the values of src and store the result into dest.
/// Right shift for n>0, left shift for n<0.
[[maybe_unused]]
inline void sdkShiftEvt(const int n, UsineEventPtr src, UsineEventPtr dest)
{
    UserModuleBase::m_masterInfo->ShiftEvt(n, src, dest);
}

/// Set the size of the event (number of elements). Typically the size of audio events is given by sdkGetBlocSize.
/// @see sdkGetBlocSize
[[maybe_unused]] [[deprecated("Deprecated: use UsineEventClass::setSize instead")]]
inline void sdkSetEvtSize(UsineEventPtr e, const int size)
{
    UserModuleBase::m_masterInfo->SetEvtSize(e, size);
}

/// Get the size of the event (number of elements).
[[maybe_unused]] [[nodiscard]] [[deprecated("Deprecated: use UsineEventClass::getSize instead")]]
inline int sdkGetEvtSize(UsineEventPtr e)
{
    return UserModuleBase::m_masterInfo->GetEvtSize(e);
}

/// Set the maximum size allowed by the event. Can be greater than the event size.
[[maybe_unused]] [[deprecated("Deprecated: use UsineEventClass::setMaxSize instead")]]
inline void sdkSetEvtMaxSize(UsineEventPtr e, const int size)
{
    UserModuleBase::m_masterInfo->SetEvtMaxSize(e, size);
}

/// Destroy the event.
[[maybe_unused]] [[deprecated("Deprecated: use UsineEventClass::destroy instead")]]
inline void sdkDestroyEvt(UsineEventPtr &e)
{
    UserModuleBase::m_masterInfo->DestroyEvt(e);
}

/// Create an event with an original size.
[[maybe_unused]] [[deprecated("Deprecated: use UsineEventClass::create instead")]]
inline void sdkCreateEvt(UsineEventPtr &e, const int originalSize)
{
    UserModuleBase::m_masterInfo->CreateEvt(e, originalSize);
}

/// Copy a 32Bits bloc of memory. The size is the number of 32bits elements.
[[maybe_unused]]
inline void sdkMoveBLOC32(TPrecision *pointerSrc, TPrecision *pointerDest, const int size)
{
    UserModuleBase::m_masterInfo->MoveBLOC32(pointerSrc, pointerDest, size);
}

/// Add values of two events and store the result into dest.
[[maybe_unused]]
inline void sdkAddEvt3(UsineEventPtr e1, UsineEventPtr e2, UsineEventPtr dest)
{
    UserModuleBase::m_masterInfo->AddEvt3(e1, e2, dest);
}

/// Add values of two events and store the result into e2.
[[deprecated("Deprecated: use UsineEventClass::add instead")]]
inline void sdkAddEvt2(UsineEventPtr e1, UsineEventPtr e2)
{
    UserModuleBase::m_masterInfo->AddEvt2(e1, e2);
}

/// Subtract values of two events and store the result into dest.
[[maybe_unused]]
inline void sdkSubEvt3(UsineEventPtr e1, UsineEventPtr e2, UsineEventPtr dest)
{
    UserModuleBase::m_masterInfo->SubEvt3(e1, e2, dest);
}

/// Subtract values of two events and store the result into e2.
[[deprecated("Deprecated: use UsineEventClass::sub instead")]]
inline void sdkSubEvt2(UsineEventPtr e1, UsineEventPtr e2)
{
    UserModuleBase::m_masterInfo->SubEvt2(e1, e2);
}

/// Divide values of e1 by values of e2 and store the result into dest.
[[maybe_unused]]
inline void sdkDivEvt3(UsineEventPtr e1, UsineEventPtr e2,
                       UsineEventPtr dest)
{
    UserModuleBase::m_masterInfo->DivEvt3(e1, e2, dest);
}


/// Multiply values of e1 by values of e2 and store the result into dest.
[[maybe_unused]]
inline void sdkMultEvt3(UsineEventPtr e1, UsineEventPtr e2, UsineEventPtr dest)
{
    UserModuleBase::m_masterInfo->MultEvt3(e1, e2, dest);
}

/// Multiply values of e1 by values of e2 and store the result into e2.
[[maybe_unused]] [[deprecated("Deprecated: use UsineEventClass::mult instead")]]
inline void sdkMultEvt2(UsineEventPtr e1, UsineEventPtr e2)
{
    UserModuleBase::m_masterInfo->MultEvt2(e1, e2);
}

/// Multiply values of e1 by values of e2 and store the result into e2. Same as sdkMultEvt2.
[[maybe_unused]] [[deprecated("Deprecated: use UsineEventClass::multAudio instead")]]
inline void sdkMultEvt2Audio(UsineEventPtr e1, UsineEventPtr e2)
{
    UserModuleBase::m_masterInfo->MultEvt2(e1, e2);
}

/// Multiply values of e by a float value inplace.
[[maybe_unused]] [[deprecated("Deprecated: use UsineEventClass::mult instead")]]
inline void sdkMultEvt1(const TPrecision m, UsineEventPtr e)
{
    UserModuleBase::m_masterInfo->MultEvt1(m, e);
}

/// Modulo operation of e1 by values of e2 and store the result into dest.
[[maybe_unused]]
inline void sdkModEvt3(UsineEventPtr e1, UsineEventPtr e2, UsineEventPtr dest)
{
    UserModuleBase::m_masterInfo->ModEvt3(e1, e2, dest);
}

/// Power values of e1 by values of e2 and store the result into dest.
[[maybe_unused]]
inline void sdkPowerEvt3(UsineEventPtr e1, UsineEventPtr e2, UsineEventPtr dest)
{
    UserModuleBase::m_masterInfo->PowerEvt3(e1, e2, dest);
}

/// Exponential of e inplace.
[[deprecated("Deprecated: use UsineEventClass::exp instead")]]
inline void sdkExpEvt1(UsineEventPtr e)
{
    UserModuleBase::m_masterInfo->ExpEvt1(e);
}

/// Square root of e inplace.
[[deprecated("Deprecated: use UsineEventClass::sqrt instead")]]
inline void sdkSqrtEvt1(UsineEventPtr in1)
{
    UserModuleBase::m_masterInfo->SqrtEvt1(in1);
}

/// Returns the maximum value of e.
[[deprecated("Deprecated: use UsineEventClass::max instead")]]
inline TPrecision sdkMaxEvt1(UsineEventPtr e)
{
    return UserModuleBase::m_masterInfo->MaxEvt1(e);
}

/// Returns the minimum value of e.
[[deprecated("Deprecated: use UsineEventClass::min instead")]]
inline TPrecision sdkMinEvt1(UsineEventPtr e)
{
    return UserModuleBase::m_masterInfo->MinEvt1(e);
}

/// calculates the maximum of each element and store the result into dest. dest[i] = max(e1[i],e2[i])
[[maybe_unused]]
inline void sdkMaxEvt3(UsineEventPtr e1, UsineEventPtr e2, UsineEventPtr dest)
{
    UserModuleBase::m_masterInfo->MaxEvt3(e1, e2, dest);
}

/// calculates the minimum of each element and store the result into dest. dest[i] = min(e1[i],e2[i])
[[maybe_unused]]
inline void sdkMinEvt3(UsineEventPtr in1, UsineEventPtr in2, UsineEventPtr out)
{
    UserModuleBase::m_masterInfo->MinEvt3(in1, in2, out);
}

/// Clip values of e in the range [min..max] and store the result into dest.
[[maybe_unused]]
inline void sdkThreshEvt2(UsineEventPtr e, UsineEventPtr dest, TPrecision min, TPrecision max)
{
    UserModuleBase::m_masterInfo->ThreshEvt2(e, dest, min, max);
}

/// Clip values of e in the range [min..max] inplace.
[[maybe_unused]] [[deprecated("Deprecated: use UsineEventClass::tresh instead")]]
inline void sdkThreshEvt1(UsineEventPtr e, TPrecision min, TPrecision max)
{
    UserModuleBase::m_masterInfo->ThreshEvt1(e, min, max);
}

/// Mix MIDI values of e1 and e2 and store the result in dest. For the MIDI, this operation equivalent to sdkConcatEvt
[[maybe_unused]] inline void sdkMixMidiEvt3(UsineEventPtr e1, UsineEventPtr e2, UsineEventPtr dest)
{
    UserModuleBase::m_masterInfo->MixMidiEvt3(e1, e2, dest);
}

/// Mix MIDI values of e1 and e2 and store the result in e2. For the MIDI, this operation is equivalent to sdkConcatEvt
[[maybe_unused]] [[deprecated("Deprecated: use UsineEventClass::mixMidi instead")]]
inline void sdkMixMidiEvt2(UsineEventPtr e1, UsineEventPtr e2)
{
    UserModuleBase::m_masterInfo->MixMidiEvt2(e1, e2);
}

/// Apply a short fadein to the audio event. The duration of the fade is given by sdkGetBlocSize.
[[maybe_unused]] [[deprecated("Deprecated: use UsineEventClass::fadeIn instead")]]
inline void sdkFadeInAudioEvt(UsineEventPtr e)
{
    UserModuleBase::m_masterInfo->FadeInAudioEvt(e);
}

/// Apply a short fadeout to the audio event. The duration of the fade is given by sdkGetBlocSize.
[[maybe_unused]] [[deprecated("Deprecated: use UsineEventClass::fadeOut instead")]]
inline void sdkFadeOutAudioEvt(UsineEventPtr e)
{
    UserModuleBase::m_masterInfo->FadeOutAudioEvt(e);
}

/// Clear an audio event. Set values to zero and add a small denormalization value.
[[maybe_unused]] [[deprecated("Deprecated: use UsineEventClass::clearAudio instead")]]
inline void sdkClearAudioEvt(UsineEventPtr e)
{
    UserModuleBase::m_masterInfo->ClearAudioEvt(e);
}

/// Add a small denormalization value to an audio event.
[[maybe_unused]] [[deprecated("Deprecated: use UsineEventClass::denormalizeAudio instead")]]
inline void sdkDenormalizeAudioEvt(UsineEventPtr e)
{
    UserModuleBase::m_masterInfo->DenormalizeAudioEvt(e);
}

/// Smooth event.
[[maybe_unused]] [[deprecated("Deprecated: use UsineEventClass::smooth instead")]]
inline void sdkSmoothPrecision(TPrecision &oldValue, UsineEventPtr e, const TPrecision factor)
{
    return UserModuleBase::m_masterInfo->SmoothPrecision(oldValue, e, factor);
}

/// Smooth Event with a target value.
[[maybe_unused]] [[deprecated("Deprecated: use UsineEventClass::smooth instead")]]
inline void sdkSmoothEvent(TPrecision &oldValue, UsineEventPtr e, const TPrecision target,
                           const TPrecision factor)
{
    return UserModuleBase::m_masterInfo->SmoothEvent(oldValue, e, target, factor);
}

/// @}

/// @name Deprecated access to Event values. Use UsineEventClass instead.
/// @{

/// Set a Text value.
[[maybe_unused]] [[deprecated("Deprecated: use UsineEventClass::setPChar instead")]]
inline void sdkSetEvtPChar(UsineEventPtr e, AnsiCharPtr textValue)
{
    UserModuleBase::m_masterInfo->SetEvtPChar(e, textValue);
}

/// Get a Text value.
[[maybe_unused]] [[deprecated("Deprecated: use UsineEventClass::getPChar instead")]]
inline AnsiCharPtr sdkGetEvtPChar(UsineEventPtr e)
{
    return UserModuleBase::m_masterInfo->GetEvtPChar(e);
}

/// Get the address of the first data in the Event
[[maybe_unused]] [[deprecated("Deprecated: use UsineEventClass::getDataAddr instead")]]
inline TPrecision *sdkGetEvtDataAddr(UsineEventPtr ev)
{
    return UserModuleBase::m_masterInfo->GetEvtDataAddr(ev);
}

/// Set a float value
[[maybe_unused]] [[deprecated("Deprecated: use UsineEventClass::setData instead")]]
inline void sdkSetEvtData(UsineEventPtr e, const TPrecision floatValue)
{
    UserModuleBase::m_masterInfo->SetEvtData(e, floatValue);
}

/// Get a float Value
[[maybe_unused]] [[deprecated("Deprecated: use UsineEventClass::getData instead")]]
inline TPrecision sdkGetEvtData(UsineEventPtr e)
{
    return UserModuleBase::m_masterInfo->GetEvtData(e);
}

/// Set a value to an Array at an index position.
[[maybe_unused]] [[deprecated("Deprecated: use UsineEventClass::setArrayData instead")]]
inline void sdkSetEvtArrayData(UsineEventPtr e, const int index, const TPrecision floatValue)
{
    UserModuleBase::m_masterInfo->SetEvtArrayData(e, index, floatValue);
}

/// Push (append) a value to an Array.
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
[[maybe_unused]] [[deprecated("Deprecated: use UsineEventClass::getArrayData instead")]]
inline TPrecision sdkGetEvtArrayData(UsineEventPtr e, const int index)
{
    return UserModuleBase::m_masterInfo->GetEvtArrayData(e, index);
}

/// Set a pointer value.
[[maybe_unused]] [[deprecated("Deprecated: use UsineEventClass::setEventPointer instead")]]
inline void sdkSetEvtPointer(UsineEventPtr e, void *pointerValue)
{
    UserModuleBase::m_masterInfo->SetEvtPointer(e, pointerValue);
}

/// Get a pointer value.
[[maybe_unused]] [[deprecated("Deprecated: use UsineEventClass::getEventPointer instead")]]
inline void *sdkGetEvtPointer(UsineEventPtr e)
{
    return UserModuleBase::m_masterInfo->GetEvtPointer(e);
}

/// Set a color value.
[[maybe_unused]] [[deprecated("Deprecated: use UsineEventClass::setColor instead")]]
inline void sdkSetEvtColor(UsineEventPtr e, const TUsineColor colorValue)
{
    UserModuleBase::m_masterInfo->SetEvtColor(e, colorValue);
}

/// Get a color value.
[[maybe_unused]] [[deprecated("Deprecated: use UsineEventClass::getColor instead")]]
inline TUsineColor sdkGetEvtColor(UsineEventPtr e)
{
    return UserModuleBase::m_masterInfo->GetEvtColor(e);
}

/// Set a color value to an array at index position.
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
[[maybe_unused]] [[deprecated("Deprecated: use UsineEventClass::setArrayMidi instead")]]
inline void sdkSetEvtArrayMidi(UsineEventPtr e, const int index, const TUsineMidiCode midiValue)
{
    UserModuleBase::m_masterInfo->SetEvtArrayMidi(e, index, midiValue);
}

/// Get a Midi value to an array at index position.
[[maybe_unused]] [[deprecated("Deprecated: use UsineEventClass::getArrayMidi instead")]]
inline TUsineMidiCode sdkGetEvtArrayMidi(UsineEventPtr e, const int index)
{
    return UserModuleBase::m_masterInfo->GetEvtArrayMidi(e, index);
}

/// Get a 3D point.
/// the event must have a line number of 3 to work,
/// @param e the event
/// @param pointIndex is the index of the point you want to get
[[maybe_unused]] [[deprecated("Deprecated: use UsineEventClass::get3DPoint instead")]]
inline T3DPointF sdkGetEvt3DPoint(const UsineEventPtr e, const int pointIndex)
{
    T3DPointF result;
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
/// @}


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

#include "UserModule.h"
#include "UsineDefinitions.h"

constexpr int PUSH_GRANULARITY = 128;
//-----------------------------------------------------------------------------
// former module functions
//-----------------------------------------------------------------------------

/// Accessors to m_masterInfo
inline const TMasterInfo *sdkGetMasterInfo() { return UserModuleBase::sdkGetMasterInfo(); }

inline void sdkSetMasterInfo(TMasterInfo *m_masterInfo) { UserModuleBase::sdkSetMasterInfo(m_masterInfo); }

/// @addtogroup UsineEvents 
/// @{

//----------------------------------------------------------------------------
/// @class UsineEventClass

/// @brief Usine event class, wrapper around UsineEventPtr. Commonly referred to as an event

/// The main data holding class of Usine's CPP SDK, its inner data is composed of a TPrecision (aka float 32)
/// pointer. In many ways this class behaves like an array but is also very often use with one only element as a regular
/// float.
/// This class also provide many event manipulation and getter functions.
/// It is used to communicate with Usine, to send and receive data from Usine.
/// It is the main data structure used in Usine to store and manipulate data.   
/// 
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
	/// The contained event will be set to nullptr.
    [[maybe_unused]]
    void destroyEvent()
    {
        UserModuleBase::m_masterInfo->DestroyEvt(p_event_);
        p_event_ = nullptr;
    }

    /// Allocate a new event and point to it.
    /// @note The former contained event will not be deleted.
    /// @see sdkCreateEvent.
    [[maybe_unused]]
    void createEvent(const int size)
    {
        UserModuleBase::m_masterInfo->CreateEvt(p_event_, size);
    }

    /// @name Events manipulation
    /// @{

    /// Compare events.
	/// @param e The event to compare with.
	/// @return TRUE if the two events are equal, FALSE otherwise.
    [[maybe_unused]] bool compare(const UsineEventClass &e) const
    {
        return UserModuleBase::m_masterInfo->CompareEvt(p_event_, e.p_event_);
    }

    /// Copy events, copy the source event into this one.
	/// @param src The source event to copy from.
    [[maybe_unused]]
    void copyfrom(const UsineEventClass &src) const { UserModuleBase::m_masterInfo->CopyEvt(src.p_event_, p_event_); }

    /// Copy events, copy to the dest event.
	/// @param dest The destination event to copy to.
    [[maybe_unused]]
    void copyto(const UsineEventClass &dest) const { UserModuleBase::m_masterInfo->CopyEvt(p_event_, dest.p_event_); }

    /// Append the other event to this one.
	/// @note This operation is equivalent to sdkConcatEvt.
	/// @param e The event to append to this one.
    [[maybe_unused]]
    void concat(const UsineEventClass &e) const
    {
        UserModuleBase::m_masterInfo->ConcatEvt(p_event_, e.p_event_, p_event_);
    }

    /// Shift the values of this event by n.
    /// Right shift for n > 0, left shift for n < 0.
	/// @param n The number of elements to shift.   
    [[maybe_unused]]
    void shift(const int n) const { UserModuleBase::m_masterInfo->ShiftEvt(n, p_event_, p_event_); }

    /// Set the size of this event (number of elements). Typically the size of audio events is given by @ref sdkGetBlocSize .
	/// @note This function can be used to resize the event.
	/// @param size The new size of the event.
    [[maybe_unused]]
    void setSize(const int size) const { UserModuleBase::m_masterInfo->SetEvtSize(p_event_, size); }

    /// Get the size of this event (number of elements).
	/// @note Typically the size of audio events is given by @ref sdkGetBlocSize.
	/// @return The size of the event.
    [[maybe_unused]]
    int getSize() const { return UserModuleBase::m_masterInfo->GetEvtSize(p_event_); }

    /// Set the maximum size allowed by this event. Can be greater than the event size.
	/// @note This function can be used to set the maximum size of the event.
	/// @param size The maximum size of the event.
    [[maybe_unused]]
    void setMaxSize(const int size) const { UserModuleBase::m_masterInfo->SetEvtMaxSize(p_event_, size); }

    /// Add the other event to this one.
	/// @note This operation is equivalent to sdkAddEvt2
	/// @param e The event to add to this one.
    [[maybe_unused]]
    void add(const UsineEventClass &e) const { UserModuleBase::m_masterInfo->AddEvt2(e.p_event_, p_event_); }

    /// Subtract the other event to this one.
	/// @note This operation is equivalent to sdkSubEvt2.
	/// @param e The event to subtract from this one.
    [[maybe_unused]]
    void sub(const UsineEventClass &e) const { UserModuleBase::m_masterInfo->SubEvt2(e.p_event_, p_event_); }

    /// Divide this event by the other.
	/// @note This operation is equivalent to sdkDivEvt3.
	/// @param e The event to divide this one by.
    [[maybe_unused]]
    void div(const UsineEventClass &e) const { UserModuleBase::m_masterInfo->DivEvt3(p_event_, e.p_event_, p_event_); }

    /// Multiply this event by a float value.
	/// @note This operation is equivalent to sdkMultEvt1.
    /// @param value The value to multiply this event by.
    [[maybe_unused]]
    void mult(const TPrecision value) const { UserModuleBase::m_masterInfo->MultEvt1(value, p_event_); }

    /// Multiply this event by the other.
	/// @note This operation is equivalent to sdkMultEvt2.
	/// @param e The event to multiply this one by.
    [[maybe_unused]]
    void mult(const UsineEventClass &e) const { UserModuleBase::m_masterInfo->MultEvt2(e.p_event_, p_event_); }

    /// Compute the modulo of this event by the other one and store the result inplace.
	// @note This operation is equivalent to sdkModEvt3.
	/// @param e The event to compute the modulo with.
    [[maybe_unused]]
    void mod(const UsineEventClass &e) const { UserModuleBase::m_masterInfo->ModEvt3(p_event_, e.p_event_, p_event_); }

    /// Compute the power of this event by the other one and store the result inplace.
	/// @note This operation is equivalent to sdkPowerEvt3.
	/// @param e The event to compute the power with.
    [[maybe_unused]]
    void power(const UsineEventClass &e) const
    {
        UserModuleBase::m_masterInfo->PowerEvt3(p_event_, e.p_event_, p_event_);
    }

    /// Multiply this event by the other but for audio.
	/// @note This operation is equivalent to sdkMultEvt2Audio.
	/// @param e The event to multiply this one by. Must be an audio event with size == sdkGetBlocSize.
    [[maybe_unused]]
    void multAudio(const UsineEventClass &e) const
    {
        UserModuleBase::m_masterInfo->MultEvt2Audio(e.p_event_, p_event_);
    }

    /// Exponential of this event inplace.
	/// @note This operation is equivalent to sdkExpEvt1.
    [[maybe_unused]]
    void exp() const { UserModuleBase::m_masterInfo->ExpEvt1(p_event_); }

    /// Square root of this event inplace.
	/// @note This operation is equivalent to sdkSqrtEvt1.
    [[maybe_unused]]
    void sqrt() const { UserModuleBase::m_masterInfo->SqrtEvt1(p_event_); }

    /// Get the maximum value of this event.
	/// @note This operation is equivalent to sdkMaxEvt1.
	/// @return The maximum value of the event.
    [[maybe_unused]]
    TPrecision max() const { return UserModuleBase::m_masterInfo->MaxEvt1(p_event_); }

    /// Get the minimum value of this event.
	/// @note This operation is equivalent to sdkMinEvt1.
	/// @return The minimum value of the event.
    [[maybe_unused]]
    TPrecision min() const { return UserModuleBase::m_masterInfo->MinEvt1(p_event_); }

    /// Clip values of this event in the range [min..max] and store the result into dest.
	/// @note This operation is equivalent to sdkThreshEvt2.
	/// @param min The minimum value of the range.
	/// @param max The maximum value of the range.
    [[maybe_unused]]
    void tresh(const TPrecision min, const TPrecision max) const
    {
        UserModuleBase::m_masterInfo->ThreshEvt1(p_event_, min, max);
    }

    /// Mix MIDI values of the other event and this one and store the result inplace. For MIDI, this operation is equivalent to sdkConcatEvt
	/// @param e The MIDI event to mix with this one.
	/// @note This function is used to mix MIDI events, it will not work with audio events.
    [[maybe_unused]]
    void mixMidi(const UsineEventClass &e) const
    {
        UserModuleBase::m_masterInfo->MixMidiEvt2(static_cast<UsineEventPtr>(e), p_event_);
    }

    /// Apply a short fadein to this audio event. The duration of the fade is given by UserModuleBase::m_masterInfo->GetBlocSize.
	/// @note This function is used to apply a fade-in effect to audio events, it will not work with MIDI events.
    [[maybe_unused]]
    void fadeIn() const { UserModuleBase::m_masterInfo->FadeInAudioEvt(p_event_); }

    /// Apply a short fadeout to this audio event. The duration of the fade is given by UserModuleBase::m_masterInfo->GetBlocSize.
	/// @note This function is used to apply a fade-out effect to audio events, it will not work with MIDI events.
    [[maybe_unused]]
    void fadeOut() const { UserModuleBase::m_masterInfo->FadeOutAudioEvt(p_event_); }

    /// Clear this audio event. Set values to zero and add a small denormalization value.
	/// @note This function is used to clear audio events, it will not work with MIDI events.
    [[maybe_unused]]
    void clearAudio() const { UserModuleBase::m_masterInfo->ClearAudioEvt(p_event_); }

    /// Add a small denormalization value to this audio event.
	/// @note This function is used to denormalize audio events, it will not work with MIDI events.
    [[maybe_unused]]
    void denormalizeAudio() const { UserModuleBase::m_masterInfo->DenormalizeAudioEvt(p_event_); }

    /// Smooth this event.
	/// @note This function will smooth the event value with a factor.
	/// @param oldValue The value to smooth.
	/// @param factor The smoothing factor, typically between 0.0 and 1.0. @see SmoothFactor
    [[maybe_unused]]
    void smooth(TPrecision &oldValue, const TPrecision factor) const
    {
        UserModuleBase::m_masterInfo->SmoothPrecision(oldValue, p_event_, factor);
    }

    /// Smooth this event with a target value.
	/// @note This function will smooth the event value with a target and a factor.
	/// @param oldValue The value to smooth.
	/// @param target The target value to smooth towards.
	/// @param factor The smoothing factor, typically between 0.0 and 1.0. @see SmoothFactor
    [[maybe_unused]]
    void smooth(TPrecision &oldValue, const TPrecision target, const TPrecision factor) const
    {
        UserModuleBase::m_masterInfo->SmoothEvent(oldValue, p_event_, target, factor);
    }
    /// @}

    /// @name Access to Event values
    /// @{

    /// Set a Text value.
	/// @note This function will set the text value of the event.
	/// @param s The text value to set.
    [[maybe_unused]]
    void setPChar(const AnsiCharPtr s) const { UserModuleBase::m_masterInfo->SetEvtPChar(p_event_, s); }

    /// Get a Text value.
	/// @note This function will return the text value of the event.
	/// @return The text value of the event.
    [[maybe_unused]]
    const char *getPChar() const { return UserModuleBase::m_masterInfo->GetEvtPChar(p_event_); }

    /// Get the address of the first data in this Event
	/// @note This function is used to get the address of the data in the event.
	/// @return A pointer to the first data in the event.
    [[maybe_unused]]
    TPrecision *getDataAddr() const
    {
        return UserModuleBase::m_masterInfo->GetEvtDataAddr(p_event_);
    }

    /// Set a float value
	/// @note This function is used to set the data of the event.
	/// @param value The value to set.
    [[maybe_unused]]
    void setData(const TPrecision value) const { UserModuleBase::m_masterInfo->SetEvtData(p_event_, value); }

    /// Get a float value
	/// @note This function is used to get the data of the event.
	/// @return The first value of the event.
    [[maybe_unused]]
    TPrecision getData() const { return UserModuleBase::m_masterInfo->GetEvtData(p_event_); }

    /// Set a value at an index position.
	/// @note If the index is greater than the current size, it will be set to the end of the event.
	/// @param index The index position in the array where the value will be set.
	/// @param value The value to set at the index position.
    [[maybe_unused]]
    void setArrayData(const int index, const TPrecision value) const
    {
        UserModuleBase::m_masterInfo->SetEvtArrayData(p_event_, index, value);
    }

    /// Push (append) a value to this event.
	/// @note If the event is full, it will be resized by PUSH_GRANULARITY.
	/// @param value The value to push to the array.
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
	/// @note If the index is greater than the current size, it will return 0.0f.
	/// @param index The index position in the array from which the value will be retrieved.
    [[maybe_unused]]
    TPrecision getArrayData(const int index) const
    {
        return UserModuleBase::m_masterInfo->GetEvtArrayData(p_event_, index);
    }

    /*
    /// Set the pointer of the contained Event Pointer.
	/// This pointer can be used to exchange data between Usine and the module.
    [[maybe_unused]]
    void setEventPointer(void *pointerValue) const
    {
        UserModuleBase::m_masterInfo->SetEvtPointer(p_event_, pointerValue);
    }

    /// Get the pointer of the contained Event Pointer .
	/// This pointer can be used to exchange data between Usine and the module.
    [[maybe_unused]]
    const void *getEventPointer() const { return UserModuleBase::m_masterInfo->GetEvtPointer(p_event_); }
    */

    /// Set a color value.
	/// @param colorValue The color value to set.
    [[maybe_unused]]
    void setColor(const TUsineColor colorValue) const
    {
        UserModuleBase::m_masterInfo->SetEvtColor(p_event_, colorValue);
    }

    /// Get a color value.
	/// @note This function will return the color value of the event.
	/// @return The color value of the event.
    [[maybe_unused]]
    TUsineColor getColor() const { return UserModuleBase::m_masterInfo->GetEvtColor(p_event_); }

    /// Set a color value at an index position.
	/// @param index The index position in the array where the color value will be set.
	/// @param colorValue The color value to set at the index position.
	/// @note If the index is greater than the current size, it will be set to the end of the event.
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
	/// @note If the index is greater than the current size, it will return 0.
	/// @param index The index position in the array from which the color value will be retrieved.
	[[maybe_unused]]
    UINT32 getArrayColor(const int index) const
    {
        // sanity check
        const int idx = std::max(0, std::min(index, UserModuleBase::m_masterInfo->GetEvtSize(p_event_)));
        const auto *data = reinterpret_cast<TUsineColor *>(UserModuleBase::m_masterInfo->GetEvtDataAddr(p_event_));
        const TUsineColor result = *(data + idx);
        return result;
    }

    /// Get a Midi value at an index position.
	/// @note If the index is greater than the current size, it will return 0.
	/// @param index The index position in the array from which the MIDI value will be retrieved.
	/// @param midiValue The MIDI value to set at the index position.
	[[maybe_unused]]
    void setArrayMidi(const int index, const TUsineMidiCode midiValue) const
    {
        UserModuleBase::m_masterInfo->SetEvtArrayMidi(p_event_, index, midiValue);
    }

    /// Get a Midi value at an index position.
	/// param index The index position in the array where the MIDI value will be get.
	/// @note If the index is greater than the current size, it will be set to the end of the event.
    [[maybe_unused]]
    TUsineMidiCode getArrayMidi(const int index) const
    {
        return UserModuleBase::m_masterInfo->GetEvtArrayMidi(p_event_, index);
    }

    /// Get a 3D point.
    /// @note this event must have a size multiple of 3 to work,
    /// @param pointIndex is the index of the point you want to get
	/// @return a T3DPointF structure with the x, y and z coordinates of the point.
    [[maybe_unused]]
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

    //-----------------------------------------------------------------------------
    /// @name PointerData
    /// PointerData's are used to exchange data between Usine and the module
    /// @{
	
    /// Get the pointer data.
	/// @note This pointer can be used to exchange data between Usine and the module.
	/// @return A pointer to the pointer data.
    [[maybe_unused]]
    TUsinePtr getPointerData() const
    {       
        return UserModuleBase::m_masterInfo->GetPointerData(p_event_);
    }
    
	/// Get the pointer data size.
	/// @note The pointer data size is the number of Bytes in the pointer data.
	/// @return The size of the pointer data in Bytes.
	[[maybe_unused]]
    UINT32 getPointerDataSize() const
    {
        return UserModuleBase::m_masterInfo->GetPointerDataSize(p_event_);
    }
    
	/// Set the pointer data size.                
	/// @note The pointer data size is the number of Bytes in the pointer data.
	/// @param size The new size of the pointer data.
    [[maybe_unused]]
    void setPointerDataSize(UINT32 size) const
    {
         UserModuleBase::m_masterInfo->SetPointerDataSize(p_event_,size);
    }

    /// Get the pointer data type. 
    /// @note This function is used to get the type of the pointer data.
	/// @return The type of the pointer data, @see UsinePointerTypes.
    [[maybe_unused]]
    UINT32 getPointerDataType() const
    {
        return UserModuleBase::m_masterInfo->GetPointerDataType(p_event_);
    }

    /// Informs Usine that the pointer data has changed.
	/// @note This function should be called when the pointer data has changed.
    [[maybe_unused]]
   void setPointerDataChanged() const
    {
        UserModuleBase::m_masterInfo->SetPointerDataChanged(p_event_);
    }
   /// @}



    operator UsineEventPtr() const { return p_event_; }

    /// Used in onGetParamInfo, declared explicit to avoid conflict
    explicit operator UsineEventPtr *() { return &p_event_; }

private:
    /// The contained UsineEventPtr.
    UsineEvent *p_event_ = nullptr;
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

#elif(USINE_WIN64)

inline void TParamInfo::setEventClass(UsineEventClass &e)
{
// disable deprecation warning for this line only.
#pragma warning(suppress : 4996)
    this->EventPtr = &e.event();
}

#endif // defined(__GNUC__)



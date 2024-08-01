///-----------------------------------------------------------------------------
///@file  
///	UsineDefinitions.h
///
///@author
///	BrainModular team
///
///@brief  
///	Datas and functions declarations exposed by Usine.
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
///  support@brainmodular.com 
///
/// All dependencies are under there own licence.
///
///@LICENSE
/// Copyright (C) 2023 BrainModular, BeSpline, Adamson
/// 
///-----------------------------------------------------------------------------

// include once, no more
#pragma once

//-----------------------------------------------------------------------------
// target platform preprocessor define
//-----------------------------------------------------------------------------
#if defined (_WIN64)
    #define USINE_WIN64 1
#elif defined (__APPLE_CPP__) || defined(__APPLE_CC__)
    #define USINE_OSX64 1
#elif defined (__linux__)
    #define USINE_LIN64 1
#else
  #error "Unknown platform!"
#endif

// platform specific defines
//-----------------------------------------------------------------------------
#if (defined (USINE_WIN64))
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#elif (defined (USINE_OSX64))
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#elif (defined (USINE_LIN64))
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#else
  #error "conditional compilation error!"
#endif
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Datatypes definitions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// for memory alignment with DELPHI
//-----------------------------------------------------------------------------
#if (defined (USINE_WIN64))
    #pragma pack(push, 4)
#elif (defined (USINE_OSX64))
    #pragma pack(push, 4)
#elif (defined (USINE_LIN64))
    #pragma pack(push, 4)
#else
  #error "conditional compilation error!"
#endif
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// version of the SDK
// to use in GetSDKVersion function to return the SDK version number
static constexpr int SDK_VERSION = 701007;


//-----------------------------------------------------------------------------
#if (defined (USINE_WIN64))
typedef  __int64 NativeInt;
typedef  unsigned __int64 NativeUInt;
typedef  unsigned __int64 UINT64;
typedef  __int64 Int64;
typedef  unsigned __int32 UINT32 ;

//-----------------------------------------------------------------------------
#elif (defined (USINE_OSX64) || defined (USINE_LIN64))
#include <stdint.h>
typedef  int64_t NativeInt;
typedef  uint64_t NativeUInt;
typedef  int64_t Int64;
typedef  uint64_t UINT64;
typedef  uint32_t UINT32 ;

//-----------------------------------------------------------------------------
#else
  #error "conditional compilation error!"
#endif
//-----------------------------------------------------------------------------
/// @addtogroup Datatypes
/// @{

/// Number of audio in or out channels for modules
#define USINE_MULTIPHONY_MAX 144
/// Max number of input audio channels for the sound card
#define USINE_MAX_AUDIO_INPUTS 256
/// Max number of output audio channels for the sound card
#define USINE_MAX_AUDIO_OUTPUTS 256
/// Max size of Audio Blocs in Usine
#define USINE_MAX_AUDIO_EVT_SIZE 1024  
/// Max number of MIDI devices connected
#define USINE_MAX_MIDI_DEVICES 64

//-----------------------------------------------------------------------------
/// Custom bool type to match the 4 bytes DELPHI LongBool type.
typedef int LongBool;

/// Pointer to a LongBool variable.
typedef LongBool* LongBoolPtr;

/// To test LongBool equality against false. @see LongBool
#define FALSE 0 

/// To test LongBool equality against true. @see LongBool 
#define TRUE  1 

//-----------------------------------------------------------------------------
/// Pointer to an ANSI string variable.
/// Strings format conformance between CPP and DELPHI
typedef const char* AnsiCharPtr;

/// Type used in all parameters data.
typedef float TPrecision;

#if (defined (USINE_WIN64))
typedef unsigned char BYTE;
#elif (defined (USINE_OSX64))
typedef uint8_t BYTE;
#endif


//----------------------------------------------------------------------------
/// Usine internal message 
/// @see onCallback
typedef struct TUsineMessage 
{
    NativeInt message; ///< the parameter value has changed
    NativeInt wParam;  ///< the parameter value has changed
    NativeInt lParam;  ///< @ref lParam "TUsineMessage.lParam possible values"
    NativeInt result;  ///< the parameter value has changed
} TUsineMessage;

/// for backward compatibility
typedef TUsineMessage UsineMessage;



//----------------------------------------------------------------------------
/// Possible values for TUsineMessage LParam.
static constexpr NativeInt MSG_CHANGE     = 0;  ///< the parameter value has changed
static constexpr NativeInt MSG_CLICK      = 1;  ///< the parameter has been clicked
static constexpr NativeInt MSG_DBLCLICK   = 2;  ///< the parameter has been double clicked
static constexpr NativeInt MSG_SETCAPTION = 3;  ///< change the caption of a control
static constexpr NativeInt MSG_MOUSEUP    = 4;  ///< mouse Up
static constexpr NativeInt MSG_MOUSEMOVE  = 5;  ///< mouse Move
static constexpr NativeInt MSG_DROP       = 6;  ///< something has been dropped on the control
static constexpr NativeInt MSG_COMMATEXT  = 7;  ///< a comma-text has changed
static constexpr NativeInt MSG_FLOATING_MOVE        = 8;  ///< a floating control is moving
static constexpr NativeInt MSG_COLLIDE              = 9;  ///< a floating control collides
static constexpr NativeInt MSG_LOCK_CHANGED         = 10; ///< the control lock state has changed
static constexpr NativeInt MSG_PLAY_RECORD_VALUE    = 11; ///< an automation is playing on the parameter
static constexpr NativeInt MSG_RIGHT_CLICK          = 12; ///< mouse right click
static constexpr NativeInt MSG_WHEEL_UP             = 13; ///< mouse wheel up
static constexpr NativeInt MSG_WHEEL_DOWN           = 14; ///< mouse wheel down
static constexpr NativeInt MSG_MOUSE_WHEEL_OFF      = 15; ///< end wheel processing

//----------------------------------------------------------------------------
/// Possible target for a notification to Usine @see sdkNotifyUsine
static constexpr NativeInt NOTIFY_TARGET_USINE        = 1;  ///< notify the main Usine panel
static constexpr NativeInt NOTIFY_TARGET_SETUP        = 2;  ///< notify the setup of Usine
static constexpr NativeInt NOTIFY_TARGET_USER_MODULE  = 3;  ///< notify the current User module

//----------------------------------------------------------------------------
/// Possible message for a notification to Usine @see sdkNotifyUsine
static constexpr NativeInt NOTIFY_MSG_AUDIO_DRIVER_CHANGED	= 0xFAB000;  ///< no param
static constexpr NativeInt NOTIFY_MSG_MIDI_DRIVER_CHANGED	= 0xFAB001;  ///< no param
static constexpr NativeInt NOTIFY_MSG_SAMPLE_RATE_CHANGED	= 0xFAB002;  ///< param1 = samplerate
static constexpr NativeInt NOTIFY_MSG_BLOC_SIZE_CHANGED		= 0xFAB003;  ///< param1 = blocsize
static constexpr NativeInt NOTIFY_MSG_RESCAN_PLUGINS_DONE	= 0xFAB004;  ///< no param
static constexpr NativeInt NOTIFY_MSG_RECREATE_CONTROLS		= 0xFAB005;  ///< no param
static constexpr NativeInt NOTIFY_MSG_KEY_DOWN		        = 0xFAB006;  ///< param1 = keycode, param2 = modifier
static constexpr NativeInt NOTIFY_MSG_KEY_UP		        = 0xFAB007;  ///< param
static constexpr NativeInt NOTIFY_MSG_FOCUS_LOST	        = 0xFAB008;  ///< no param

// used by usine at startup to notify devices (in onCallback ()) that the loaded phase is complete
static constexpr NativeInt NOTIFY_MSG_USINE_LOADED	        = 0xFAB009;  ///< param1 = 0, param2 = 0
static constexpr NativeInt NOTIFY_MSG_USINE_CALLBACK        = 0xFAB679;  ///< identify a Message->message as a callback for user module
static constexpr NativeInt NOTIFY_MSG_ON_TOP                = 0xFAB67A;  ///< need all windows on top
static constexpr NativeInt NOTIFY_MSG_TO_BACK               = 0xFAB67B;  ///< need all windows to back
static constexpr NativeInt NOTIFY_MSG_RESCAN_MIDI_DEVICES   = 0xFAB67D;  ///< tell Usine that Midi devices have been rescanned
static constexpr NativeInt NOTIFY_MSG_RESET_MIDI_DEVICES    = 0xFAB67E;  ///< tell Usine that Midi devices have been reset

static constexpr NativeInt CALLBACK_WPARAM_LIMIT = 0xF000000;



//----------------------------------------------------------------------------
/// keys state modifier.
/// @see MouseEventsCallbacks
typedef UINT32 TShiftState;

/// Possible value for a TShiftState variable.
/// @see onMouseMove, onMouseDown, onMouseUp, onMouseMoveMulti, onMouseDownMulti, onMouseUpMulti
static constexpr UINT32 ssShift  = 0x1;	///< Shift keyboard state
static constexpr UINT32 ssAlt    = 0x2;  ///< Alt keyboard state
static constexpr UINT32 ssCtrl   = 0x4;  ///< Ctrl keyboard state
static constexpr UINT32 ssLeft   = 0x8;  ///< Left mouse button state
static constexpr UINT32 ssRight  = 0x10; ///< Right mouse button state
static constexpr UINT32 ssMiddle = 0x20; ///< Middle mouse button state
static constexpr UINT32 ssDouble = 0x40; ///< Mouse Double click state

//----------------------------------------------------------------------------
/// Mouse buttons available for a mouse event callback.
/// @see onMouseMove, onMouseDown, onMouseUp, onMouseMoveMulti, onMouseDownMulti, onMouseUpMulti
typedef enum TMouseButton
{
    mbLeft,		///< Left mouse button
    mbRight,    ///< Right mouse button
    mbMiddle    ///< Middle mouse button
} TMouseButton;


//-----------------------------------------------------------------------------
/// Scale type available for a parameter.
/// @see TParamInfo
typedef enum TScale
{
    scLinear,   ///< Linear type of scale
    scLog,      ///< Logarithmic  type of scale
    scExp       ///< Exponential  type of scale
} TScale;

//-----------------------------------------------------------------------------
/// Usine native color format.
typedef UINT32 TUsineColor;

// for backward compatibility
typedef TUsineColor UsineColor;



//-----------------------------------------------------------------------------
/// handle to an audio file.
typedef void* TAudioFilePtr;

//-----------------------------------------------------------------------------
/// handle to a critical section.
typedef void* TCriticalSectionPtr;

//-----------------------------------------------------------------------------
/// handle to a thread.
typedef void* TThreadPtr;

/// Thread priorities.
typedef enum TThreadPriority
{
    tpIDLE,     ///< executed when the application is in IDLE state
    tpLow,      ///< low priority
    tpMedium,   ///< medium priority
    tpHigh      ///< time critical priority
} TThreadPriority;

//-----------------------------------------------------------------------------
/// Synchronization objects to synchronize threads.
typedef void* TSyncObjectPtr;


//-----------------------------------------------------------------------------
/// Color set used by Usine.
static constexpr UINT32 clg0 = 1;
static constexpr UINT32 clg1 = 2;
static constexpr UINT32 clg2 = 3;
static constexpr UINT32 clg3 = 4;
static constexpr UINT32 clg4 = 5;
static constexpr UINT32 clg5 = 6;
static constexpr UINT32 clg6 = 7;
static constexpr UINT32 clg7 = 8;
static constexpr UINT32 clg8 = 9;
static constexpr UINT32 clg9 = 10;
static constexpr UINT32 clg10 = 11;
static constexpr UINT32 clg11 = 12;
static constexpr UINT32 clWhite = 13;
static constexpr UINT32 cl0 = 14;
static constexpr UINT32 cl1 = 15;
static constexpr UINT32 cl2 = 16;
static constexpr UINT32 cl3 = 17;
static constexpr UINT32 cl4 = 18;
static constexpr UINT32 cl5 = 19;
static constexpr UINT32 cl6 = 20;
static constexpr UINT32 cl7 = 21;
static constexpr UINT32 cl8 = 22;
static constexpr UINT32 cl9 = 23;
static constexpr UINT32 cl10 = 24;
static constexpr UINT32 cl11 = 25;
static constexpr UINT32 cl12 = 26;
static constexpr UINT32 cl13 = 27;
static constexpr UINT32 cl14 = 28;
static constexpr UINT32 cl15 = 29;
static constexpr UINT32 cl16 = 30;
static constexpr UINT32 cl17 = 31;
static constexpr UINT32 cl18 = 32;
static constexpr UINT32 cl19 = 33;
static constexpr UINT32 cl20 = 34;
static constexpr UINT32 cl21 = 35;
static constexpr UINT32 cl22 = 36;
static constexpr UINT32 cl23 = 37;
static constexpr UINT32 clRed = 38;
static constexpr UINT32 clMainBack = 39;
static constexpr UINT32 clDarkPanelBack = 40;
static constexpr UINT32 clLiterPanelBack = 41;
static constexpr UINT32 clMoreLitePanelBack = 42;
static constexpr UINT32 clVeryLitePanelBack = 43;
static constexpr UINT32 clFontLight = 44;
static constexpr UINT32 clFontColored = 47;
static constexpr UINT32 clSwitchOnColored = 48;
static constexpr UINT32 clDataFlow = 50;
static constexpr UINT32 clMIDIFlow = 51;
static constexpr UINT32 clAudioFlow = 52;
static constexpr UINT32 clArrayFlow = 53;
static constexpr UINT32 clTextFlow = 54;
static constexpr UINT32 clColorFlow = 55;
static constexpr UINT32 clPointerFlow = 56;
static constexpr UINT32 clVuMeterMIDI = 57;
static constexpr UINT32 clVuMeterAudio = 58;
static constexpr UINT32 clBitwiseFlow = 59;
static constexpr UINT32 clVideoFlow = 60;
static constexpr UINT32 clSwitchFlow = 61;
static constexpr UINT32 clTriggerFlow = 62;
static constexpr UINT32 clListboxFlow = 63;
static constexpr UINT32 clSelected = 64;
static constexpr UINT32 clDragOver = 65;
static constexpr UINT32 clMouseOver = 66;
static constexpr UINT32 clMIDIlearning = 67;
static constexpr UINT32 clMIDILearnedIcon = 68;
static constexpr UINT32 clCursor = 71;
static constexpr UINT32 clInvalidLink = 72;
static constexpr UINT32 clInterfaceDesignModuleColor = 73;
static constexpr UINT32 clVideoModuleColor = 74;
static constexpr UINT32 clAudioModuleColor = 75;
static constexpr UINT32 clPluginModuleColor = 76;
static constexpr UINT32 clMIDIModuleColor = 77;
static constexpr UINT32 clEventModuleColor = 78;
static constexpr UINT32 clDataModuleColor = 79;
static constexpr UINT32 clArrayModuleColor = 80;
static constexpr UINT32 clMathModuleColor = 81;
static constexpr UINT32 clSubPatchModuleColor = 82;
static constexpr UINT32 clScriptModuleColor = 83;
static constexpr UINT32 clOSCModuleColor = 84;
static constexpr UINT32 clInterfaceCtrlModuleColor = 85;
static constexpr UINT32 clTextModuleColor = 86;
static constexpr UINT32 clFixtureModuleColor = 87;
static constexpr UINT32 clColorModuleColor = 88;
static constexpr UINT32 clNetworkModuleColor = 102;
static constexpr UINT32 clLiterAUDIOFlow = 89;
static constexpr UINT32 clLiterMIDIFlow = 90;
static constexpr UINT32 clLiterDATAFlow = 91;
static constexpr UINT32 clLiterARRAYFlow = 92;
static constexpr UINT32 clLiterTextFlow = 93;
static constexpr UINT32 clLiterColorFlow = 94;
static constexpr UINT32 clLiterPointerFlow = 95;
static constexpr UINT32 clLiterBitwiseFlow = 96;
static constexpr UINT32 clLiterVideoFlow = 97;
static constexpr UINT32 clLiterSwitchFlow = 98;
static constexpr UINT32 clLiterTriggerFlow = 99;
static constexpr UINT32 clLiterListboxFlow = 100;
static constexpr UINT32 clGridColor = 101;
static constexpr UINT32 clMenuForeGround = 103;
static constexpr UINT32 clMenuMouseOver = 104;
static constexpr UINT32 clHintsBack = 105;
static constexpr UINT32 clHintsFont = 106;

//-----------------------------------------------------------------------------
/// @brief Event associated to the parameter
struct UsineEvent;


/// @brief Handle to an Usine Event. Now deprecated for the final user, use @ref UsineEventClass instead.
typedef UsineEvent* UsineEventPtr;

//-----------------------------------------------------------------------------
/// parameters type available in usine.
/// @see TParamInfo
using TParamType = enum TParamType
{
    ptTextField,            ///< Param contain a string
    ptChooseColor,          ///< Color chooser parameter
    ptMidi,                 ///< MIDI parameter
    ptGainFader,            ///< Audio Gain parameter
    ptAudio,                ///< Audio input/output
    ptDataField,            ///< General data parameter without fader (only the value is displayed)
    ptDataFader,            ///< General data parameter with fader (can be modified with the mouse)
    ptButton,               ///< Button, trigger
    ptListBox,              ///< List box
    ptSwitch,               ///< Switch
    ptArray,                ///< Array parameter
    ptIpAddress,            ///< Designed to display IP addresses X.X.X.X
    ptSmpte,                ///< Designed to Display SMPTE code HH:MM:SS:FF
    ptMidiNoteFader,        ///< MIDI note number in the Range 0..127
    ptPointerBitMap,        ///< Video or Bitmap parameters
    ptPointer,              ///< General purpose pointer
    ptRightLed,             ///< Switch with a led on the right
    ptChooseFolder,         ///< Choose folder dialog box
    ptLeftLed,              ///< Switch with a led on the left
    ptTriggerLed,           ///< Switch with a led on the right dedicated to triggers
    ptFileName,             ///< Choose filename dialog box
    ptBitwise,              ///< Bitwise 32bits parameter
    ptOther                 ///< Not defined
};

//-----------------------------------------------------------------------------
/// Possible type of callback for a parameter.
/// @see TParamInfo
typedef enum TFastCallBackType
{
	ctNormal,       ///< Processed in the normal Usine messages handler. The latency is defined by the User in the Usine Setup.
	ctImmediate,    ///< Each time the parameter change, callback is processed immediately. Be careful, the the callBack procedure is called in the audio thread.
	ctAsynchronous  ///< Processed in the window message handler (slower than ctNormal). Use this if the process is very long or if you open a modal window.
} TFastCallBackType;

// Forward declaration
class UsineEventClass;

//-----------------------------------------------------------------------------
/// Usine parameter description.
/// Contain all the properties to define a parameter.
/// @see onGetParamInfo
struct TParamInfo
{
	TParamType        ParamType;            ///< parameter type
	AnsiCharPtr       Caption;              ///< displayed caption
	LongBool          IsInput;              ///< TRUE if the parameter has input port
	LongBool          IsOutput;             ///< TRUE if the parameter has output port
	TScale            Scale;                ///< scale, if the param is a fader
	TUsineColor       Color;                ///< front color of the fader or on Color of the switch
	TUsineColor       OffColor;             ///< off color only if the parameter is a switch
	TPrecision        MinValue;             ///< min param value
	TPrecision        MaxValue;             ///< max param value
	TPrecision        DefaultValue;         ///< default param value, when created
	AnsiCharPtr       Symbol;               ///< displayed symbol
	AnsiCharPtr       Format;               ///< format string of the displayed value
	AnsiCharPtr       ListBoxStrings;       ///< if listbox looks like '"item1","item2"'
	AnsiCharPtr       TextValue;            ///< default text value if param type = ptTextField
	LongBool          ReadOnly;             ///< TRUE if the user cant modify value
	TFastCallBackType CallBackType;         ///< see TFastCallBackType definition
	LongBool          DontSave;             ///< specifies if the parameter need to be saved or not
	int               DisplayOrder;         ///< <b>Deprecated</b> Parameters are now ordered according to their param index choosen in onGetParamInfo
	LongBool          IsSeparator;          ///< optional, determines if the parameter add a blank line above him in the list
	LongBool          IsInvisible;          ///< set the parameter totally invisible
	LongBool          IsStoredInPreset;     ///< set the param stored in the preset (default false)
	AnsiCharPtr       SavedName;            ///< optional: ident used to save the value in inifiles. if NULL Caption is used
	AnsiCharPtr	      SeparatorCaption;     ///< optional, if isSeparator,  show a section name
	NativeInt         CallBackId;           ///< the id of the param for the callback, setEvent and param (choose a value up to 0x00000FFE)
	LongBool		  IsVisibleByDefault;   ///< TRUE by default
	LongBool		  NotUsed;              ///< Not used anymore
	LongBool		  Translate;            ///< usine auto translate
	[[deprecated("Deprecated: use setEventClass(UsineEventClass& e) instead")]]
    UsineEventPtr*    EventPtr;             ///< Pointer to Event set by Usine to acces to the value of the parameter, introduced
											///< in HH3. Replace the deprecated SetEventAddress
	AnsiCharPtr       FileNameFilter;       ///< optional: filter used to open file when ParamType = ptFileName
											///< ie: "All files (*.*)|*.*" or "'All Images |*.png; *.jpg; *.jpeg; *.bmp; *.tiff; *.gif"
    void setEventClass(UsineEventClass&);   /// Bind the given event to

};
/// for backward compatibility
typedef TParamInfo ParamInfo;

//-----------------------------------------------------------------------------
/// Data type for video Frames pixels used in video modules
typedef struct TUsinePixel
{
#if (defined (USINE_WIN64))
    BYTE B; ///< Blue
    BYTE G; ///< Green
    BYTE R; ///< Red
    BYTE A; ///< Alpha, not used in video Frames
#elif (defined (USINE_OSX64))
    BYTE R; ///< Blue
    BYTE G; ///< Green
    BYTE B; ///< Red
    BYTE A; ///< Alpha, not used in video Frames
#endif
} TUsinePixel;

//-----------------------------------------------------------------------------
/// data type Pixels Pointer
typedef TUsinePixel* TUsinePixelPtr;

//-----------------------------------------------------------------------------
/// Data type for video Frames
typedef struct TUsineFrame
{
	int Width;  ///< Width of the frame in pixels
	int Height; ///< Height of the frame in pixels
	TUsinePixelPtr Pixels; ///< Pointer to the pixels array of Width * Height pixels
	LongBool Valid;      ///< flag, set as TRUE if the frame is valid
	void* intern;        ///< intern pointer, do not use
	void* notused1;      ///< No longer used
	void* notused2;      ///< No longer used

} TUsineFrame;

typedef TUsineFrame* TUsineFramePtr;

//-----------------------------------------------------------------------------
/// Data type for ILDA Frames
typedef struct TUsineILDAPoint
{
	float x;  ///< point x coordinate
	float y;  ///< point y coordinate
	float z;  ///< point z coordinate
	TUsineColor       Color; ///< point color
} TUsineILDAPoint;

//-----------------------------------------------------------------------------
/// Usine Midi code format.
typedef struct TUsineMidiCode
{
    unsigned char Channel;	///< Midi channel of the midi code
    unsigned char Msg;		///< Message part of the midi code
    unsigned char Data1;	///< Data1 part of the midi code
    unsigned char Data2;	///< Data2 of the midi code
} TUsineMidiCode;

/// for backward compatibility
typedef TUsineMidiCode UsineMidiCode;

/// Possible value for a TUsineMidiCode::Msg variable.
static constexpr unsigned char MIDI_ALLNOTESOFF     = 0x7B; ///< Midi msg
static constexpr unsigned char MIDI_NOTEON          = 0x90; ///< Midi msg
static constexpr unsigned char MIDI_NOTEOFF         = 0x80; ///< Midi msg
static constexpr unsigned char MIDI_KEYAFTERTOUCH   = 0xa0; ///< Midi msg
static constexpr unsigned char MIDI_CONTROLCHANGE   = 0xb0; ///< Midi msg
static constexpr unsigned char MIDI_PROGRAMCHANGE   = 0xc0; ///< Midi msg
static constexpr unsigned char MIDI_CHANAFTERTOUCH  = 0xd0; ///< Midi msg
static constexpr unsigned char MIDI_PITCHBEND       = 0xe0; ///< Midi msg
static constexpr unsigned char MIDI_SYSTEMMESSAGE   = 0xf0; ///< Midi msg
static constexpr unsigned char MIDI_BEGINSYSEX      = 0xf0; ///< Midi msg
static constexpr unsigned char MIDI_MTCQUARTERFRAME = 0xf1; ///< Midi msg
static constexpr unsigned char MIDI_SONGPOSPTR      = 0xf2; ///< Midi msg
static constexpr unsigned char MIDI_SONGSELECT      = 0xf3; ///< Midi msg
static constexpr unsigned char MIDI_ENDSYSEX        = 0xF7; ///< Midi msg
static constexpr unsigned char MIDI_TIMINGCLOCK     = 0xF8; ///< Midi msg
static constexpr unsigned char MIDI_START           = 0xFA; ///< Midi msg
static constexpr unsigned char MIDI_CONTINUE        = 0xFB; ///< Midi msg
static constexpr unsigned char MIDI_STOP            = 0xFC; ///< Midi msg
static constexpr unsigned char MIDI_ACTIVESENSING   = 0xFE; ///< Midi msg
static constexpr unsigned char MIDI_SYSTEMRESET     = 0xFF; ///< Midi msg


//-----------------------------------------------------------------------------
/// VST Time info structure as defined by the Steinberg VST SDK.
/// Used by Usine to deal with time at a musical point of view.
/// @see TMasterInfo, sdkGetVstTimeInfo
typedef struct TVstTimeInfo
{
    double samplePos;           ///< current location
    double sampleRate;          ///< current sample rate
    double nanoSeconds;         ///< system time
    double PpqPos;              ///< 1 ppq
    double tempo;               ///< in bpm
    double barStartPos;         ///< last bar start, in 1 ppq
    double cycleStartPos;       ///< 1 ppq
    double cycleEndPos;         ///< 1 ppq
    long   timeSigNumerator;    ///< time signature
    long   timeSigDenominator;  ///<
    long   smpteOffset;         ///<
    long   smpteFrameRate;      ///< 0:24, 1:25, 2:29.97, 3:30, 4:29.97 df, 5:30 df
    long   samplesToNextClock;  ///< midi clock resolution (24 ppq), can be negative
    long   flags;               ///<
    double cyclePpqPos;         ///< usine cycle ppq
} TVstTimeInfo;


//-----------------------------------------------------------------------------
/// Internal messages
static constexpr int PW_CMD_BLOC_CHANGED			= 1;
static constexpr int PW_CMD_SAMPLE_RATE_CHANGED		= 2;
static constexpr int PW_CMD_PARAM_VALUE				= 3;
static constexpr int PW_CMD_PARAM_DISPLAY_VALUE		= 26;
static constexpr int PW_CMD_CHUNK					= 4;
static constexpr int PW_CMD_SHOW_PLUGIN				= 5;
static constexpr int PW_CMD_HIDE_PLUGIN				= 6;
static constexpr int PW_CMD_TOGGLE_SHOW_HIDE_PLUGIN = 7;
static constexpr int PW_CMD_PROGRAM_NUM				= 8;
static constexpr int PW_CMD_CHUNK_REQUEST			= 9;
static constexpr int PW_CMD_KILL					= 10;
static constexpr int PW_CMD_PING					= 11;
static constexpr int PW_CMD_PLUGIN_NAME				= 12;
static constexpr int PW_CMD_NUM_CHANELS_IN			= 13;
static constexpr int PW_CMD_NUM_CHANELS_OUT			= 14;
static constexpr int PW_CMD_NUM_PARAMS				= 15;
static constexpr int PW_CMD_PARAM_NAME				= 16;
static constexpr int PW_CMD_SET_PARAM_VISIBLE		= 17;
static constexpr int PW_CMD_SET_ALL_PARAM_VISIBLE	= 18;
static constexpr int PW_CMD_PROGRAM_LIST			= 21;
static constexpr int PW_CMD_KEYPRESSED				= 22;
static constexpr int PW_CMD_READY					= 23;
static constexpr int PW_CMD_PONG					= 24;
static constexpr int PW_CMD_PROCESS					= 25;


//------------------------------------------------------------------
/// Internal plugin-lister scan mode
typedef enum TRescanPluginListerMode
{
	  rsmVerify
	, rsmAll
	, rsmChanges
} TRescanPluginListerMode;

/// Internal flag for the trace window.
static constexpr int US_CMD_TRACE					= 0xFA;
static constexpr int US_CMD_TRACE_ERROR				= 0xFB;
static constexpr int US_CMD_TRACE_WARNING			= 0xFC;
static constexpr int US_CMD_TRACE_PROGRESSFORM		= 0xFD;
static constexpr int US_CMD_TRACE_LOG				= 0xFE;
static constexpr int US_FLAG_NOTIFY					= 0xFF;
static constexpr int PL_CMD_RESCAN_PLUGINS_DONE		= 31;
static constexpr int PL_CMD_RESCANING				= 32;


//-----------------------------------------------------------------------------
/// Internal Command structure
typedef struct TCommandPacket
{
	union
	{
		struct
		{
			int cmd;
			int intValue1;
			int intValue2;
			float floatValue;
			char textValue[64];
		};

		struct
		{
			int blocksize;
			int sizes[USINE_MULTIPHONY_MAX];
			int nbMidi;
			int processCounter;
			TVstTimeInfo timeInfo;
		};
	};
} TCommandPacket;




//-----------------------------------------------------------------------------
/// Dialogs popup results
/// The value returned by a popup
/// @see UserModuleBase::sdkDialogConfirmationYesNoCancel, UserModuleBase::sdkDialogConfirmationYesNo, UserModuleBase::sdkDialogInformationOk, UserModuleBase::sdkDialogConfirmationOKCancel
typedef enum TDialogsResults
{
	  idOk       = 1
	, idCancel   = 2
	, idAbort    = 3
	, idRetry    = 4
	, idIgnore   = 5
	, idYes      = 6
	, idNo       = 7
	, idClose    = 8
	, idHelp     = 9
	, idTryAgain = 10
	, idContinue = 11
	, mrNone     = 0
	, mrOk       = idOk
	, mrCancel   = idCancel
	, mrAbort    = idAbort
	, mrRetry    = idRetry
	, mrIgnore   = idIgnore
	, mrYes      = idYes
	, mrNo       = idNo
	, mrClose    = idClose
	, mrHelp     = idHelp
	, mrTryAgain = idTryAgain
	, mrContinue = idContinue
	, mrAll      = mrContinue + 1
	, mrNoToAll  = mrAll + 1
	, mrYesToAll = mrNoToAll + 1
} TDialogsResults;


//-----------------------------------------------------------------------------
/// To store 2D coordinates in coefficient (from 0 to 1).
typedef struct TPointF
{
    float x;  ///< X Coordinates
    float y;  ///< Y coordinates
} TPointF;

/// To store an array of 2D coordinates.
typedef TPointF* TPointFPtr;

/// To store 3D coordinates in coefficient (from 0 to 1).
typedef struct T3DPointF
{
    float x; ///< X Coordinates
    float y; ///< Y Coordinates
    float z; ///< Z Coordinates
} T3DPointF;

/// To store an array of 3D coordinates.
typedef T3DPointF* T3DPointFPtr;

/// To store Rectangle coordinates in coefficient (from 0 to 1).
struct TRectF
{
    float left;    ///< left position
    float top;     ///< top position
    float right;   ///< right position
    float bottom;  ///< bottom position
};

/// Text vertical alignement.
/// @see sdkFillText
typedef enum TTextAlign
{
    taCenter = 0, ///< centered
    taLeading,    ///< on the top
    taTrailing   ///< at the bottom
} TTextAlign;

// forward declarations
struct TMasterInfo;
struct TModuleInfo;

//-----------------------------------------------------------------------------
/// Settings Panel Tab's name
/// @see sdkAddSettingLineCaption ...
static const auto PROPERTIES_TAB_NAME	= "tab_properties";	///< Used to populate the properties tab of the Settings panel
static const auto DESIGN_TAB_NAME        = "tab_design";		///< Used to populate the design tab of the Settings panel
static const auto MOUSE_TAB_NAME         = "tab_mouse";		///< Used to populate the mouse tab of the Settings panel
static const auto OTHER_TAB_NAME			= "tab_other";		///< Used to populate the other tab of the Settings panel
static const auto REMOTE_TAB_NAME		= "tab_remote";		///< Used to populate the remote tab of the Settings panel
static const auto CURVES_TAB_NAME		= "tab_curves";		///< Used to populate the curves tab of the Settings panel
static const auto LAN_TAB_NAME		    = "tab_lan";		///< Used to populate the lan tab of the Settings panel
static const auto SIZE_TAB_NAME          = "tab_size";	    ///< Used to populate the size tab of the Settings panel

/// Numeric format
/// @see sdkAddSettingLineSingle ...
static const auto DEFAULT_FORMAT_FLOAT_3 = "%.3f"; ///< Default format for a 3 decimals number.
static const auto DEFAULT_FORMAT_FLOAT_2 = "%.2f"; ///< Default format for a 2 decimals number.
static const auto DEFAULT_FORMAT_FLOAT_1 = "%.1f"; ///< Default format for a 1 decimal number.
static const auto DEFAULT_FORMAT_INTEGER  = "%.0f"; ///<  Default format for an integer number.
static const auto DEFAULT_FORMAT_GENERAL  = "%g";   ///<  Default format for a string.

/// smooth factor used in smoothing functions
/// @see sdkSmoothPrecision sdkSmoothEvent
static constexpr TPrecision SMOOTH_VERY_VERY_SLOW  = 0.99995f;
static constexpr TPrecision SMOOTH_VERY_SLOW       = 0.9999f;
static constexpr TPrecision SMOOTH_SLOW            = 0.9995f;
static constexpr TPrecision SMOOTH_MED_SLOW        = 0.999f;
static constexpr TPrecision SMOOTH_FAST            = 0.99f;
static constexpr TPrecision SMOOTH_VERY_FAST       = 0.9f;
static constexpr TPrecision SMOOTH_ULTRA_FAST      = 0.5f;
static constexpr TPrecision SMOOTH_NO_SMOOTH       = 0.f;

//-----------------------------------------------------------------------------
// functions pointers used in TMasterInfo structure (see below)
//-----------------------------------------------------------------------------
// Function pointer for a dialog box
typedef int (*FuncDialogBox)	(AnsiCharPtr msg);
typedef AnsiCharPtr (*FuncDialogInputBox)(AnsiCharPtr caption, AnsiCharPtr prompt, AnsiCharPtr defaultValue);

// layouts
typedef void (*FuncRepaintPanel)			(TModuleInfo* pModuleInfo);
typedef void (*FuncAddSettingLineCaption)	(TModuleInfo* pModuleInfo, AnsiCharPtr tab, AnsiCharPtr caption, TUsineColor color, LongBool Translate);
typedef void (*FuncAddSettingLineColor)		(TModuleInfo* pModuleInfo, AnsiCharPtr tab, TUsineColor* pVal, AnsiCharPtr Caption, LongBool Translate);
typedef void (*FuncAddSettingLineBoolean)	(TModuleInfo* pModuleInfo, AnsiCharPtr tab, LongBool* pVal, AnsiCharPtr Caption, LongBool Translate);
typedef void (*FuncAddSettingLineInteger)	(TModuleInfo* pModuleInfo, AnsiCharPtr tab, int* pVal, AnsiCharPtr caption, int min, int max, TScale scale, AnsiCharPtr symbol, int defaultValue, LongBool Translate);
typedef void (*FuncAddSettingLineSingle)	(TModuleInfo* pModuleInfo, AnsiCharPtr tab, float* pVal, AnsiCharPtr caption, float min, float max, TScale scale, AnsiCharPtr symbol, AnsiCharPtr format, float defaultValue, LongBool Translate);
typedef void (*FuncAddSettingLineCombobox)	(TModuleInfo* pModuleInfo, AnsiCharPtr tab, int* pVal, AnsiCharPtr caption, AnsiCharPtr commaText, LongBool Translate);
typedef void (*FuncAddSettingsLineString)	(TModuleInfo* pModuleInfo, AnsiCharPtr tab, AnsiCharPtr pVal, AnsiCharPtr caption, LongBool Translate);

// internal messages
typedef void (*FuncSendMessage)    (TModuleInfo* pModuleInfo, AnsiCharPtr Msg);

// events manipulation
typedef bool (*FuncCompareEvt)     (UsineEventPtr e1, UsineEventPtr e2);
typedef void (*FuncCopyEvt)        (UsineEventPtr scr, UsineEventPtr dest );
typedef void (*FuncConcatEvt)      (UsineEventPtr i1, UsineEventPtr i2, UsineEventPtr o);
typedef void (*FuncShiftEvt)       (int n, UsineEventPtr input, UsineEventPtr output);
typedef void (*FuncSetEvtSize)     (UsineEventPtr ev, int size);
typedef int  (*FuncGetEvtSize)     (UsineEventPtr ev);
typedef void (*FuncSetEvtMaxSize)  (UsineEventPtr ev, int size);
typedef int  (*FuncGetEvtMaxSize)  (UsineEventPtr ev);
typedef void (*FuncDestroyEvt)     (UsineEventPtr &ev);
typedef void (*FuncCreateEvt)      (UsineEventPtr &ev, int originalsize);
typedef void (*FuncMoveBLOC32)     (TPrecision* src, TPrecision* dest, int size);

typedef void (*FuncNotImplemented) ();
// access to evt values
typedef void            (*FuncSetEvtData)          (UsineEventPtr ev, TPrecision value);
typedef TPrecision      (*FuncGetEvtData)          (UsineEventPtr ev);
typedef void            (*FuncSetEvtArrayData)     (UsineEventPtr ev, int idx, TPrecision value);
typedef TPrecision      (*FuncGetEvtArrayData)     (UsineEventPtr ev, int idx);
typedef void            (*FuncSetEvtPointer)       (UsineEventPtr ev, void* value);            
typedef void*           (*FuncGetEvtPointer)       (UsineEventPtr ev);   
typedef void            (*FuncSetEvtColor)         (UsineEventPtr ev, TUsineColor value);            
typedef TUsineColor     (*FuncGetEvtColor)         (UsineEventPtr ev);                        
typedef void            (*FuncSetEvtArrayMidi)     (UsineEventPtr ev, int idx, TUsineMidiCode midi);
typedef TUsineMidiCode  (*FuncGetEvtArrayMidi)     (UsineEventPtr ev, int idx);
typedef void            (*FuncSetEvtPChar)         (UsineEventPtr ev, AnsiCharPtr delphistring);
typedef AnsiCharPtr     (*FuncGetEvtPChar)         (UsineEventPtr ev);
typedef TPrecision*     (*FuncGetEvtDataAddr)      (UsineEventPtr ev);

// evt data manipulation
typedef void  (*FuncAddEvt3)   (UsineEventPtr in1, UsineEventPtr in2, UsineEventPtr out);
typedef void  (*FuncAddEvt2)   (UsineEventPtr in1, UsineEventPtr in2);
typedef void  (*FuncSubEvt3)   (UsineEventPtr in1, UsineEventPtr in2, UsineEventPtr out);
typedef void  (*FuncSubEvt2)   (UsineEventPtr in1, UsineEventPtr in2);
typedef void  (*FuncDivEvt3)   (UsineEventPtr in1, UsineEventPtr in2, UsineEventPtr out);
typedef void  (*FuncMultEvt3)  (UsineEventPtr in1, UsineEventPtr in2, UsineEventPtr out);
typedef void  (*FuncMultEvt2)  (UsineEventPtr in1, UsineEventPtr in2);
typedef void  (*FuncMultEvt1)  (TPrecision m, UsineEventPtr out);
typedef void  (*FuncModEvt3)   (UsineEventPtr in1, UsineEventPtr in2, UsineEventPtr out);
typedef void  (*FuncPowerEvt3) (UsineEventPtr in1, UsineEventPtr in2, UsineEventPtr out);
typedef void  (*FuncExpEvt1)   (UsineEventPtr in1);
typedef void  (*FuncSqrtEvt1)  (UsineEventPtr in1);

typedef TPrecision  (*FuncMaxEvt1) (UsineEventPtr in1);
typedef TPrecision  (*FuncMinEvt1) (UsineEventPtr in1);
typedef void        (*FuncMaxEvt3) (UsineEventPtr in1, UsineEventPtr in2, UsineEventPtr out);
typedef void        (*FuncMinEvt3) (UsineEventPtr in1, UsineEventPtr in2, UsineEventPtr out);

typedef void  (*FuncThreshEvt2)      (UsineEventPtr in1, UsineEventPtr out, TPrecision min, TPrecision max);
typedef void  (*FuncThreshEvt1)      (UsineEventPtr in1, TPrecision min, TPrecision max);
typedef void  (*FuncMixMidiEvt3)     (UsineEventPtr in1, UsineEventPtr in2, UsineEventPtr out);
typedef void  (*FuncMixMidiEvt2)     (UsineEventPtr in1, UsineEventPtr in2);
typedef void  (*FuncFadeInAudioEvt)  (UsineEventPtr TPCMin);
typedef void  (*FuncFadeOutAudioEvt) (UsineEventPtr TPCMin);
typedef void  (*FuncClearAudioEvt)   (UsineEventPtr in1); 
typedef void  (*FuncDenormalizeAudioEvt)   (UsineEventPtr in1);

// Audio File manipulation
typedef TAudioFilePtr   (*FuncCreateAudioFile)          ();
typedef void            (*FuncDestroyAudioFile)         (TAudioFilePtr audioFile);
typedef TPrecision      (*FuncGetSampleAudioFile)       (TAudioFilePtr audioFile, int channel, int pos);
typedef TPrecision*     (*FuncGetSampleArrayAudioFile)  (TAudioFilePtr audioFile, int channel);
typedef void            (*FuncResampleAudioFile)        (TAudioFilePtr audioFile, TPrecision factor);
typedef void            (*FuncGetBlocSampleAudioFile)   (TAudioFilePtr audioFile, int channel, int pos, UsineEventPtr evt);
typedef int             (*FuncGetSizeAudioFile)         (TAudioFilePtr audioFile);
typedef int             (*FuncGetChannelAudioFile)      (TAudioFilePtr audioFile);
typedef int             (*FuncGetSampleRateAudioFile)   (TAudioFilePtr audioFile);
typedef int             (*FuncGetBitPerSampleAudioFile) (TAudioFilePtr audioFile);
typedef void            (*FuncLoadInMemoryAudioFile)    (TAudioFilePtr audioFile, AnsiCharPtr name);
typedef void            (*FuncLoadStayOnDiskAudioFile)  (TAudioFilePtr audioFile, AnsiCharPtr name);
typedef void            (*FuncGetPeaksAudioFile)        (TAudioFilePtr audioFile, UsineEventPtr evt);
typedef void            (*FuncSaveToDiskAudioFile)      (TAudioFilePtr audioFile, AnsiCharPtr name, int nbChannels);
typedef void            (*FuncSetChannelsAudioFile)     (TAudioFilePtr audioFile, int nbChannels);
typedef void            (*FuncSetSizeAudioFile)         (TAudioFilePtr audioFile, int size);
typedef void            (*FuncSetSampleAudioFile)       (TAudioFilePtr audioFile, int channel, int pos, TPrecision sample);
typedef void            (*FuncClearAudioFile)           (TAudioFilePtr audioFile);

// math util
typedef TPrecision (*FuncLinearInterpolation) (TPrecision f, TPrecision a, TPrecision b);
typedef TPrecision (*FuncCubicInterpolation)  (TPrecision fr, TPrecision inm1, TPrecision inp, TPrecision inp1, TPrecision inp2);
typedef TPrecision (*FuncSplineInterpolation) (TPrecision fr, TPrecision inm1, TPrecision inp, TPrecision inp1, TPrecision inp2);

// files manipulation
typedef LongBool    (*FuncProcessOpenDialog) (AnsiCharPtr* filename, AnsiCharPtr initialDir, AnsiCharPtr filter);
typedef LongBool    (*FuncProcessSaveDialog) (AnsiCharPtr* filename, AnsiCharPtr initialDir, AnsiCharPtr filter);
typedef AnsiCharPtr (*FuncFindFile)          (AnsiCharPtr* filename, AnsiCharPtr initialDir);
    
// trace functions
typedef void (*FuncTraceChar)       (AnsiCharPtr s);
typedef void (*FuncTraceInteger)    (Int64 i);
typedef void (*FuncTracePrecision)  (TPrecision f);
typedef void (*FuncTraceLogChar)    (AnsiCharPtr s, LongBool showInSplashForm);
typedef void (*FuncTraceSplashChar) (AnsiCharPtr s, int autoClose);
typedef void (*FuncTraceErrorChar)  (AnsiCharPtr s);

// smooth functions
typedef void (*FuncSmoothPrecision)    (TPrecision& oldValue, UsineEventPtr currentEvent, TPrecision factor);
typedef void (*FuncSmoothEvent)        (TPrecision& oldValue, UsineEventPtr currentEvent, TPrecision target, TPrecision factor);
typedef void (*FuncMultEvt2Audio)      (UsineEventPtr in1, UsineEventPtr in2);

// update param callback
typedef void (*FuncSetListBoxCommaText)	(TModuleInfo* pModuleInfo, int numParam, AnsiCharPtr commaText);
typedef void (*FuncSetParamCaption)	    (TModuleInfo* pModuleInfo, int numParam, AnsiCharPtr caption);
typedef void (*FuncSetParamVisible)	    (TModuleInfo* pModuleInfo, int numParam, LongBool visible);
typedef void (*FuncRecreateParam)	    (TModuleInfo* pModuleInfo, int numParam, TParamInfo* pParamInfo);
typedef void (*FuncRepaintParam)		(TModuleInfo* pModuleInfo, int numParam);
typedef void (*FuncSetParamValueText)	(TModuleInfo* pModuleInfo, int numParam, AnsiCharPtr valueText);
typedef LongBool (*FuncLoading)	        (TModuleInfo* pModuleInfo);

// audio callbacks
// Changed for linux testing by basile
typedef void (*FuncAudioDeviceIOCallback)	(const float *const *inputChannelData, int numInputChannels, float *const *outputChannelData, int numOutputChannels, int numSamples);

// midi callbacks
typedef void (*FuncMidiDeviceCallback)		(int deviceID, double TimeStamp, TUsineMidiCode midiData);
typedef void (*FuncMidiSysexDeviceCallback)	(int deviceID, double TimeStamp, char** sysEx, int size);

// language translation
typedef AnsiCharPtr    (*FuncGetTranslationSDK)			( AnsiCharPtr StringID );

typedef double			(*FuncGetSampleRate)		();
typedef TVstTimeInfo*	(*FuncGetVstTimeInfo)		(TModuleInfo* pModuleInfo);
typedef int				(*FuncCreatePlugInsTree)    ();

typedef void (*FuncNotifyUsine)	(TModuleInfo* pModuleInfo, NativeInt Target, NativeInt Msg, NativeInt Param1, NativeInt Param2, int Delay);

// draw functions
typedef void (*FuncDrawPoint)		(TModuleInfo* pModuleInfo, TPointF point, TUsineColor color, float size, LongBool rounded);
typedef void (*FuncDrawLine)		(TModuleInfo* pModuleInfo, TPointF p1, TPointF p2, TUsineColor color, float strokeThickness);
typedef void (*FuncDrawPolyLine)	(TModuleInfo* pModuleInfo, TPointFPtr points, int sizeList, TUsineColor color, float strokeThickness);
// draw path functions
typedef void (*FuncDrawPathMoveTo)		(TModuleInfo* pModuleInfo, TPointF point);
typedef void (*FuncDrawPathLineTo)		(TModuleInfo* pModuleInfo, TPointF point);
typedef void (*FuncDrawPathQuadCurveTo)	(TModuleInfo* pModuleInfo, TPointF control, TPointF point);
typedef void (*FuncDrawPathDraw)		(TModuleInfo* pModuleInfo, TUsineColor color, float strokeThickness);
typedef void (*FuncDrawPathFill)		(TModuleInfo* pModuleInfo, TUsineColor color);
typedef void (*FuncDrawPathClear)		(TModuleInfo* pModuleInfo);
typedef void (*FuncDrawPathClose)		(TModuleInfo* pModuleInfo);

// fill functions
typedef void (*FuncFillRect)	    (TModuleInfo* pModuleInfo, TRectF rect, TUsineColor color, float radius, TUsineColor borderColor, float borderWith);
typedef void (*FuncFillText)	    (TModuleInfo* pModuleInfo, TRectF rect, AnsiCharPtr text, TUsineColor textColor, float fontSize, LongBool isBold, LongBool isWordWrap, TTextAlign alignHorizontal, TTextAlign alignVertical, LongBool isVertical);
typedef void (*FuncFillPolyLine)	(TModuleInfo* pModuleInfo, TPointFPtr points, int sizeList, TUsineColor color);
// blit
typedef void (*FuncBitBlit)	(TModuleInfo* pModuleInfo, TUsinePixelPtr pArray, int W, int H, TRectF destRect);

// record functions
typedef void (*FuncStopRecord)		(TModuleInfo* pModuleInfo);
typedef void (*FuncProcessRecord)	(TModuleInfo* pModuleInfo, TPrecision X, TPrecision Y, TPrecision Z);


typedef TUsineColor (*FuncGetUsineColor) ( UINT32 colorName);

typedef void  (*FuncAddCommand)           (TModuleInfo* pModuleInfo, AnsiCharPtr name, NativeInt CallbackId, LongBool Translate);
typedef void  (*FuncAddCommandSeparator)  (TModuleInfo* pModuleInfo, AnsiCharPtr name, LongBool Translate);
typedef NativeInt (*FuncGetUsineMainWindow) (); // NSWindow sur mac
typedef void* (*FuncGetMacNSView) (); // NSView sur mac

typedef void*  (*FuncNeedRemoteUpdate)          (TModuleInfo* pModuleInfo, int numParam);
typedef double (*FuncGetTimeMs)                 (TModuleInfo* pModuleInfo); // precision up to nanoseconde
typedef void   (*FuncSetDeskWindowClientSize)   (TModuleInfo* pModuleInfo, int clientWidth, int clientHeight);
typedef void*  (*FuncCreateDeskWindow)          (TModuleInfo* pModuleInfo);
typedef void   (*FuncDestroyDeskWindow)         (TModuleInfo* pModuleInfo);
typedef void*  (*FuncShowDeskWindow)            (TModuleInfo* pModuleInfo);
typedef void   (*FuncHideDeskWindow)            (TModuleInfo* pModuleInfo);
typedef void   (*FuncSetDeskWindowCaption)      (TModuleInfo* pModuleInfo, AnsiCharPtr name);

typedef LongBool(*FuncBoolean)                  (TModuleInfo* pModuleInfo);


// setter and getter for the name of the module (as it appear in the patch, on the module title)
typedef void(*FuncSetModuleUserName)			(TModuleInfo* pModuleInfo, AnsiCharPtr name);
typedef AnsiCharPtr(*FuncGetModuleUserName)		(TModuleInfo* pModuleInfo);

// return the nb off audio channels from QueryIndex
typedef int   (*FuncGetAudioQueryToNbChannels) (int qQueryIndex);

// return the number index label from QueryIndex
// optional prefix (ex: "in " or "out ")
// attention index starts by 1 (and not 0)
typedef AnsiCharPtr    (*FuncGetAudioQueryChannelNames) (AnsiCharPtr prefix, int index, int queryIndex);

// setter and getter for a setting line value
typedef void(*FuncSetSettingValue)	(TModuleInfo* pModuleInfo, AnsiCharPtr settingName, UsineEventPtr settingEvent);
typedef void(*FuncGetSettingValue)	(TModuleInfo* pModuleInfo, AnsiCharPtr settingName, UsineEventPtr& settingEvent);

// Frames manipulation 
typedef void(*FuncGetInputFrame)	(TModuleInfo* pModuleInfo, int numInput, TUsineFramePtr frame);
typedef void(*FuncSetOutputFrame)	(TModuleInfo* pModuleInfo, int numOutput, TUsineFramePtr frame);
typedef void(*FuncClearFrame)    	(TModuleInfo* pModuleInfo, TUsineColor color, TUsineFramePtr frame);
typedef void(*FuncGetNewFrame)	    (TModuleInfo* pModuleInfo, int width, int height, LongBool permanent, TUsineFramePtr frame);
typedef void(*FuncCopyFrame)	    (TUsineFramePtr srcFrame, TUsineFramePtr destFrame);
typedef TUsinePixel (*FuncColorToPixel) (TUsineColor color);
typedef TUsineColor (*FuncPixelToColor) (TUsinePixel pixel);
typedef void(*FuncReleaseFrame)	    (TModuleInfo* pModuleInfo, TUsineFramePtr frame);
typedef void(*FuncSetDimmerFrame)   (TModuleInfo* pModuleInfo, float dimmer, TUsineFramePtr frame);

typedef void(*FuncLockCriticalSection)                            (TModuleInfo* pModuleInfo);
typedef void(*FuncCriticalSectionLock)                  (TModuleInfo* pModuleInfo, TCriticalSectionPtr pCriticalSection, UINT32 timeout);
typedef void(*FuncCriticalSectionUnLock)                (TModuleInfo* pModuleInfo, TCriticalSectionPtr pCriticalSection);
typedef void(*FuncCriticalSectionDestroy)               (TModuleInfo* pModuleInfo, TCriticalSectionPtr pCriticalSection);
typedef LongBool(*FuncCriticalSectionTryLock)           (TModuleInfo* pModuleInfo, TCriticalSectionPtr pCriticalSection);
typedef TCriticalSectionPtr(*FuncCriticalSectionCreate) (TModuleInfo* pModuleInfo, AnsiCharPtr name, LongBool shared);

//global array
typedef UINT64(*FuncGlobalArrayGetHash)	        (AnsiCharPtr name);
typedef void(*FuncGlobalArraySetValueFloat)	    (UINT64 hash, int index, TPrecision value, AnsiCharPtr errorMsg);
typedef void(*FuncGlobalArraySetValueColor)	    (UINT64 hash, int index, TUsineColor value, AnsiCharPtr errorMsg);
typedef void(*FuncGlobalArraySetValueAnsiChar)	(UINT64 hash, int index, AnsiCharPtr value, AnsiCharPtr errorMsg);
typedef void(*FuncGlobalArraySetValueEvt)	    (UINT64 hash, UsineEventPtr value, AnsiCharPtr errorMsg);

typedef TPrecision(*FuncGlobalArrayGetSize)	            (UINT64 hash, AnsiCharPtr errorMsg);
typedef TPrecision(*FuncGlobalArrayGetValueFloat)	    (UINT64 hash, int index, AnsiCharPtr errorMsg);
typedef TUsineColor(*FuncGlobalArrayGetValueColor)	    (UINT64 hash, int index, AnsiCharPtr errorMsg);
typedef AnsiCharPtr(*FuncGlobalArrayGetValueAnsiChar)	(UINT64 hash, int index, AnsiCharPtr errorMsg);
typedef void(*FuncGlobalArrayGetValueEvt)	            (UINT64 hash, AnsiCharPtr errorMsg, UsineEventPtr result);
typedef AnsiCharPtr(*FuncGlobalArrayGetAllNames)     	();

typedef void(*FuncUsineObjectSetFloat)      (TModuleInfo* pModuleInfo, AnsiCharPtr name, TPrecision v);
typedef void(*FuncUsineObjectSetInteger)    (TModuleInfo* pModuleInfo, AnsiCharPtr name, int v); 
typedef void(*FuncUsineObjectSetArray)      (TModuleInfo* pModuleInfo, AnsiCharPtr name, int indexr,TPrecision v); 
typedef void(*FuncUsineObjectSetMIDI)       (TModuleInfo* pModuleInfo, AnsiCharPtr name, int index, TUsineMidiCode v);
typedef void(*FuncUsineObjectSetColor)      (TModuleInfo* pModuleInfo, AnsiCharPtr name, TUsineColor c);
typedef void(*FuncUsineObjectSetArrayColor) (TModuleInfo* pModuleInfo, AnsiCharPtr name, int index, TUsineColor c);
typedef void(*FuncUsineObjectSetTrigger)    (TModuleInfo* pModuleInfo, AnsiCharPtr name ); 
typedef void(*FuncUsineObjectSetAnsiChar)   (TModuleInfo* pModuleInfo, AnsiCharPtr name, AnsiCharPtr v); 

typedef TPrecision     (*FuncUsineObjectGetFloat)       (TModuleInfo* pModuleInfo, AnsiCharPtr name);
typedef int            (*FuncUsineObjectGetInteger)     (TModuleInfo* pModuleInfo, AnsiCharPtr name);
typedef TPrecision     (*FuncUsineObjectGetArray)       (TModuleInfo* pModuleInfo, AnsiCharPtr name, int index);
typedef TUsineMidiCode (*FuncUsineObjectGetMIDI)        (TModuleInfo* pModuleInfo, AnsiCharPtr name, int index);
typedef TUsineColor    (*FuncUsineObjectGetColor)       (TModuleInfo* pModuleInfo, AnsiCharPtr name);
typedef TUsineColor    (*FuncUsineObjectGetArrayColor)  (TModuleInfo* pModuleInfo, AnsiCharPtr name, int index);
typedef AnsiCharPtr    (*FuncUsineObjectGetAnsiChar)    (TModuleInfo* pModuleInfo, AnsiCharPtr name );
typedef int            (*FuncUsineObjectGetLength)      (TModuleInfo* pModuleInfo, AnsiCharPtr name);
typedef void           (*FuncUsineObjectSetLength)      (TModuleInfo* pModuleInfo, AnsiCharPtr name, int l);
typedef AnsiCharPtr    (*FuncUsineObjectFind)           (TModuleInfo* pModuleInfo, AnsiCharPtr name);
typedef LongBool       (*FuncUsineObjectExists)         (TModuleInfo* pModuleInfo, AnsiCharPtr name);
typedef AnsiCharPtr    (*FuncUsineObjectListHash)       (TModuleInfo* pModuleInfo);
class UserModuleBase;
typedef void       (*FuncThreadProcess)     (void* pModule, TThreadPtr pThread);
typedef TThreadPtr (*FuncThreadCreate)      (TModuleInfo* pModuleInfo, AnsiCharPtr name, FuncThreadProcess ProcessThreadProc, TThreadPriority priority, UINT32 timeout);
typedef void       (*FuncThreadDestroy)     (TThreadPtr pThread);
typedef void       (*FuncThreadRestart)     (TThreadPtr pThread);
typedef void       (*FuncThreadSetTimeOut)  (TThreadPtr pThread,UINT32 timeout);

typedef TSyncObjectPtr (*FuncSyncObjectCreate)   ();
typedef void           (*FuncSyncObjectDestroy)  (TSyncObjectPtr pSyncObject);
typedef void           (*FuncSyncObjectSet)      (TSyncObjectPtr pSyncObject);
typedef void           (*FuncSyncObjectReset)    (TSyncObjectPtr pSyncObject);
typedef LongBool       (*FuncSyncObjectWait)  (TSyncObjectPtr pSyncObject, UINT32 timeout);



//-----------------------------------------------------------------------------
/// Contains globals infos provided by Usine and all functions handle of the module wrapper.
struct TMasterInfo 
{
    int                     BlocSize;				///< size of Usine audio blocs (number of samples)
    NativeUInt              NSApplication;
    FuncRepaintPanel        RepaintPanel;			///< ask Usine to repaint the module Panel
    FuncGetUsineMainWindow  GetUsineMainWindow;		///< Usine mainform handle (THandle)
    FuncGetMacNSView        GetMacNSView;		    ///< pointer to Usine internal User Module

    // add layout lines in the layout panel
    FuncAddSettingLineCaption	AddSettingLineCaption;
    FuncAddSettingLineColor	    AddSettingLineColor;
    FuncAddSettingLineBoolean	AddSettingLineBoolean;
    FuncAddSettingLineInteger	AddSettingLineInteger;

    // the color set used by usine 
    FuncGetUsineColor         GetUsineColor;
                                                                
    // Internal usine message
    FuncSendMessage    SendUsineMsg;

    // Events manipulation
    FuncCopyEvt        CopyEvt;
    FuncCompareEvt     CompareEvt;
    FuncConcatEvt      ConcatEvt;
    FuncShiftEvt       ShiftEvt;
    FuncSetEvtSize     SetEvtSize;
    FuncSetEvtMaxSize  SetEvtMaxSize;
    FuncDestroyEvt     DestroyEvt;
    FuncCreateEvt      CreateEvt;
    FuncGetEvtSize     GetEvtSize;

    // Events values access
    FuncSetEvtData         SetEvtData;
    FuncGetEvtData         GetEvtData;
    FuncSetEvtArrayData    SetEvtArrayData;
    FuncGetEvtArrayData    GetEvtArrayData;
    FuncSetEvtPointer      SetEvtPointer;
    FuncGetEvtPointer      GetEvtPointer;
    FuncNotImplemented	   SetEvtArrayPointer;
    FuncNotImplemented	   GetEvtArrayPointer;
    FuncSetEvtArrayMidi    SetEvtArrayMidi;
    FuncGetEvtArrayMidi    GetEvtArrayMidi;
    FuncSetEvtPChar        SetEvtPChar;
    FuncGetEvtPChar        GetEvtPChar;

    FuncGetEvtDataAddr     GetEvtDataAddr;
    FuncMoveBLOC32 		   MoveBLOC32;

    // Events data manipulation
    FuncAddEvt3    AddEvt3;
    FuncAddEvt2    AddEvt2;
    FuncSubEvt3    SubEvt3;
    FuncSubEvt2    SubEvt2;
    FuncMultEvt3   MultEvt3;
    FuncMultEvt2   MultEvt2;
    FuncMultEvt1   MultEvt1;
    FuncDivEvt3    DivEvt3;
    FuncModEvt3    ModEvt3;
    FuncPowerEvt3  PowerEvt3;
    FuncExpEvt1    ExpEvt1;
    FuncSqrtEvt1   SqrtEvt1;

    FuncMaxEvt1    MaxEvt1;
    FuncMinEvt1    MinEvt1;
    FuncMaxEvt3    MaxEvt3;
    FuncMinEvt3    MinEvt3;

    FuncThreshEvt2      ThreshEvt2;
    FuncThreshEvt1      ThreshEvt1;
    FuncMixMidiEvt3     MixMidiEvt3;
    FuncMixMidiEvt2     MixMidiEvt2;
    FuncFadeInAudioEvt  FadeInAudioEvt;
    FuncFadeOutAudioEvt FadeOutAudioEvt;
	FuncClearAudioEvt   ClearAudioEvt;
	

    // Audio File manipulation
    FuncCreateAudioFile            CreateAudioFile;	
    FuncDestroyAudioFile           DestroyAudioFile;			
    FuncGetSampleAudioFile         GetSampleAudioFile;			
    FuncGetBlocSampleAudioFile     GetBlocSampleAudioFile;		
    FuncGetSizeAudioFile           GetSizeAudioFile;			
    FuncGetChannelAudioFile        GetChannelAudioFile;		
    FuncGetSampleRateAudioFile	   GetSampleRateAudioFile;		
    FuncGetBitPerSampleAudioFile   GetBitPerSampleAudioFile;	
    FuncLoadInMemoryAudioFile      LoadInMemoryAudioFile;		
    FuncLoadStayOnDiskAudioFile    LoadStayOnDiskAudioFile;	
    FuncGetPeaksAudioFile          GetPeaksAudioFile;			
    FuncSaveToDiskAudioFile        SaveToDiskAudioFile;		
    FuncSetChannelsAudioFile       SetChannelsAudioFile;		
    FuncSetSizeAudioFile           SetSizeAudioFile;			
    FuncSetSampleAudioFile         SetSampleAudioFile;			
    FuncClearAudioFile             ClearAudioFile;				

    // Math util
    FuncLinearInterpolation        LinearInterpolation;	
    FuncCubicInterpolation	       CubicInterpolation;		
    FuncSplineInterpolation        SplineInterpolation;

    // Files manipulation
    FuncProcessOpenDialog  ProcessOpenDialog;
    FuncProcessSaveDialog  ProcessSaveDialog;
    FuncFindFile           FindFile;
    
    // Usine path
    AnsiCharPtr   SoundPath;
    AnsiCharPtr   UserLibPath; 
    AnsiCharPtr   CurrentWorkspacePath; 
    AnsiCharPtr   CurrentPatchPath; 
    AnsiCharPtr   RecordPath;

    // Trace functions
    FuncTraceChar         TraceChar;
    FuncTraceInteger      TraceInteger;
    FuncTracePrecision    TracePrecision;
    FuncTraceLogChar      TraceLogChar;
    FuncTraceErrorChar    TraceErrorChar;
 
    // smooth functions
    FuncSmoothPrecision    SmoothPrecision;
    FuncSmoothEvent        SmoothEvent;
    FuncMultEvt2Audio      MultEvt2Audio;
    [[deprecated("Deprecated: use the constant USINE_MAX_AUDIO_INPUTS instead")]]
	int	MAX_AUDIO_INPUTS;
    [[deprecated("Deprecated: use the constant USINE_MAX_AUDIO_OUTPUTS instead")]]
    int MAX_AUDIO_OUTUTS;
    [[deprecated("Deprecated: use the constant USINE_MAX_MIDI_DEVICES instead")]]
    int MAX_MIDI_DEVICES;
    [[deprecated("Deprecated: use the constant USINE_MULTIPHONY_MAX instead")]]
    int MULTIPHONY_MAX;

	FuncSetListBoxCommaText	    SetListBoxCommaText;
    AnsiCharPtr					UsineTempPath;
	FuncAudioDeviceIOCallback	AudioDeviceIOCallback;

	FuncMidiDeviceCallback		MidiDeviceCallback;
	FuncMidiSysexDeviceCallback	MidiSysexDeviceCallback;

	FuncSetParamCaption	        SetParamCaption;
	FuncGetTranslationSDK	    GetTranslation;

	FuncTraceErrorChar  TraceWarningChar;
	FuncSetParamVisible	SetParamVisible;
	FuncRepaintParam	RepaintParam;
	FuncGetSampleRate	GetSampleRate;
	FuncGetVstTimeInfo	GetVstTimeInfo;
    
    AnsiCharPtr             GlobalApplicationPath;
    FuncCreatePlugInsTree   CreatePlugInsTree;
    FuncNotifyUsine         NotifyUsine;
    FuncTraceSplashChar     TraceSplashChar;	

    FuncDialogBox		DialogConfirmationYesNoCancel;
    FuncDialogBox		DialogConfirmationYesNo;	
	FuncDialogBox		DialogInformationOk;		
	FuncDialogBox		DialogConfirmationOKCancel;
	LongBoolPtr		    NeedAllNoteOff;
	UsineEventPtr		AllNotOffEvent;
	FuncDrawPoint		DrawPoint;
	FuncDrawLine		DrawLine;
	FuncDrawPolyLine	DrawPolyLine;
	FuncFillRect		FillRect;
	FuncFillText		FillText;
		
	FuncAddSettingLineSingle	AddSettingLineSingle;
    AnsiCharPtr				    PlugInsPath;
	FuncSetEvtColor		        SetEvtColor;
	FuncGetEvtColor		        GetEvtColor;
    
	FuncFillPolyLine		FillPolyLine;
	FuncStopRecord			StopRecord;
	FuncProcessRecord		ProcessRecord;
    
    FuncAddCommand              AddCommand;
    FuncAddCommandSeparator     AddCommandSeparator;
	FuncAddSettingLineCombobox  AddSettingLineCombobox;
    FuncNeedRemoteUpdate        NeedRemoteUpdate;
    
    AnsiCharPtr                 UsineVersion; 
    FuncSetParamValueText       SetParamValueText;
    FuncGetTimeMs               GetTimeMs;
    FuncSetDeskWindowClientSize SetDeskWindowClientSize;
    FuncCreateDeskWindow        CreateDeskWindow;
    FuncDestroyDeskWindow       DestroyDeskWindow;
    FuncShowDeskWindow          ShowDeskWindow;
    FuncHideDeskWindow          HideDeskWindow;
    FuncSetDeskWindowCaption    SetDeskWindowCaption;

    // audio multi channels 
    FuncGetAudioQueryToNbChannels   AudioQueryToNbChannels;
    FuncGetAudioQueryChannelNames   AudioQueryChannelNames;
    AnsiCharPtr                     AudioQueryChannelList; // comma-text to put in the query choice;

    int                             UsineSaveVersion;
    // draw path functions
    FuncDrawPathClear               DrawPathClear;
    FuncDrawPathMoveTo              DrawPathMoveTo;
    FuncDrawPathLineTo              DrawPathLineTo;
    FuncDrawPathDraw                DrawPathDraw;
    FuncDrawPathFill                DrawPathFill;
    FuncDrawPathClose               DrawPathClose;
	FuncNotImplemented              SetEvtNbLinesDeprecated;
	FuncBoolean                     PatchIsRunning;
    FuncDrawPathQuadCurveTo         DrawPathQuadCurveTo;	
    FuncNotImplemented              GetEvtNbLinesDeprecated;
    [[deprecated ("Usine Version is no longer used in the SDK;\nYou should not use it,\n Usine will send always the value 0")]]
    int                             UsineVersionType;
    AnsiCharPtr                     UsineLanguage;
	AnsiCharPtr                     AudioQueryTitle;
	AnsiCharPtr                     BlockSizeList;
	int							    BlockSizeDefaultIndex;
	FuncSetModuleUserName			SetModuleUserName;
	FuncGetModuleUserName			GetModuleUserName;

	// add setting line in the layout panel
	FuncAddSettingsLineString		AddSettingsLineString;

	// setting line getter/setter
	FuncSetSettingValue				SetSettingValue;
	FuncGetSettingValue				GetSettingValue;

	// video frames
    FuncGetInputFrame               GetInputFrame;
    FuncGetNewFrame                 GetNewFrame;
    FuncClearFrame                  ClearFrame;
	FuncSetOutputFrame              SetOutputFrame;
	FuncCopyFrame                   CopyFrame;
	FuncColorToPixel                ColorToPixel;
	FuncPixelToColor                PixelToColor;
	FuncReleaseFrame                ReleaseFrame;
	FuncSetDimmerFrame              SetDimmerFrame;

	// various
	FuncDialogInputBox              DialogInputBox;
	FuncRecreateParam               RecreateParam;
	FuncLoading                     PatchLoading;
	FuncGetSampleArrayAudioFile     GetSampleArrayAudioFile;
	FuncLockCriticalSection                   LockPatch;
	FuncLockCriticalSection                   UnLockPatch;
	FuncResampleAudioFile           ResampleAudioFile;
	FuncDenormalizeAudioEvt         DenormalizeAudioEvt;
	FuncBoolean                     PatchJustActivated;
	// clocks
	FuncBoolean                     RefreshSpeedClock;
	FuncBoolean                     SlowClock;

	// global array
	FuncGlobalArrayGetHash          GlobalArrayGetHash;
	FuncGlobalArraySetValueFloat    GlobalArraySetValueFloat;
	FuncGlobalArraySetValueColor    GlobalArraySetValueColor;
	FuncGlobalArraySetValueAnsiChar GlobalArraySetValueAnsiChar;
	FuncGlobalArraySetValueEvt      GlobalArraySetValueEvt;
	FuncGlobalArrayGetValueFloat    GlobalArrayGetValueFloat;
	FuncGlobalArrayGetValueColor    GlobalArrayGetValueColor;
	FuncGlobalArrayGetValueAnsiChar GlobalArrayGetValueAnsiChar;
	FuncGlobalArrayGetValueEvt      GlobalArrayGetValueEvt;
	FuncGlobalArrayGetAllNames      GlobalArrayGetAllNames;

	FuncUsineObjectSetFloat         UsineObjectSetFloat;
	FuncUsineObjectSetInteger		UsineObjectSetInteger;
	FuncUsineObjectSetArray			UsineObjectSetArray;
	FuncUsineObjectSetMIDI			UsineObjectSetMIDI;
	FuncUsineObjectSetColor			UsineObjectSetColor;
	FuncUsineObjectSetArrayColor	UsineObjectSetArrayColor;
	FuncUsineObjectSetTrigger		UsineObjectSetTrigger;
	FuncUsineObjectSetAnsiChar		UsineObjectSetAnsiChar;
		
	FuncUsineObjectGetFloat			UsineObjectGetFloat;
	FuncUsineObjectGetInteger		UsineObjectGetInteger;
	FuncUsineObjectGetArray			UsineObjectGetArray;
	FuncUsineObjectGetMIDI			UsineObjectGetMIDI;
	FuncUsineObjectGetColor			UsineObjectGetColor;
	FuncUsineObjectGetArrayColor	UsineObjectGetArrayColor;
	FuncUsineObjectGetAnsiChar		UsineObjectGetAnsiChar;

	FuncUsineObjectGetLength		UsineObjectGetLength;
	FuncUsineObjectSetLength		UsineObjectSetLength;

	FuncUsineObjectFind				UsineObjectFind;
	FuncUsineObjectExists			UsineObjectExists;
	FuncUsineObjectListHash			UsineObjectListHash;

    FuncCriticalSectionLock         CriticalSectionLock;
    FuncCriticalSectionTryLock      CriticalSectionTryLock;
    FuncCriticalSectionUnLock       CriticalSectionUnLock;
    FuncCriticalSectionCreate       CriticalSectionCreate;
    FuncCriticalSectionDestroy      CriticalSectionDestroy;

    FuncThreadCreate                ThreadCreate;
    FuncThreadDestroy               ThreadDestroy;
    FuncThreadRestart               ThreadRestart;
    FuncThreadSetTimeOut            ThreadSetTimeOut;

    FuncSyncObjectCreate            SyncObjectCreate;
    FuncSyncObjectDestroy           SyncObjectDestroy;
    FuncSyncObjectSet               SyncObjectSet;
    FuncSyncObjectReset             SyncObjectReset;
    FuncSyncObjectWait              SyncObjectWait;

    FuncGetEvtMaxSize               GetEvtMaxSize;
    FuncBitBlit                     BitBlit;
    FuncLockCriticalSection         LockUsineEngine;
    FuncLockCriticalSection         UnLockUsineEngine;

    FuncGlobalArrayGetSize          GlobalArrayGetSize;

};
/// for backward compatibility
typedef TMasterInfo MasterInfo;

//-----------------------------------------------------------------------------
/// Possible type of user modules.
/// @see onGetModuleInfo, TModuleInfo
typedef enum TModuleType 
{ 
    mtSimple,           ///< module without graphic canvas
    mtVideo,            ///< video module
    mtControl,          ///< module with a graphic canvas
	mtDeviceAudio,      ///< not for public use
	mtDeviceMidi,       ///< not for public use
	mtDeviceOther,      ///< not for public use
	mtPluginWrapper,    ///< not for public use
    mtPluginLister,     ///< not for public use
    mtDeviceDMX,        ///< not for public use
	mtDeviceLaser,       ///< not for public use
	mtOther             ///< not for public use
} TModuleType;





//-----------------------------------------------------------------------------
/// Contain characteristics and infos about the module.
/// You fill the TModuleInfo structure in the onGetModuleInfo callback to inform Usine of the module specs.
struct TModuleInfo 
{
    AnsiCharPtr     Name{};           ///< short name displayed in the patch view
    AnsiCharPtr     Description{};    ///< long name displayed in the  Browser
    TModuleType     ModuleType;     ///< module type: simple, form, control
    TUsineColor     BackColor{};      ///< module color in the patch view
    int             NumberOfParams{}; ///< number of parameters of the module

    /// Default Panel width
    ///<   if moduletype = mtControl : DefaultWidth is set to TMasterInfo.PanelWidth
    ///<   if moduletype = mtSimple : ignored
    float       DefaultWidth{};   ///< in pixel
    
    // Default Panel height
    ///<   if moduletype = mtControl : DefaultHeight is set to TMasterInfo.PanelHeight
    ///<   if moduletype = mtSimple : ignored
    float       DefaultHeight{}; ///< in pixel

    LongBool    DontProcess{};        ///< FALSE by default. if TRUE, the module doesn't need any processing
    LongBool	CanRecord{};    		///< FALSE by default. if TRUE, the module will have the Automation record functionality.

    //option for not show additional parameters, false by default  
    LongBool DoNotCreateAddCtrl{} ;   ///< if true usine don't create parameters like mousedown, hint, etc.

    // pointer to Usine internal User Module
    void*   UsineUserModule{};
    
    // to verify that the module has been created properly
    int     CheckCode{}; ///< @internal not for public use

    // stuff for the Audio Query system and query lists
    AnsiCharPtr     QueryListString{};        ///< string to ask about entries numbers
    AnsiCharPtr     QueryListValues{};        ///< coma separated list with possible response
    int             QueryListDefaultIdx{};    ///< pre selected choice index for the coma list (0 to N-1)
                                        
	AnsiCharPtr     Version{};            ///< version of the module. Displayed in the Browser
    
    /// FALSE by default, set to TRUE to activate the global Randomize feature.
    /// If activated you need to implement onRandomize Callback, Usine can call it at any time to initiate a Randomize. 
    /// When activated, a new entry is added in the properties tab named can be randomized and a randomize icon appear in the Contextual 
    /// Menu of the module if any and on the toolbar if any.	
    LongBool        CanBeRandomized{};

	// for video modules only
	int     NumberOfVideoInputs{}; ///< number of video inputs must be in [0..2]
	int     NumberOfVideoOutputs{}; ///< number of video outputs must be in [0..2]
    /// FALSE by default, set to TRUE to activate the global Reset feature.
    /// If activated you need to implement onReset Callback, Usine can call it at any time to initiate a Reset.
    /// When activated, a new entry is added in the properties tab named can be reset and a reset icon appear in 
    /// the Contextual Menu of the module if any and on the toolbar if any.	
	LongBool        CanBeReset{};
	LongBool        CanBeSavedInPreset = true;
    LongBool        TransparentBackground{};

    
    // stuff for general queries
    AnsiCharPtr QueryLabel1{};  ///< query displayed label 1
    int QueryDefaultValue1{};      ///< default query value 1 value
    int QueryMaxValue1{};          ///< max query value 1 value
    int QueryMinValue1{};          ///< min query value 1 value

    AnsiCharPtr QueryLabel2{};  ///< query displayed label 2
    int QueryDefaultValue2{};      ///< default query value 2 value
    int QueryMaxValue2{};          ///< max query value 2 value
    int QueryMinValue2{};          ///< min query value 2 value
    
};

/// for backward compatibility
typedef TModuleInfo ModuleInfo;

//-----------------------------------------------------------------------------
/// To store 2D coordinates in pixels (integer).
struct TPointI 
{
    int x;  ///<X Coordinates
    int y; ///<X Coordinates
};

//-----------------------------------------------------------------------------
/// To store color information in ARGB format (from 0 to 1).
struct TColorArgb
{
    float a; ///< Alpha 
    float r; ///< Red
    float g; ///< Green
    float b; ///< Blue
}; 

//-----------------------------------------------------------------------------
/// To store color information in AHSL format (from 0 to 1).
struct TColorAhsl
{  
    float a; ///< Alpha
    float h; ///< Hue in degrees
    float s; ///< Saturation
    float l; ///< Luminance
}; 

//-----------------------------------------------------------------------------
// end of memory alignment with DELPHI
#pragma pack(pop)


//-----------------------------------------------------------------------------
// globals functions exposed by the module to communicate with usine
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
#if (defined (USINE_WIN64))
//-----------------------------------------------------------------------------
#define USINE_MODULE_EXPORT extern "C" __declspec( dllexport )
//-----------------------------------------------------------------------------
#elif (defined (USINE_OSX64) || defined(USINE_LIN64))
//-----------------------------------------------------------------------------
#define USINE_MODULE_EXPORT extern "C" __attribute__ (( visibility ("default") ))
//-----------------------------------------------------------------------------
#else
  #error "condidionnal compilation error!"
#endif
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// create, general info and destroy
// called by usine to get the version of the SDK used
USINE_MODULE_EXPORT int GetSDKVersion(void);
// called by usine to get implementation type (language + compilator) of the module
//USINE_MODULE_EXPORT SdkImplementation GetImplementation(void);
// Create Module
USINE_MODULE_EXPORT void Create(void* &pModule, AnsiCharPtr optionalString, LongBool Flag, TMasterInfo* pMasterInfo, AnsiCharPtr optionalContent);
// Destroy Module
USINE_MODULE_EXPORT void Destroy(void* pModule);
// called by usine to get module informations for the browser
USINE_MODULE_EXPORT void BrowserInfo (TModuleInfo* pModuleInfo);
// called by usine to get module informations
USINE_MODULE_EXPORT void GetModuleInfo (void* pModule, TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo);

//-----------------------------------------------------------------------------
// query system and init
// Get total parameters number of the module
USINE_MODULE_EXPORT int GetNumberOfParams(void* pModule, int queryResult1, int queryResult2);
// called after the query popup
USINE_MODULE_EXPORT void AfterQuery (void* pModule, TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo, int queryResult1, int queryResult2);
// initialization procedure
USINE_MODULE_EXPORT void InitModule (void* pModule, TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo);

//-----------------------------------------------------------------------------
// parameters and process
// called by usine to get params informations
USINE_MODULE_EXPORT void GetParamInfo (void* pModule, int ParamIndex, TParamInfo* pParamInfo);
// tels to the module what are the effectives events address
// no longer used in HH3 see TModuleInfo::EventPtr
USINE_MODULE_EXPORT void SetEventAddress (void* pModule, int ParamIndex, UsineEventPtr pEvent);
// called by Usine when a parameter value has changed
USINE_MODULE_EXPORT void CallBack (void* pModule, TUsineMessage *Message);
// main process procedure
USINE_MODULE_EXPORT void Process (void* pModule);
// main process procedure
USINE_MODULE_EXPORT void ProcessVideo(void* pModule);

// midi out callbacks
/// called by Usine to send out midi
USINE_MODULE_EXPORT void MidiSendOut (void* pModule, int DeviceID, TUsineMidiCode Code);

/// called by Usine to send out several midi at the same time
USINE_MODULE_EXPORT void MidiSendOutArray (void* pModule, int DeviceID, TUsineMidiCode** arrayCode, int arraySize);

/// called by Usine to send out midi sysex
USINE_MODULE_EXPORT void MidiSysexSendOut (void* pModule, int DeviceID, char** Sysex, int sysexSize);

/// called by Usine to send out OSC messages
USINE_MODULE_EXPORT void DMXSendOut (void* pModule, int deviceId, char* ByteArray, int len, int universeNum);

// Laser out callbacks
/// called by Usine to send ILDA frames
USINE_MODULE_EXPORT void LaserSendOut(void* pModule, int DeviceID, TUsineILDAPoint** arrayPoint, int arraySize, int speedPPS);

// chunk system
/// returns the chunk string len, needed for master memory allocation
USINE_MODULE_EXPORT int GetChunkLen (void* pModule, LongBool Preset);
/// get the chunk string used to store modules intern data
/// ! all parameters values are stored automatically by Usine
USINE_MODULE_EXPORT void GetChunk (void* pModule, void* chunk, LongBool Preset);

/// chunk string send by Usine when loading
USINE_MODULE_EXPORT void SetChunk(void* pModule, const void* chunk, LongBool Preset, int sizeInBytes);

/// called after the module is loaded
USINE_MODULE_EXPORT void AfterLoading (void* pModule);

// layout option and commands
/// create user defined layout options in the 'edit layout' panel
USINE_MODULE_EXPORT void CreateSettings(void* pModule);
/// Called when something has changed in the layout
USINE_MODULE_EXPORT void SettingsHasChanged(void* pModule);
/// resize the panel
USINE_MODULE_EXPORT void Resize (void* pModule, float W, float H);
/// called by Usine to paint the panel
USINE_MODULE_EXPORT void Paint (void* pModule);
/// called by Usine to create commands of the module
USINE_MODULE_EXPORT void CreateCommands(void* pModule);

// mouse and multi touch
/// mouse move event
USINE_MODULE_EXPORT void MouseMove(void* pModule, TShiftState Shift, float X, float Y);
/// mouse down event
USINE_MODULE_EXPORT void MouseDown(void* pModule, TMouseButton MouseButton, TShiftState Shift, float X,float Y);
/// mouse up event
USINE_MODULE_EXPORT void MouseUp (void* pModule,  TMouseButton MouseButton, TShiftState Shift, float X,float Y);
/// mouse wheel event
USINE_MODULE_EXPORT void MouseWheel (void* pModule,  TShiftState Shift, int WheelDelta);
/// multi touch move event
USINE_MODULE_EXPORT void MouseMoveMulti (void* pModule, TShiftState Shift, UsineEventPtr X, UsineEventPtr Y, UsineEventPtr Pressed);
/// multi touch down event
USINE_MODULE_EXPORT void MouseDownMulti (void* pModule, TMouseButton MouseButton, TShiftState Shift, UsineEventPtr X, UsineEventPtr Y, UsineEventPtr Pressed);
/// multi touch up event
USINE_MODULE_EXPORT void MouseUpMulti (void* pModule, TMouseButton MouseButton, TShiftState Shift, UsineEventPtr X, UsineEventPtr Y, UsineEventPtr Pressed);
/// when double clicking on the module in the patch, should open the editor is any (VST or AU plugin)
USINE_MODULE_EXPORT void OpenEditor(void* pModule);
//Called by Usine to put the plugin Window to front
USINE_MODULE_EXPORT void BringToFront(void* pModule);
//Called by Usine to close the plugin Window
USINE_MODULE_EXPORT void CloseEditor(void* pModule);
//Called by Usine to resize the plugin Window
USINE_MODULE_EXPORT void ResizeEditor(void* pModule, int width, int height);

// usine infos callbacks
/// Called by when the bloc size changes
USINE_MODULE_EXPORT void OnBlocSizeChange (void* pModule, int BlocSize);
/// Called by when the audio sample rate changes
USINE_MODULE_EXPORT void OnSampleRateChange (void* pModule, double SampleRate);

/// usine recording callbacks
USINE_MODULE_EXPORT void SetRecordedValue (void* pModule, TPrecision X, TPrecision Y, TPrecision Z);

/// Called by Usine on randomize
USINE_MODULE_EXPORT void Randomize (void* pModule);

/// Called by Usine on reset
USINE_MODULE_EXPORT void ResetModule(void* pModule);

/// Called by Usine when the user change the global color of the module
USINE_MODULE_EXPORT void SetQuickColor(void* pModule, TUsineColor color);

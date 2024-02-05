///----------------------------------------------------------------------------
///@file UserModule.h
///
///@author
///	BrainModular team
///
///@brief  
///	A generic class provided to derive user modules from.
///
///@HISTORIC 
///	2013/05/15
///    first release for Hollyhock CPP SDK 6.00.226 
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
///----------------------------------------------------------------------------

// include once, no more
#ifndef __USER_MODULE_H__
#define __USER_MODULE_H__

const int PUSH_GRANULOSITY = 128;
//----------------------------------------------------------------------------
// includes
//----------------------------------------------------------------------------
//#include <cmath>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include "UsineDefinitions.h"
#include "UserUtils.h"


//-----------------------------------------------------------------------------
/// @addtogroup Globals
/// @{ 

/// Usine need them to instantiate and destroy the module.
/// @name  Mandatory Functions
/// @{ 
/// @fn void CreateModule(void* &pModule, AnsiCharPtr optionalString, LongBool Flag, TMasterInfo* pMasterInfo, AnsiCharPtr optionalContent)
/// @brief Called by Usine to instantiate the User Module
/// When opening the Browser section where is stored the module.
/// When adding the module in a patch.
/// When loading a saved workspace or patch.
/// @param pModule Handle for the new instantiated module
/// @param optionalString optional string passed to the constructor ie. the plugin filename
/// @param Flag compatibility flag must be FALSE on recent versions of Usine 
/// @param pMasterInfo Handle to the TMasterInfo structure
/// @param optionalContent 
void CreateModule(void* &pModule, AnsiCharPtr optionalString, LongBool Flag, TMasterInfo* pMasterInfo, AnsiCharPtr optionalContent);

/// @fn void DestroyModule(void* pModule)
/// @brief Called by Usine to destroy the User Module
/// When clearing a workspace or patch.
/// Before recreate module.
/// When closing Usine.
/// @param pModule Handle for the new instantiated module
void DestroyModule(void* pModule);

/// @fn void GetBrowserInfo (TModuleInfo* pModuleInfo)
/// @brief Called by usine to get module informations for the browser
/// When the browser open the folder where the module is stored.
void GetBrowserInfo (TModuleInfo* pModuleInfo);
/// @}


//-----------------------------------------------------------------------------
// class definition
//-----------------------------------------------------------------------------
/// @class UserModuleBase
/// @nosubgrouping
/// Base class for every User's module.
/// A User module is a class inherited from UserModuleBase,
/// providing complete communication with Usine and all the necessary stuff to customize the module behaviour. <br>
/// In addition, this base class provides a ton of utility functions to help you integrate your data in the module.
class UserModuleBase 
{
    //-------------------------------------------------------------------------
    // public constructors/destructors
    //-------------------------------------------------------------------------
public:
	UserModuleBase();
	 virtual ~UserModuleBase();

    //-----------------------------------------------------------------------------
	// Constants
    //-----------------------------------------------------------------------------
    /// Used by usine to populate browser infos and module infos.
    /// @name Module Constants
	/// @{ 
    static const AnsiCharPtr MODULE_NAME;
    static const AnsiCharPtr MODULE_DESC;
    static const AnsiCharPtr MODULE_VERSION;
    /// @}

    //-----------------------------------------------------------------------------
	// Usine Callbacks
    //-----------------------------------------------------------------------------
public:
	
	/// Usine need them to collect mandatory infos and update the module.
    /// @name Mandatory Callbacks
	/// @{
	
    /// Usine call it to obtain the module characteristics. 
	/// You have to fill the TModuleInfo structure with infos describing the module.
	/// TMasterInfo can be used to obtain some useful infos but always test if the handle is valid as it can be nullptr. 
    /// @param pMasterInfo The TMasterInfo structure. can be null.
    /// @param pModuleInfo The TModuleInfo structure to describe the module characteristics
	virtual void onGetModuleInfo (TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo)=0;

    /// Usine call it for each parameters of the module to obtain it's characteristics. 
    /// @param ParamIndex The index of the parameter.
    /// @param pParamInfo Handle to the info structure of the parameter.
	virtual void onGetParamInfo (int ParamIndex, TParamInfo* pParamInfo)=0;
	

    /// Usine call it for each parameters of the module to obtain the event handle of the parameter. 
	/// Deprecated in HH3, use TModuleInfo::EventPtr
    /// @param ParamIndex The index of the parameter.
    /// @param pEvent Handle to the event of the parameter.
    virtual void onSetEventAddress (int ParamIndex, UsineEventPtr pEvent){}
	

    /// Usine call it to inform the module something happened. 
    /// @param Message Provide various infos about what happened.
	virtual void onCallBack (TUsineMessage *Message){}
    /// @}
	
    //-----------------------------------------------------------------------------
    /// Needed if Query system is used.
	/// @name Query System Callbacks
	/// @{
	
	/// Usine call it to obtain the number of parameters of the module depending of the query result 
    /// @param queryIndex The index of the choice made in the query popup
    /// @return Number of parameters of the module
	virtual int  onGetNumberOfParams(int queryResult1, int queryResult2)
	{
		return m_moduleInfo->NumberOfParams;
	}
	
	/// Usine call it after the query pop-up is closed. It let you do some things depending of the query choice.  
    /// @param pMasterInfo The TMasterInfo structure.
    /// @param pModuleInfo The TModuleInfo structure.
    /// @param queryIndex The index of the choice made in the query pop-up
	virtual void onAfterQuery (TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo, int queryResult1, int queryResult2) {}
    /// @}

	//-----------------------------------------------------------------------------
	// process
    //-----------------------------------------------------------------------------
    /// Usine call it if TModuleInfo::DontProcess is not set to TRUE in onGetModuleInfo.
	/// @name Real-time Audio and Data Callback 
 	/// @{
	
	/// Be careful here because it's called from the audio thread at each audio tick of the audio engine. 
	/// Keep it optimized and try to avoid any memory allocation.
    /// @see TModuleInfo::DontProcess
	virtual void onProcess (){}
    /// @}
	
    /// @name Real-time Video processing
 	/// @{
	/// Called by the the video thread to process video frames only for TModuleInfo::ModuleType == mtVideo  
	/// Don't process video outside this procedure.
	/// @see TModuleInfo::ModuleType
	virtual void onProcessVideo() {}
	/// @}

	//-----------------------------------------------------------------------------
	/// Implement them if the module need to save some additional info.
	/// A chunk is a memory block containing some binary data.
	/// Usine store those chunk in patch and workspace files and use them to store/restore presets.
	/// @name Load and Save Chunk System
	/// @{
	
	/// To get the chunk length estimation
	/// @param Preset Tell if it's a preset or a patch chunk. TRUE = preset chunk, FALSE = .pat/.wkp chunk.
	/// @return The estimated size of the chunk in byte.
	virtual int  onGetChunkLen (LongBool Preset){return 0;}
	
	/// Provide a chunk handle to fill with additional info's.
	/// @param chunk The chunk pointer to fill.
	/// @param Preset Tell if it's a preset or a patch chunk. TRUE = preset chunk, FALSE = .pat/.wkp chunk.
	virtual void onGetChunk (void* chunk, LongBool Preset){}
	
	/// Provide a chunk handle filled by Usine to restore them into the module.
	/// @param chunk The chunk pointer to read from.
	/// @param sizeInBytes The size of the chunk.
	/// @param Preset Tell if it's a preset or a patch chunk. TRUE = preset chunk, FALSE = .pat/.wkp chunk.
	virtual void onSetChunk (const void* chunk, int sizeInBytes, LongBool Preset){}

	/// Called after the module is loaded in Usine.
	virtual void onAfterLoading() {}


    /// @}

	//-----------------------------------------------------------------------------
	/// Implement them if the module need to add some entries in the Settings Panel.
	/// @name Settings Callbacks
	/// @{
	
	/// To add some entries in the Settings panel.
	/// Typical use is some calls to Settings options functions 
	virtual void onCreateSettings () {}
	
	/// Called when something change in one of entries.
	virtual void onSettingsHasChanged () {}
	/// @}

	//-----------------------------------------------------------------------------
	/// Implement them if the module type is mtControl.
	/// @name Graphics Canvas Callbacks
	/// @{
	
	/// Called when Usine resize the Graphic canvas
	/// @param contentWidth the new width of the canvas.
	/// @param contentHeight the new height of the canvas.
    virtual void onResize (float contentWidth, float contentHeight) {}
	
	/// Called before Usine draw the module canvas
	/// Use it to draw the module canvas.
	/// @see Graphic Canvas
	virtual void onPaint () {}
    /// @}

	//-----------------------------------------------------------------------------
	/// Implement it if the module need to add some entries in the Contextual Menu.
	/// @name Contextual Menu Callback
	/// @{
	
	/// To add some entries in the Contextual Menu.
	/// Typical use is some calls to Contextual Menus commands 
    virtual void onCreateCommands () {}
    /// @}

	//-----------------------------------------------------------------------------
    /// Usine call it if TModuleInfo::CanRecord is set to TRUE in onGetModuleInfo.
	/// @name Automation Recording Callback
	/// @{
	
	/// Usine call it when replaying recorded curves. The module have to affect those values to the concerned variables.
    /// @param X New value of the first curve.
    /// @param Y New value of the second curve.
    /// @param Z New value of the third curve.
	virtual void onSetRecordedValue (TPrecision X, TPrecision Y, TPrecision Z){}
    /// @}
	
    //-----------------------------------------------------------------------------
	/// Implement it to respond to a randomize command from Usine.
    /// @name Randomize Callback
 	/// @{
	
	/// Activate it with pModuleInfo->CanBeRandomized = TRUE in onGetModuleInfo.
	virtual void onRandomize (){}
    /// @}

	//-----------------------------------------------------------------------------
	/// Implement it to respond to a reset command from Usine.
	/// @name Reset Callback
	/// @{

	/// Activate it with pModuleInfo->CanBeReset = TRUE in onGetModuleInfo.
	virtual void onReset() {}
	/// @}

	//-----------------------------------------------------------------------------
	/// Implement it to respond to a Set quick colorfrom Usine.
	/// @name SetQuickColor Callback
	/// @{
	/// @param color choosen color in the quick color menu.
	virtual void onSetQuickColor(TUsineColor color) {}
	/// @}
    
	//-----------------------------------------------------------------------------
	/// Implement them if the  module type is mtControl and need to be informed of mouse and/or multi-touch events.
	/// @name Mouse and Multi-touch Callbacks
	/// @{

	/// Called by Usine when the mouse moves on the control
	///@name onMouseMove
	virtual void onMouseMove (TShiftState Shift, float X, float Y){}
	
	/// Called by Usine when the mouse is clicked down on the control
	virtual void onMouseDown (TMouseButton MouseButton, TShiftState Shift, float X,float Y){}
	
	/// Called by Usine when the mouse is clicked up on the control
	virtual void onMouseUp (TMouseButton MouseButton, TShiftState Shift, float X,float Y){}

	/// Called by Usine when the mouse wheel is moved on the control
    virtual void onMouseWheel (TShiftState Shift, int WheelDelta){};

	/// Called by Usine when control receives a touch-screen move event
	virtual void onMouseMoveMulti(TShiftState Shift, UsineEventPtr X, UsineEventPtr Y, UsineEventPtr Pressed) {};
	
	/// Called by Usine when control receives a touch-screen move event
	virtual void onMouseMoveMulti(TShiftState Shift, UsineEventClass* X, UsineEventClass* Y, UsineEventClass* Pressed) {};

	/// Called by Usine when control receives a touch-screen down event
	virtual void onMouseDownMulti(TMouseButton MouseButton, TShiftState Shift, UsineEventPtr X, UsineEventPtr Y, UsineEventPtr Pressed){};

	///Called by Usine when control receives a touch-screen down event
	virtual void onMouseDownMulti(TMouseButton MousButton, TShiftState Shift, UsineEventClass* X, UsineEventClass* Y, UsineEventClass* Pressed){};
	
	/// Called by Usine when control receives a touch-screen up event
	virtual void onMouseUpMulti(TMouseButton MouseButton, TShiftState Shift,UsineEventPtr X, UsineEventPtr Y, UsineEventPtr Pressed){};
	virtual void onMouseUpMulti(TMouseButton MouseButton, TShiftState Shift, UsineEventClass* X, UsineEventClass* Y, UsineEventClass* Pressed){};
    /// @}

	//-----------------------------------------------------------------------------
	/// @name Optional Callbacks
	/// @{
	
	/// Usine call it after onGetModuleInfo. It let you initialising some stuff.  
    /// @param pMasterInfo The TMasterInfo structure.
    /// @param pModuleInfo The TModuleInfo structure.
	virtual void onInitModule (TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo){};

	/// Usine call it when the Usine BlockSize is changed.  
    /// @param BlocSize The new BlocSize.
	virtual void onBlocSizeChange (int BlocSize){};
	
	/// Usine call it  when the SampleRate is changed.  
    /// @param SampleRate The new SampleRate.
	virtual void onSampleRateChange (double SampleRate){};
    /// @}

	//-----------------------------------------------------------------------------
	/// Implement them if the  module implement a child window.
	/// @see Module child window
	/// @name Child Window Callbacks
	/// @{
	
	
	virtual void onOpenEditor () {};
	
	virtual void onBringToFront () {};
	
	virtual void onCloseEditor () {};
	
	virtual void onResizeEditor (int width, int height){ };
    /// @}
	
	//-----------------------------------------------------------------------------
	/// Usine call them to send MIDI messages to a Midi Out Device.
	/// Not for public use.
	/// @name MIDI Out Callbacks
	/// @{
	
	/// Send one midi code.
	/// @param DeviceID Index of the device to address. 
	/// @param Code The midi message to send.
	virtual void onMidiSendOut (int DeviceID, TUsineMidiCode Code){};
	
	/// Send a block of midi code.
	/// @param DeviceID Index of the device to address.
	/// @param arrayCode The array of midi message to send.
	/// @param arraySize The size of the array.
    virtual void onMidiSendOutArray (int DeviceID, TUsineMidiCode** arrayCode, int arraySize){};
	
	/// Send a Sysex message. 
	/// @param DeviceID Index of the device to address.
	/// @param Sysex The Sysex message to send.
	/// @param sysexSize The size of the Sysex message in byte.
	virtual void onMidiSysexSendOut (int DeviceID,  char** Sysex, int sysexSize){};
    /// @}
	
	//-----------------------------------------------------------------------------
	/// Usine call it to send ILDA frame to a ILDA Out Device.
	/// Not for public use.
	/// @name ILDA Out Callback
	/// @{

	/// Send an Laser frame. 
	/// @param DeviceID Index of the device to address.
	/// @param arrayPoint The array of ILDAPoints to send.
	/// @param arraySize The size of the ILDAPoints array.
	virtual void onLaserSendOut(int DeviceID, TUsineILDAPoint** arrayPoint, int arraySize, int speedPPS) {};
	/// @}


	//-----------------------------------------------------------------------------
	/// Usine call it to send DMX packet to a Dmx Out Device.
	/// Not for public use.
	/// @name DMX Out Callback
	/// @{
	
	/// Send a Dmx message. 
	/// @param deviceId Index of the device to address.
	/// @param ByteArray The Dmx packet to send.
	/// @param len The size of the Dmx packet in byte.
	/// @param universeNum The DMX universe to address, in case the device can manage more than one, start at 1, default value of 1.
	virtual void onDMXSendOut (int deviceId, char* ByteArray, int len, int universeNum = 1){};
    /// @}
	
	//----------------------------------------------------------------------------
    // internal use
    inline void AfterQueryPopup (TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo, int queryResult1, int queryResult2);
    inline void Init(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo);
    void InitInfosStructures(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo);
	inline void CallBack (TUsineMessage *Message);
    inline void ResizeModule (float contentWidth, float contentHeight);



public:
	//-----------------------------------------------------------------------------
    // wrappers for TMasterInfo functions
    //-----------------------------------------------------------------------------
    
	//-----------------------------------------------------------------------------
	// graphics methods
    //-----------------------------------------------------------------------------
	/// Use them in the onPaint callback to draw into a module canvas.
    /// @name  Graphic Canvas
	/// @{ 
	
	/// Draw a point 
	inline void sdkDrawPoint (TPointF point, TUsineColor color, float pointSize, LongBool rounded){ 
		m_masterInfo->DrawPoint (m_moduleInfo, point, color, pointSize, rounded);};
    
	/// Draw a line
	inline void sdkDrawLine (TPointF p1, TPointF p2, TUsineColor color, float strokeThickness) { 
		m_masterInfo->DrawLine (m_moduleInfo, p1, p2, color, strokeThickness);};
	
	/// Draw a polygon, points are stored in an array
	inline void sdkDrawPolyLine (TPointFPtr points, int numOfPoints, TUsineColor color, float strokeThickness) { 
		m_masterInfo->DrawPolyLine (m_moduleInfo, points, numOfPoints, color, strokeThickness);};
	
	/// Fille a polygon, points are stored in an array
	inline void sdkFillPolyLine (TPointFPtr points, int numOfPoints, TUsineColor color) { 
		m_masterInfo->FillPolyLine (m_moduleInfo, points, numOfPoints, color);};
	
	/// Fill a round rectangle
	inline void sdkFillRect (TRectF rect, TUsineColor color, float roundRadius, TUsineColor borderColor, float borderWith) { 
		m_masterInfo->FillRect (m_moduleInfo, rect, color, roundRadius, borderColor, borderWith);};
	
	///	Fill a text 
	inline void sdkFillText (TRectF rect, AnsiCharPtr text, TUsineColor textColor, float fontSize, LongBool isBold = FALSE, LongBool isWordWrap = FALSE, TTextAlign alignHorizontal = taCenter, TTextAlign alignVertical = taCenter, LongBool isVertical = FALSE) { 
		m_masterInfo->FillText (m_moduleInfo, rect, text, textColor, fontSize, isBold, isWordWrap, alignHorizontal, alignVertical, isVertical);};

	/// Draw an array of pixels.
	/// @param pArray pointer to the first element of the pixels array
	/// @param W width of the pixels array
	/// @param H height of the pixels array
	/// @param destRect destination rectangle in normalized coordinates (0..1).
	/// No verification of the pixels array size is made so be sure that W and H have the right values.
	inline void sdkBitBlit(TUsinePixelPtr pArray, int W, int H, TRectF destRect) {
		m_masterInfo->BitBlit(m_moduleInfo, pArray,W,H,destRect);
	};

    /// @}
    
	/// @{ 
    /// @name  Graphic Path
    /// 
    ///   
    ///   Example of constructing a path @code 
    ///   // begin draw path sequence
    ///   sdkDrawPathStart ();
	///
    ///   // move to where we want to start the path
    ///   sdkDrawPathMoveTo (sdkPointF(0.25, 0.25));
    ///
    ///   // add a line 
    ///   sdkDrawPathLineTo (sdkPointF(0.75, 0.25));
    ///   
    ///   // add a curve 
    ///   sdkDrawPathQuadCurveTo (sdkPointF(0.75, 0.75), sdkPointF(0.25, 0.75));
    ///   
    ///   // close the path 
    ///   sdkDrawPathClose ();
    ///
    ///   // end the command sequence and ask to draw the path
    ///   sdkDrawPathDraw (colorTrajectory, thicknessTrajectory);
    ///@endcode
       
	/// Tell Usine to start a path sequence.
	/// A path must begin with this function, followed by a sequence of command describing the path structure and finally drawn or filled.
	inline void sdkDrawPathStart () { 
		m_masterInfo->DrawPathClear (m_moduleInfo);};
    
	/// Move the current position inside the path. @see sdkDrawPathStart
	inline void sdkDrawPathMoveTo (TPointF point) { 
		m_masterInfo->DrawPathMoveTo (m_moduleInfo, point);};
   
	/// Add a line to the path, from the current position to the point provided. @see sdkDrawPathStart
	inline void sdkDrawPathLineTo (TPointF point) { 
		m_masterInfo->DrawPathLineTo (m_moduleInfo, point);};
    	
	/// Add a quadratic curve to the path, from the current position to the point provided.
    /// The curve don't pass by the control point.
    /// @see sdkDrawPathStart
	inline void sdkDrawPathQuadCurveTo (TPointF control, TPointF point) { 
		m_masterInfo->DrawPathQuadCurveTo (m_moduleInfo, control, point);};
    
	/// Add a line to close the path, from the current position to the first point of the path.
    /// @see sdkDrawPathStart
	inline void sdkDrawPathClose () { 
		m_masterInfo->DrawPathClose (m_moduleInfo);};
   
	/// Draw the current path with the color and thickness provided
    /// @see sdkDrawPathStart
	inline void sdkDrawPathDraw (TUsineColor color, float strokeThickness) { 
		m_masterInfo->DrawPathDraw (m_moduleInfo, color, strokeThickness);};
   
	/// Fill a valid path with the color and thickness provided
    /// @see sdkDrawPathStart
	inline void sdkDrawPathFill (TUsineColor color) { 
		m_masterInfo->DrawPathFill (m_moduleInfo, color);
	};
    
    /// Ask Usine to repaint the module Panel.
    /// The result is a call to the onPaint callback.
    inline void sdkRepaintPanel (void)
    { 
        if ( (m_masterInfo != 0) && (m_moduleInfo != 0) ) { m_masterInfo->RepaintPanel( m_moduleInfo ); }; 
    };
    /// @}

    //-----------------------------------------------------------------------------
    // Automation Recording functions
    //-----------------------------------------------------------------------------
	/// To record some values for the Automation Recording system.
    /// @name Automation Recording
	/// @{

	/// Stops the recording
	inline void sdkStopRecord () { 
		m_masterInfo->StopRecord( m_moduleInfo);}

    /// Start the recording process if it is not started and processes the recording.
	/// to stop the recording use sdkStopRecord
	/// @see sdkStopRecord 
	inline void sdkProcessRecord(TPrecision X, TPrecision Y, TPrecision Z ) { 
		m_masterInfo->ProcessRecord( m_moduleInfo, X, Y, Z );};
    /// @}

    //----------------------------------------------------------------------------
	/// Use them in onCreateSettings to add some entries in the Settings Panel.
    /// @name Settings options 
	/// @{
	
	/// To add a text entry in a Tab of the Settings Panel. 
    inline void sdkAddSettingLineCaption (AnsiCharPtr tab, AnsiCharPtr caption = "", TUsineColor color = 0, LongBool Translate = FALSE)
		{ m_masterInfo->AddSettingLineCaption (m_moduleInfo, tab, caption, color, Translate); };
	
	/// To add a color selector entry in a Tab of the Settings Panel.
    inline void sdkAddSettingLineColor (AnsiCharPtr tab, TUsineColor* pVal, AnsiCharPtr Caption, LongBool Translate = FALSE)              
		{ m_masterInfo->AddSettingLineColor (m_moduleInfo, tab, pVal, Caption, Translate); };
	
	/// To add a boolean entry in a Tab of the Settings Panel.
    inline void sdkAddSettingLineBoolean (AnsiCharPtr tab, LongBool* pVal, AnsiCharPtr Caption, LongBool Translate = FALSE)
		{ m_masterInfo->AddSettingLineBoolean (m_moduleInfo, tab, pVal, Caption, Translate); };
	
	/// To add an integer entry in a Tab of the Settings Panel.
    inline void sdkAddSettingLineInteger (AnsiCharPtr tab, int* pVal, AnsiCharPtr Caption, int min, int max, TScale scale, AnsiCharPtr symbol, int defaultValue, LongBool Translate = FALSE) 
		{ m_masterInfo->AddSettingLineInteger (m_moduleInfo, tab, pVal, Caption, min, max, scale, symbol, defaultValue, Translate); };
	
	/// To add a decimal number entry in a Tab of the Settings Panel.
	inline void sdkAddSettingLineSingle (AnsiCharPtr tab, float* pVal, AnsiCharPtr caption, float min, float max, TScale scale, AnsiCharPtr symbol, AnsiCharPtr format, float defaultValue, LongBool Translate = FALSE)
		{ m_masterInfo->AddSettingLineSingle (m_moduleInfo, tab, pVal, caption, min, max, scale, symbol, format, defaultValue, Translate); };
		
	/// To add a combo box entry in a Tab of the Settings Panel.
    inline void sdkAddSettingLineCombobox (AnsiCharPtr tab, int* pVal, AnsiCharPtr caption, AnsiCharPtr commaText, LongBool Translate = FALSE){
		m_masterInfo->AddSettingLineCombobox(m_moduleInfo, tab, pVal, caption, commaText, Translate);};

	/// To add a string entry in a Tab of the Settings Panel.
	inline void sdkAddSettingsLineString(AnsiCharPtr tab, AnsiCharPtr pVal, AnsiCharPtr caption, LongBool Translate = FALSE){
		m_masterInfo->AddSettingsLineString(m_moduleInfo, tab, pVal, caption, Translate);};

	/// Setter for an existing setting line entry
	inline void sdkSetSettingValue(AnsiCharPtr settingName, UsineEventPtr settingEvent){
		m_masterInfo->SetSettingValue(m_moduleInfo, settingName, settingEvent);}

	/// Getter for an existing setting line entry
	inline UsineEventPtr sdkGetSettingValue(AnsiCharPtr settingName){
		UsineEventPtr settingEvent = nullptr;
		m_masterInfo->GetSettingValue(m_moduleInfo, settingName, settingEvent);
		return settingEvent;}
	/// @}

    //----------------------------------------------------------------------------
	/// Use them in the onCreateCommands callback to add commands in the module Contextual Menu.
    /// @name Contextual Menus commands 
	/// @{

	/// To add a Command entry in the module Contextual Menu.
    inline void sdkAddCommand(AnsiCharPtr name, NativeInt CallbackId, LongBool Translate = FALSE)      { 
		m_masterInfo->AddCommand (m_moduleInfo, name, CallbackId, Translate); };

	/// To add a named separator in the module Contextual Menu.
    inline void sdkAddCommandSeparator(AnsiCharPtr name, LongBool Translate = FALSE)                   {
		m_masterInfo->AddCommandSeparator (m_moduleInfo, name, Translate); };
    /// @}


	//----------------------------------------------------------------------------
	/// Utils functions to help manipulate video frames
	/// @name Video Frame manipulation 
	/// @{
	
	/// Get incoming video Frames
	inline void sdkGetInputFrame(int numInput, TUsineFramePtr frame) { 
		m_masterInfo->GetInputFrame(m_moduleInfo, numInput, frame); };

	/// Set outgoing video Frames
	inline void sdkSetOutputFrame(int numOutput, TUsineFramePtr frame) { 
		m_masterInfo->SetOutputFrame(m_moduleInfo, numOutput, frame); }

    /// Clear, fill a frame with a color
	inline void sdkClearFrame(TUsineFramePtr frame, TUsineColor color) { 
		m_masterInfo->ClearFrame(m_moduleInfo, color, frame); }
	
	/// Get a new frame
	/// @param permanent Determines if the frame has a short lifespan (FALSE) or an infinite (TRUE)
	///
	/// Permanent frames must be destroyed by sdkReleaseFrame
	///
	/// Non permanent frames are released automatically
	///@see sdkReleaseFrame
	inline void sdkGetNewFrame(TUsineFramePtr frame, int width, int height, LongBool permanent) { 
		m_masterInfo->GetNewFrame(m_moduleInfo, width, height, permanent, frame); }

	/// Release permanent frames
	inline void sdkReleaseFrame(TUsineFramePtr frame) { 
		m_masterInfo->ReleaseFrame(m_moduleInfo, frame); }

    /// Copy a frame into another
	inline void sdkCopyFrame(TUsineFramePtr srcFrame, TUsineFramePtr destFrame) { 
		m_masterInfo->CopyFrame(srcFrame,destFrame); }

    /// Apply a dimmer in the range [0..1] to a frame
	inline void sdkSetDimmerFrame(TUsineFramePtr frame, float dimmer) { 
		m_masterInfo->SetDimmerFrame(m_moduleInfo, dimmer, frame); }

    /// Set a pixel at position X,Y in the frame
	inline void sdkSetPixel(TUsineFramePtr frame, int X, int Y, TUsinePixel pixel){
		TUsinePixelPtr pix = frame->Pixels;
		pix = pix + frame->Width*Y + X;
		*pix = pixel;}

    /// Get a pixel at position X,Y in the frame
	inline TUsinePixel sdkGetPixel(TUsineFramePtr frame, int X, int Y){
		TUsinePixelPtr pix = frame->Pixels;
		pix = pix + frame->Width*Y + X;
		return *pix;}

    /// Transforms a TUsineColor into a TUsinePixel 
	inline TUsinePixel sdkColorToPixel(TUsineColor color) { 
		return m_masterInfo->ColorToPixel(color); }

    /// Transforms a TUsinePixel into a TUsineColor
	inline TUsineColor sdkPixelToColor(TUsinePixel pixel) { 
		return m_masterInfo->PixelToColor(pixel); }

	/// @}

	//----------------------------------------------------------------------------
	/// Critical sections functions to protect patches calculation.
	/// @name Critical Sections 
	/// @{

	/// Lock the execution of the current patch to prevent memory access conflicts between the audio thread and other threads like IDLE, video, etc.
	inline void	sdkLockPatch() { 
		m_masterInfo->LockPatch(m_moduleInfo); };

	/// UnLock the execution of the current patch.
	inline void	sdkUnLockPatch() { 
		m_masterInfo->UnLockPatch(m_moduleInfo); 
	};

	/// Lock a critical section.
	/// @param timeout must be used with precautions 
	inline void sdkCriticalSectionLock (TCriticalSectionPtr pCriticalSection, UINT32 timeout = 0 ){
		m_masterInfo->CriticalSectionLock(m_moduleInfo, pCriticalSection, timeout);
	}

	/// TryLock a critical section.
	inline LongBool sdkCriticalSectionTryLock(TCriticalSectionPtr pCriticalSection) {
		return m_masterInfo->CriticalSectionTryLock(m_moduleInfo, pCriticalSection);
	}
	
	/// UnLock a critical section.
	inline void sdkCriticalSectionUnLock(TCriticalSectionPtr pCriticalSection) {
		m_masterInfo->CriticalSectionUnLock(m_moduleInfo, pCriticalSection);
	}

	/// Create a critical section.
	/// @param name is a name givent to the CS for debug purposes
	inline TCriticalSectionPtr sdkCriticalSectionCreate(AnsiCharPtr name, LongBool shared = FALSE) {
		return m_masterInfo->CriticalSectionCreate(m_moduleInfo, name, shared);
	}

	/// Destroy a critical section.
	inline void sdkCriticalSectionDestroy(TCriticalSectionPtr pCriticalSection) {
		m_masterInfo->CriticalSectionDestroy(m_moduleInfo, pCriticalSection);
	}
	
	/// @}


	//----------------------------------------------------------------------------
	/// Threads for multi-threading 
	/// @name Threads 
	/// @{
	 
	/// Create a thread.
	/// @param priority priority of the thread : tpIDLE,tpLow,tpMedium,tpHigh
	/// @param timeout timeout of the thread (execution clock) in ms. if timeout is UINT32_MAX then the thread will wait until the sdkThreadRestart is called 
	/// 
	inline TThreadPtr sdkThreadCreate(AnsiCharPtr name, FuncThreadProcess ProcessThreadProc, TThreadPriority priority, UINT32 timeout) {
		return m_masterInfo->ThreadCreate(m_moduleInfo, name, ProcessThreadProc, priority, timeout);
	}

	/// Destroy a thread.
	inline void sdkThreadDestroy(TThreadPtr pThread) {
		m_masterInfo->ThreadDestroy(pThread);
	}

	/// restart a thread.
	inline void sdkThreadRestart(TThreadPtr pThread) {
		m_masterInfo->ThreadRestart(pThread);
	}

	/// set timeout of a thread.
	/// @param timeout timeout of the thread (execution clock) in ms
	inline void sdkThreadSetTimeOut(TThreadPtr pThread, UINT32 timeout) {
		m_masterInfo->ThreadSetTimeOut(pThread, timeout);
	}
	/// @}

    //----------------------------------------------------------------------------
 	/// Utils functions to update some Parameter property other than the Event data.
    /// @name Parameters setters 
	/// @{

	/// If the parameter is of type ptListBox, change the comma text defining the list content. @see onGetParamInfo;
	/// @param numParam the param index.
    inline void	sdkSetListBoxCommaText( int numParam, AnsiCharPtr commaText )  { 
		m_masterInfo->SetListBoxCommaText( m_moduleInfo, numParam, commaText ); };
    
	/// Change the name of a parameter. It's the name showed in Usine.
	/// @param numParam the param index.
	inline void	sdkSetParamCaption( int numParam, AnsiCharPtr caption )		 { 
		m_masterInfo->SetParamCaption( m_moduleInfo, numParam, caption ); };
    
	/// Set a parameter visible or not.
	/// @param numParam the param index.
	inline void	sdkSetParamVisible( int numParam, LongBool visible )	        { 
		m_masterInfo->SetParamVisible( m_moduleInfo, numParam, visible ); };

	/// Recreate a parameter with new settings.
	/// @param numParam the param index.
	/// @param pParamInfo pointer to TParamInfo structure 
	inline void	sdkRecreateParam(int numParam, TParamInfo* pParamInfo) { 
		m_masterInfo->RecreateParam(m_moduleInfo, numParam, pParamInfo); };

	/// If the parameter is of type ptTextField, change the text content. @see onGetParamInfo;
	/// @param numParam the param index.
	inline void	sdkSetParamValueText( int numParam, AnsiCharPtr valueText )		 { 
		m_masterInfo->SetParamValueText( m_moduleInfo, numParam, valueText ); };	
    
	/// Ask Usine to Update the parameter.
	/// Usually used after one of the function above.
	/// @param numParam the param index.
	inline void	sdkRepaintParam( int numParam)							            { 
		m_masterInfo->RepaintParam(m_moduleInfo, numParam); };
    /// @}

	//----------------------------------------------------------------------------
	/// @name Usine Objects functions 
	/// @{

	/// Set the float value of an Usine-Object
	inline void sdkUsineObjectSetFloat(AnsiCharPtr name, TPrecision floatValue) { 
		m_masterInfo->UsineObjectSetFloat(m_moduleInfo, name, floatValue); };
	
	/// Set the integer value of an Usine-Object
	inline void sdkUsineObjectSetInteger (AnsiCharPtr name, int integerValue) { 
		m_masterInfo->UsineObjectSetInteger(m_moduleInfo, name, integerValue); };
	
	/// Set the float value of an Usine-Object array at the index position
    inline void sdkUsineObjectSetArray(AnsiCharPtr name, int index, TPrecision floatValue) { 
		m_masterInfo->UsineObjectSetArray(m_moduleInfo, name, index, floatValue); };
	
	/// Set the Midi value of an Usine-Object array at the index position
	inline void sdkUsineObjectSetMIDI (AnsiCharPtr name, int index, TUsineMidiCode midiValue) { 
		m_masterInfo->UsineObjectSetMIDI(m_moduleInfo, name, index, midiValue); };
	
	/// Set the color value of an Usine-Object 
	inline void sdkUsineObjectSetColor (AnsiCharPtr name, TUsineColor colorValue) {
		m_masterInfo->UsineObjectSetColor(m_moduleInfo, name, colorValue); };
    
	/// Set the color value of an Usine-Object array at the index position
	inline void sdkUsineObjectSetArrayColor (AnsiCharPtr name, int index, TUsineColor colorValue) {
		m_masterInfo->UsineObjectSetArrayColor(m_moduleInfo, name, index, colorValue); };
	
	/// Set a trigger value of an Usine-Object
	inline void sdkUsineObjectSetTrigger (AnsiCharPtr name) { 
		m_masterInfo->UsineObjectSetTrigger(m_moduleInfo, name); };
	
    /// Set the text value of an Usine-Object
    inline void sdkUsineObjectSetAnsiChar(AnsiCharPtr name, AnsiCharPtr v) {
		m_masterInfo->UsineObjectSetAnsiChar(m_moduleInfo, name, v); };
	
	/// Get the float value of an Usine-Object
	inline TPrecision sdkUsineObjectGetFloat (AnsiCharPtr name) { 
		return m_masterInfo->UsineObjectGetFloat(m_moduleInfo, name);};
	
	/// Get the integer value of an Usine-Object 
	inline int  sdkUsineObjectGetInteger(AnsiCharPtr name) { 
		return m_masterInfo->UsineObjectGetInteger(m_moduleInfo, name); };
	
	/// Get the float value of an Usine-Object array at the index position
	inline TPrecision sdkUsineObjectGetArray(AnsiCharPtr name, int index) {
		return m_masterInfo->UsineObjectGetArray(m_moduleInfo, name, index); };
	
	/// Get the Midi value of an Usine-Object array at the index position
	inline TUsineMidiCode sdkUsineObjectGetMIDI(AnsiCharPtr name, int index) { 
		return m_masterInfo->UsineObjectGetMIDI(m_moduleInfo, name, index); };
	
	/// Get the color value of an Usine-Object
	inline TUsineColor sdkUsineObjectGetColor(AnsiCharPtr name) {
		return m_masterInfo->UsineObjectGetColor(m_moduleInfo, name); };
	
	/// Get the array value of an Usine-Object array at the index position
    inline TUsineColor sdkUsineObjectGetArrayColor(AnsiCharPtr name, int index) {
		return m_masterInfo->UsineObjectGetArrayColor(m_moduleInfo, name, index);};
	
	/// Get the text value of an Usine-Object
	inline AnsiCharPtr sdkUsineObjectGetAnsiChar(AnsiCharPtr name) {
		return m_masterInfo->UsineObjectGetAnsiChar(m_moduleInfo, name); };
	
	/// Get the size of the Event associated to an Usine-Object
    inline int sdkUsineObjectGetLength(AnsiCharPtr name) {
		return m_masterInfo->UsineObjectGetLength(m_moduleInfo, name); };
	
	/// Set the size of the Event associated to an Usine-Object
	inline void        sdkUsineObjectSetLength(AnsiCharPtr name, int l) { 
		m_masterInfo->UsineObjectSetLength(m_moduleInfo, name, l); };

	/// Returns the list (as a comma-text) of all Usine-Objects that match with the name parameter
	inline AnsiCharPtr sdkUsineObjectFind(AnsiCharPtr name) {
		return m_masterInfo->UsineObjectFind(m_moduleInfo, name); };
	
	/// Returns TRUE if the Usine-Objects exists
	inline LongBool sdkUsineObjectExists   (AnsiCharPtr name) { 
		return m_masterInfo->UsineObjectExists(m_moduleInfo, name); };

	/// Returns the Hash of all	Usine-Objects. This allows to determines if the list of all the Usine-Objects has changed.
	inline AnsiCharPtr sdkUsineObjectListHash() {
		return m_masterInfo->UsineObjectListHash(m_moduleInfo);	};

	/// @} 

	//----------------------------------------------------------------------------
	/// @name Usine Core infos and services
	/// @{
	    
    /// Usine internal messages
    inline void sdkSendUsineMsg( AnsiCharPtr Msg ) { 
		m_masterInfo->SendUsineMsg( m_moduleInfo, Msg ); };
   
    /// Usine notification
	///
	/// Send a message to Usine, Usine's setup or this user module.
	/// A delay maybe specified.
    inline void	sdkNotifyUsine(NativeInt Target, NativeInt Msg, NativeInt WParam = 0, NativeInt LParam = 0, int Delay = 0){
		m_masterInfo->NotifyUsine(m_moduleInfo, Target, Msg, WParam, LParam, Delay);};
 
    /// To access the current TVstTimeInfo structure of Usine.
	inline TVstTimeInfo* sdkGetVstTimeInfo() {
		return m_masterInfo->GetVstTimeInfo(m_moduleInfo);};
    
    /// To get the Usine global time in millisecond, precision up to nanoseconde
    inline double sdkGetTimeMs () { 
		return m_masterInfo->GetTimeMs (m_moduleInfo); };

	/// To check if the patch containing the module is running.
    inline LongBool	sdkPatchIsRunning    () { 
		return m_masterInfo->PatchIsRunning (m_moduleInfo); };
    
	/// To check if the patch containing the module is loading.
	inline LongBool sdkPatchLoading() { 
		return m_masterInfo->PatchLoading(m_moduleInfo); }

	/// To check if the patch has just started.
	inline LongBool	sdkPatchJustActivated() { 
		return m_masterInfo->PatchJustActivated(m_moduleInfo); };

	/// Gets a tick clock at the refresh-speed of Usine, generally every 25 ms.
	/// this can reduce the cpu load for non priority tasks
	///
	///  if (sdkRefreshSpeedClock()) { doSomething(); }
	///
	inline LongBool	sdkRefreshSpeedClock() { 
		return m_masterInfo->RefreshSpeedClock(m_moduleInfo); };
	
	/// Gets a tick clock at the slow-speed of Usine, generally every second.
	/// this can reduce the cpu load for very low priority tasks
	///
	///  if (sdkSlowClock()) { doSomething(); }
	///
	inline LongBool	sdkSlowClock() { 
		return m_masterInfo->SlowClock(m_moduleInfo); };

	//-----------------------------------------------------------------------------
    // wrappers for TModuleInfo functions
    //-----------------------------------------------------------------------------
    /// To get the pointer of the Usine module. Do not use directly.
    inline void* sdkGetUsineUserModule() { 
		return m_moduleInfo->UsineUserModule; };
	
    /// To get the version number of Usine as a string.  
    inline AnsiCharPtr sdkGetUsineVersionNum() { 
		return m_masterInfo->UsineVersion; }; 

	/// set the name of the module (as it appear in the patch, on the module title)
	inline void sdkSetModuleUserName(AnsiCharPtr name) { 
		m_masterInfo->SetModuleUserName(m_moduleInfo, name); };
	
	/// get the name of the module (as it appear in the patch, on the module title)
	inline AnsiCharPtr sdkGetModuleUserName() { 
		return m_masterInfo->GetModuleUserName(m_moduleInfo); };
    /// @}		
    
    //----------------------------------------------------------------------------
    /// A User module can create and manage an independent child window for it's own use.
    /// Note that only one child window at a time per module is possible.
	/// @see Child Window Callbacks
	/// @name Module child window
	/// @{	 
	 
    inline void* sdkCreateDeskWindow () { 
		return m_masterInfo->CreateDeskWindow (m_moduleInfo); };
    
	inline void	sdkDestroyDeskWindow () { 
		m_masterInfo->DestroyDeskWindow (m_moduleInfo); };
    
	inline void* sdkShowDeskWindow    () { 
		return m_masterInfo->ShowDeskWindow (m_moduleInfo); };
    
	inline void	sdkHideDeskWindow    () { 
		m_masterInfo->HideDeskWindow (m_moduleInfo); };
 
    inline void	sdkSetDeskWindowClientSize  (int clientWidth, int clientHeight){
        m_masterInfo->SetDeskWindowClientSize  (m_moduleInfo, clientWidth, clientHeight);
    };   
    inline void	sdkSetDeskWindowCaption    (TModuleInfo* pModuleInfo, AnsiCharPtr name){ 
		m_masterInfo->SetDeskWindowCaption (m_moduleInfo, name); };
    /// @}	

    //----------------------------------------------------------------------------
    /// Useful to convert between pixel coord and graphic canvas unit.
	/// @name Converter utils
	/// @{	 

	/// Transform the Y position in pixels into [0..1] 
    inline float sdkPixelToHeightPercent(float pixel){
        float result = 0.0f;
        if (panelHeight > 0.0f)
            result = pixel / panelHeight;
        return result;};

    /// Transform the X position in pixels into [0..1] 
    inline float sdkPixelToWidthPercent(float pixel){
        float result = 0.0f;
        if (panelHeight > 0.0f)
            result = pixel / panelWidth;        
        return result;};

    /// Transform [0..1] percent Y position into the position in pixels
    inline float sdkHeightPercentToPixel(float percent){
        float result = 0.0f;
        if (panelHeight > 0.0f)
            result = panelHeight * percent;
        return result;};

    /// Transform [0..1] percent X position into the position in pixels
    inline float sdkWidthPercentToPixel(float percent){
        float result = 0.0f;
        if (panelHeight > 0.0f)
            result = panelWidth * percent;        
        return result;};
    /// @}

	//------------------------------------------------------------------------
    // TMasterInfo
    //------------------------------------------------------------------------

	static void sdkSetMasterInfo(TMasterInfo* masterInfo) { m_masterInfo = masterInfo; };
	static const TMasterInfo* sdkGetMasterInfo() { return m_masterInfo; };
	// Pointer to TMasterInfo provided by Usine.
	static TMasterInfo* m_masterInfo;

    //-------------------------------------------------------------------------
    // protected members
    //-------------------------------------------------------------------------
protected:

    float panelWidth; // in pixel
    float panelHeight; // in pixel


    // Pointer TModuleInfo provided by usine
    TModuleInfo* m_moduleInfo;

    //-----------------------------------------------------------------------------
    // protected methods
    //-----------------------------------------------------------------------------
protected:


    //-----------------------------------------------------------------------------
    // private methods
    //-----------------------------------------------------------------------------
private:

    // declare non copyable
	UserModuleBase (const UserModuleBase&);
    UserModuleBase& operator= (const UserModuleBase&);

}; // class UserModuleBase


#endif // __USER_MODULE_H__


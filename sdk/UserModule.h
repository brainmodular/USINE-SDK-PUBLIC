///----------------------------------------------------------------------------
///@file UserModule.h
///
///@author
///	BrainModular team
///
///@brief  
///	A generic class provided to derive user modules from.
///
///HISTORIC 
///	2013/05/15
///    first release for Hollyhock CPP SDK 6.00.226 
///
///IMPORTANT
///	This file is part of the Usine Hollyhock CPP SDK
///
///  Please, report bugs and patch to Usine forum :
///  support@brainmodular.com 
///
/// All dependencies are under there own licence.
///
///@LICENCE
/// Copyright (C) 2013, 2014, 2015, 2019 BrainModular
/// 
/// Permission is hereby granted, free of charge, to any person obtaining a copy of 
/// this software and associated documentation files (the "Software"), 
/// to deal in the Software without restriction, including without limitation 
/// the rights to use, copy, modify, merge, publish, distribute, sublicense, 
/// and/or sell copies of the Software, and to permit persons to whom the Software 
/// is furnished to do so, subject to the following conditions:
/// 
/// The above copyright notice and this permission notice shall be included in all 
///     copies or substantial portions of the Software.
/// 
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
/// SOFTWARE.
///
///----------------------------------------------------------------------------

// include once, no more
#ifndef __USER_MODULE_H__
#define __USER_MODULE_H__

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
	virtual int  onGetNumberOfParams( int queryIndex)
	{
		return m_moduleInfo->NumberOfParams;
	}
	
	/// Usine call it after the query pop-up is closed. It let you do some things depending of the query choice.  
    /// @param pMasterInfo The TMasterInfo structure.
    /// @param pModuleInfo The TModuleInfo structure.
    /// @param queryIndex The index of the choice made in the query pop-up
	virtual void onAfterQuery (TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo, int queryIndex) {}
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
	virtual void onMouseMoveMulti (TShiftState Shift, UsineEventPtr X, UsineEventPtr Y, UsineEventPtr Pressed){};
	
	/// Called by Usine when control receives a touch-screen down event
	virtual void onMouseDownMulti (TMouseButton MouseButton, TShiftState Shift, UsineEventPtr X, UsineEventPtr Y, UsineEventPtr Pressed){};
	
	/// Called by Usine when control receives a touch-screen up event
	virtual void onMouseUpMulti (TMouseButton MouseButton, TShiftState Shift,UsineEventPtr X, UsineEventPtr Y, UsineEventPtr Pressed){};
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
    inline void AfterQueryPopup (TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo, int queryIndex);
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
	/// Utils functions to help manipulate events
    /// @name Events manipulation 
	/// @{

    /// Compare Events, returns TRUE if e1 and e2 are equal.
    inline bool sdkCompareEvt( UsineEventPtr e1, UsineEventPtr e2 )                     { 
		return m_masterInfo->CompareEvt( e1, e2 ); };

	/// Copy Events, copies src to dest.
    inline void sdkCopyEvt(UsineEventPtr scr, UsineEventPtr dest)                       { 
		m_masterInfo->CopyEvt( scr, dest ); };

	/// Event Concatenation dest is the concatenation of e1 and e2.
    inline void sdkConcatEvt( UsineEventPtr e1, UsineEventPtr e2, UsineEventPtr dest )     { 
		m_masterInfo->ConcatEvt( e1, e2, dest ); };
    
	/// Shift the values of src and store the result into dest. 
	/// Right shift for n>0, left shift for n<0.
    inline void sdkShiftEvt( int n, UsineEventPtr src, UsineEventPtr dest )         { 
		m_masterInfo->ShiftEvt( n, src, dest ); };
    
	/// Set the size of the event (number of elements). Typically the size of audio events is given by sdkGetBlocSize.
	/// @see sdkGetBlocSize 
    inline void sdkSetEvtSize( UsineEventPtr e, int size )                     { 
		m_masterInfo->SetEvtSize( e, size ); };
    
	/// Get the size of the event (number of elements).
    inline int  sdkGetEvtSize( UsineEventPtr e)                                { 
		return m_masterInfo->GetEvtSize( e ); };

    /// Set the maximum size allowed by the event. Can be greater than the event size.
    inline void sdkSetEvtMaxSize( UsineEventPtr e, int size )                  { 
		m_masterInfo->SetEvtMaxSize( e, size ); };
    
	/// Destroy the event.
    inline void sdkDestroyEvt( UsineEventPtr &e )                              { 
		m_masterInfo->DestroyEvt( e ); };
    
	/// Create an event with an original size.
    inline void sdkCreateEvt( UsineEventPtr &e, int originalSize )             { 
		m_masterInfo->CreateEvt( e, originalSize ); };
    
	/// Copy a 32Bits bloc of memory. The size is the number of 32bits elements.
    inline void sdkMoveBLOC32( TPrecision* pointerSrc, TPrecision* pointerDest, int size )      { 
		m_masterInfo->MoveBLOC32( pointerSrc, pointerDest, size ); };

    /// Add values of two events and store the result into dest.
	inline void  sdkAddEvt3(UsineEventPtr e1, UsineEventPtr e2, UsineEventPtr dest) { 
		m_masterInfo->AddEvt3(e1, e2, dest); }

    /// Add values of two events and store the result into e2.	
	inline void  sdkAddEvt2(UsineEventPtr e1, UsineEventPtr e2) { 
		m_masterInfo->AddEvt2(e1, e2); }

	/// Subtract values of two events and store the result into dest.
	inline void  sdkSubEvt3(UsineEventPtr e1, UsineEventPtr e2, UsineEventPtr dest) { 
		m_masterInfo->SubEvt3(e1, e2, dest); }

	/// Subtract values of two events and store the result into e2.
	inline void  sdkSubEvt2(UsineEventPtr e1, UsineEventPtr e2) { 
		m_masterInfo->SubEvt2(e1, e2); }

	/// Divide values of e1 by values of e2 and store the result into dest.
     inline void  sdkDivEvt3(UsineEventPtr e1, UsineEventPtr e2, UsineEventPtr dest) { 
		m_masterInfo->DivEvt3(e1, e2, dest); }

	/// Multiply values of e1 by values of e2 and store the result into dest.
	inline void  sdkMultEvt3(UsineEventPtr e1, UsineEventPtr e2, UsineEventPtr dest) { 
		m_masterInfo->MultEvt3(e1, e2, dest); }

	/// Multiply values of e1 by values of e2 and store the result into e2.
	inline void  sdkMultEvt2(UsineEventPtr e1, UsineEventPtr e2) { 
		m_masterInfo->MultEvt2(e1, e2); }

    /// Multiply values of e1 by values of e2 and store the result into e2. Same as sdkMultEvt2.
	inline void  sdkMultEvt2Audio(UsineEventPtr e1, UsineEventPtr e2) { 
		m_masterInfo->MultEvt2(e1, e2); }

    /// Multiply values of e by a float value inplace.
	inline void  sdkMultEvt1(TPrecision m, UsineEventPtr e) { 
		m_masterInfo->MultEvt1(m, e); }

	/// Modulo operation of e1 by values of e2 and store the result into dest.
	inline void  sdkModEvt3(UsineEventPtr e1, UsineEventPtr e2, UsineEventPtr dest) { 
		m_masterInfo->ModEvt3(e1, e2, dest); }

	/// Power values of e1 by values of e2 and store the result into dest.
	inline void  sdkPowerEvt3(UsineEventPtr e1, UsineEventPtr e2, UsineEventPtr dest) { 
		m_masterInfo->PowerEvt3(e1, e2, dest); }

	/// Exponential of e inplace.
	inline void  sdkExpEvt1(UsineEventPtr e) { 
		m_masterInfo->ExpEvt1(e); }

	/// Square root of e inplace.
	inline void  sdkSqrtEvt1(UsineEventPtr in1) { 
		m_masterInfo->SqrtEvt1(in1); }

    /// Returns the maximum value of e.
	inline TPrecision sdkMaxEvt1(UsineEventPtr e) { 
		return m_masterInfo->MaxEvt1(e); };

    /// Returns the minimum value of e.
	inline TPrecision	sdkMinEvt1(UsineEventPtr e) { 
		return m_masterInfo->MinEvt1(e); };
    
	/// calculates the maximum of each element and store the result into dest. dest[i] = max(e1[i],e2[i])
	inline void	sdkMaxEvt3(UsineEventPtr e1, UsineEventPtr e2, UsineEventPtr dest) { 
		m_masterInfo->MaxEvt3(e1, e2, dest); };

	/// calculates the minimum of each element and store the result into dest. dest[i] = min(e1[i],e2[i])
	inline void	sdkMinEvt3(UsineEventPtr in1, UsineEventPtr in2, UsineEventPtr out) { 
		m_masterInfo->MinEvt3(in1, in2, out); };

	/// Clip values of e in the range [min..max] and store the result into dest.
	inline void  sdkThreshEvt2(const UsineEventPtr e, UsineEventPtr dest, TPrecision min, TPrecision max) { 
		m_masterInfo->ThreshEvt2(e, dest, min, max); };

	/// Clip values of e in the range [min..max] inplace.
	inline void  sdkThreshEvt1(UsineEventPtr e, TPrecision min, TPrecision max) { 
		m_masterInfo->ThreshEvt1(e, min, max); };

	/// Mix MIDI values of e1 and e2 and store the result in dest. For the MIDI, this operation equivalent to sdkConcatEvt
	inline void  sdkMixMidiEvt3(UsineEventPtr e1, UsineEventPtr e2, UsineEventPtr dest) { 
		m_masterInfo->MixMidiEvt3(e1, e2, dest); };

	/// Mix MIDI values of e1 and e2 and store the result in e2. For the MIDI, this operation equivalent to sdkConcatEvt
	inline void  sdkMixMidiEvt2(UsineEventPtr e1, UsineEventPtr e2) { 
		m_masterInfo->MixMidiEvt2(e1, e2); };

	/// Apply a short fadein to the audio event. The duration of the fade is given by sdkGetBlocSize.
	inline void  sdkFadeInAudioEvt(UsineEventPtr e) { 
		m_masterInfo->FadeInAudioEvt(e); };

	/// Apply a short fadeout to the audio event. The duration of the fade is given by sdkGetBlocSize.
	inline void  sdkFadeOutAudioEvt(UsineEventPtr e) { 
		m_masterInfo->FadeOutAudioEvt(e); };

	/// Clear an audio event. Set values to zero and add a small denormalization value.
	inline void  sdkClearAudioEvt(UsineEventPtr e) { 
		m_masterInfo->ClearAudioEvt(e); };

	/// Add a small denormalization value to an audio event.
	inline void  sdkDenormalizeAudioEvt(UsineEventPtr e) { 
		m_masterInfo->DenormalizeAudioEvt(e); };

	/// Smooth event.
	inline void sdkSmoothPrecision(TPrecision& oldValue, UsineEventPtr e, TPrecision factor) {
		return m_masterInfo->SmoothPrecision(oldValue, e, factor);};

    /// Smooth Event with a target value.
	inline void sdkSmoothEvent(TPrecision& oldValue, UsineEventPtr e, TPrecision target, TPrecision factor){
		return m_masterInfo->SmoothEvent(oldValue, e, target, factor);}

	/// @}

    /// @name Access to Event values
    /// @{
	
	/// Set a float value
	inline void			sdkSetEvtData( UsineEventPtr e, TPrecision floatValue )                 { 
		m_masterInfo->SetEvtData( e, floatValue ); };
    
	/// Get a float Value
    inline TPrecision	sdkGetEvtData( UsineEventPtr e )									{ 
		return m_masterInfo->GetEvtData( e ); };

    /// Set a value to an Array at an index position.
    inline void			sdkSetEvtArrayData( UsineEventPtr e, int index, TPrecision floatValue )   { 
		m_masterInfo->SetEvtArrayData( e, index, floatValue ); };

    /// Get a value from an Array at an index position.
    inline   TPrecision	sdkGetEvtArrayData( UsineEventPtr e, int index )						{ 
		return m_masterInfo->GetEvtArrayData( e, index ); };
    
    /// Set a pointer value.
	inline void	 sdkSetEvtPointer( UsineEventPtr e, void* pointerValue )               { 
		m_masterInfo->SetEvtPointer( e, pointerValue ); };

    /// Get a pointer value.
    inline void* sdkGetEvtPointer( UsineEventPtr e )                            { 
		return m_masterInfo->GetEvtPointer( e ); };

	 /// Set a color value.
     inline void			sdkSetEvtColor( UsineEventPtr e, TUsineColor colorValue )           { 
		m_masterInfo->SetEvtColor( e, colorValue ); };

    /// Get a color value.
	inline TUsineColor	sdkGetEvtColor( UsineEventPtr e )                              { 
		return m_masterInfo->GetEvtColor( e ); };

    /// Set a color value to an array at index position.
    inline void	sdkSetEvtArrayColor( UsineEventPtr e, int index, TUsineColor colorValue )   {
		// sanity check
		int idx = std::max(0, std::min(index, sdkGetEvtSize( e)));
		TUsineColor* data =  (TUsineColor*)sdkGetEvtDataAddr (e);
		data += idx;
		*data = colorValue;};

    /// Get a color value to an array at index position.
    inline TUsineColor sdkGetEvtArrayColor( UsineEventPtr e, int index ) { 
		// sanity check
		int idx = std::max(0, std::min(index, sdkGetEvtSize(e)));
		TUsineColor* data =  (TUsineColor*)sdkGetEvtDataAddr (e);
		TUsineColor result = *(data + idx);
		return result;};

    /// Set a Midi value to an array at index position.
    inline void	sdkSetEvtArrayMidi( UsineEventPtr e, int index, TUsineMidiCode midiValue ) { 
		m_masterInfo->SetEvtArrayMidi( e, index, midiValue ); };

    /// Get a Midi value to an array at index position.
	inline TUsineMidiCode	sdkGetEvtArrayMidi( UsineEventPtr e, int index )					{	
		return m_masterInfo->GetEvtArrayMidi( e, index ); };

    /// Set a Text value.
    inline void	 sdkSetEvtPChar( UsineEventPtr e,  AnsiCharPtr textValue )                { 
		m_masterInfo->SetEvtPChar( e, textValue ); };
    
    /// Get a Text value.
	inline AnsiCharPtr sdkGetEvtPChar( UsineEventPtr e )                             { 
		return m_masterInfo->GetEvtPChar( e ); };
    
	/// Get the address of the first data in the Event
    inline TPrecision* sdkGetEvtDataAddr( UsineEventPtr ev )                         { 
		return m_masterInfo->GetEvtDataAddr( ev ); };	
    
	/// Get a 3D point.
	/// the event must have a line number of 3 to work,
    /// @param pointIndex is the index of the point you want to get
    inline T3DPointF sdkGetEvt3DPoint (UsineEventPtr e, int pointIndex){
        T3DPointF result;
        if (pointIndex < (sdkGetEvtSize (e)/3))
        {
            TPrecision* pointer3DPoint = sdkGetEvtDataAddr (e) + (pointIndex*3);
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
	/// Critical sections functions to protect patches calculation.
	/// @name Critical Sections 
	/// @{

	/// Lock the execution of the current patch to prevent memory access conflicts between the audio thread and other threads like IDLE, video, etc.
	inline void	sdkLockPatch() { 
		m_masterInfo->LockPatch(m_moduleInfo); };

	/// UnLock the execution of the current patch.
	inline void	sdkUnLockPatch() { 
		m_masterInfo->UnLockPatch(m_moduleInfo); };

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
    /// To use with the audio multi channels query.
    /// @name Audio Channels query 
	/// @{
    
    /// Get the title string for the popup.
    inline AnsiCharPtr  sdkGetAudioQueryTitle () { return m_masterInfo->AudioQueryTitle; };
    
    /// Get the query string list for an multi channels query. 
    inline AnsiCharPtr  sdkGetAudioQueryChannelList () { return m_masterInfo->AudioQueryChannelList; };

	/// Get the number of channels from the query index choice.
    inline int  sdkGetAudioQueryToNbChannels (int qQueryIndex) { return m_masterInfo->AudioQueryToNbChannels (qQueryIndex); };
	
	/// Get the name of a channel. 
    inline AnsiCharPtr  sdkGetAudioQueryChannelNames (AnsiCharPtr prefix, int index, int queryIndex)
        { return m_masterInfo->AudioQueryChannelNames (prefix, index, queryIndex); };
    /// @} 


	//----------------------------------------------------------------------------
	/// @name Global Array functions 
	/// @{

	/// Get the Hash (internal ID) of the Global Array needed for function bellow
	UINT64 sdkGlobalArrayGetHash(AnsiCharPtr name) { return m_masterInfo->GlobalArrayGetHash(name); };

	/// Set a float value of a data Global Array at the index 
	inline void  sdkGlobalArraySetValueFloat(UINT64 hash, int index, TPrecision value, AnsiCharPtr errorMsg) { 
		m_masterInfo->GlobalArraySetValueFloat(hash, index, value, errorMsg); };
	/// Set a color value of a color Global Array at the index 
	inline void  sdkGlobalArraySetValueColor(UINT64 hash, int index, TUsineColor value, AnsiCharPtr errorMsg) { 
		m_masterInfo->GlobalArraySetValueColor(hash, index, value, errorMsg); };
	/// Set a string value of a string Global Array at the index 
	inline void  sdkGlobalArraySetValueAnsiChar(UINT64 hash, int index, AnsiCharPtr value, AnsiCharPtr errorMsg) { 
		m_masterInfo->GlobalArraySetValueAnsiChar(hash, index, value, errorMsg); };
	/// Fill an entire global array with the content of an Event
	inline void  sdkGlobalArraySetValueEvt(UINT64 hash, UsineEventPtr value, AnsiCharPtr errorMsg) { 
		m_masterInfo->GlobalArraySetValueEvt(hash, value, errorMsg); };

	/// Get a float value from a data Global Array at the index 
	inline TPrecision sdkGlobalArrayGetValueFloat(UINT64 hash, int index, AnsiCharPtr errorMsg) { 
		return m_masterInfo->GlobalArrayGetValueFloat(hash, index, errorMsg); };
	/// Get a color value from a color Global Array at the index 
	inline TUsineColor sdkGlobalArrayGetValueColor(UINT64 hash, int index, AnsiCharPtr errorMsg) { 
		return m_masterInfo->GlobalArrayGetValueColor(hash, index, errorMsg); };
	/// get a string value from a string Global Array at the index 
	inline AnsiCharPtr sdkGlobalArrayGetValueAnsiChar(UINT64 hash, int index, AnsiCharPtr errorMsg) { 
		return m_masterInfo->GlobalArrayGetValueAnsiChar(hash, index, errorMsg); };
	/// Get an entire global array and fill its content in an Event
	inline void sdkGlobalArrayGetValueEvt(UINT64 hash, AnsiCharPtr errorMsg, UsineEventPtr result) { 
		return m_masterInfo->GlobalArrayGetValueEvt(hash, errorMsg, result); };
	/// Get the list of all available global-array's names as a comma-text. This function can be slow.
	inline AnsiCharPtr sdkGlobalArrayGetAllNames() { 
		return m_masterInfo->GlobalArrayGetAllNames(); };


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

	/// Get the list of possible usine block size. 
	inline AnsiCharPtr  sdkGetUsineBlockSizeList() { 
		return m_masterInfo->BlockSizeList; };

	/// Get the default usine block size. 
	inline int  sdkGetUsineDefaultBlockSizeIndex() { 
		return m_masterInfo->BlockSizeDefaultIndex; };

	/// Low level , not for public use
	/// Usine audio callback
	inline void	sdkUsineAudioDeviceIOCallback(const float** inputChannelData, int numInputChannels, float** outputChannelData, int numOutputChannels, int numSamples ){ 
		m_masterInfo->AudioDeviceIOCallback( inputChannelData, numInputChannels, outputChannelData, numOutputChannels, numSamples );};

	/// Low level , not for public use
	/// Usine  midi callback
	inline void sdkUsineMidiDeviceCallback (int deviceID, double TimeStamp, TUsineMidiCode midiData)	{ 
		m_masterInfo->MidiDeviceCallback (deviceID, TimeStamp, midiData);};
	
	/// Low level , not for public use
	/// Usine midi sysex callback
	inline void sdkUsineMidiSysexDeviceCallback (int deviceID, double TimeStamp, char** sysEx, int size){ 
		m_masterInfo->MidiSysexDeviceCallback (deviceID, TimeStamp, sysEx, size);};
    
    /// Usine internal messages
    inline void sdkSendUsineMsg( AnsiCharPtr Msg ) { 
		m_masterInfo->SendUsineMsg( m_moduleInfo, Msg ); };
   
    /// Usine notification
	///
    inline void	sdkNotifyUsine(NativeInt Target, NativeInt Msg, NativeInt WParam = 0, NativeInt LParam = 0){
		m_masterInfo->NotifyUsine(m_moduleInfo, Target, Msg, WParam, LParam);};
 
    /// To access the current TVstTimeInfo structure of Usine.
    inline TVstTimeInfo* sdkGetVstTimeInfo()      { 
		return m_masterInfo->GetVstTimeInfo(m_moduleInfo); };

    /// To get the current Usine block size.
    inline int		sdkGetBlocSize()              { 
		return m_masterInfo->BlocSize; };
	
    /// To get the current Usine sample rate
	inline double	sdkGetSampleRate()			  { 
		return m_masterInfo->GetSampleRate(); };
	    
    /// PC only, Usine mainform handle (THandle)
    inline NativeInt sdkGetUsineMainWindow()          { 
		return m_masterInfo->GetUsineMainWindow(); };
	
    /// MAC only, Usine NSView
    inline void* sdkGetUsineNSView()              { 
		return m_masterInfo->GetMacNSView(); };
	
    /// MAC only, Usine NSApplication
    inline void* sdkGetUsineNSApplication()       { 
		return (void*)m_masterInfo->NSApplication; };

    /// To get a named color from the Usine ColorSet.
    inline TUsineColor sdkGetUsineColor(TUsineColorSet colorName){
        TUsineColor result = 0;        
        if (m_masterInfo != 0)
            result = m_masterInfo->GetUsineColor ((int)colorName);            
        return result;};
    
    /// To get the Usine global time in millisecond, precision up to nanoseconde
    inline double sdkGetTimeMs () { 
		return m_masterInfo->GetTimeMs (m_moduleInfo); };

	///Translate a StringID to the current Usine language.
	inline AnsiCharPtr sdkGetTrans (AnsiCharPtr StringID) { 
		return m_masterInfo->GetTranslation (StringID); };

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
	
    /// To get the version type of Usine @see TUsineVersionType. 
    inline TUsineVersionType sdkGetUsineVersionType() { 
		return m_masterInfo->UsineVersionType; };  
	
    /// To get the current language used in Usine. 
	/// @return a string naming the current language (En, Fr...)
    inline AnsiCharPtr sdkGetUsineLanguage() { 
		return m_masterInfo->UsineLanguage; };
	
    /// To get the of the save format of Usine. 
    inline int sdkGetUsineSaveVersion()    { 
		return m_masterInfo->UsineSaveVersion; };  
    	
    /// To get the maximum audio inputs Usine can accept. 
    inline int sdkGetUsineMaxAudioInputs ()         { 
		return m_masterInfo->MAX_AUDIO_INPUTS; };
	
    /// To get the maximum audio outputs Usine can accept. 
    inline int sdkGetUsineMaxAudioOutputs ()        { 
		return m_masterInfo->MAX_AUDIO_OUTUTS; };
	
    /// To get the maximum Midi device Usine can accept. 
    inline int sdkGetUsineMaxMidiDevices ()         { 
		return m_masterInfo->MAX_MIDI_DEVICES; };
	
    /// To get the current polyphony of Usine.  
    inline int sdkGetUsineMaxPolyphony ()         { 
		return m_masterInfo->MULTIPHONY_MAX; };

	/// set the name of the module (as it appear in the patch, on the module title)
	inline void sdkSetModuleUserName(AnsiCharPtr name) { 
		m_masterInfo->SetModuleUserName(m_moduleInfo, name); };
	
	/// get the name of the module (as it appear in the patch, on the module title)
	inline AnsiCharPtr sdkGetModuleUserName() { 
		return m_masterInfo->GetModuleUserName(m_moduleInfo); };
    /// @}	

    //----------------------------------------------------------------------------
	/// To get the path to access some important folders of Usine.
    /// @name Usine Paths getters 
	/// @{

	
	/// The Sound folder as configured in the Global setting Tab.
    inline AnsiCharPtr    sdkGetUsineSoundPath  ()		    { 
		return m_masterInfo->SoundPath; };
    
	/// The Patch library folder as configured in the Global setting Tab.
	inline AnsiCharPtr    sdkGetUserLibPath ()			    { 
		return m_masterInfo->UserLibPath; };
    
	/// The Record Out folder as configured in the Global setting Tab.    
	inline AnsiCharPtr    sdkGetUsineRecordPath ()		    { 
		return m_masterInfo->RecordPath; };
    
	/// The install folder of Usine.
	inline AnsiCharPtr    sdkGetGlobalApplicationPath ()   { 
		return m_masterInfo->GlobalApplicationPath; };
    
	/// The path of the current workspace.
	inline AnsiCharPtr    sdkGetCurrentWorkspacePath ()	{ 
		return m_masterInfo->CurrentWorkspacePath; };
    
	/// The path of the patch containing the module.
	inline AnsiCharPtr    sdkGetCurrentPatchPath ()		{ 
		return m_masterInfo->CurrentPatchPath; };
	
	/// The Temp folder Used by Usine.
	inline AnsiCharPtr    sdkGetUsineTempPath ()			{ 
		return m_masterInfo->UsineTempPath; };
    
	/// Not for public use.
	inline AnsiCharPtr    sdkGetUsinePlugInsPath ()		{ 
		return m_masterInfo->PlugInsPath; };
    /// @}	

    //----------------------------------------------------------------------------
    /// @name Files utils
	/// @{

    /// Show a dialog window to open a file.
    inline LongBool	sdkProcessOpenDialog ( AnsiCharPtr* filename, AnsiCharPtr initialDir, AnsiCharPtr filter ) { 
		return m_masterInfo->ProcessOpenDialog	( filename, initialDir, filter ); };
    
    /// Show a dialog window to save a file.
	inline LongBool	sdkProcessSaveDialog ( AnsiCharPtr* filename, AnsiCharPtr initialDir, AnsiCharPtr filter ) { 
		return m_masterInfo->ProcessSaveDialog	( filename, initialDir, filter ); };
    
    /// Ask Usine to search if a file exist.
	inline AnsiCharPtr    sdkFindFile          ( AnsiCharPtr* filename, AnsiCharPtr initialDir )               { 
		return m_masterInfo->FindFile			( filename, initialDir ); };
    /// @}	
    
	//----------------------------------------------------------------------------
    /// @name Audio Files manipulation
	/// @{

    /// Create an audio file handle
    inline TAudioFilePtr	sdkCreateAudioFile          (){ 
		return m_masterInfo->CreateAudioFile(); };

    /// Destroy an audio file handle
	inline void			sdkDestroyAudioFile         (TAudioFilePtr audioFile){ 
		m_masterInfo->DestroyAudioFile( audioFile); };
	
	/// Get a sample value of an audio file at the position pos of the audio channel 
	inline TPrecision   sdkGetSampleAudioFile       (TAudioFilePtr audioFile, int channel, int pos)                      { 
		return m_masterInfo->GetSampleAudioFile(audioFile, channel, pos); };
	
	/// Get all samples values (as an array) of an audio file for an audio channel 
	inline TPrecision*  sdkGetSampleArrayAudioFile  (TAudioFilePtr audioFile, int channel)                               { 
		return m_masterInfo->GetSampleArrayAudioFile(audioFile, channel); };
	
	/// Get a bloc of samples values of an audio file at position pos for an audio channel and store it in an Event
	inline void			sdkGetBlocSampleAudioFile   (TAudioFilePtr audioFile, int channel, int pos, UsineEventPtr evt)	{ 
		m_masterInfo->GetBlocSampleAudioFile			(audioFile, channel, pos, evt); };
    
	/// Get the numbers of samples of an audio file
	inline int			sdkGetSizeAudioFile         (TAudioFilePtr audioFile)											{ 
		return m_masterInfo->GetSizeAudioFile			(audioFile); };
    
	/// Get the the number of channels of an audio file
	inline int			sdkGetChannelAudioFile      (TAudioFilePtr audioFile)											{ 
		return m_masterInfo->GetChannelAudioFile		( audioFile); };
    
	///Get the samplerate of an audio file
	inline int			sdkGetSampleRateAudioFile   (TAudioFilePtr audioFile)											{ 
		return m_masterInfo->GetSampleRateAudioFile	( audioFile); };
    
	/// Get the bit per sample of an audio file
	inline int			sdkGetBitPerSampleAudioFile (TAudioFilePtr audioFile)											{ 
		return m_masterInfo->GetBitPerSampleAudioFile	( audioFile); };
    
	/// Load an audio file in memory
	inline void			sdkLoadInMemoryAudioFile    (TAudioFilePtr audioFile, AnsiCharPtr name)							{ 
		m_masterInfo->LoadInMemoryAudioFile	( audioFile, name); };
    
	/// Load an audio file but keep it on disk
	inline void			sdkLoadStayOnDiskAudioFile  (TAudioFilePtr audioFile, AnsiCharPtr name)							{ 
		m_masterInfo->LoadStayOnDiskAudioFile	( audioFile, name); };
    
	/// Get the peak of the audio files (to draw the wave form) and store it in an Event
	inline void			sdkGetPeaksAudioFile        (TAudioFilePtr audioFile, UsineEventPtr evt)							{ 
		m_masterInfo->GetPeaksAudioFile		( audioFile, evt); };

    /// Save an audio file to disk
	inline void			sdkSaveToDiskAudioFile      (TAudioFilePtr audioFile, AnsiCharPtr name, int nbChannels)			{ 
		m_masterInfo->SaveToDiskAudioFile		( audioFile, name, nbChannels); };
    
	/// Set the number of channels of an audio file
	inline void			sdkSetChannelsAudioFile     (TAudioFilePtr audioFile, int nbChannels)							{ 
		m_masterInfo->SetChannelsAudioFile		( audioFile, nbChannels); };
    
	/// Set the number of samples of an audio file
	inline void			sdkSetSizeAudioFile         (TAudioFilePtr audioFile, int size)									{ 
		m_masterInfo->SetSizeAudioFile			( audioFile, size); };
    
	/// Set a sample value of an audio file at the position pos of the audio channel 
    inline void			sdkSetSampleAudioFile       (TAudioFilePtr audioFile, int channel, int pos, TPrecision sample)	{ 
		m_masterInfo->SetSampleAudioFile		( audioFile, channel, pos, sample); };
	
	/// Clear an audio file
	inline void			sdkClearAudioFile           (TAudioFilePtr audioFile)                                            { 
		m_masterInfo->ClearAudioFile(audioFile); };
	
	/// Resample (stretch) an audio file by a factor
	inline void			sdkResampleAudioFile        (TAudioFilePtr audioFile, TPrecision factor)                         { 
		m_masterInfo->ResampleAudioFile(audioFile,factor); };
	/// @}	

    //----------------------------------------------------------------------------
    /// @name Math utils 
	/// @{

	/// Linear interpolation  on a TPrecision value.
	/// @return The interpolated value.
    inline TPrecision sdkLinearInterpolation ( TPrecision f,  TPrecision a, TPrecision b )											{ 
		return m_masterInfo->LinearInterpolation	( f, a, b ); };
    
	/// Cubic interpolation  on a TPrecision value.
	/// @return The interpolated value.
	inline TPrecision sdkCubicInterpolation  ( TPrecision fr, TPrecision inm1, TPrecision inp, TPrecision inp1, TPrecision inp2 )	{ 
		return m_masterInfo->CubicInterpolation	( fr, inm1, inp, inp1, inp2 ); };
    
	/// Spline interpolation on a TPrecision value.
	/// @return The interpolated value.
	inline TPrecision sdkSplineInterpolation ( TPrecision fr, TPrecision inm1, TPrecision inp, TPrecision inp1, TPrecision inp2 )	{ 
		return m_masterInfo->SplineInterpolation	( fr, inm1, inp, inp1, inp2 ); };
    /// @}	
    
    //----------------------------------------------------------------------------
    /// @name Trace functions 
	/// @{
    
	/// Show a string in the Trace Panel.
    inline void sdkTraceChar        (AnsiCharPtr s)			     { 
		return m_masterInfo->TraceChar	      ( s ); };
	
	/// Show an integer in the Trace Panel.
    inline void sdkTraceInt         (int i)			                 { 
		return m_masterInfo->TraceInteger 	  ( i ); };
	
	/// Show a decimal number in the Trace Panel.
    inline void sdkTracePrecision   (TPrecision f)	                 { 
		return m_masterInfo->TracePrecision	  ( f ); };
	
	/// Show a string in the Trace Panel with the error tag.
    inline void sdkTraceErrorChar   (AnsiCharPtr s)			     { 
		return m_masterInfo->TraceErrorChar	  ( s ); };
	
	/// Show a string in the Trace Panel with the warning tag.
    inline void sdkTraceWarningChar (AnsiCharPtr s)		         { 
		return m_masterInfo->TraceWarningChar  ( s ); };
	
	/// Show a string in the SplashForm.
    inline void sdkTraceSplashChar  (AnsiCharPtr s, int autoClose)  { 
		return m_masterInfo->TraceSplashChar   ( s, autoClose ); };
	
	/// Print a string in the LogUsine.log file. Optionally show it in the SplashForm.
    inline void sdkTraceLogChar     (AnsiCharPtr s, LongBool showInSplashForm = FALSE){
        return m_masterInfo->TraceLogChar (s, showInSplashForm);};
    /// @}		
   
    
    //-----------------------------------------------------------------------------
    /// @name Dialog popups
	/// @{
	
    /// A dialog popup with Yes, No and Cancel buttons. 
    /// @param msg The message to show on the popup popup.
    /// @return The choice of the user.
    /// @see TDialogsResults
	inline int sdkDialogConfirmationYesNoCancel	( AnsiCharPtr msg ) {
		return m_masterInfo->DialogConfirmationYesNoCancel(msg);};
    
    /// A dialog popup with Yes and No buttons. 
    /// @param msg The message to show on the popup popup.
    /// @return The choice of the user.
    /// @see TDialogsResults   
    inline int sdkDialogConfirmationYesNo			( AnsiCharPtr msg ) {
		return m_masterInfo->DialogConfirmationYesNo(msg);};

    /// A dialog popup with OK button. 
    /// @param msg The message to show on the popup popup.
    /// @return The choice of the user.
    /// @see TDialogsResults
	inline int sdkDialogInformationOk				( AnsiCharPtr msg ) {
		return m_masterInfo->DialogInformationOk(msg);};

    /// A dialog popup with OK and Cancel buttons. 
    /// @param msg The message to show on the popup popup.
    /// @return The choice of the user.
    /// @see TDialogsResults
    inline int sdkDialogConfirmationOKCancel		( AnsiCharPtr msg ) {
		return m_masterInfo->DialogConfirmationOKCancel(msg);};

	/// A dialog popup with a string input box . 
	/// @param msg The message to show on the popup popup.
	/// @return the edited string.
	/// @see TDialogsResults
	inline AnsiCharPtr sdkDialogInputBox(AnsiCharPtr caption, AnsiCharPtr prompt, AnsiCharPtr defaultValue) { 
		return m_masterInfo->DialogInputBox(caption,prompt,defaultValue); };

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
     	
    void setStringTrace (std::string newString){
        stringTrace = newString;};

	
    //-------------------------------------------------------------------------
    // protected members
    //-------------------------------------------------------------------------
protected:

    float panelWidth; // in pixel
    float panelHeight; // in pixel


    // Pointer to TMasterInfo and TModuleInfo provided by usine
    TMasterInfo* m_masterInfo;
    TModuleInfo* m_moduleInfo;
    
    std::string stringTrace;

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


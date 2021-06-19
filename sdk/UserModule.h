///----------------------------------------------------------------------------
///@file UserModule.h
///
///@author
///	Martin FLEURENT aka 'martignasse'
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
///  support@brainmodular.org 
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
//using namespace std;

//-----------------------------------------------------------------------------
/// @addtogroup Globals
/// @{ 

/// Usine need them to instantiate and destroy the module.
/// @name  Mandatory Functions
/// @{ 
/// @fn void CreateModule(void* &pModule, AnsiCharPtr optionalString, LongBool Flag, MasterInfo* pMasterInfo, AnsiCharPtr optionalContent)
/// @brief Called by Usine to instantiate the User Module
/// When opening the Browser section where is stored the module.
/// When adding the module in a patch.
/// When loading a saved workspace or patch.
///
/// @param pModule Handle for the new instanciated module
/// @param optionalString
/// @param Flag 
/// @param pMasterInfo Handle to the MasterInfo structure
/// @param optionalContent 
void CreateModule(void* &pModule, AnsiCharPtr optionalString, LongBool Flag, MasterInfo* pMasterInfo, AnsiCharPtr optionalContent);

/// @fn void DestroyModule(void* pModule)
/// @brief Called by Usine to destroy the User Module
/// When clearing a workspace or patch.
/// Before recreate module.
/// When closing Usine.
void DestroyModule(void* pModule);

/// @fn void GetBrowserInfo (ModuleInfo* pModuleInfo)
/// @brief Called by usine to get module informations for the browser
/// When the browser open the folder where the module is stored.
void GetBrowserInfo (ModuleInfo* pModuleInfo);
/// @}
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
	/// You have to fill the ModuleInfo structure with infos describing the module.
	/// MasterInfo can be used to obtain some useful infos but always test if the handle is valid as it can be nullptr. 
    /// @param pMasterInfo The MasterInfo structure. can be null.
    /// @param pModuleInfo The ModuleInfo structure to describe the module characteristics
	virtual void onGetModuleInfo (MasterInfo* pMasterInfo, ModuleInfo* pModuleInfo)=0;

    /// Usine call it for each parameters of the module to obtain it's characteristics. 
    /// @param ParamIndex The index of the parameter.
    /// @param pParamInfo Handle to the info structure of the parameter.
	virtual void onGetParamInfo (int ParamIndex, TParamInfo* pParamInfo)=0;
	

    /// Usine call it for each parameters of the module to obtain the event handle of the parameter. 
	/// Deprecated in HH3, use ModuleInfo::EventPtr
    /// @param ParamIndex The index of the parameter.
    /// @param pEvent Handle to the event of the parameter.
    virtual void onSetEventAddress (int ParamIndex, UsineEventPtr pEvent){}
	

    /// Usine call it to inform the module something happened. 
    /// @param Message Provide various infos about what happened.
	virtual void onCallBack (UsineMessage *Message){}
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
    /// @param pMasterInfo The MasterInfo structure.
    /// @param pModuleInfo The ModuleInfo structure.
    /// @param queryIndex The index of the choice made in the query pop-up
	virtual void onAfterQuery (MasterInfo* pMasterInfo, ModuleInfo* pModuleInfo, int queryIndex) {}
    /// @}

	//-----------------------------------------------------------------------------
	// process
    //-----------------------------------------------------------------------------
    /// Usine call it if ModuleInfo::DontProcess is not set to FALSE in onGetModuleInfo.
	/// @name Real-time Audio and Data Callback 
 	/// @{
	
	/// Be careful here because it's called from the audio thread at each audio tick of the audio engine. 
	/// Keep it optimized and try to avoid any memory allocation.
    /// @see ModuleInfo::DontProcess
	virtual void onProcess (){}
    /// @}
	
	/// Called by the the video thread to process video frames only for ModuleInfo::ModuleType == mtVideo  
	/// Don't process video outside this procedure.
	/// @see ModuleInfo::ModuleType
	virtual void onProcessVideo() {}
	/// @}

	//-----------------------------------------------------------------------------
	/// Implement them if the module need to save some additional info.
	/// A chunk is a memory block containing some binary data.
	/// Usine store those chunk in patch and workspace files and use them to store/restore presets.
	/// @name Chunk System Callbacks
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
    /// Usine call it if ModuleInfo::CanRecord is set to TRUE in onGetModuleInfo.
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
	virtual void onSetQuickColor(TColorUsine color) {}
	/// @}
    
	//-----------------------------------------------------------------------------
	/// Implement them if the  module type is mtControl and need to be informed of mouse and/or multi-touch events.
	/// @name Mouse and Multi-touch Callbacks
	/// @{
	virtual void onMouseMove (TShiftState Shift, float X, float Y){}
	
	virtual void onMouseDown (TMouseButton MouseButton, TShiftState Shift, float X,float Y){}
	
	virtual void onMouseUp (TMouseButton MouseButton, TShiftState Shift, float X,float Y){}

	virtual void onMouseWheel (TShiftState Shift, int WheelDelta){};
	
	virtual void onMouseMoveMulti (TShiftState Shift, UsineEventPtr X, UsineEventPtr Y, UsineEventPtr Pressed){};
	
	virtual void onMouseDownMulti (TMouseButton MouseButton, TShiftState Shift, UsineEventPtr X, UsineEventPtr Y, UsineEventPtr Pressed){};
	
	virtual void onMouseUpMulti (TMouseButton MouseButton, TShiftState Shift,UsineEventPtr X, UsineEventPtr Y, UsineEventPtr Pressed){};
    /// @}

	//-----------------------------------------------------------------------------
	/// @name Optional Callbacks
	/// @{
	
	/// Usine call it after onGetModuleInfo. It let you initialising some stuff.  
    /// @param pMasterInfo The MasterInfo structure.
    /// @param pModuleInfo The ModuleInfo structure.
	virtual void onInitModule (MasterInfo* pMasterInfo, ModuleInfo* pModuleInfo){};

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
	virtual void onMidiSendOut (int DeviceID, UsineMidiCode Code){};
	
	/// Send a block of midi code.
	/// @param DeviceID Index of the device to address.
	/// @param arrayCode The array of midi message to send.
	/// @param arraySize The size of the array.
    virtual void onMidiSendOutArray (int DeviceID, UsineMidiCode** arrayCode, int arraySize){};
	
	/// Send a Sysex message. 
	/// @param DeviceID Index of the device to address.
	/// @param Sysex The Sysex message to send.
	/// @param sysexSize The size of the Sysex message in byte.
	virtual void onMidiSysexSendOut (int DeviceID,  char** Sysex, int sysexSize){};
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
    inline void AfterQueryPopup (MasterInfo* pMasterInfo, ModuleInfo* pModuleInfo, int queryIndex);
    inline void Init(MasterInfo* pMasterInfo, ModuleInfo* pModuleInfo);
    void InitInfosStructures(MasterInfo* pMasterInfo, ModuleInfo* pModuleInfo);
	inline void CallBack (UsineMessage *Message);
    inline void ResizeModule (float contentWidth, float contentHeight);



    //-------------------------------------------------------------------------
    // public methodes
    //-----------------------------------------------------------------------------
public:

    //-----------------------------------------------------------------------------
	// Wrappers for graphics methods
    //-----------------------------------------------------------------------------
	/// Use them in the onPaint callback to draw into a module canvas.
    /// @name  Graphic Canvas
	/// @{ 
	
	inline void sdkDrawPoint (TPointF point, TColorUsine color, float size, LongBool rounded)             
	{ 
		m_masterInfo->DrawPoint (m_moduleInfo, point, color, size, rounded);
	};

	//
	inline void sdkDrawLine (TPointF p1, TPointF p2, TColorUsine color, float strokeThickness)             
	{ 
		m_masterInfo->DrawLine (m_moduleInfo, p1, p2, color, strokeThickness);
	};
	
	//
	inline void sdkDrawPolyLine (TPointFArray points, int sizeList, TColorUsine color, float strokeThickness)             
	{ 
		m_masterInfo->DrawPolyLine (m_moduleInfo, points, sizeList, color, strokeThickness);
	};
	
	//
	inline void sdkFillPolyLine (TPointFArray points, int sizeList, TColorUsine color)             
	{ 
		m_masterInfo->FillPolyLine (m_moduleInfo, points, sizeList, color);
	};
	
	//
	inline void sdkFillRect (TRectF rect, TColorUsine color, float radius, TColorUsine borderColor, float borderWith)             
	{ 
		m_masterInfo->FillRect (m_moduleInfo, rect, color, radius, borderColor, borderWith);
	};
		
	//
	inline void sdkFillText (TRectF rect, AnsiCharPtr text, TColorUsine textColor, float fontSize, LongBool isBold = FALSE, LongBool isWordWrap = FALSE, TTextAlign alignHorizontal = taCenter, TTextAlign alignVertical = taCenter, LongBool isVertical = FALSE)            
	{ 
		m_masterInfo->FillText (m_moduleInfo, rect, text, textColor, fontSize, isBold, isWordWrap, alignHorizontal, alignVertical, isVertical);
	};
    
    /**  
	   Tell Usine to start a path sequence.
	   A path must begin with this function, followed by a sequence of command describing the path structure and finally drawn or filled.
        
       Example of constructing a path @code 
       // begin draw path sequence
       sdkDrawPathStart ();
       
       // move to where we want to start the path
       sdkDrawPathMoveTo (sdkPointF(0.25, 0.25));
       
       // add a line 
       sdkDrawPathLineTo (sdkPointF(0.75, 0.25));
       
       // add a curve 
       sdkDrawPathQuadCurveTo (sdkPointF(0.75, 0.75), sdkPointF(0.25, 0.75));
       
       // close the path 
       sdkDrawPathClose ();

       // end the command sequence and ask to draw the path
       sdkDrawPathDraw (colorTrajectory, thicknessTrajectory);
       
       @endcode
       
     */ 
	inline void sdkDrawPathStart ()            
	{ 
		m_masterInfo->DrawPathClear (m_moduleInfo);
	};
    
	/// Move the current position inside the path. @see sdkDrawPathStart
	inline void sdkDrawPathMoveTo (TPointF point)            
	{ 
		m_masterInfo->DrawPathMoveTo (m_moduleInfo, point);
	};
   
	/// Add a line to the path, from the current position to the point provided. @see sdkDrawPathStart
	inline void sdkDrawPathLineTo (TPointF point)            
	{ 
		m_masterInfo->DrawPathLineTo (m_moduleInfo, point);
	};
    	
	/// Add a quadratic curve to the path, from the current position to the point provided.
    /// The curve don't pass by the control point.
    /// @see sdkDrawPathStart
	inline void sdkDrawPathQuadCurveTo (TPointF control, TPointF point)            
	{ 
		m_masterInfo->DrawPathQuadCurveTo (m_moduleInfo, control, point);
	};
    
	/// Add a line to close the path, from the current position to the first point of the path.
    /// @see sdkDrawPathStart
	inline void sdkDrawPathClose ()            
	{ 
		m_masterInfo->DrawPathClose (m_moduleInfo);
	};
   
	/// Draw the current path with the color and thickness provided
    /// @see sdkDrawPathStart
	inline void sdkDrawPathDraw (TColorUsine color, float strokeThickness)            
	{ 
		m_masterInfo->DrawPathDraw (m_moduleInfo, color, strokeThickness);
	};
   
	/// Fill a valid path with the color and thickness provided
    /// @see sdkDrawPathStart
	inline void sdkDrawPathFill (TColorUsine color)            
	{ 
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
	
	inline void sdkStopRecord ()             
	{ 
		m_masterInfo->StopRecord( m_moduleInfo);
	};

    //
	inline void sdkProcessRecord(TPrecision X, TPrecision Y, TPrecision Z )             
	{ 
		m_masterInfo->ProcessRecord( m_moduleInfo, X, Y, Z );
	};
    /// @}

    //-----------------------------------------------------------------------------
    // wrappers for MasterInfo functions
    //-----------------------------------------------------------------------------

    //----------------------------------------------------------------------------
	/// Use them in onCreateSettings to add some entries in the Settings Panel.
    /// @name Settings options 
	/// @{
	
	/// To add a text entry in a Tab of the Settings Panel. 
    inline void sdkAddSettingLineCaption (AnsiCharPtr tab, AnsiCharPtr caption = "", TColorUsine color = 0, LongBool Translate = FALSE)
		{ m_masterInfo->AddSettingLineCaption (m_moduleInfo, tab, caption, color, Translate); };
	
	/// To add a color selector entry in a Tab of the Settings Panel.
    inline void sdkAddSettingLineColor (AnsiCharPtr tab, TColorUsine* pVal, AnsiCharPtr Caption, LongBool Translate = FALSE)              
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
    inline void sdkAddSettingLineCombobox (AnsiCharPtr tab, int* pVal, AnsiCharPtr caption, AnsiCharPtr commaText, LongBool Translate = FALSE)
	{
		m_masterInfo->AddSettingLineCombobox(m_moduleInfo, tab, pVal, caption, commaText, Translate);
	};

	/// To add a string entry in a Tab of the Settings Panel.
	inline void sdkAddSettingsLineString(AnsiCharPtr tab, AnsiCharPtr pVal, AnsiCharPtr caption, LongBool Translate = FALSE)
	{
		m_masterInfo->AddSettingsLineString(m_moduleInfo, tab, pVal, caption, Translate);
	};

	/// Setter for an existing setting line entry
	inline void sdkSetSettingValue(AnsiCharPtr settingName, UsineEventPtr settingEvent)
	{
		m_masterInfo->SetSettingValue(m_moduleInfo, settingName, settingEvent);
	}

	/// Getter for an existing setting line entry
	inline UsineEventPtr sdkGetSettingValue(AnsiCharPtr settingName)
	{
		UsineEventPtr settingEvent = nullptr;
		m_masterInfo->GetSettingValue(m_moduleInfo, settingName, settingEvent);

		return settingEvent;
	}
	/// @}

    //----------------------------------------------------------------------------
	/// Use them in the onCreateCommands callback to add commands in the module Contextual Menu.
    /// @name Contextual Menus commands 
	/// @{

	/// To add a Command entry in the module Contextual Menu.
    inline void sdkAddCommand(AnsiCharPtr name, NativeInt CallbackId, LongBool Translate = FALSE)      { m_masterInfo->AddCommand (m_moduleInfo, name, CallbackId, Translate); };

	/// To add a named separator in the module Contextual Menu.
    inline void sdkAddCommandSeparator(AnsiCharPtr name, LongBool Translate = FALSE)                   {m_masterInfo->AddCommandSeparator (m_moduleInfo, name, Translate); };
    /// @}
       
    //----------------------------------------------------------------------------
	/// Use it if a parameter have the LAN Remote activated. @see TParamInfo::IsShared
    /// @name Lan Remote request 
	/// @{
    
	/// Ask Usine to update a parameter on LAN Remote clients.
    inline void	sdkNeedRemoteUpdate ( int numParam)		{ m_masterInfo->NeedRemoteUpdate (m_moduleInfo, numParam); };
    /// @}

	//----------------------------------------------------------------------------
	/// Utils functions to help manipulate video frames
	/// @name Frame manipulation 
	/// @{
	inline void sdkGetInputFrame(int numInput, PTUsineFrame frame) { m_masterInfo->GetInputFrame(m_moduleInfo, numInput, frame); };
	inline void sdkSetOutputFrame(int numOutput, PTUsineFrame frame) { m_masterInfo->SetOutputFrame(m_moduleInfo, numOutput, frame); }
	inline void sdkClearFrame(PTUsineFrame frame, TColorUsine color) { m_masterInfo->ClearFrame(m_moduleInfo, color, frame); }
	inline void sdkReleaseFrame(PTUsineFrame frame) { m_masterInfo->ReleaseFrame(m_moduleInfo, frame); }
	inline void sdkGetNewFrame(PTUsineFrame frame, int width, int height, LongBool permanent) { m_masterInfo->GetNewFrame(m_moduleInfo, width, height, permanent, frame); }
	inline void sdkCopyFrame(PTUsineFrame srcFrame, PTUsineFrame destFrame) { m_masterInfo->CopyFrame(srcFrame,destFrame); }
	inline void sdkSetDimmerFrame(PTUsineFrame frame, float dimmer) { m_masterInfo->SetDimmerFrame(m_moduleInfo, dimmer, frame); }
	inline void sdkSetPixel(PTUsineFrame frame, int X, int Y, TUsinePixel pixel)
	{
		PTUsinePixel pix = frame->Pixels;
		pix = pix + frame->Width*Y + X;
		*pix = pixel;
	}
	inline TUsinePixel sdkGetPixel(PTUsineFrame frame, int X, int Y)
	{
		PTUsinePixel pix = frame->Pixels;
		pix = pix + frame->Width*Y + X;
		return *pix;
	}
	inline TUsinePixel sdkColorToPixel(TColorUsine color) { return m_masterInfo->ColorToPixel(color); }
	inline TColorUsine sdkPixelToColor(TUsinePixel pixel) { return m_masterInfo->PixelToColor(pixel); }

	/// @}

    //----------------------------------------------------------------------------
	/// Utils functions to help manipulate events
    /// @name Events manipulation 
	/// @{
    
    inline bool sdkCompareEvt( UsineEventPtr e1, UsineEventPtr e2 )                     { return m_masterInfo->CompareEvt( e1, e2 ); };
 
	inline void sdkCopyEvt(UsineEventPtr scr, UsineEventPtr dest)                       { m_masterInfo->CopyEvt( scr, dest ); };

    inline void sdkConcatEvt( UsineEventPtr i1, UsineEventPtr i2, UsineEventPtr o )             { m_masterInfo->ConcatEvt( i1, i2, o ); };

    inline void sdkShiftEvt( int n, UsineEventPtr input, UsineEventPtr output )         { m_masterInfo->ShiftEvt( n, input, output ); };

    inline void sdkSetEvtSize( UsineEventPtr ev, int size )                     { m_masterInfo->SetEvtSize( ev, size ); };

    inline int  sdkGetEvtSize( UsineEventPtr ev)                                { return m_masterInfo->GetEvtSize( ev ); };

    inline void sdkSetEvtMaxSize( UsineEventPtr ev, int size )                  { m_masterInfo->SetEvtMaxSize( ev, size ); };

    inline void sdkDestroyEvt( UsineEventPtr &ev )                              { m_masterInfo->DestroyEvt( ev ); };

    inline void sdkCreateEvt( UsineEventPtr &ev, int originalsize )             { m_masterInfo->CreateEvt( ev, originalsize ); };

    inline void sdkMoveBLOC32( TPrecision* src, TPrecision* dest, int size )      { m_masterInfo->MoveBLOC32( src, dest, size ); };

    // acces to evt values
    inline void			sdkSetEvtData( UsineEventPtr ev, TPrecision value )                 { m_masterInfo->SetEvtData( ev, value ); };

    inline TPrecision	sdkGetEvtData( UsineEventPtr ev )									{ return m_masterInfo->GetEvtData( ev ); };

    inline void			sdkSetEvtArrayData( UsineEventPtr ev, int idx, TPrecision value )   { m_masterInfo->SetEvtArrayData( ev, idx, value ); };

    inline TPrecision	sdkGetEvtArrayData( UsineEventPtr ev, int idx )						{ return m_masterInfo->GetEvtArrayData( ev, idx ); };
    
    inline void	 sdkSetEvtPointer( UsineEventPtr ev, void* value )               { m_masterInfo->SetEvtPointer( ev, value ); };

    inline void* sdkGetEvtPointer( UsineEventPtr ev )                            { return m_masterInfo->GetEvtPointer( ev ); };

    inline void			sdkSetEvtColor( UsineEventPtr ev, TColorUsine value )           { m_masterInfo->SetEvtColor( ev, value ); };

    inline TColorUsine	sdkGetEvtColor( UsineEventPtr ev )                              { return m_masterInfo->GetEvtColor( ev ); };

    //inline void	SetEvtArrayPointer( UsineEventPtr ev, int idx, void* value )   { m_masterInfo->SetEvtArrayPointer( ev, idx, value ); };
    //inline void* GetEvtArrayPointer( UsineEventPtr ev, int idx )              { return m_masterInfo->GetEvtArrayPointer( ev, idx ); };
    
    inline void	sdkSetEvtArrayColor( UsineEventPtr ev, int idx, TColorUsine value )   
	{
		// sanity check
		int index = std::max(0, std::min(idx, sdkGetEvtSize( ev)));
		TColorUsine* data =  (TColorUsine*)sdkGetEvtDataAddr (ev);

		data += index;

		*data = value;
	};

    inline TColorUsine sdkGetEvtArrayColor( UsineEventPtr ev, int idx )              
	{ 
		// sanity check
		int index = std::max(0, std::min(idx, sdkGetEvtSize( ev)));
		TColorUsine* data =  (TColorUsine*)sdkGetEvtDataAddr (ev);

		TColorUsine result = *(data + index);
		return result;
	};

    inline void			sdkSetEvtArrayMidi( UsineEventPtr ev, int idx, UsineMidiCode midi ) { m_masterInfo->SetEvtArrayMidi( ev, idx, midi ); };
    
	inline UsineMidiCode	sdkGetEvtArrayMidi( UsineEventPtr ev, int idx )					{	return m_masterInfo->GetEvtArrayMidi( ev, idx ); };
    
    inline void	 sdkSetEvtPChar( UsineEventPtr ev,  AnsiCharPtr text )                { m_masterInfo->SetEvtPChar( ev, text ); };
    
	inline AnsiCharPtr sdkGetEvtPChar( UsineEventPtr ev )                             { return m_masterInfo->GetEvtPChar( ev ); };
    
    inline TPrecision* sdkGetEvtDataAddr( UsineEventPtr ev )                         { return m_masterInfo->GetEvtDataAddr( ev ); };	
    
    // 3DPoints stuff
	// the event must have a line number of 3 to work
    // pointIndex is the index of the point you want
    inline T3DPointF sdkGetEvt3DPoint (UsineEventPtr ev, int pointIndex)
    {
        T3DPointF result;

        if (sdkGetEvtNbLines (ev) == 3 && pointIndex < (sdkGetEvtSize (ev)/3))
        {
            TPrecision* pointer3DPoint = sdkGetEvtDataAddr (ev) + (pointIndex*3);

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
	/// Utils functions to operate on events data's
    /// @name Events data utils 
	/// @{   
	
    // evt data manipulation
    inline void  sdkAddEvt3( UsineEventPtr in1, UsineEventPtr in2, UsineEventPtr out )          { m_masterInfo->AddEvt3( in1, in2, out ); }
    inline void  sdkAddEvt2( UsineEventPtr in1, UsineEventPtr in2 )                             { m_masterInfo->AddEvt2( in1, in2 ); }
    inline void  sdkSubEvt3( UsineEventPtr in1, UsineEventPtr in2, UsineEventPtr out )          { m_masterInfo->SubEvt3( in1, in2, out ); }
    inline void  sdkSubEvt2( UsineEventPtr in1, UsineEventPtr in2 )                             { m_masterInfo->SubEvt2( in1, in2 ); }
    inline void  sdkDivEvt3( UsineEventPtr in1, UsineEventPtr in2, UsineEventPtr out )          { m_masterInfo->DivEvt3( in1, in2, out ); }
    inline void  sdkMultEvt3( UsineEventPtr in1, UsineEventPtr in2, UsineEventPtr out )         { m_masterInfo->MultEvt3( in1, in2, out ); }
    inline void  sdkMultEvt2( UsineEventPtr in1, UsineEventPtr in2 )                            { m_masterInfo->MultEvt2( in1, in2 ); }
    inline void  sdkMultEvt2Audio( UsineEventPtr in1, UsineEventPtr in2 )                       { m_masterInfo->MultEvt2( in1, in2 ); }
    inline void  sdkMultEvt1( TPrecision m, UsineEventPtr out )                                 { m_masterInfo->MultEvt1( m, out ); }
    inline void  sdkModEvt3( UsineEventPtr in1, UsineEventPtr in2, UsineEventPtr out )          { m_masterInfo->ModEvt3( in1, in2, out ); }
    inline void  sdkPowerEvt3( UsineEventPtr in1, UsineEventPtr in2, UsineEventPtr out )        { m_masterInfo->PowerEvt3( in1, in2, out ); }
    inline void  sdkExpEvt1( UsineEventPtr in1 )                                                { m_masterInfo->ExpEvt1( in1 ); }
    inline void  sdkSqrtEvt1( UsineEventPtr in1 )                                               { m_masterInfo->SqrtEvt1( in1 ); }
    inline void  sdkSetEvtNbLines( UsineEventPtr in1, int nbLines)                              { m_masterInfo->SetEvtNbLines( in1, nbLines); }
    inline int   sdkGetEvtNbLines( UsineEventPtr in1)                                           { return m_masterInfo->GetEvtNbLines( in1); };

    inline TPrecision	sdkMaxEvt1( UsineEventPtr in1 )                                         { return m_masterInfo->MaxEvt1( in1 ); };
    inline TPrecision	sdkMinEvt1( UsineEventPtr in1 )                                         { return m_masterInfo->MinEvt1( in1 ); };
    inline void			sdkMaxEvt3( UsineEventPtr in1, UsineEventPtr in2, UsineEventPtr out )   { m_masterInfo->MaxEvt3( in1, in2, out ); };
    inline void			sdkMinEvt3( UsineEventPtr in1, UsineEventPtr in2, UsineEventPtr out )   { m_masterInfo->MinEvt3( in1, in2, out ); };

    inline void  sdkThreshEvt2( const UsineEventPtr in1, UsineEventPtr out, TPrecision min, TPrecision max ) { m_masterInfo->ThreshEvt2( in1, out, min, max ); };
    inline void  sdkThreshEvt1( UsineEventPtr in1, TPrecision min, TPrecision max )                          { m_masterInfo->ThreshEvt1( in1, min, max ); };
    inline void  sdkMixMidiEvt3( UsineEventPtr in1, UsineEventPtr in2, UsineEventPtr out )                   { m_masterInfo->MixMidiEvt3( in1, in2, out ); };
    inline void  sdkMixMidiEvt2( UsineEventPtr in1, UsineEventPtr in2 )                                      { m_masterInfo->MixMidiEvt2( in1, in2 ); };
    inline void  sdkFadeInAudioEvt( UsineEventPtr TPCMin )                                 { m_masterInfo->FadeInAudioEvt( TPCMin ); };
    inline void  sdkFadeOutAudioEvt( UsineEventPtr TPCMin )                                { m_masterInfo->FadeOutAudioEvt( TPCMin ); };
    inline void  sdkClearAudioEvt( UsineEventPtr in1 )                                     { m_masterInfo->ClearAudioEvt( in1 ); }; 

    // smooth functions
    inline void sdkSmoothPrecision ( TPrecision& oldValue, UsineEventPtr currentEvent, TPrecision factor )
    { 
		return m_masterInfo->SmoothPrecision ( oldValue, currentEvent, factor ); 
	};
    inline void sdkSmoothEvent ( TPrecision& oldValue, UsineEventPtr currentEvent, TPrecision target, TPrecision factor ) 
	{ 
		return m_masterInfo->SmoothEvent  ( oldValue, currentEvent, target, factor ); 
	}
    /// @}

	//----------------------------------------------------------------------------
	/// Critical sections functions to protect patches calculation.
	/// @name Critical Sections 
	/// @{
	inline void	sdkLockPatch() { m_masterInfo->LockPatch(m_moduleInfo); };
	inline void	sdkUnLockPatch() { m_masterInfo->unLockPatch(m_moduleInfo); };

	/// @}


    //----------------------------------------------------------------------------
 	/// Utils functions to update some Parameter property other than the Event data.
    /// @name Parameters setters 
	/// @{

	/// If the parameter is of type ptListBox, change the comma text defining the list content. @see onGetParamInfo;
	/// @param numParam the param index.
    inline void	sdkSetListBoxCommatext( int numParam, AnsiCharPtr commaText )  { m_masterInfo->SetListBoxCommatext( m_moduleInfo, numParam, commaText ); };
    
	/// Change the name of a parameter. It's the name showed in Usine.
	/// @param numParam the param index.
	inline void	sdkSetParamCaption( int numParam, AnsiCharPtr caption )		 { m_masterInfo->SetParamCaption( m_moduleInfo, numParam, caption ); };
    
	/// Set a parameter visible or not.
	/// @param numParam the param index.
	inline void	sdkSetParamVisible( int numParam, LongBool visible )	        { m_masterInfo->SetParamVisible( m_moduleInfo, numParam, visible ); };

	/// Recreate a parameter with new settings.
	/// @param numParam the param index.
	/// @param pParamInfo pointer to TParamInfo structure 
	inline void	sdkRecreateParam(int numParam, TParamInfo* pParamInfo) { m_masterInfo->RecreateParam(m_moduleInfo, numParam, pParamInfo); };

	/// If the parameter is of type ptTextField, change the text content. @see onGetParamInfo;
	/// @param numParam the param index.
	inline void	sdkSetParamValueText( int numParam, AnsiCharPtr valueText )		 { m_masterInfo->SetParamValueText( m_moduleInfo, numParam, valueText ); };	
    
	/// Ask Usine to Update the parameter.
	/// Usually used after one of the function above.
	/// @param numParam the param index.
	inline void	sdkRepaintParam( int numParam)							            { m_masterInfo->RepaintParam(m_moduleInfo, numParam); };
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
    /// @name Usine Core infos and services 
	/// @{

	/// Get the list of possible usine block size. 
	inline AnsiCharPtr  sdkGetUsineBlockSizeList() { return m_masterInfo->BlockSizeList; };

	/// Get the default usine block size. 
	inline int  sdkGetUsineDefaultBlockSizeIndex() { return m_masterInfo->BlockSizeDefaultIndex; };

	/// Low level , not for public use
	/// Usine audio callback
	inline void	sdkUsineAudioDeviceIOCallback(const float** inputChannelData, int numInputChannels, float** outputChannelData, int numOutputChannels, int numSamples )
	{ 
		m_masterInfo->AudioDeviceIOCallback( inputChannelData, numInputChannels, outputChannelData, numOutputChannels, numSamples );
	};

	/// Low level , not for public use
	/// Usine  midi callback
	inline void sdkUsineMidiDeviceCallback (int deviceID, double TimeStamp, UsineMidiCode midiData)
	{ 
		m_masterInfo->MidiDeviceCallback (deviceID, TimeStamp, midiData);
	};
	
	/// Low level , not for public use
	/// Usine midi sysex callback
	inline void sdkUsineMidiSysexDeviceCallback (int deviceID, double TimeStamp, char** sysEx, int size)
	{ 
		m_masterInfo->MidiSysexDeviceCallback (deviceID, TimeStamp, sysEx, size);
	};
    
    /// Usine internal messages
    inline void sdkSendUsineMsg( AnsiCharPtr Msg )                            { m_masterInfo->SendUsineMsg( m_moduleInfo, Msg ); };
   
    /// Usine notification
    inline void	sdkNotifyUsine(NativeInt Target, NativeInt Msg, NativeInt Param1 = 0, NativeInt Param2 = 0)
	{
		m_masterInfo->NotifyUsine(m_moduleInfo, Target, Msg, Param1, Param2); 
	};
 
    /// To access the current VstTimeInfo structure of Usine.
    inline VstTimeInfo* sdkGetVstTimeInfo()      { return m_masterInfo->GetVstTimeInfo(m_moduleInfo); };

    /// To get the current Usine block size.
    inline int		sdkGetBlocSize()              { return m_masterInfo->BlocSize; };
	
    /// To get the current Usine sample rate
	inline double	sdkGetSampleRate()			  { return m_masterInfo->GetSampleRate(); };
	    
    /// PC only, Usine mainform handle (THandle)
    inline NativeInt sdkGetUsineMainWindow()          { return m_masterInfo->GetUsineMainWindow(); };
	
    /// MAC only, Usine NSView
    inline void* sdkGetUsineNSView()              { return m_masterInfo->GetMacNSView(); };
	
    /// MAC only, Usine NSApplication
    inline void* sdkGetUsineNSApplication()       { return (void*)m_masterInfo->NSApplication; };

    /// To get a named color from the Usine ColorSet.
    inline TColorUsine sdkGetUsineColor(TUsineColorSet colorName)
    {
        TColorUsine result = 0;
        
        if (m_masterInfo != 0)
            result = m_masterInfo->GetUsineColor ((int)colorName);
            
        return result;
    };
    
    /// To get the Usine global time in millisecond, precision up to nanoseconde
    inline double sdkGetTimeMs () { return m_masterInfo->GetTimeMs (m_moduleInfo); };


	///Translate a StringID to the current Usine language.
	inline AnsiCharPtr sdkGetTrans (AnsiCharPtr StringID) { return m_masterInfo->GetTranslation (StringID); };

	/// To check if the patch containing the module is running.
    inline LongBool	sdkPatchIsRunning    () { return m_masterInfo->PatchIsRunning (m_moduleInfo); };
    
	/// To check if the patch containing the module is loading.
	inline LongBool sdkPatchLoading() { return m_masterInfo->PatchLoading(m_moduleInfo); }

	//-----------------------------------------------------------------------------
    // wrappers for ModuleInfo functions
    //-----------------------------------------------------------------------------
    /// To get the pointer of the module
    inline void* sdkGetUsineUserModule() { return m_moduleInfo->UsineUserModule; };
/*
    // query index accessor
    inline int GetQueryIdx() { return m_moduleInfo->QueryIdx; };
 */
	
    /// To get the version number of Usine as a string.  
    inline AnsiCharPtr sdkGetUsineVersionNum() { return m_masterInfo->UsineVersion; }; 
	
    /// To get the version type of Usine @see TUsineVersionType. 
    inline TUsineVersionType sdkGetUsineVersionType() { return m_masterInfo->UsineVersionType; };  
	
    /// To get the current language used in Usine. 
	/// @return a string naming the current language (En, Fr...)
    inline AnsiCharPtr sdkGetUsineLanguage() { return m_masterInfo->UsineLanguage; };
	
    /// To get the of the save format of Usine. 
    inline int sdkGetUsineSaveVersion()    { return m_masterInfo->UsineSaveVersion; };  
    
	
    /// To get the maximum audio inputs Usine can accept. 
    inline int sdkGetUsineMaxAudioInputs ()         { return m_masterInfo->MAX_AUDIO_INPUTS; };
	
    /// To get the maximum audio outputs Usine can accept. 
    inline int sdkGetUsineMaxAudioOutputs ()        { return m_masterInfo->MAX_AUDIO_OUTUTS; };
	
    /// To get the maximum Midi device Usine can accept. 
    inline int sdkGetUsineMaxMidiDevices ()         { return m_masterInfo->MAX_MIDI_DEVICES; };
	
    /// To get the current polyphony of Usine.  
    inline int sdkGetUsineMaxPolyphony ()         { return m_masterInfo->MULTIPHONY_MAX; };

	/// set the name of the module (as it appear in the patch, on the module title)
	inline void sdkSetModuleUserName(AnsiCharPtr name) { m_masterInfo->SetModuleUserName(m_moduleInfo, name); };
	
	/// get the name of the module (as it appear in the patch, on the module title)
	inline AnsiCharPtr sdkGetModuleUserName() { return m_masterInfo->GetModuleUserName(m_moduleInfo); };
    /// @}	

    //----------------------------------------------------------------------------
	/// To get the path to access some important folders of Usine.
    /// @name Usine Paths getters 
	/// @{

	
	/// The Sound folder as configured in the Global setting Tab.
    inline AnsiCharPtr    sdkGetUsineSoundPath  ()		    { return m_masterInfo->SoundPath; };
    
	/// The Patch library folder as configured in the Global setting Tab.
	inline AnsiCharPtr    sdkGetUserLibPath ()			    { return m_masterInfo->UserLibPath; };
    
	/// The Record Out folder as configured in the Global setting Tab.    
	inline AnsiCharPtr    sdkGetUsineRecordPath ()		    { return m_masterInfo->RecordPath; };
    
	/// The install folder of Usine.
	inline AnsiCharPtr    sdkGetGlobalApplicationPath ()   { return m_masterInfo->GlobalApplicationPath; };
    
	/// The path of the current workspace.
	inline AnsiCharPtr    sdkGetCurrentWorkspacePath ()	{ return m_masterInfo->CurrentWorkspacePath; };
    
	/// The path of the patch containing the module.
	inline AnsiCharPtr    sdkGetCurrentPatchPath ()		{ return m_masterInfo->CurrentPatchPath; };
	
	/// The Temp folder Used by Usine.
	inline AnsiCharPtr    sdkGetUsineTempPath ()			{ return m_masterInfo->UsineTempPath; };
    
	/// Not for public use.
	inline AnsiCharPtr    sdkGetUsinePlugInsPath ()		{ return m_masterInfo->PlugInsPath; };
    /// @}	

    //----------------------------------------------------------------------------
    /// @name Files utils
	/// @{

    /// Show a dialog window to open a file.
    inline LongBool	sdkProcessOpenDialog ( AnsiCharPtr* filename, AnsiCharPtr initialDir, AnsiCharPtr filter ) { return m_masterInfo->ProcessOpenDialog	( filename, initialDir, filter ); };
    
    /// Show a dialog window to save a file.
	inline LongBool	sdkProcessSaveDialog ( AnsiCharPtr* filename, AnsiCharPtr initialDir, AnsiCharPtr filter ) { return m_masterInfo->ProcessSaveDialog	( filename, initialDir, filter ); };
    
    /// Ask Usine to search if a file exist.
	inline AnsiCharPtr    sdkFindFile          ( AnsiCharPtr* filename, AnsiCharPtr initialDir )               { return m_masterInfo->FindFile			( filename, initialDir ); };
    /// @}	
    
	//----------------------------------------------------------------------------
    /// @name Audio Files manipulation
	/// @{

    // audio files
    inline AudioFilePtr	sdkCreateAudioFile          ()																    { return m_masterInfo->CreateAudioFile(); };
    inline void			sdkDestroyAudioFile         (AudioFilePtr audiofile)											{ m_masterInfo->DestroyAudioFile( audiofile); };
	inline TPrecision   sdkGetSampleAudioFile       (AudioFilePtr audiofile, int channel, int pos)                      { return m_masterInfo->GetSampleAudioFile(audiofile, channel, pos); };
	inline TPrecision*  sdkGetSampleArrayAudioFile  (AudioFilePtr audiofile, int channel)                               { return m_masterInfo->GetSampleArrayAudioFile(audiofile, channel); };
	inline void			sdkGetBlocSampleAudioFile   (AudioFilePtr audiofile, int channel, int pos, UsineEventPtr evt)	{ m_masterInfo->GetBlocSampleAudioFile			(audiofile, channel, pos, evt); };
    inline int			sdkGetSizeAudioFile         (AudioFilePtr audiofile)											{ return m_masterInfo->GetSizeAudioFile			(audiofile); };
    inline int			sdkGetChannelAudioFile      (AudioFilePtr audiofile)											{ return m_masterInfo->GetChannelAudioFile		( audiofile); };
    inline int			sdkGetSampleRateAudioFile   (AudioFilePtr audiofile)											{ return m_masterInfo->GetSampleRateAudioFile	( audiofile); };
    inline int			sdkGetBitPerSampleAudioFile (AudioFilePtr audiofile)											{ return m_masterInfo->GetBitPerSampleAudioFile	( audiofile); };
    inline void			sdkLoadInMemoryAudioFile    (AudioFilePtr audiofile, AnsiCharPtr name)							{ m_masterInfo->LoadInMemoryAudioFile	( audiofile, name); };
    inline void			sdkLoadStayOnDiskAudioFile  (AudioFilePtr audiofile, AnsiCharPtr name)							{ m_masterInfo->LoadStayOnDiskAudioFile	( audiofile, name); };
    inline void			sdkGetPeaksAudioFile        (AudioFilePtr audiofile, UsineEventPtr evt)							{ m_masterInfo->GetPeaksAudioFile		( audiofile, evt); };
    inline void			sdkSaveToDiskAudioFile      (AudioFilePtr audiofile, AnsiCharPtr name, int nbChannels)			{ m_masterInfo->SaveToDiskAudioFile		( audiofile, name, nbChannels); };
    inline void			sdkSetChannelsAudioFile     (AudioFilePtr audiofile, int nbChannels)							{ m_masterInfo->SetChannelsAudioFile		( audiofile, nbChannels); };
    inline void			sdkSetSizeAudioFile         (AudioFilePtr audiofile, int size)									{ m_masterInfo->SetSizeAudioFile			( audiofile, size); };
    inline void			sdkSetSampleAudioFile       (AudioFilePtr audiofile, int channel, int pos, TPrecision sample)	{ m_masterInfo->SetSampleAudioFile		( audiofile, channel, pos, sample); };
	inline void			sdkClearAudioFile           (AudioFilePtr audiofile)                                            { m_masterInfo->ClearAudioFile(audiofile); };
	inline void			sdkResampleAudioFile        (AudioFilePtr audiofile, TPrecision factor)                         { m_masterInfo->resampleAudioFile(audiofile,factor); };
	/// @}	

    //----------------------------------------------------------------------------
    /// @name Math utils 
	/// @{

	/// Linear interpolation  on a TPrecision value.
	/// @return The interpolated value.
    inline TPrecision sdkLinearInterpolation ( TPrecision f,  TPrecision a, TPrecision b )											{ return m_masterInfo->LinearInterpolation	( f, a, b ); };
    
	/// Cubic interpolation  on a TPrecision value.
	/// @return The interpolated value.
	inline TPrecision sdkCubicInterpolation  ( TPrecision fr, TPrecision inm1, TPrecision inp, TPrecision inp1, TPrecision inp2 )	{ return m_masterInfo->CubicInterpolation	( fr, inm1, inp, inp1, inp2 ); };
    
	/// Spline interpolation on a TPrecision value.
	/// @return The interpolated value.
	inline TPrecision sdkSplineInterpolation ( TPrecision fr, TPrecision inm1, TPrecision inp, TPrecision inp1, TPrecision inp2 )	{ return m_masterInfo->SplineInterpolation	( fr, inm1, inp, inp1, inp2 ); };
    /// @}	
    
    //----------------------------------------------------------------------------
    /// @name Trace functions 
	/// @{
    
	/// Show a string in the Trace Panel.
    inline void sdkTraceChar        (AnsiCharPtr s)			     { return m_masterInfo->TraceChar	      ( s ); };
	
	/// Show an integer in the Trace Panel.
    inline void sdkTraceInt         (int i)			                 { return m_masterInfo->TraceInteger 	  ( i ); };
	
	/// Show a decimal number in the Trace Panel.
    inline void sdkTracePrecision   (TPrecision f)	                 { return m_masterInfo->TracePrecision	  ( f ); };
	
	/// Show a string in the Trace Panel with the error tag.
    inline void sdkTraceErrorChar   (AnsiCharPtr s)			     { return m_masterInfo->TraceErrorChar	  ( s ); };
	
	/// Show a string in the Trace Panel with the warning tag.
    inline void sdkTraceWarningChar (AnsiCharPtr s)		         { return m_masterInfo->TraceWarningChar  ( s ); };
	
	/// Show a string in the SplashForm.
    inline void sdkTraceSplashChar  (AnsiCharPtr s, int autoClose)  { return m_masterInfo->TraceSplashChar   ( s, autoClose ); };
	
	/// Print a string in the LogUsine.log file. Optionally show it in the SplashForm.
    inline void sdkTraceLogChar     (AnsiCharPtr s, LongBool showInSplashForm = FALSE)
    {
        return m_masterInfo->TraceLogChar (s, showInSplashForm);
    };
    /// @}		
   
    
    //-----------------------------------------------------------------------------
    /// @name Dialog popups
	/// @{
	
    /// A dialog popup with Yes, No and Cancel buttons. 
    /// @param msg The message to show on the popup popup.
    /// @return The choice of the user.
    /// @see TDialogsResults
	inline int sdkDialogConfirmationYesNoCancel	( AnsiCharPtr msg ) {return m_masterInfo->DialogConfirmationYesNoCancel(msg);};
    
    /// A dialog popup with Yes and No buttons. 
    /// @param msg The message to show on the popup popup.
    /// @return The choice of the user.
    /// @see TDialogsResults   
    inline int sdkDialogConfirmationYesNo			( AnsiCharPtr msg ) {return m_masterInfo->DialogConfirmationYesNo(msg);};

    /// A dialog popup with OK button. 
    /// @param msg The message to show on the popup popup.
    /// @return The choice of the user.
    /// @see TDialogsResults
	inline int sdkDialogInformationOk				( AnsiCharPtr msg ) {return m_masterInfo->DialogInformationOk(msg);};

    /// A dialog popup with OK and Cancel buttons. 
    /// @param msg The message to show on the popup popup.
    /// @return The choice of the user.
    /// @see TDialogsResults
    inline int sdkDialogConfirmationOKCancel		( AnsiCharPtr msg ) {return m_masterInfo->DialogConfirmationOKCancel(msg);};

	/// A dialog popup with a string input box . 
	/// @param msg The message to show on the popup popup.
	/// @return the edited string.
	/// @see TDialogsResults
	inline AnsiCharPtr sdkDialogInputBox(AnsiCharPtr caption, AnsiCharPtr prompt, AnsiCharPtr defaultValue) { return m_masterInfo->DialogInputBox(caption,prompt,defaultValue); };

	/// @}	
    //----------------------------------------------------------------------------
    /// A User module can create and manage an independent child window for it's own use.
    /// Note that only one child window at a time per module is possible.
	/// @see Child Window Callbacks
	/// @name Module child window
	/// @{	 
	 
    inline void* sdkCreateDeskWindow () { return m_masterInfo->CreateDeskWindow (m_moduleInfo); };
    inline void	sdkDestroyDeskWindow () { m_masterInfo->DestroyDeskWindow (m_moduleInfo); };
    inline void* sdkShowDeskWindow    () { return m_masterInfo->ShowDeskWindow (m_moduleInfo); };
    inline void	sdkHideDeskWindow    () { m_masterInfo->HideDeskWindow (m_moduleInfo); };
 
    inline void	sdkSetDeskWindowClientSize  (int clientWidth, int clientHeight)
    {
        m_masterInfo->SetDeskWindowClientSize  (m_moduleInfo, clientWidth, clientHeight);
    };   
    inline void	sdkSetDeskWindowCaption    (ModuleInfo* pModuleInfo, AnsiCharPtr name){ m_masterInfo->SetDeskWindowCaption (m_moduleInfo, name); };
    /// @}	

    //----------------------------------------------------------------------------
    /// Useful to convert between pixel coord and graphic canvas unit.
	/// @name Converter utils
	/// @{	 
	
    inline float sdkPixelToHeightPercent(float pixel)
    {
        float result = 0.0f;

        if (panelHeight > 0.0f)
            result = pixel / panelHeight;

        return result;
    };

    inline float sdkPixelToWidthPercent(float pixel)
    {
        float result = 0.0f;

        if (panelHeight > 0.0f)
            result = pixel / panelWidth;
        
        return result;
    };

    inline float sdkHeightPercentToPixel(float percent)
    {
        float result = 0.0f;

        if (panelHeight > 0.0f)
            result = panelHeight * percent;

        return result;
    };

    inline float sdkWidthPercentToPixel(float percent)
    {
        float result = 0.0f;

        if (panelHeight > 0.0f)
            result = panelWidth * percent;
        
        return result;
    };
    /// @}	
     	
    void setStringTrace (std::string newString)
    {
        stringTrace = newString;
    };

	
    //-------------------------------------------------------------------------
    // protected members
    //-------------------------------------------------------------------------
protected:

    float panelWidth; // in pixel
    float panelHeight; // in pixel


    // Pointer to MasterInfo and ModuleInfo provided by usine
    MasterInfo* m_masterInfo;
    ModuleInfo* m_moduleInfo;
    
    std::string stringTrace;

    //-----------------------------------------------------------------------------
    // protected methodes
    //-----------------------------------------------------------------------------
protected:


    //-----------------------------------------------------------------------------
    // private methodes
    //-----------------------------------------------------------------------------
private:

    // declare non copyable
	UserModuleBase (const UserModuleBase&);
    UserModuleBase& operator= (const UserModuleBase&);

}; // class UserModuleBase


#endif // __USER_MODULE_H__


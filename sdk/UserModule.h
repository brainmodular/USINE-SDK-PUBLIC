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
///----------------------------------------------------------------------------

// include once, no more
#pragma once
//----------------------------------------------------------------------------
// includes
//----------------------------------------------------------------------------
#include <algorithm>
#include <sstream>
#include <iomanip>
#include "UsineDefinitions.h"
//#include "UserUtils.h"

#define SDK_UNUSED(expr) do { (void)(expr); } while (0)

class UsineEventClass;


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
/// @param optionalContent A string of optionnal content
void CreateModule(void * &pModule, AnsiCharPtr optionalString, LongBool Flag, TMasterInfo *pMasterInfo,
                  AnsiCharPtr optionalContent);

/// @fn void DestroyModule(void* pModule)
/// @brief Called by Usine to destroy the User Module
/// When clearing a workspace or patch.
/// Before recreate module.
/// When closing Usine.
/// @param pModule Handle for the new instantiated module
void DestroyModule(void *pModule);

/// @fn void GetBrowserInfo (TModuleInfo* pModuleInfo)
/// @brief Called by usine to get module informations for the browser
/// When the browser open the folder where the module is stored.
void GetBrowserInfo(TModuleInfo *pModuleInfo);

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

    /// Usine need them to collect mandatory infos and update the module.
    /// @name Mandatory Callbacks
    /// @{

    /// Usine call it to obtain the module characteristics. 
	/// You have to fill the TModuleInfo structure with infos describing the module.
	/// TMasterInfo can be used to obtain some useful infos but always test if the handle is valid as it can be nullptr. 
    /// @param pMasterInfo The TMasterInfo structure. can be null.
    /// @param pModuleInfo The TModuleInfo structure to describe the module characteristics
    virtual void onGetModuleInfo(TMasterInfo *pMasterInfo, TModuleInfo *pModuleInfo) =0;

    /// Usine call it for each parameters of the module to obtain it's characteristics. 
    /// @param ParamIndex The index of the parameter.
    /// @param pParamInfo Handle to the info structure of the parameter.
    virtual void onGetParamInfo(int ParamIndex, TParamInfo *pParamInfo) =0;


	/// Deprecated since HH3, use pParamInfo->setEventClass	(...); Called only for backward compatibility.
    /// Usine calls it for each parameters of the module to obtain the event handle of the parameter. 
	/// @param ParamIndex The index of the parameter.
    /// @param pEvent Handle to the event of the parameter.
    virtual void onSetEventAddress(const int ParamIndex, const UsineEventPtr pEvent)
    {
        SDK_UNUSED(ParamIndex);
        SDK_UNUSED(pEvent);
    }


    /// Usine call it to inform the module something happened.
    /// @param Message Provide various infos about what happened.
    virtual void onCallBack(TUsineMessage *Message)
    {
        SDK_UNUSED(Message);
    }

    // Used for managing asynchronous job. Start the background job 
    virtual void startJob();

    /// Used for managing asynchronous job. Is called before the job start
    virtual void onJobBegin()
    {
    }

    /// Used for managing asynchronous job. Execute long running task here
    virtual void onJobProcess()
    {
    }

    /// Used for managing asynchronous job. It is called after the job terminated
    virtual void onJobEnd()
    {
    }

    /// @}

    //-----------------------------------------------------------------------------
    /// Needed if Query system is used.
	/// @name Query System Callbacks
	/// @{

    /// Usine call it to obtain the number of parameters of the module depending of the query result
    /// @param queryResult1 The value of the first choice made in the query popup
    /// @param queryResult2 The value of the second choice made in the query popup
    /// @return Number of parameters of the module
    virtual int onGetNumberOfParams(int queryResult1, int queryResult2)
    {
        return m_moduleInfo->NumberOfParams;
    }

    /// Usine call it after the query pop-up is closed. It let you do some things depending of the query choice.
    /// @param pMasterInfo The TMasterInfo structure.
    /// @param pModuleInfo The TModuleInfo structure.
    /// @param queryResult1 The value of the first choice made in the query pop-up
    /// @param queryResult2 The value of the fsecond choice made in the query pop-up
    virtual void onAfterQuery(TMasterInfo *pMasterInfo, TModuleInfo *pModuleInfo, const int queryResult1, const int queryResult2)
    {
        SDK_UNUSED(pMasterInfo);
        SDK_UNUSED(pModuleInfo);
        SDK_UNUSED(queryResult1);
        SDK_UNUSED(queryResult2);
    }

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
    virtual void onProcess()
    {
    }

    /// @}

    /// @name Real-time Video processing
 	/// @{
	/// Called by the the video thread to process video frames only for TModuleInfo::ModuleType == mtVideo
	/// Don't process video outside this procedure.
	/// @see TModuleInfo::ModuleType
    virtual void onProcessVideo()
    {
    }

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
    virtual int onGetStateChunkLen(LongBool Preset) { return 0; }

    /// Provide a chunk handle to fill with additional info's.
    /// @param chunk The chunk pointer to fill.
    /// @param Preset Tell if it's a preset or a patch chunk. TRUE = preset chunk, FALSE = .pat/.wkp chunk.
    virtual void onGetStateChunk(void *chunk, const LongBool Preset)
    {
        SDK_UNUSED(chunk);
        SDK_UNUSED(Preset);
        /* Override me! */
    }

    /// Provide a chunk handle filled by Usine to restore them into the module.
    /// @param chunk The chunk pointer to read from.
    /// @param sizeInBytes The size of the chunk.
    /// @param Preset Tell if it's a preset or a patch chunk. TRUE = preset chunk, FALSE = .pat/.wkp chunk.
    virtual void onSetStateChunk(const void *chunk, const int sizeInBytes, const LongBool Preset)
    {
        SDK_UNUSED(chunk);
        SDK_UNUSED(sizeInBytes);
        SDK_UNUSED(Preset);
        /* Override me! */
    }

    /// Called after the module is loaded in Usine.
    virtual void onAfterLoading()
    {
        /* Override me! */
    }


    /// @}

    //-----------------------------------------------------------------------------
    /// Implement them if the module need to add some entries in the Settings Panel.
    /// @name Settings Callbacks
    /// @{

    /// Used to add some settings entries in the Settings panel.
    /// Typical use is some calls to Settings options functions
    virtual void onCreateSettings()
    {
    }

    /// Called when something change in one of entries.
	/// Use it to update the module behaviour after a setting change
    virtual void onSettingsHasChanged()
    {
    }

    /// @}

    //-----------------------------------------------------------------------------
    /// Implement them if the module type is mtControl.
    /// @name Graphics Canvas Callbacks
    /// @{

    /// Called when Usine resize the Graphic canvas
    /// @param contentWidth the new width of the canvas.
    /// @param contentHeight the new height of the canvas.
	virtual void onResize(float contentWidth, float contentHeight)
    {
    }

    /// Called before Usine draw the module canvas
    /// Use it to draw the module canvas.
    virtual void onPaint()
    {
    }

    /// @}

    //-----------------------------------------------------------------------------
    /// Implement it if the module need to add some entries in the Contextual Menu.
    /// @name Contextual Menu Callback
    /// @{

    /// To add some entries in the Contextual Menu.
    /// Typical use is some calls to Contextual Menus commands
    virtual void onCreateCommands()
    {
    }

    /// @}

    //-----------------------------------------------------------------------------
    /// Usine call it if TModuleInfo::CanRecord is set to TRUE in onGetModuleInfo.
	/// @name Automation Recording Callback
	/// @{

    /// Usine call it when replaying recorded curves. The module have to affect those values to the concerned variables.
    /// @param X New value of the first curve.
    /// @param Y New value of the second curve.
    /// @param Z New value of the third curve.
    virtual void onSetRecordedValue(TPrecision X, TPrecision Y, TPrecision Z)
    {
    }

    /// @}

    //-----------------------------------------------------------------------------
    /// Implement it to respond to a randomize command from Usine.
    /// @name Randomize Callback
    /// @{
    /// Activate it with pModuleInfo->CanBeRandomized = TRUE in onGetModuleInfo.
	/// If you need to implement onRandomize Callback, Usine can call it at any time to initiate a Randomize.
    virtual void onRandomize()
    {
    }

    /// @}

    //-----------------------------------------------------------------------------
    /// Implement it to respond to a reset command from Usine.
    /// @name Reset Callback
    /// @{

    /// Activate it with pModuleInfo->CanBeReset = TRUE in onGetModuleInfo.
	/// If you need to implement onReset Callback, Usine can call it at any time to initiate a Reset.
    virtual void onReset()
    {
    }

    /// @}

    //-----------------------------------------------------------------------------
    /// Implement it to respond to a Set quick colorfrom Usine.
    /// @name SetQuickColor Callback
    /// @{
    /// @param color choosen color in the quick color menu.
	/// This callback is called when the user changes the global color of the module.
    virtual void onSetQuickColor(TUsineColor color)
    {
    }

    /// @}

    //-----------------------------------------------------------------------------
    /// Implement them if the  module type is mtControl and needs to be informed of mouse and/or multi-touch events.
    /// @name Mouse and Multi-touch Callbacks
    /// @{

    /// Called by Usine when the mouse moves on the control
    /// @name onMouseMove
    /// @param Shift State of the shift modifier key
    /// @param X Cursor position first coordinate
    /// @param Y Cursor position second coordinate
    virtual void onMouseMove(TShiftState Shift, float X, float Y)
    {
    }

    /// Called by Usine when the mouse is clicked down on the control
    /// @name onMouseDown
    /// @param MouseButton Which button has been pressed @see TMouseButton
    /// @param Shift State of the shift modifier key
    /// @param X Cursor position first coordinate
    /// @param Y Cursor position second coordinate
    virtual void onMouseDown(TMouseButton MouseButton, TShiftState Shift, float X, float Y)
    {
    }

    /// Called by Usine when the mouse is clicked up on the control
    /// @name onMouseUp
    /// @param MouseButton Which button has been released @see TMouseButton
    /// @param Shift State of the shift modifier key
    /// @param X Cursor position first coordinate
    /// @param Y Cursor position second coordinate
    virtual void onMouseUp(TMouseButton MouseButton, TShiftState Shift, float X, float Y)
    {
    }

    /// Called by Usine when the mouse wheel is moved on the control
    /// @name onMouseWheel
    /// @param Shift State of the shift modifier key
    /// @param WheelDelta The amount of scrolling done
    virtual void onMouseWheel(TShiftState Shift, int WheelDelta)
    {
    }

    /// Called by Usine when control receives a touch-screen move event
	/// @name onMouseMoveMulti
	/// @param Shift State of the shift modifier key 
	/// @param X Cursor position first coordinate
	/// @param Y Cursor position second coordinate
	/// @param Pressed State of the pressed buttons
    /// @note This is called when the user touches the screen, not when the mouse is clicked.
    virtual void onMouseMoveMulti(TShiftState Shift, UsineEventPtr X, UsineEventPtr Y, UsineEventPtr Pressed)
    {
    }

	/// Called by Usine when control receives a touch-screen move event
    /// @name onMouseMoveMulti
    /// @param Shift State of the shift modifier key
	/// @param X Cursor position first coordinate
	/// @param Y Cursor position second coordinate
	/// @param Pressed State of the pressed buttons
    /// @note This is called when the user touches the screen, not when the mouse is clicked.
    virtual void onMouseMoveMulti(TShiftState Shift, UsineEventClass &X, UsineEventClass &Y, UsineEventClass &Pressed)
    {
    }

    /// Called by Usine when control receives a touch-screen down event
	/// @name onMouseDownMulti
	/// @param MouseButton Which button has been pressed @see TMouseButton
	/// @param Shift State of the shift modifier key
	/// @param X Cursor position first coordinate
	/// @param Y Cursor position second coordinate
	/// @param Pressed State of the pressed buttons
	/// @note This is called when the user touches the screen, not when the mouse is clicked.
    virtual void onMouseDownMulti(TMouseButton MouseButton, TShiftState Shift, UsineEventPtr X, UsineEventPtr Y,
                                  UsineEventPtr Pressed)
    {
    }

	/// Called by Usine when control receives a touch-screen down event
	/// @name onMouseDownMulti
	/// @param MouseButton Which button has been pressed @see TMouseButton
	/// @param Shift State of the shift modifier key
	/// @param X Cursor position first coordinate
	/// @param Y Cursor position second coordinate
	/// @param Pressed State of the pressed buttons
    /// @note This is called when the user touches the screen, not when the mouse is clicked.
    virtual void onMouseDownMulti(TMouseButton MouseButton, TShiftState Shift, UsineEventClass &X, UsineEventClass &Y,
                                  UsineEventClass &Pressed)
    {
    }

    /// Called by Usine when control receives a touch-screen up event
	/// @name onMouseUpMulti
	/// @param MouseButton Which button has been released @see TMouseButton
	/// @param Shift State of the shift modifier key
	/// @param X Cursor position first coordinate   
	/// @param Y Cursor position second coordinate
	/// @param Pressed State of the pressed buttons
	/// @note This is called when the user touches the screen, not when the mouse is clicked.
    virtual void onMouseUpMulti(TMouseButton MouseButton, TShiftState Shift, UsineEventPtr X, UsineEventPtr Y,
                                UsineEventPtr Pressed)
    {
    }
	/// Called by Usine when control receives a touch-screen up event
	/// @name onMouseUpMulti
	/// @param MouseButton Which button has been released @see TMouseButton
	/// @param Shift State of the shift modifier key
	/// @param X Cursor position first coordinate
	/// @param Y Cursor position second coordinate
	/// @param Pressed State of the pressed buttons
	/// @note This is called when the user touches the screen, not when the mouse is clicked.
    virtual void onMouseUpMulti(TMouseButton MouseButton, TShiftState Shift, UsineEventClass &X, UsineEventClass &Y,
                                UsineEventClass &Pressed)
    {
    }
    /// @}

    //-----------------------------------------------------------------------------
    /// @name Optional Callbacks
    /// @{

    /// Usine calls this after onGetModuleInfo. It lets you initialize stuff.
    /// @param pMasterInfo The TMasterInfo structure.
    /// @param pModuleInfo The TModuleInfo structure.
    virtual void onInitModule(TMasterInfo *pMasterInfo, TModuleInfo *pModuleInfo)
    {
    }

    /// Usine call it when the Usine BlockSize is changed.
    /// @param BlocSize The new BlocSize.
    virtual void onBlocSizeChange(int BlocSize)
    {
    }

    /// Usine call it  when the SampleRate is changed.
    /// @param SampleRate The new SampleRate.
    virtual void onSampleRateChange(double SampleRate)
    {
    }
    /// @}

    //-----------------------------------------------------------------------------
    /// Implement them if the  module implement a child window.
    /// @see Module child window
    /// @name Child Window Callbacks
    /// @{

    virtual void onOpenEditor()
    {
    }

    virtual void onBringToFront()
    {
    }

    virtual void onCloseEditor()
    {
    }

    virtual void onResizeEditor(int width, int height)
    {
    }
    /// @}

    //-----------------------------------------------------------------------------
    /// Usine call them to send MIDI messages to a Midi Out Device.
    /// Not for public use.
    /// @name MIDI Out Callbacks
    /// @{

    /// Send one midi code.
    /// @param DeviceID Index of the device to address.
    /// @param Code The midi message to send.
	/// @note The DeviceID is the index of the device in the list of midi devices.
    virtual void onMidiSendOut(int DeviceID, TUsineMidiCode Code)
    {
    }

    /// Send a block of midi code.
    /// @param DeviceID Index of the device to address.
    /// @param arrayCode The array of midi message to send.
    /// @param arraySize The size of the array.
	/// @note The DeviceID is the index of the device in the list of midi devices.
    virtual void onMidiSendOutArray(int DeviceID, TUsineMidiCode **arrayCode, int arraySize)
    {
    }

    /// Send a Sysex message.
    /// @param DeviceID Index of the device to address.
    /// @param Sysex The Sysex message to send.
    /// @param sysexSize The size of the Sysex message in byte.
	/// @note The DeviceID is the index of the device in the list of midi devices.
    virtual void onMidiSysexSendOut(int DeviceID, char **Sysex, int sysexSize)
    {
    }
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
    /// @param speedPPS in points per second
	/// @note The DeviceID is the index of the device in the list of ILDA devices.
    virtual void onLaserSendOut(int DeviceID, TUsineILDAPoint **arrayPoint, int arraySize, int speedPPS)
    {
    }
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
	/// @note The deviceId is the index of the device in the list of Dmx devices.
    virtual void onDMXSendOut(int deviceId, char *ByteArray, int len, int universeNum = 1)
    {
    }
    /// @}

    //----------------------------------------------------------------------------
    // internal use
    void AfterQueryPopup(TMasterInfo *pMasterInfo, TModuleInfo *pModuleInfo, int queryResult1, int queryResult2);
    void Init(TMasterInfo *pMasterInfo, TModuleInfo *pModuleInfo);
    void InitInfosStructures(TMasterInfo *pMasterInfo, TModuleInfo *pModuleInfo);
    void CallBack(TUsineMessage *Message);
    void ResizeModule(float contentWidth, float contentHeight);

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

	/// Draw a point to the module canvas.
	/// @param point The point to draw in normalized coordinates (0..1).
	/// @param color The color of the point
	/// @param pointSize The size of the point in normalized coordinates (0..1).
	/// @param rounded If TRUE, the point is drawn as a circle, otherwise as a square.
    void sdkDrawPoint(const TPointF point, const TUsineColor color, const float pointSize, const LongBool rounded) const
    {
        m_masterInfo->DrawPoint(m_moduleInfo, point, color, pointSize, rounded);
    }
 
    /// Draw Multiple points to the module canvas.
    /// @param points Pointer to the first element of the points array in normalized coordinates (0..1).
    /// @param colors Pointer to the first element of the colors array, each point can have its own color.
    /// @param pointSize The size of the points in normalized coordinates (0..1).
    /// @param rounded If TRUE, the points are drawn as circles, otherwise as squares.
    /// @param numberOfPoints The number of points to draw.
    void sdkDrawMultiPoints(const TPointFPtr points,TUsineColor* colors, const float pointSize,
                            const LongBool rounded, const int numberOfPoints) const
    {
        m_masterInfo->DrawMultiPoints(m_moduleInfo, points, colors, pointSize, rounded, numberOfPoints);
    }

	/// Draw a line to the module canvas.
	/// @param p1 The first point of the line in normalized coordinates (0..1).
	/// @param p2 The second point of the line in normalized coordinates (0..1).
	/// @param color The color of the line
	/// @param strokeThickness The thickness of the line in pixels.
    void sdkDrawLine(const TPointF p1, const TPointF p2, const TUsineColor color, const float strokeThickness) const
    {
        m_masterInfo->DrawLine(m_moduleInfo, p1, p2, color, strokeThickness);
    }


    /// Draw a polygon, points are stored in an array
	/// @param points Pointer to the first element of the points array in normalized coordinates (0..1).
	/// @param numOfPoints Number of points in the array
	/// @param color The color of the polygon
	/// @param strokeThickness The thickness of the polygon border in pixels.
    void sdkDrawPolyLine(const TPointFPtr points, const int numOfPoints, const TUsineColor color, const float strokeThickness) const
    {
        m_masterInfo->DrawPolyLine(m_moduleInfo, points, numOfPoints, color, strokeThickness);
    }

    /// Fill a polygon, points are stored in an array
	/// @param points Pointer to the first element of the points array in normalized coordinates (0..1).
	/// @param numOfPoints Number of points in the array
	/// @param color The color of the polygon
	/// @note The polygon must be closed, Usine will not close it for you.
    void sdkFillPolyLine(const TPointFPtr points, const int numOfPoints, const TUsineColor color) const
    {
        m_masterInfo->FillPolyLine(m_moduleInfo, points, numOfPoints, color);
    }

    /// Fill a round rectangle
	/// @param rect The rectangle to fill in normalized coordinates (0..1).
	/// @param color The color of the rectangle
	/// @param roundRadius The radius of the rounded corners in normalized coordinates (0..1).
	/// @param borderColor The color of the rectangle border
	/// @param borderWith The thickness of the rectangle border in normalized coordinates (0..1).
    void sdkFillRect(const TRectF rect, const TUsineColor color, const float roundRadius, const TUsineColor borderColor,
                     const float borderWith) const
    {
        m_masterInfo->FillRect(m_moduleInfo, rect, color, roundRadius, borderColor, borderWith);
    }

    ///	Fill a text
	/// @param rect The rectangle where the text is drawn in normalized coordinates (0..1).
	/// @param text The text to draw
	/// @param textColor The color of the text
	/// @param fontSize The size of the text in normalized coordinates (0..1).
	/// @param isBold If TRUE, the text is drawn in bold.
	/// @param isWordWrap If TRUE, the text is wrapped to fit the rectangle.
	/// @param alignHorizontal The horizontal alignment of the text @see TTextAlign
	/// @param alignVertical The vertical alignment of the text @see TTextAlign
	/// @param isVertical If TRUE, the text is drawn vertically.
	/// @note The text is drawn with the current font, you can change it with sdkSetFont.
    void sdkFillText(const TRectF rect, const AnsiCharPtr text, const TUsineColor textColor, const float fontSize,
                     const LongBool isBold = FALSE, const LongBool isWordWrap = FALSE, const TTextAlign alignHorizontal = taCenter,
                     const TTextAlign alignVertical = taCenter, const LongBool isVertical = FALSE) const
    {
        m_masterInfo->FillText(m_moduleInfo, rect, text, textColor, fontSize, isBold, isWordWrap, alignHorizontal,
                               alignVertical, isVertical);
    }

    /// Draw an array of pixels.
    /// @param pArray pointer to the first element of the pixels array
    /// @param W width of the pixels array
    /// @param H height of the pixels array
    /// @param destRect destination rectangle in normalized coordinates (0..1).
    /// No verification of the pixels array size is made so be sure that W and H have the right values.
    void sdkBitBlit(const TUsinePixelPtr pArray, const int W, const int H, const TRectF destRect) const
    {
        m_masterInfo->BitBlit(m_moduleInfo, pArray, W, H, destRect);
    }

  
    /// Tell Usine to start a path sequence.
    /// A path must begin with this function, followed by a sequence of command describing the path structure and finally drawn or filled.
    void sdkDrawPathStart() const
    {
        m_masterInfo->DrawPathClear(m_moduleInfo);
    }

    /// Move the current position inside the path. @see sdkDrawPathStart
	/// @param point The point to move to in normalized coordinates (0..1).
	/// This is the first command to call when starting a path.
    void sdkDrawPathMoveTo(const TPointF point) const
    {
        m_masterInfo->DrawPathMoveTo(m_moduleInfo, point);
    }

    /// Add a line to the path, from the current position to the point provided. @see sdkDrawPathStart
	/// @param point The point to draw the line to in normalized coordinates (0..1).
    void sdkDrawPathLineTo(const TPointF point) const
    {
        m_masterInfo->DrawPathLineTo(m_moduleInfo, point);
    }

    /// Add a quadratic curve to the path, from the current position to the point provided.
    /// The curve don't pass by the control point.
	/// @param control The control point of the curve in normalized coordinates (0..1).
	/// @param point The point to draw the curve to in normalized coordinates (0..1).
	/// The curve is a quadratic Bezier curve, it is defined by the current position, the control point and the end point.
    void sdkDrawPathQuadCurveTo(const TPointF control, const TPointF point) const
    {
        m_masterInfo->DrawPathQuadCurveTo(m_moduleInfo, control, point);
    }

    /// Add a line to close the path, from the current position to the first point of the path.
	/// This is the last command to call when finishing a path.
    void sdkDrawPathClose() const
    {
        m_masterInfo->DrawPathClose(m_moduleInfo);
    }
    
    /// Draw the current path with the color and thickness provided
	/// @param color The color of the path
	/// @param strokeThickness The thickness of the path in pixels.
	/// This function must be called after the path is built with the sdkDrawPath* functions.
    void sdkDrawPathDraw(const TUsineColor color, const float strokeThickness) const
    {
        m_masterInfo->DrawPathDraw(m_moduleInfo, color, strokeThickness);
    }

    /// Fill a valid path with a color
	/// @param color The color to fill the path with
	/// This function must be called after the path is built with the sdkDrawPath* functions.
    void sdkDrawPathFill(const TUsineColor color) const
    {
        m_masterInfo->DrawPathFill(m_moduleInfo, color);
    }

    /// Ask Usine to repaint the module Panel.
    /// The result is a call to the onPaint callback.
	/// This is useful when you need to update the module canvas after some changes.
    void sdkRepaintPanel() const
    {
        if ((m_masterInfo != nullptr) && (m_moduleInfo != nullptr)) { m_masterInfo->RepaintPanel(m_moduleInfo); }
    }
    /// @}

    //-----------------------------------------------------------------------------
    // Automation Recording functions
    //-----------------------------------------------------------------------------
    /// To record some values for the Automation Recording system.
    /// @name Automation Recording
    /// @{

    /// Stops the recording
    void sdkStopRecordAutomation() const
    {
        m_masterInfo->StopRecordAutomation(m_moduleInfo);
    }

    /// Start the recording process if it is not started and processes the recording.
	/// to stop the recording use sdkStopRecord
    void sdkProcessRecordAutomation(const TPrecision X, const TPrecision Y, const TPrecision Z) const
    {
        m_masterInfo->ProcessRecordAutomation(m_moduleInfo, X, Y, Z);
    }
    /// @}

    //----------------------------------------------------------------------------
    /// Use them in onCreateSettings to add some entries in the Settings Panel.
    /// @name Settings options
    /// @{

    /// To add a text entry in a Tab of the Settings Panel.
	/// @param tab The name of the Tab where the entry will be added. @ref SettingsPanelTabs
	/// @param caption The caption of the entry.
	/// @param color The color of the caption, default 0 means default color.
	/// @param Translate If TRUE, the caption is translated using the Usine translation system.
    /// @ref TTextAlign
    void sdkAddSettingLineCaption(const AnsiCharPtr tab, const AnsiCharPtr caption = "", const TUsineColor color = 0,
                                  const LongBool Translate = FALSE) const
    {
        m_masterInfo->AddSettingLineCaption(m_moduleInfo, tab, caption, color, Translate);
    }

    /// To add a color selector entry in a Tab of the Settings Panel.
	/// @param tab The name of the Tab where the entry will be added. @ref SettingsPanelTabs
	/// @param pVal Pointer to a TUsineColor variable that will be updated with the selected color.
	/// @param Caption The caption of the entry.
	/// @param Translate If TRUE, the caption is translated using the Usine translation system.
	/// @note The color selector will open a color picker dialog when clicked.
    void sdkAddSettingLineColor(const AnsiCharPtr tab, TUsineColor *pVal, const AnsiCharPtr Caption,
                                const LongBool Translate = FALSE) const
    {
        m_masterInfo->AddSettingLineColor(m_moduleInfo, tab, pVal, Caption, Translate);
    }

    /// To add a boolean entry in a Tab of the Settings Panel.
	/// @param tab The name of the Tab where the entry will be added. @ref SettingsPanelTabs
	/// @param pVal Pointer to a LongBool variable that will be updated with the selected value.
	/// @param Caption The caption of the entry.
	/// @param Translate If TRUE, the caption is translated using the Usine translation system.
	/// @note The boolean entry will be displayed as a checkbox.
    void sdkAddSettingLineBoolean(const AnsiCharPtr tab, LongBool *pVal, const AnsiCharPtr Caption,
                                  const LongBool Translate = FALSE) const
    {
        m_masterInfo->AddSettingLineBoolean(m_moduleInfo, tab, pVal, Caption, Translate);
    }

    /// To add an integer entry in a Tab of the Settings Panel.
	/// @param tab The name of the Tab where the entry will be added. @ref SettingsPanelTabs
	/// @param pVal Pointer to an int variable that will be updated with the selected value.
	/// @param Caption The caption of the entry.
	/// @param min The minimum value of the entry.
	/// @param max The maximum value of the entry.
	/// @param scale The scale of the entry, @see TScale
	/// @param symbol The symbol to display after the value.
	/// @param defaultValue The default value of the entry.
	/// @param Translate If TRUE, the caption is translated using the Usine translation system.
	/// @note The integer entry will be displayed as a slider.
    void sdkAddSettingLineInteger(const AnsiCharPtr tab, int *pVal, const AnsiCharPtr Caption, const int min, const int max,
                                  const TScale scale, const AnsiCharPtr symbol, const int defaultValue, const LongBool Translate = FALSE) const
    {
        m_masterInfo->AddSettingLineInteger(m_moduleInfo, tab, pVal, Caption, min, max, scale, symbol, defaultValue, Translate);
    }

    /// To add a float number entry in a Tab of the Settings Panel.
	/// @param tab The name of the Tab where the entry will be added. @ref SettingsPanelTabs
	/// @param pVal Pointer to a float variable that will be updated with the selected value.
	/// @param caption The caption of the entry.
	/// @param min The minimum value of the entry.
	/// @param max The maximum value of the entry.
	/// @param scale The scale of the entry, @see TScale
	/// @param symbol The symbol to display after the value.
	/// @param format The format of the value, for example "%.2f" for two decimal places.
	/// @param defaultValue The default value of the entry.
	/// @param Translate If TRUE, the caption is translated using the Usine translation system.
	/// @note The float number entry will be displayed as a slider.
    void sdkAddSettingLineSingle(const AnsiCharPtr tab, float *pVal, const AnsiCharPtr caption, const float min, const float max,
                                 const TScale scale, const AnsiCharPtr symbol, const AnsiCharPtr format, const float defaultValue,
                                 const LongBool Translate = FALSE) const
    {
        m_masterInfo->AddSettingLineSingle(m_moduleInfo, tab, pVal, caption, min, max, scale, symbol, format,
                                           defaultValue, Translate);
    }

    /// To add a combo box entry in a Tab of the Settings Panel.
	/// @param tab The name of the Tab where the entry will be added. @ref SettingsPanelTabs
	/// @param pVal Pointer to an int variable that will be updated with the selected value.
	/// @param caption The caption of the entry.
	/// @param commaText A comma-separated list of items to display in the combo box.
	/// @param Translate If TRUE, the caption is translated using the Usine translation system.
	/// @note The combo box will display the items in the comma-separated list.
	/// @note The items in the combo box are translated using the Usine translation system if Translate is TRUE.
	/// @param Translate If TRUE, the caption is translated using the Usine translation system.
	/// @note The combo box will display the items in the comma-separated list.
    void sdkAddSettingLineCombobox(const AnsiCharPtr tab, int *pVal, const AnsiCharPtr caption, const AnsiCharPtr commaText,
                                   const LongBool Translate = FALSE) const
    {
        m_masterInfo->AddSettingLineCombobox(m_moduleInfo, tab, pVal, caption, commaText, Translate);
    }

    /// To add a string entry in a Tab of the Settings Panel.
	/// @param tab The name of the Tab where the entry will be added. @ref SettingsPanelTabs
	/// @param pVal Pointer to a AnsiCharPtr variable that will be updated with the selected value.
	/// @param caption The caption of the entry.
	/// @param Translate If TRUE, the caption is translated using the Usine translation system.
	/// @note The string entry will be displayed as a text input field.
    void sdkAddSettingsLineString(const AnsiCharPtr tab, const AnsiCharPtr pVal, const AnsiCharPtr caption,
                                  const LongBool Translate = FALSE) const
    {
        m_masterInfo->AddSettingsLineString(m_moduleInfo, tab, pVal, caption, Translate);
    }

    /// Setter for an existing setting line entry
	/// @param settingName The name of the setting to set.
	/// @param settingEvent Pointer to a UsineEvent that contains the value to set.
	/// @note The settingEvent must be created with sdkCreateEvent.
	void sdkSetSettingValue(const AnsiCharPtr settingName, const UsineEventPtr settingEvent) const
    {
        m_masterInfo->SetSettingValue(m_moduleInfo, settingName, settingEvent);
    }

    /// Getter for an existing setting line entry
	/// @param settingName The name of the setting to get.
	/// @return A UsineEventPtr that contains the value of the setting.
    UsineEventPtr sdkGetSettingValue(const AnsiCharPtr settingName) const
    {
        UsineEventPtr settingEvent = nullptr;
        m_masterInfo->GetSettingValue(m_moduleInfo, settingName, settingEvent);
        return settingEvent;
    }

    /// Sets a callback id for the last setting created
    /// When the id is set, any modification of the setting value, will be reported in the OnCallback()
    /// @param id The id of the callback (WParam) in the onCallback method.
    void sdkSetLastSettingCallbackID(NativeInt id)
    {
        m_masterInfo->SetLastSettingCallbackID(m_moduleInfo, id);
    }

	/// Sets the last setting in the outlet mode. 
	/// Determines if the last created setting appears as an inlet, outlet, both or invible.
	/// @param Mode The mode to set, TSettingInOutletMode
    /// @see TSettingInOutletMode
    void sdkSetLastSettingInOutletMode(TSettingInOutletMode Mode)
    {
        m_masterInfo->SetLastSettingInOutletMode(m_moduleInfo, Mode);
    }

    /// @}

    //----------------------------------------------------------------------------
    /// Use them in the onCreateCommands callback to add commands in the module Contextual Menu.
    /// @name Contextual Menus commands
    /// @{

    /// To add a Command entry in the module Contextual Menu.
	/// @param name The name of the command.
	/// @param CallbackId The ID of the callback to call when the command is executed.
	/// @param Translate If TRUE, the name is translated using the Usine translation system.
	/// @note The CallbackId is the ID of the callback that will be called when the command is executed.
    void sdkAddCommand(const AnsiCharPtr name, const NativeInt CallbackId, const LongBool Translate = FALSE) const
    {
        m_masterInfo->AddCommand(m_moduleInfo, name, CallbackId, Translate);
    }

    /// To add a named separator in the module Contextual Menu.
	/// @param name The name of the separator.
	/// @param Translate If TRUE, the name is translated using the Usine translation system.
	/// @note The separator is a visual element that separates commands in the menu.
    void sdkAddCommandSeparator(const AnsiCharPtr name, const LongBool Translate = FALSE) const
    {
        m_masterInfo->AddCommandSeparator(m_moduleInfo, name, Translate);
    }
    /// @}


    //----------------------------------------------------------------------------
    /// Utils functions to help manipulate video frames
    /// @name Video Frame manipulation
    /// @{

    /// Get incoming video Frames
	/// @param numInput The index of the input frame to get.
	/// @param frame ptr to a @ref TUsineFrame
	/// @note The frame must be created with sdkGetNewFrame before calling this function.
    void sdkGetInputFrame(const int numInput, const TUsineFramePtr frame) const
    {
        m_masterInfo->GetInputFrame(m_moduleInfo, numInput, frame);
    }
    
    /// Set outgoing video Frames
	/// @param numOutput The index of the output frame to set.
	/// @param frame ptr to a @ref TUsineFrame
	/// @note The frame must be created with sdkGetNewFrame before calling this function.
    void sdkSetOutputFrame(const int numOutput, const TUsineFramePtr frame) const
    {
        m_masterInfo->SetOutputFrame(m_moduleInfo, numOutput, frame);
    }

    /// Clear, fill a frame with a color
	/// @param frame ptr to a @ref TUsineFrame
	/// @param color The color to fill the frame with
	/// @note The frame must be created with sdkGetNewFrame before calling this function.
    void sdkClearFrame(const TUsineFramePtr frame, const TUsineColor color) const
    {
        m_masterInfo->ClearFrame(m_moduleInfo, color, frame);
    }

    /// Get a new frame
    /// @param frame ptr to a @ref TUsineFrame
    /// @param width width in pixels
    /// @param height height in pixels
    /// @param permanent Determines if the frame has a short lifespan (FALSE) or an infinite (TRUE)
    ///
    /// Permanent frames must be destroyed by sdkReleaseFrame
    ///
    /// Non permanent frames are released automatically
    ///@see sdkReleaseFrame
    void sdkGetNewFrame(const TUsineFramePtr frame, const int width, const int height, const LongBool permanent) const
    {
        m_masterInfo->GetNewFrame(m_moduleInfo, width, height, permanent, frame);
    }

    /// Release permanent frames
	/// @param frame ptr to a @ref TUsineFrame
    void sdkReleaseFrame(const TUsineFramePtr frame) const
    {
    m_masterInfo->ReleaseFrame(m_moduleInfo, frame);
    }

    /// Copy a frame into another
	/// @param srcFrame The source frame to copy from
	/// @param destFrame The destination frame to copy to
    static void sdkCopyFrame(const TUsineFramePtr srcFrame, const TUsineFramePtr destFrame)
    {
        m_masterInfo->CopyFrame(srcFrame, destFrame);
    }

    /// Apply a dimmer in the range [0..1] to a frame
	/// @param frame ptr to a @ref TUsineFrame
	/// @param dimmer The dimmer value in the range [0..1]. 0 means no dimming, 1 means full dimming.
    void sdkSetDimmerFrame(const TUsineFramePtr frame, const float dimmer) const
    {
        m_masterInfo->SetDimmerFrame(m_moduleInfo, dimmer, frame);
    }

    /// Set a pixel at position X,Y in the frame
	/// @param frame ptr to a @ref TUsineFrame
	/// @param X The X coordinate of the pixel in the frame
	/// @param Y The Y coordinate of the pixel in the frame
	/// @param pixel The pixel to set at position X,Y
	/// @note The pixel is a TUsinePixel structure that contains the color and alpha values.
    static void sdkSetFramePixel(const TUsineFramePtr frame, const int X, const int Y, const TUsinePixel pixel)
    {
        TUsinePixelPtr pix = frame->Pixels;
        pix = pix + frame->Width * Y + X;
        *pix = pixel;
    }

    /// Get a pixel at position X,Y in the frame
	/// @param frame ptr to a @ref TUsineFrame
	/// @param X The X coordinate of the pixel in the frame
	/// @param Y The Y coordinate of the pixel in the frame
	/// @return The pixel at position X,Y in the frame
    static TUsinePixel sdkGetFramePixel(const TUsineFramePtr frame, const int X, const int Y)
    {
        TUsinePixelPtr pix = frame->Pixels;
        pix = pix + frame->Width * Y + X;
        return *pix;
    }


    /// @}

    //----------------------------------------------------------------------------
    /// Critical sections functions to protect patches calculation.
    /// @name Critical Sections
    /// @{

    /// Lock the execution of the current patch to prevent memory access conflicts between the audio thread and other threads like IDLE, video, etc.
	/// This is useful when you need to access shared resources in a thread-safe manner.
    void sdkLockPatch() const
    {
        m_masterInfo->LockPatch(m_moduleInfo);
    }

    /// UnLock the execution of the current patch.
	/// This must be called after the sdkLockPatch to allow other threads to access the patch.
    void sdkUnLockPatch() const
    {
        m_masterInfo->UnLockPatch(m_moduleInfo);
    }
    /// Lock the execution of the main audio engine of Usine.
	/// This is useful when you need to access shared resources in a thread-safe manner.
    void sdkLockUsineEngine() const
    {
        m_masterInfo->LockPatch(m_moduleInfo);
    }

    /// UnLock the execution of the current patch.
	/// This must be called after the sdkLockUsineEngine to allow other threads to access all the patches.
    void sdkUnLockUsineEngine() const
    {
        m_masterInfo->UnLockPatch(m_moduleInfo);
    }


    /// Lock a critical section.
    /// @param pCriticalSection
    /// @param timeout must be used with precautions. O means no timeout.
	/// If the timeout is reached, the function will return without locking the critical section.
	/// @note This is a blocking call, it will wait until the critical section is available.
    void sdkCriticalSectionLock(const TCriticalSectionPtr pCriticalSection, const UINT32 timeout = 0) const
    {
        m_masterInfo->CriticalSectionLock(m_moduleInfo, pCriticalSection, timeout);
    }

    /// TryLock a critical section.
	/// @param pCriticalSection
	/// @return TRUE if the critical section is locked, FALSE if it is already locked by another thread.
	/// @note This is a non-blocking call, it will return immediately.
    LongBool sdkCriticalSectionTryLock(const TCriticalSectionPtr pCriticalSection) const
    {
        return m_masterInfo->CriticalSectionTryLock(m_moduleInfo, pCriticalSection);
    }

    /// UnLock a critical section.
	/// @param pCriticalSection
	/// @note This must be called after the sdkCriticalSectionLock to allow other threads to access the critical section.
    void sdkCriticalSectionUnLock(const TCriticalSectionPtr pCriticalSection) const
    {
        m_masterInfo->CriticalSectionUnLock(m_moduleInfo, pCriticalSection);
    }


    /// Create a critical section.
    /// @param name is a name givent to the CS for debug purposes
    /// @param shared indicates whether the function is meant to be shared by all modules or not
	/// @return a pointer to the created critical section.
	/// This is useful when you need to protect shared resources in a thread-safe manner.
    TCriticalSectionPtr sdkCriticalSectionCreate(const AnsiCharPtr name, const LongBool shared = FALSE) const
    {
        return m_masterInfo->CriticalSectionCreate(m_moduleInfo, name, shared);
    }

	/// Destroy a critical section created by sdkCriticalSectionCreate.
	/// @param pCriticalSection pointer to the critical section to destroy
	void sdkCriticalSectionDestroy(const TCriticalSectionPtr pCriticalSection) const
    {
        m_masterInfo->CriticalSectionDestroy(m_moduleInfo, pCriticalSection);
    }

    /// @}


    //----------------------------------------------------------------------------
    /// Threads for multi-threading
    /// @name Threads
    /// @{

    /// Create a thread.
    /// @param name thread name
    /// @param ProcessThreadProc function pointer to bind with the thread
    /// @param priority priority of the thread : tpIDLE,tpLow,tpMedium,tpHigh
    /// @param timeout timeout of the thread (execution clock) in ms. if timeout is UINT32_MAX then the thread will wait until the sdkThreadRestart is called
	/// @return a pointer to the created thread.
	/// This is useful when you need to run a process in a separate thread.
    TThreadPtr sdkThreadCreate(const AnsiCharPtr name, const FuncThreadProcess ProcessThreadProc, const TThreadPriority priority,
                               const UINT32 timeout) const
    {
        return m_masterInfo->ThreadCreate(m_moduleInfo, name, ProcessThreadProc, priority, timeout);
    }

    /// Destroy a thread.
	/// @param pThread the thread to destroy
	static void sdkThreadDestroy(const TThreadPtr pThread)
    {
        m_masterInfo->ThreadDestroy(pThread);
    }

    /// restart a thread.
	/// @param pThread the thread to restart
	/// @note The thread must have been created with sdkThreadCreate.
    static void sdkThreadRestart(const TThreadPtr pThread)
    {
        m_masterInfo->ThreadRestart(pThread);
    }

    /// set timeout of a thread.
    /// @param pThread the thread
    /// @param timeout timeout of the thread (execution clock) in ms
	/// @note If timeout is UINT32_MAX then the thread will wait until the sdkThreadRestart is called
    static void sdkThreadSetTimeOut(const TThreadPtr pThread, const UINT32 timeout)
    {
        m_masterInfo->ThreadSetTimeOut(pThread, timeout);
    }

    /// @}
    
    //----------------------------------------------------------------------------
    /// Utils functions to update some Parameter property other than the Event data.
    /// @name Parameters setters
	/// @{

    /// If the parameter is of type ptListBox, change the comma text defining the list content. @see onGetParamInfo;
    /// @param numParam the param index.
    /// @param commaText the comma text
	/// @note The comma text is a string containing the items of the list separated by commas.
    void sdkSetListBoxCommaText(const int numParam, const AnsiCharPtr commaText) const
    {
        m_masterInfo->SetListBoxCommaText(m_moduleInfo, numParam, commaText);
    }

    /// Change the name of a parameter. It's the name showed in Usine.
    /// @param numParam the param index.
    /// @param caption the parameter's caption
	/// @note The caption is the name of the parameter that is displayed in Usine.
    void sdkSetParamCaption(const int numParam, const AnsiCharPtr caption) const
    {
        m_masterInfo->SetParamCaption(m_moduleInfo, numParam, caption);
    }

    /// Set a parameter visible or not.
    /// @param numParam the param index.
    /// @param visible is visible
	/// @note If the parameter is not visible, it will not be displayed in Usine.
    void sdkSetParamVisible(const int numParam, const LongBool visible) const
    {
        m_masterInfo->SetParamVisible(m_moduleInfo, numParam, visible);
    }

    /// Recreate a parameter with new settings.
    /// @param numParam the param index.
    /// @param pParamInfo pointer to TParamInfo structure
	/// @note This function is used to change the type of a parameter or its properties.
    void sdkRecreateParam(const int numParam, TParamInfo *pParamInfo) const
    {
        m_masterInfo->RecreateParam(m_moduleInfo, numParam, pParamInfo);
    }

    /// If the parameter is of type ptText, change the text content. @see onGetParamInfo;
    /// @param numParam the param index.
    /// @param valueText the value string
	/// @note The valueText is the text that is displayed in the text field parameter.
    void sdkSetParamValueText(const int numParam, const AnsiCharPtr valueText) const
    {
        m_masterInfo->SetParamValueText(m_moduleInfo, numParam, valueText);
    }

    /// Ask Usine to Update the parameter.
    /// Usually used after one of the function above.
    /// @param numParam the param index.
	/// @note This function is used to notify Usine that the parameter has changed and needs to be updated.
    void sdkRepaintParam(const int numParam) const
    {
        m_masterInfo->RepaintParam(m_moduleInfo, numParam);
    }
    /// @}

    //----------------------------------------------------------------------------
    /// @name Usine Objects functions
    /// @{

    /// Set the float value of an Usine-Object
	/// @param name The name of the Usine-Object
	/// @param floatValue The float value to set
    void sdkUsineObjectSetFloat(const AnsiCharPtr name, const TPrecision floatValue) const
    {
        m_masterInfo->UsineObjectSetFloat(m_moduleInfo, name, floatValue);
    }

    /// Set the integer value of an Usine-Object
	/// @param name The name of the Usine-Object
	/// @param integerValue The integer value to set
    void sdkUsineObjectSetInteger(const AnsiCharPtr name, const int integerValue) const
    {
        m_masterInfo->UsineObjectSetInteger(m_moduleInfo, name, integerValue);
    }

    /// Set the float value of an Usine-Object array at the index position
	/// @param name The name of the Usine-Object
	/// @param index The index position in the array
	/// @param floatValue The float value to set
    void sdkUsineObjectSetArray(const AnsiCharPtr name, const int index, const TPrecision floatValue) const
    {
        m_masterInfo->UsineObjectSetArray(m_moduleInfo, name, index, floatValue);
    }

    /// Set the Midi value of an Usine-Object array at the index position
	/// @param name The name of the Usine-Object
	/// @param index The index position in the array
	/// @param midiValue The Midi value to set
    void sdkUsineObjectSetMIDI(const AnsiCharPtr name, const int index, const TUsineMidiCode midiValue) const
    {
        m_masterInfo->UsineObjectSetMIDI(m_moduleInfo, name, index, midiValue);
    }

    /// Set the color value of an Usine-Object
	/// @param name The name of the Usine-Object
	/// @param colorValue The color value to set
	/// @note The colorValue is a TUsineColor structure that contains the color and alpha values.
    void sdkUsineObjectSetColor(const AnsiCharPtr name, const TUsineColor colorValue) const
    {
        m_masterInfo->UsineObjectSetColor(m_moduleInfo, name, colorValue);
    }

    /// Set the color value of an Usine-Object array at the index position
	/// @param name The name of the Usine-Object
	/// @param index The index position in the array
	/// @param colorValue The color value to set
    void sdkUsineObjectSetArrayColor(const AnsiCharPtr name, const int index, const TUsineColor colorValue) const
    {
        m_masterInfo->UsineObjectSetArrayColor(m_moduleInfo, name, index, colorValue);
    }

    /// Set a trigger value of an Usine-Object
	/// @param name The name of the Usine-Object
	void sdkUsineObjectSetTrigger(const AnsiCharPtr name) const
    {
        m_masterInfo->UsineObjectSetTrigger(m_moduleInfo, name);
    }

    /// Set the text value of an Usine-Object
    /// @param name The name of the Usine-Object
	/// @param v The text value to set
    void sdkUsineObjectSetAnsiChar(const AnsiCharPtr name, const AnsiCharPtr v) const
    {
        m_masterInfo->UsineObjectSetAnsiChar(m_moduleInfo, name, v);
    }

    /// Get the float value of an Usine-Object
	/// @param name The name of the Usine-Object
	/// @return The float value of the Usine-Object
    TPrecision sdkUsineObjectGetFloat(const AnsiCharPtr name) const
    {
        return m_masterInfo->UsineObjectGetFloat(m_moduleInfo, name);
    }

    /// Get the integer value of an Usine-Object
	/// @param name The name of the Usine-Object
	/// @return The integer value of the Usine-Object
    int sdkUsineObjectGetInteger(const AnsiCharPtr name) const
    {
        return m_masterInfo->UsineObjectGetInteger(m_moduleInfo, name);
    }

    /// Get the float value of an Usine-Object array at the index position
	/// @param name The name of the Usine-Object
	/// @param index The index position in the array
    TPrecision sdkUsineObjectGetArray(const AnsiCharPtr name, const int index) const
    {
        return m_masterInfo->UsineObjectGetArray(m_moduleInfo, name, index);
    }

    /// Get the Midi value of an Usine-Object array at the index position
	/// @param name The name of the Usine-Object
	/// @param index The index position in the array
    TUsineMidiCode sdkUsineObjectGetMIDI(const AnsiCharPtr name, const int index) const
    {
        return m_masterInfo->UsineObjectGetMIDI(m_moduleInfo, name, index);
    }

    /// Get the color value of an Usine-Object
	/// @param name The name of the Usine-Object
	/// @return The color value of the Usine-Object
    TUsineColor sdkUsineObjectGetColor(const AnsiCharPtr name) const
    {
        return m_masterInfo->UsineObjectGetColor(m_moduleInfo, name);
    }

    /// Get the array value of an Usine-Object array at the index position
	/// @param name The name of the Usine-Object
	/// @param index The index position in the array
    TUsineColor sdkUsineObjectGetArrayColor(const AnsiCharPtr name, const int index) const
    {
        return m_masterInfo->UsineObjectGetArrayColor(m_moduleInfo, name, index);
    }

    /// Get the text value of an Usine-Object
	/// @param name The name of the Usine-Object
	/// @return The text value of the Usine-Object
    AnsiCharPtr sdkUsineObjectGetAnsiChar(const AnsiCharPtr name) const
    {
        return m_masterInfo->UsineObjectGetAnsiChar(m_moduleInfo, name);
    }

    /// Get the size of the Event associated to an Usine-Object
	/// @param name The name of the Usine-Object
	/// @return The size of the Event associated to the Usine-Object
    int sdkUsineObjectGetLength(const AnsiCharPtr name) const
    {
        return m_masterInfo->UsineObjectGetLength(m_moduleInfo, name);
    }

    /// Set the size of the Event associated to an Usine-Object
	/// @param name The name of the Usine-Object
	/// @param l The length to set
    void sdkUsineObjectSetLength(const AnsiCharPtr name, const int l) const
    {
        m_masterInfo->UsineObjectSetLength(m_moduleInfo, name, l);
    }

    /// Returns the list (as a comma-text) of all Usine-Objects that match with the name parameter
	/// @param name The name of the Usine-Object to search for
	/// @return A comma-separated list of Usine-Objects names that match the search criteria.
    AnsiCharPtr sdkUsineObjectFind(const AnsiCharPtr name) const
    {
        return m_masterInfo->UsineObjectFind(m_moduleInfo, name);
    }

    /// Returns TRUE if the Usine-Objects exists
	/// @param name The name of the Usine-Object to check
	/// @return TRUE if the Usine-Object exists, FALSE otherwise.
    LongBool sdkUsineObjectExists(const AnsiCharPtr name) const
    {
        return m_masterInfo->UsineObjectExists(m_moduleInfo, name);
    }

    /// Returns the Hash of all	Usine-Objects. This allows to determines if the list of all the Usine-Objects has changed.
	/// @return A string containing the hash of all Usine-Objects.
    AnsiCharPtr sdkUsineObjectListHash() const
    {
        return m_masterInfo->UsineObjectListHash(m_moduleInfo);
    }

    /// @}

    //----------------------------------------------------------------------------
    /// @name Usine Core infos and services
    /// @{

    /// Usine internal messages
	/// Send an internal message to Usine, Usine's setup or this user module.
	/// @param Msg The message to send.
    void sdkSendUsineMsg(const AnsiCharPtr Msg) const
    {
        m_masterInfo->SendUsineMsg(m_moduleInfo, Msg);
    }

    /// Usine notification
	///
	/// Send a message to Usine, Usine's setup or this user module.
	/// A delay maybe specified.
	/// @param Target The target of the message, can be a module or Usine's setup.
	/// @param Msg The message to send.
	/// @param WParam The first parameter of the message, can be used to pass additional information.
	/// @param LParam The second parameter of the message, can be used to pass additional information.
	/// @param Delay The delay in milliseconds before the message is sent.
    void sdkNotifyUsine(const NativeInt Target, const NativeInt Msg, const NativeInt WParam = 0, const NativeInt LParam = 0,
                        const TNotifyUsineMode CallbackMode = TNotifyUsineMode::numRealTime, const int Delay = 0) const
    {
        m_masterInfo->NotifyUsine(m_moduleInfo, Target, Msg, WParam, LParam, CallbackMode, Delay);
    }

    void sdkNotifyUsineOld(const NativeInt Target, const NativeInt Msg, const NativeInt WParam = 0, const NativeInt LParam = 0,
        const int Delay = 0) const
    {
        m_masterInfo->NotifyUsineOld(m_moduleInfo, Target, Msg, WParam, LParam, Delay);
    }

    /// To access the current TVstTimeInfo structure of Usine.
	/// This structure contains the current time information of Usine, such as the current position in the song, tempo, etc.
    TVstTimeInfo *sdkGetVstTimeInfo() const
    {
        return m_masterInfo->GetVstTimeInfo(m_moduleInfo);
    }

    /// To get the Usine global time in millisecond, precision up to nanoseconde
	/// @return The current time in milliseconds.
    double sdkGetTimeMs() const
    {
        return m_masterInfo->GetTimeMs(m_moduleInfo);
    }

    /// To check if the patch containing the module is running.
	/// This is useful to determine if the patch is currently active and processing.
	/// @return TRUE if the patch is running, FALSE otherwise.
    LongBool sdkPatchIsRunning() const
    {
        return m_masterInfo->PatchIsRunning(m_moduleInfo);
    }

    /// To check if the patch containing the module is loading.
	/// This is useful to determine if the patch is currently being loaded.
	/// @return TRUE if the patch is loading, FALSE otherwise.
    LongBool sdkPatchLoading() const
    {
        return m_masterInfo->PatchLoading(m_moduleInfo);
    }

    /// To check if the patch has just started.
	/// This is useful to determine if the patch has just been activated.
	/// @return TRUE if the patch has just started, FALSE otherwise.
    LongBool sdkPatchJustActivated() const
    {
        return m_masterInfo->PatchJustActivated(m_moduleInfo);
    }

    /// Gets a tick clock at the refresh-speed of Usine, generally every 25 ms.
    /// this can reduce the cpu load for non priority tasks
    ///
    ///  if (sdkRefreshSpeedClock()) { doSomething(); }
    ///
    LongBool sdkRefreshSpeedClock() const
    {
        return m_masterInfo->RefreshSpeedClock(m_moduleInfo);
    }

    /// Gets a tick clock at the slow-speed of Usine, generally every second.
    /// this can reduce the cpu load for very low priority tasks
    ///
    ///  if (sdkSlowClock()) { doSomething(); }
    ///
    LongBool sdkSlowClock() const
    {
        return m_masterInfo->SlowClock(m_moduleInfo);
    }
	
    /// show the progress bar of Usine
	/// @param caption the caption of the progress bar
	/// @param color the color of the progress bar 
	/// @param progress the progress value in percent [0..100]. The default 0 means no progress gauge.
	void sdkProgressBar(const AnsiCharPtr caption, const TUsineColor color, const int progress=0) const
	{
		m_masterInfo->ProgressBar(caption, color, progress);
	}

	// try to locate a file using Usine's file locator
	/// @param filename the name of the file to locate
	/// @param disableDialog if TRUE, the file dialog is disabled and only the internal search is performed
	/// @return the located file path or the same filename if not found
    AnsiCharPtr sdkLocalizeFileName(AnsiCharPtr filename, LongBool disableDialog)
    {
        return m_masterInfo->LocateFile(m_moduleInfo,filename,disableDialog);
    }



    //-----------------------------------------------------------------------------
    // wrappers for TModuleInfo functions
    //-----------------------------------------------------------------------------
    /// To get the pointer of the Usine module. Do not use directly.
    void *sdkGetUsineUserModule() const
    {
        return m_moduleInfo->UsineUserModule;
    }

    /// To get the version number of Usine as a string.
	/// @return A string containing the version number of Usine.
    static AnsiCharPtr sdkGetUsineVersionNum()
    {
        return m_masterInfo->UsineVersion;
    }

    /// set the name of the module (as it appear in the patch, on the module's caption)
	/// @param user name The name of the module to set.
    void sdkSetModuleUserName(const AnsiCharPtr name) const
    {
        m_masterInfo->SetModuleUserName(m_moduleInfo, name);
    }

    /// get the name of the module (as it appear in the patch, on the module title)
	/// @return A string containing the user name of the module.
    AnsiCharPtr sdkGetModuleUserName() const
    {
        return m_masterInfo->GetModuleUserName(m_moduleInfo);
    }
    /// @}

    //----------------------------------------------------------------------------
    /// @name Chunks functions
    /// @{

	/// get a chunk by name. if the chunk doesn't exist, it is created. So this function will always return a valid chunk pointer.
	/// @param chunkname The name of the chunk to get.
	/// @return A pointer to the chunk. @ref TUsineChunk
    [[maybe_unused]] inline TUsineChunkPtr sdkGetChunk(AnsiCharPtr chunkname) const
    {
        return m_masterInfo->GetChunk(m_moduleInfo, chunkname);
    }

	// Get the number of items in a chunk.
	// @param chunk The chunk to get the item count from obtained by @ref sdkGetChunk
	// @return The number of items in the chunk.
    [[maybe_unused]] inline int sdkGetChunkItemCount(TUsineChunkPtr chunk) const
    {
        return m_masterInfo->GetChunkItemCount(m_moduleInfo, chunk);
    }

    
    /// Performs a chunk operation.
	/// @param chunk The chunk to operate on obtained by @ref sdkGetChunk
	/// @param itemName The name of the item to operate on within the chunk. Can be nullptr for operations that don't require an item name.
	/// @param flowType The flow type of the chunk operation. @ref TFlowType
	/// @param payload A pointer to a UsineEvent that contains the data for the operation. Can be nullptr for operations that don't require a payload.
	/// @param operation The type of operation to perform on the chunk. @ref TChunkOperation
	/// @param index The index position for operations that require it, such as inserting or deleting items in a list. Default is -1 for all indexes.
	/// @note The payload must be created with sdkCreateEvent before calling this function or can be @ref UsineEventClass.
    [[maybe_unused]] inline void sdkChunkOperation (TUsineChunkPtr chunk, AnsiCharPtr itemName, TFlowType flowType, UsineEventPtr payload, TChunkOperation operation, int index = -1) const
    {
		m_masterInfo->ChunkOperation(m_moduleInfo, chunk, itemName, flowType, payload, operation, index);
    }

	/// Subscribe to a chunk to receive notifications when the chunk changes in the Callback procedure
    /// Subscribing to a chunk allows the module to be notified of changes to the chunk's data, enabling dynamic and responsive behavior.
	/// @param chunk The chunk to subscribe to obtained by @ref sdkGetChunk
	/// @param itemName The name of the item to subscribe to within the chunk. Can be nullptr to subscribe to all items in the chunk.
	/// @param callbackID The ID of the callback to be used when the chunk changes. This ID will be passed as WParam in the OnCallback method.
	/// @param immediate If TRUE, the callback is called immediately after the change occurs. When FALSE the callback is queued in the messages queue of Usine with typically a delay of 25ms
	/// @note LParam in the OnCallback method contains the following notification:
	/// - MSG_CHANGE : the chunk or chunk item has changed
	/// - MSG_CHUNK_CLEARED : the chunk has been cleared
	/// - MSG_CHUNK_DELETED : the chunk has been deleted
	    [[maybe_unused]] inline void sdkChunkSubscribe(TUsineChunkPtr chunk, AnsiCharPtr itemName, NativeUInt callbackID, LongBool immediate) const
    {
        m_masterInfo->ChunkSubscribe(m_moduleInfo, chunk, itemName, callbackID, immediate);
	}

	/// Unsubscribe from a chunk to stop receiving notifications when the chunk changes.
	/// @param chunk The chunk to unsubscribe from obtained by @ref sdkGetChunk
	/// @param itemName The name of the item to unsubscribe from within the chunk. Can be nullptr to unsubscribe from all items in the chunk.
	/// @note After calling this function, the module will no longer receive notifications for changes to the specified chunk or item.
    [[maybe_unused]] inline void sdkChunkUnSubscribe(TUsineChunkPtr chunk, AnsiCharPtr itemName) const
    {
        m_masterInfo->ChunkUnSubscribe(m_moduleInfo, chunk, itemName);
    }

	/// Unsubscribe from all chunks to stop receiving any notifications.
	/// @note After calling this function, the module will no longer receive any notifications for changes to any chunks.
    [[maybe_unused]] inline void sdkChunkUnSubscribeAll() const
    {
        m_masterInfo->ChunkUnSubscribeAll(m_moduleInfo);
	}

    /// @}



    //----------------------------------------------------------------------------
    /// A User module can create and manage an independent child window for it's own use.
    /// Note that only one child window at a time per module is possible.
    /// @note Those functions are not implemented.
	/// @see Child Window Callbacks
	/// @name Module child window
	/// @{

    void *sdkCreateDeskWindow() const
    {
        return m_masterInfo->CreateDeskWindow(m_moduleInfo);
    }

    void sdkDestroyDeskWindow() const
    {
        m_masterInfo->DestroyDeskWindow(m_moduleInfo);
    }

    void *sdkShowDeskWindow() const
    {
        return m_masterInfo->ShowDeskWindow(m_moduleInfo);
    }

    void sdkHideDeskWindow() const
    {
        m_masterInfo->HideDeskWindow(m_moduleInfo);
    }

    void sdkSetDeskWindowClientSize(const int clientWidth, const int clientHeight) const
    {
        m_masterInfo->SetDeskWindowClientSize(m_moduleInfo, clientWidth, clientHeight);
    }

    void sdkSetDeskWindowCaption(TModuleInfo *pModuleInfo, const AnsiCharPtr name) const
    {
        m_masterInfo->SetDeskWindowCaption(m_moduleInfo, name);
    }
    /// @}

    
    //----------------------------------------------------------------------------
    /// Useful to convert between pixel coord and graphic canvas unit.
	/// @name Converter utils
	/// @{

    /// Transform the Y position in pixels into [0..1]
	/// @param pixel The Y position in pixels.
	/// @return The Y position in [0..1] percent.
    float sdkPixelToHeightPercent(const float pixel) const
    {
        float result = 0.0f;
        if (panelHeight > 0.0f)
            result = pixel / panelHeight;
        return result;
    }

    /// Transform the X position in pixels into [0..1]
	/// @param pixel The X position in pixels.
	/// @return The X position in [0..1] percent.
    float sdkPixelToWidthPercent(const float pixel) const
    {
        float result = 0.0f;
        if (panelHeight > 0.0f)
            result = pixel / panelWidth;
        return result;
    }

    /// Transform [0..1] percent Y position into the position in pixels
	/// @param percent The Y position in [0..1] percent.
	/// @return The Y position in pixels.
    float sdkHeightPercentToPixel(const float percent) const
    {
        float result = 0.0f;
        if (panelHeight > 0.0f)
            result = panelHeight * percent;
        return result;
    }

    /// Transform [0..1] percent X position into the position in pixels
	/// @param percent The X position in [0..1] percent.
	/// @return The X position in pixels.
    float sdkWidthPercentToPixel(const float percent) const
    {
        float result = 0.0f;
        if (panelHeight > 0.0f)
            result = panelWidth * percent;
        return result;
    }
    /// @}



    //------------------------------------------------------------------------
    // TMasterInfo
    //------------------------------------------------------------------------

    static void sdkSetMasterInfo(TMasterInfo *masterInfo) { m_masterInfo = masterInfo; }
    static const TMasterInfo *sdkGetMasterInfo() { return m_masterInfo; }
    // Pointer to TMasterInfo provided by Usine.
    static TMasterInfo *m_masterInfo;

    //-------------------------------------------------------------------------
    // protected members
    //-------------------------------------------------------------------------
protected:
    
    float panelWidth; // in pixel
    float panelHeight; // in pixel

    // Pointer TModuleInfo provided by usine
    TModuleInfo* m_moduleInfo;

private:
    //-----------------------------------------------------------------------------
    // private methods
    //-----------------------------------------------------------------------------
    // declare non copyable
    UserModuleBase(const UserModuleBase &) = delete;

    // This section is specific to manage asynchronous jobs
    TThreadPtr m_job_thread = nullptr;
    bool m_taskRunning = false;
    bool m_taskStarted = false;

    static void processThreadJob(void* pModule, TThreadPtr pThread);
    void notifyUsineOnJobEnd() const;
    void terminateJob();

    //UserModuleBase &operator=(const UserModuleBase &);
}; // class UserModuleBase

//----------------------------------------------------------------------------
///@file  
///	UserModule.cpp
//
///@author
///	BrainModular team
///
///@brief  
///	Implementation of the abstract class provided to derive user modules from.
///
///@HISTORIC 
///	2013/05/15
///    first release for Hollyhock CPP SDK 6.00.226
/// 2023/11/28
///    added a more modern class system to the CPP SDK
///
///@IMPORTANT
///	This file is part of the Usine CPP SDK Version 6
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


//----------------------------------------------------------------------------
// includes
//----------------------------------------------------------------------------
#include "UserModule.h"
#include "UsineEventClass.h"
// constructor
UserModuleBase::UserModuleBase()
    : panelWidth(10)
      , panelHeight(10)
      , m_moduleInfo(nullptr)
{}

TMasterInfo* UserModuleBase::m_masterInfo = nullptr;

// destructor
UserModuleBase::~UserModuleBase() = default;;


//-----------------------------------------------------------------------------
// module initialisation

void UserModuleBase::AfterQueryPopup (TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo, const int queryResult1, const int queryResult2)
{
	InitInfosStructures (pMasterInfo, pModuleInfo);
	pModuleInfo->NumberOfParams = onGetNumberOfParams (queryResult1,queryResult2);
	onAfterQuery (pMasterInfo, pModuleInfo, queryResult1, queryResult2);
}

// IMPORTANT : it's up to you to initialize the user module before further use
void UserModuleBase::Init(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo) 
{ 
	InitInfosStructures (pMasterInfo, pModuleInfo);

	onInitModule (m_masterInfo, m_moduleInfo);
};

// IMPORTANT : it's up to you to initialize the user module before further use
void UserModuleBase::InitInfosStructures(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo)
{
	if (pMasterInfo != nullptr)
	{
		m_masterInfo = pMasterInfo;
	}
	if (pModuleInfo != nullptr)
	{
		m_moduleInfo = pModuleInfo;
	}
    
};

void UserModuleBase::CallBack (TUsineMessage *Message)
{
    try
	{
        // sanity check
        if (Message->wParam < CALLBACK_WPARAM_LIMIT)
            onCallBack (Message);
	}
	catch (...) // (...) or (std::exception& e)
	{
		std::string os;
		os += "CallBack, paramIndex=";
		os += std::to_string(Message->wParam);
        sdkTraceErrorChar ( os.c_str());
	}
};

void UserModuleBase::ResizeModule (float contentWidth, float contentHeight)
{
    panelWidth = contentWidth;
    panelHeight = contentHeight;

    onResize (contentWidth, contentHeight);
}

// C++ interface with Usine implementation
//-----------------------------------------------------------------------------
// create, general info and destroy
int GetSDKVersion(void)
{
	return SDK_VERSION;
}

//
void Create(void* &pModule, AnsiCharPtr optionalString, LongBool Flag, TMasterInfo* pMasterInfo, AnsiCharPtr optionalContent)
{ 
  
    if (pMasterInfo != nullptr)
    {
        UserModuleBase::m_masterInfo = pMasterInfo;
        // TODO : parse pMasterInfo->UsineVersion and sdk version to compare them (major + minor only)
        // TODO : check usine version against sdk version
    }
	sdkTraceLogChar("Create");

	try
	{
	    CreateModule (pModule, optionalString , Flag,  pMasterInfo, optionalContent);
	}
	catch (...)
	{
        // error at module creation
		// give back null pointer to Usine.
		pModule = nullptr;
		// then propagate the exception upstack for usine to handle.
		// On windows, suppress MSVC warning for throwing inside an exported function
#ifdef _WIN32
#pragma warning(suppress: 4297)
		throw;
#else
		throw;
#endif
	}   
}

//
void Destroy(void* pModule)
{ 
	sdkTraceLogChar("Destroy");
	DestroyModule (pModule);
} 
//
void BrowserInfo(TModuleInfo* pModuleInfo)
{ 
	sdkTraceLogChar("BrowserInfo");
	GetBrowserInfo (pModuleInfo);
} 

//
void GetModuleInfo (void* pModule, TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo)
{ 
	sdkTraceLogChar("GetModuleInfo");
	auto* userModule = static_cast <UserModuleBase*>(pModule);
	userModule->InitInfosStructures (pMasterInfo, pModuleInfo);
	userModule->onGetModuleInfo (pMasterInfo, pModuleInfo);
}

//-----------------------------------------------------------------------------
// query system and init

int GetNumberOfParams(void* pModule, int queryResult1, int queryResult2)
{	
	sdkTraceLogChar("GetNumberOfParams");
	auto* userModule = static_cast <UserModuleBase*>(pModule);
	return userModule->onGetNumberOfParams (queryResult1,queryResult2);
}

void AfterQuery (void* pModule, TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo, int queryResult1, int queryResult2)
{	
	auto* userModule = static_cast <UserModuleBase*>(pModule);
	sdkTraceLogChar("AfterQuery");
	userModule->AfterQueryPopup (pMasterInfo, pModuleInfo, queryResult1, queryResult2);
}

void InitModule (void* pModule, TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo)
{	
	auto* userModule = static_cast <UserModuleBase*>(pModule);
	sdkTraceLogChar("InitModule");
	userModule->Init (pMasterInfo, pModuleInfo);
}

//-----------------------------------------------------------------------------
// parameters and process
void GetParamInfo (void* pModule, int ParamIndex, TParamInfo* pParamInfo)
{	
	if (ParamIndex == 0)
	{
		sdkTraceLogChar("GetParamInfo");
	}
	auto* userModule = static_cast <UserModuleBase*>(pModule);
            	userModule->onGetParamInfo (ParamIndex, pParamInfo);
}

void SetEventAddress (void* pModule, int ParamIndex, UsineEventPtr pEvent)
{	
	if (ParamIndex == 0)
	{
		sdkTraceLogChar("SetEventAddress");
	}
	auto* userModule = static_cast <UserModuleBase*>(pModule);
	userModule->onSetEventAddress (ParamIndex, pEvent);
}

void CallBack (void* pModule, TUsineMessage *Message)
{	
	auto* userModule = static_cast <UserModuleBase*>(pModule);
	userModule->CallBack (Message);
}

void Process (void* pModule)
{	
	auto* userModule = static_cast <UserModuleBase*>(pModule);
	userModule->onProcess ();
}

void ProcessVideo(void* pModule)
{
	auto* userModule = static_cast <UserModuleBase*>(pModule);
    userModule->onProcessVideo();
}

//-----------------------------------------------------------------------------
// midi out callbacks
void MidiSendOut (void* pModule, int DeviceID, TUsineMidiCode Code)
{	
	auto* userModule = static_cast <UserModuleBase*>(pModule);
	userModule->onMidiSendOut (DeviceID, Code);
}

void MidiSendOutArray (void* pModule, int DeviceID, TUsineMidiCode** arrayCode, int arraySize)
{	
	auto* userModule = static_cast <UserModuleBase*>(pModule);
	userModule->onMidiSendOutArray (DeviceID, arrayCode, arraySize);
}

void MidiSysexSendOut (void* pModule, int DeviceID,  char** Sysex, int sysexSize)
{	
	auto* userModule = static_cast <UserModuleBase*>(pModule);
	userModule->onMidiSysexSendOut (DeviceID, Sysex, sysexSize);
}

void LaserSendOut(void* pModule, int DeviceID, TUsineILDAPoint** arrayPoint, int arraySize, int speedPPS)
{
	auto* userModule = static_cast <UserModuleBase*>(pModule);
	userModule->onLaserSendOut(DeviceID, arrayPoint, arraySize, speedPPS);
}


void DMXSendOut (void* pModule, int deviceId, char* ByteArray, int len, int universeNum)
{	
	auto* userModule = static_cast <UserModuleBase*>(pModule);
	userModule->onDMXSendOut (deviceId, ByteArray, len, universeNum);
}

//-----------------------------------------------------------------------------
// chunk system
int GetChunkLen (void* pModule, LongBool Preset)
{
	auto* userModule = static_cast <UserModuleBase*>(pModule);
	sdkTraceLogChar("GetChunklen");
	return userModule->onGetChunkLen (Preset);
}

void GetChunk (void* pModule, void* chunk, LongBool Preset)
{
	auto* userModule = static_cast <UserModuleBase*>(pModule);
	sdkTraceLogChar("GetChunk");
	userModule->onGetChunk (chunk, Preset);
}

void SetChunk (void* pModule, const void* chunk, LongBool Preset, int sizeInBytes)
{
	auto* userModule = static_cast <UserModuleBase*>(pModule);
	sdkTraceLogChar("SetChunk");
	userModule->onSetChunk (chunk, sizeInBytes, Preset);
}

void AfterLoading(void* pModule)
{
	sdkTraceLogChar("AfterLoading");
	auto* userModule = static_cast <UserModuleBase*>(pModule);
	userModule->onAfterLoading();
}

//-----------------------------------------------------------------------------
// layout
void CreateSettings(void* pModule)
{
	sdkTraceLogChar("CreateSettings");
	auto* userModule = static_cast <UserModuleBase*>(pModule);
	userModule->onCreateSettings ();
}

void CreateCommands(void* pModule)
{
	sdkTraceLogChar("CreateCommands");
	auto* userModule = static_cast <UserModuleBase*>(pModule);
	userModule->onCreateCommands ();
}

void SettingsHasChanged(void* pModule)
{
	sdkTraceLogChar("SettingsHasChanged");
	auto* userModule = static_cast <UserModuleBase*>(pModule);
	userModule->onSettingsHasChanged ();
}

void Resize (void* pModule, float W, float H) 
{
	auto* userModule = static_cast <UserModuleBase*>(pModule);
	userModule->ResizeModule ( W, H);
}

void Paint (void* pModule)
{
	auto* userModule = static_cast <UserModuleBase*>(pModule);
	userModule->onPaint ();
}


//-----------------------------------------------------------------------------
// mouse and multi touch interaction
void MouseMove(void* pModule, TShiftState Shift, float X, float Y)
{
	auto* userModule = static_cast <UserModuleBase*>(pModule);
	userModule->onMouseMove (Shift, X, Y);
}

void MouseDown(void* pModule, TMouseButton MouseButton, TShiftState Shift, float X,float Y)
{
	auto* userModule = static_cast <UserModuleBase*>(pModule);
	userModule->onMouseDown (MouseButton, Shift, X, Y);
}

void MouseUp (void* pModule,  TMouseButton MouseButton, TShiftState Shift, float X,float Y)
{
	auto* userModule = static_cast <UserModuleBase*>(pModule);
	userModule->onMouseUp (MouseButton, Shift, X, Y);
}

void MouseWheel (void* pModule,  TShiftState Shift, int WheelDelta)
{
	auto* userModule = static_cast <UserModuleBase*>(pModule);
	userModule->onMouseWheel (Shift, WheelDelta);
}

void MouseMoveMulti(void* pModule, TShiftState Shift, UsineEventPtr X, UsineEventPtr Y, UsineEventPtr Pressed)
{
    auto* userModule = static_cast<UserModuleBase*>(pModule);
	UsineEventClass X_AsClass(X);
	UsineEventClass Y_AsClass(Y);
	UsineEventClass P_AsClass(Pressed);

	// Call both overloaded module functions for backwards compatibility with 
	// UsineEventPtr implementations
	userModule->onMouseMoveMulti(Shift, X_AsClass, Y_AsClass, P_AsClass);
	userModule->onMouseMoveMulti (Shift, X, Y, Pressed);
}

void MouseDownMulti(void* pModule, TMouseButton MouseButton, TShiftState Shift, UsineEventPtr X, UsineEventPtr Y, UsineEventPtr Pressed)
{
    auto* userModule = static_cast <UserModuleBase*>(pModule);
    UsineEventClass X_AsClass(X);
    UsineEventClass Y_AsClass(Y);
    UsineEventClass P_AsClass(Pressed);

	// Call both overloaded module functions for backwards compatibility with 
	// UsineEventPtr implementations
	userModule->onMouseDownMulti(MouseButton, Shift, X_AsClass, Y_AsClass, P_AsClass);
	userModule->onMouseDownMulti (MouseButton, Shift, X, Y, Pressed);
}

void MouseUpMulti (void* pModule, const TMouseButton MouseButton, const TShiftState Shift, UsineEventPtr X, UsineEventPtr Y, UsineEventPtr Pressed)
{
	auto* userModule = static_cast <UserModuleBase*>(pModule);
    UsineEventClass X_AsClass(X);
    UsineEventClass Y_AsClass(Y);
    UsineEventClass P_AsClass(Pressed);
	// Call both overloaded module functions for backwards compatibility with 
	// UsineEventPtr implementations
	userModule->onMouseUpMulti(MouseButton, Shift, X_AsClass, Y_AsClass, P_AsClass);
	userModule->onMouseUpMulti (MouseButton, Shift, X, Y, Pressed);
}

void OpenEditor(void* pModule)
{
	sdkTraceLogChar("OpenEditor");
	auto* userModule = static_cast <UserModuleBase*>(pModule);
	userModule->onOpenEditor ();
}

void BringToFront(void* pModule)
{
	sdkTraceLogChar("BringToFront");
	auto* userModule = static_cast <UserModuleBase*>(pModule);
	userModule->onBringToFront ();
}

void CloseEditor(void* pModule)
{
	sdkTraceLogChar("CloseEditor");
	auto* userModule = static_cast <UserModuleBase*>(pModule);
	userModule->onCloseEditor ();
}

void ResizeEditor(void* pModule, int width, int height)
{
	auto* userModule = static_cast <UserModuleBase*>(pModule);
	userModule->onResizeEditor (width, height);
}
//-----------------------------------------------------------------------------
// audio setup update
void OnBlocSizeChange (void* pModule, int BlocSize)
{
	sdkTraceLogChar("OnBlocSizeChange");
	auto* userModule = static_cast <UserModuleBase*>(pModule);
	return userModule->onBlocSizeChange (BlocSize);
}

void OnSampleRateChange (void* pModule, double SampleRate)
{
	sdkTraceLogChar("OnSampleRateChange");
	auto* userModule = static_cast <UserModuleBase*>(pModule);
	userModule->onSampleRateChange (SampleRate);
}

//-----------------------------------------------------------------------------
// recording
void SetRecordedValue (void* pModule, TPrecision X, TPrecision Y, TPrecision Z)
{
	auto* userModule = static_cast <UserModuleBase*>(pModule);
	userModule->onSetRecordedValue (X, Y, Z);
}


//-----------------------------------------------------------------------------
// usine randomize
void Randomize (void* pModule)
{
	sdkTraceLogChar("Randomize");
	auto* userModule = static_cast <UserModuleBase*>(pModule);
	userModule->onRandomize ();
}

//-----------------------------------------------------------------------------
// usine reset
void ResetModule(void* pModule)
{
	auto* userModule = static_cast <UserModuleBase*>(pModule);
	sdkTraceLogChar("ResetModule");
	userModule->onReset();
}


//-----------------------------------------------------------------------------
// usine set quick color
void SetQuickColor(void* pModule, TUsineColor color)
{
	sdkTraceLogChar("SetQuickColor");
	auto* userModule = static_cast <UserModuleBase*>(pModule);
	userModule->onSetQuickColor(color);
}


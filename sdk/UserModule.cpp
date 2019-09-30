//----------------------------------------------------------------------------
//@file  
//	UserModule.cpp
//
//@author
//	Martin FLEURENT aka 'martignasse'
//
//@brief  
//	Implementation of the abstract class provided to derive user modules from.
//
//HISTORIC 
//	2013/05/15
//    first release for Hollyhock CPP SDK 6.00.226 
//
//IMPORTANT
//	This file is part of the Usine CPP SDK Version 6
//
//  Please, report bugs and patch to Usine forum :
//  http://www.sensomusic.org/support/ 
//  
// All dependencies are under there own licence.
//
//@copyright
// Copyright (C) 2013 Sensomusic
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy of 
// this software and associated documentation files (the "Software"), 
// to deal in the Software without restriction, including without limitation 
// the rights to use, copy, modify, merge, publish, distribute, sublicense, 
// and/or sell copies of the Software, and to permit persons to whom the Software 
// is furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all 
//     copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
// SOFTWARE.
//
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
// includes
//----------------------------------------------------------------------------
#include "UserModule.h"

// constructor
UserModuleBase::UserModuleBase()
    : m_masterInfo (nullptr)
    , m_moduleInfo (nullptr)
    , panelWidth (10)
    , panelHeight (10)
	, stringTrace ()
{
}

// destructor
UserModuleBase::~UserModuleBase() {};


//-----------------------------------------------------------------------------
// module initialisation

void UserModuleBase::AfterQueryPopup (MasterInfo* pMasterInfo, ModuleInfo* pModuleInfo, int queryIndex)
{
	InitInfosStructures (pMasterInfo, pModuleInfo);
    
	pModuleInfo->NumberOfParams = onGetNumberOfParams (queryIndex);
	onAfterQuery (pMasterInfo, pModuleInfo, queryIndex);
}

// IMPORTANT : it's up to you to initialize the user module before further use
void UserModuleBase::Init(MasterInfo* pMasterInfo, ModuleInfo* pModuleInfo) 
{ 
	InitInfosStructures (pMasterInfo, pModuleInfo);

	onInitModule (m_masterInfo, m_moduleInfo);

    //stringTrace = std::string ("[") + std::string (m_moduleInfo->Name) + std::string ("]");
};

// IMPORTANT : it's up to you to initialize the user module before further use
void UserModuleBase::InitInfosStructures(MasterInfo* pMasterInfo, ModuleInfo* pModuleInfo)
{
	if (pMasterInfo != nullptr)
	{
		this->m_masterInfo = pMasterInfo;
	}
	if (pModuleInfo != nullptr)
	{
		this->m_moduleInfo = pModuleInfo;
	}
    
};

void UserModuleBase::CallBack (UsineMessage *Message)
{
    try
	{
        // sanity check
        if (Message->wParam < CALLBACK_WPARAM_LIMIT)
            onCallBack (Message);
	}
	catch (...) // (...) or (std::exception& e)
	{
        std::ostringstream os;
        os << stringTrace << "CallBack, paramIndex=" << Message->wParam;
        stringTrace =  os.str();
        sdkTraceErrorChar ( (AnsiCharPtr) stringTrace.c_str());
		//sdkTraceErrorChar(" in CallBack");
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
void Create(void* &pModule, AnsiCharPtr optionalString, LongBool Flag, MasterInfo* pMasterInfo, AnsiCharPtr optionalContent)
{ 
	//UserModuleBase* userModule = static_cast <UserModuleBase*>(pModule);
    
    if (pMasterInfo != nullptr)
    {
        // TODO : parse pMasterInfo->UsineVersion and sdk version to compare them (major + minor only)
        // TODO : check usine version against sdk version
    }

	try
	{
	    CreateModule (pModule, optionalString , Flag,  pMasterInfo, optionalContent);
	}
	catch (...) // std::exception& e
	{
        // error at module creation
		pModule = nullptr;
	}   

    //if (pModule != nullptr)
    //{
    //    // TODO : affect pMasterInfo to pModule->masterInfo ??
    //}

}

//
void Destroy(void* pModule)
{ 
	try
	{
		DestroyModule (pModule);
	}
	catch (std::exception& /*e*/) // std::exception& e
	{
		//const char* toto = e.what ();
		
	}   
} 
//
void BrowserInfo(ModuleInfo* pModuleInfo)
{ 
	try
	{
		GetBrowserInfo (pModuleInfo);
	}
	catch (std::exception& /*e*/) // std::exception& e
	{
		//const char* toto = e.what ();
		
	}   
} 

//
void GetModuleInfo (void* pModule, MasterInfo* pMasterInfo, ModuleInfo* pModuleInfo)
{ 
	UserModuleBase* userModule = static_cast <UserModuleBase*>(pModule);

	try
	{
		userModule->InitInfosStructures (pMasterInfo, pModuleInfo);
		userModule->onGetModuleInfo (pMasterInfo, pModuleInfo);
        
        userModule->setStringTrace (std::string ("[") + std::string (pModuleInfo->Name) + std::string ("]"));
	}
	catch (...) // std::exception& e
	{
		if (userModule != nullptr && pMasterInfo != nullptr) 
		{
			userModule->sdkTraceErrorChar(" in GetModuleInfo");
		}
	}   
}

//-----------------------------------------------------------------------------
// query system and init

int GetNumberOfParams(void* pModule,  int queryIndex)
{	
	UserModuleBase* userModule = static_cast <UserModuleBase*>(pModule);

	try
	{
		return userModule->onGetNumberOfParams (queryIndex);
	}
	catch (...) // std::exception& e
	{
		userModule->sdkTraceErrorChar(" in GetNumberOfParams");
		return -1;
	} 
}

void AfterQuery (void* pModule, MasterInfo* pMasterInfo, ModuleInfo* pModuleInfo, int queryIndex)
{	
	UserModuleBase* userModule = static_cast <UserModuleBase*>(pModule);

	try
	{
		userModule->AfterQueryPopup (pMasterInfo, pModuleInfo, queryIndex);
	}
	catch (...) // std::exception& e
	{
		userModule->sdkTraceErrorChar(" in AfterQuery");
	} 
}

void InitModule (void* pModule, MasterInfo* pMasterInfo, ModuleInfo* pModuleInfo)
{	
	UserModuleBase* userModule = static_cast <UserModuleBase*>(pModule);

	try
	{
		userModule->Init (pMasterInfo, pModuleInfo);
	}
	catch (...) // std::exception& e
	{
		userModule->sdkTraceErrorChar(" in InitModule");
	} 
}

//-----------------------------------------------------------------------------
// parameters and process
void GetParamInfo (void* pModule, int ParamIndex, TParamInfo* pParamInfo)
{	
	UserModuleBase* userModule = static_cast <UserModuleBase*>(pModule);

	try
	{
		userModule->onGetParamInfo (ParamIndex, pParamInfo);
	}
	catch (...) // std::exception& e
	{
		userModule->sdkTraceErrorChar(" in GetParamInfo");
	} 
}

void SetEventAddress (void* pModule, int ParamIndex, UsineEventPtr pEvent)
{	
	UserModuleBase* userModule = static_cast <UserModuleBase*>(pModule);

	try
	{
		userModule->onSetEventAddress (ParamIndex, pEvent);
	}
	catch (...) // std::exception& e
	{
		userModule->sdkTraceErrorChar(" in SetEventAddress");
	} 
}

void CallBack (void* pModule, UsineMessage *Message)
{	
	UserModuleBase* userModule = static_cast <UserModuleBase*>(pModule);

	try
	{
		userModule->CallBack (Message);
	}
	catch (...) // std::exception& e
	{
		userModule->sdkTraceErrorChar(" in CallBack");
	} 
}

void Process (void* pModule)
{	
	UserModuleBase* userModule = static_cast <UserModuleBase*>(pModule);

	try
	{
		userModule->onProcess ();
	}
	catch (...) // std::exception& e
	{
		userModule->sdkTraceErrorChar(" in Process");
	} 
}

void ProcessVideo(void* pModule)
{
	UserModuleBase* userModule = static_cast <UserModuleBase*>(pModule);

	try
	{
		userModule->onProcessVideo();
	}
	catch (...) // std::exception& e
	{
		userModule->sdkTraceErrorChar(" in ProcessVideo");
	}
}

//-----------------------------------------------------------------------------
// midi out callbacks
void MidiSendOut (void* pModule, int DeviceID, UsineMidiCode Code)
{	
	UserModuleBase* userModule = static_cast <UserModuleBase*>(pModule);

	try
	{
		userModule->onMidiSendOut (DeviceID, Code);
	}
	catch (...) // std::exception& e
	{
		userModule->sdkTraceErrorChar(" in MidiSendOut");
	} 
}

void MidiSendOutArray (void* pModule, int DeviceID, UsineMidiCode** arrayCode, int arraySize)
{	
	UserModuleBase* userModule = static_cast <UserModuleBase*>(pModule);

	try
	{
		userModule->onMidiSendOutArray (DeviceID, arrayCode, arraySize);
	}
	catch (...) // std::exception& e
	{
		userModule->sdkTraceErrorChar(" in MidiSendOutArray");
	} 
}

void MidiSysexSendOut (void* pModule, int DeviceID,  char** Sysex, int sysexSize)
{	
	UserModuleBase* userModule = static_cast <UserModuleBase*>(pModule);

	try
	{
		userModule->onMidiSysexSendOut (DeviceID, Sysex, sysexSize);
	}
	catch (...) // std::exception& e
	{
		userModule->sdkTraceErrorChar(" in MidiSysexSendOut");
	} 
}

void DMXSendOut (void* pModule, int deviceId, char* ByteArray, int len, int universeNum)
{	
	UserModuleBase* userModule = static_cast <UserModuleBase*>(pModule);

	try
	{
		userModule->onDMXSendOut (deviceId, ByteArray, len, universeNum);
	}
	catch (...) // std::exception& e
	{
		userModule->sdkTraceErrorChar(" in DMXSendOut");
	} 
}

//-----------------------------------------------------------------------------
// chunk system
int GetChunkLen (void* pModule, LongBool Preset)
{
	UserModuleBase* userModule = static_cast <UserModuleBase*>(pModule);

	try
	{
		return userModule->onGetChunkLen (Preset);
	}
	catch (...) // std::exception& e
	{
		userModule->sdkTraceErrorChar(" in GetChunkLen");
		return -1;
	}
}

void GetChunk (void* pModule, void* chunk, LongBool Preset)
{
	UserModuleBase* userModule = static_cast <UserModuleBase*>(pModule);

	try
	{
		userModule->onGetChunk (chunk, Preset);
	}
	catch (...) // std::exception& e
	{
		userModule->sdkTraceErrorChar(" in GetChunk");
	}
}

void SetChunk (void* pModule, const void* chunk, LongBool Preset, int sizeInBytes)
{
	UserModuleBase* userModule = static_cast <UserModuleBase*>(pModule);

	try
	{
		userModule->onSetChunk (chunk, sizeInBytes, Preset);
	}
	catch (...) // std::exception& e
	{
		userModule->sdkTraceErrorChar(" in SetChunk");
	}
}


//-----------------------------------------------------------------------------
// layout
void CreateSettings(void* pModule)
{
	UserModuleBase* userModule = static_cast <UserModuleBase*>(pModule);

	try
	{
		userModule->onCreateSettings ();
	}
	catch (...) // std::exception& e
	{
		userModule->sdkTraceErrorChar(" in CreateSettings");
	}
}

void CreateCommands(void* pModule)
{
	UserModuleBase* userModule = static_cast <UserModuleBase*>(pModule);

	try
	{
		userModule->onCreateCommands ();
	}
	catch (...) // std::exception& e
	{
		userModule->sdkTraceErrorChar(" in CreateCommands");
	}
}

void SettingsHasChanged(void* pModule)
{
	UserModuleBase* userModule = static_cast <UserModuleBase*>(pModule);

	try
	{
		userModule->onSettingsHasChanged ();
	}
	catch (...) // std::exception& e
	{
		userModule->sdkTraceErrorChar(" in SettingsHasChanged");
	}
}

void Resize (void* pModule, float W, float H) 
{
	UserModuleBase* userModule = static_cast <UserModuleBase*>(pModule);

	try
	{
		userModule->ResizeModule ( W, H);
	}
	catch (...) // std::exception& e
	{
		userModule->sdkTraceErrorChar(" in Resize");
	}
}

void Paint (void* pModule)
{
	UserModuleBase* userModule = static_cast <UserModuleBase*>(pModule);

	try
	{
		userModule->onPaint ();
	}
	catch (...) // std::exception& e
	{
		userModule->sdkTraceErrorChar(" in Paint");
	}
}


//-----------------------------------------------------------------------------
// mouse and multi touch interaction
void MouseMove(void* pModule, TShiftState Shift, float X, float Y)
{
	UserModuleBase* userModule = static_cast <UserModuleBase*>(pModule);

	try
	{
		userModule->onMouseMove (Shift, X, Y);
	}
	catch (...) // std::exception& e
	{
		userModule->sdkTraceErrorChar(" in MouseMove");
	}
}

void MouseDown(void* pModule, TMouseButton MouseButton, TShiftState Shift, float X,float Y)
{
	UserModuleBase* userModule = static_cast <UserModuleBase*>(pModule);

	try
	{
		userModule->onMouseDown (MouseButton, Shift, X, Y);
	}
	catch (...) // std::exception& e
	{
		userModule->sdkTraceErrorChar(" in MouseDown");
	}
}

void MouseUp (void* pModule,  TMouseButton MouseButton, TShiftState Shift, float X,float Y)
{
	UserModuleBase* userModule = static_cast <UserModuleBase*>(pModule);

	try
	{
		userModule->onMouseUp (MouseButton, Shift, X, Y);
	}
	catch (...) // std::exception& e
	{
		userModule->sdkTraceErrorChar(" in MouseUp");
	}
}

void MouseWheel (void* pModule,  TShiftState Shift, int WheelDelta)
{
	UserModuleBase* userModule = static_cast <UserModuleBase*>(pModule);

	try
	{
		userModule->onMouseWheel (Shift, WheelDelta);
	}
	catch (...) // std::exception& e
	{
		userModule->sdkTraceErrorChar(" in MouseWheel");
	}
}

void MouseMoveMulti(void* pModule, TShiftState Shift, UsineEventPtr X, UsineEventPtr Y, UsineEventPtr Pressed)
{
	UserModuleBase* userModule = static_cast <UserModuleBase*>(pModule);

	try
	{
		userModule->onMouseMoveMulti (Shift, X, Y, Pressed);
	}
	catch (...) // std::exception& e
	{
		userModule->sdkTraceErrorChar(" in MouseMoveMultiProc");
	}
}

void MouseDownMulti(void* pModule, TMouseButton MouseButton, TShiftState Shift, UsineEventPtr X, UsineEventPtr Y, UsineEventPtr Pressed)
{
	UserModuleBase* userModule = static_cast <UserModuleBase*>(pModule);

	try
	{
		userModule->onMouseDownMulti (MouseButton, Shift, X, Y, Pressed);
	}
	catch (...) // std::exception& e
	{
		userModule->sdkTraceErrorChar(" in MouseDownMultiProc");
	}
}

void MouseUpMulti (void* pModule, TMouseButton MouseButton, TShiftState Shift,UsineEventPtr X, UsineEventPtr Y, UsineEventPtr Pressed)
{
	UserModuleBase* userModule = static_cast <UserModuleBase*>(pModule);

	try
	{
		userModule->onMouseUpMulti (MouseButton, Shift, X, Y, Pressed);
	}
	catch (...) // std::exception& e
	{
		userModule->sdkTraceErrorChar(" in MouseUpMultiProc");
	}
}

void OpenEditor(void* pModule)
{
	UserModuleBase* userModule = static_cast <UserModuleBase*>(pModule);

	try
	{
		userModule->onOpenEditor ();
	}
	catch (...) // std::exception& e
	{
		userModule->sdkTraceErrorChar(" in OpenEditor");
	}
}

void BringToFront(void* pModule)
{
	UserModuleBase* userModule = static_cast <UserModuleBase*>(pModule);

	try
	{
		userModule->onBringToFront ();
	}
	catch (...) // std::exception& e
	{
		userModule->sdkTraceErrorChar(" in BringToFront");
	}
}

void CloseEditor(void* pModule)
{
	UserModuleBase* userModule = static_cast <UserModuleBase*>(pModule);

	try
	{
		userModule->onCloseEditor ();
	}
	catch (...) // std::exception& e
	{
		userModule->sdkTraceErrorChar(" in CloseEditor");
	}
}

void ResizeEditor(void* pModule, int width, int height)
{
	UserModuleBase* userModule = static_cast <UserModuleBase*>(pModule);
    
	try
	{
		userModule->onResizeEditor (width, height);
	}
	catch (...) // std::exception& e
	{
		userModule->sdkTraceErrorChar(" in ResizeEditor");
	}
}
//-----------------------------------------------------------------------------
// audio setup update
void OnBlocSizeChange (void* pModule, int BlocSize)
{
	UserModuleBase* userModule = static_cast <UserModuleBase*>(pModule);

	try
	{
		return userModule->onBlocSizeChange (BlocSize);
	}
	catch (...) // std::exception& e
	{
		userModule->sdkTraceErrorChar(" in OnBlocSizeChange");
	}
}

void OnSampleRateChange (void* pModule, double SampleRate)
{
	UserModuleBase* userModule = static_cast <UserModuleBase*>(pModule);

	try
	{
		userModule->onSampleRateChange (SampleRate);
	}
	catch (...) // std::exception& e
	{
		userModule->sdkTraceErrorChar(" in OnSampleRateChange");
	}
}

//-----------------------------------------------------------------------------
// recording
void SetRecordedValue (void* pModule, TPrecision X, TPrecision Y, TPrecision Z)
{
	UserModuleBase* userModule = static_cast <UserModuleBase*>(pModule);

	try
	{
		userModule->onSetRecordedValue (X, Y, Z);
	}
	catch (...) // std::exception& e
	{
		userModule->sdkTraceErrorChar(" in SetRecordedValue");
	}
}


//-----------------------------------------------------------------------------
// usine randomize
void Randomize (void* pModule)
{
	UserModuleBase* userModule = static_cast <UserModuleBase*>(pModule);

	try
	{
		userModule->onRandomize ();
	}
	catch (...) // std::exception& e
	{
		userModule->sdkTraceErrorChar(" in Randomize");
	}
}

//-----------------------------------------------------------------------------
// usine reset
void Reset(void* pModule)
{
	UserModuleBase* userModule = static_cast <UserModuleBase*>(pModule);

	try
	{
		userModule->onReset();
	}
	catch (...) // std::exception& e
	{
		userModule->sdkTraceErrorChar(" in Reset");
	}
}


//-----------------------------------------------------------------------------
// usine set quick color
void SetQuickColor(void* pModule, TColorUsine color)
{
	UserModuleBase* userModule = static_cast <UserModuleBase*>(pModule);

	try
	{
		userModule->onSetQuickColor(color);
	}
	catch (...) // std::exception& e
	{
		userModule->sdkTraceErrorChar(" in SetQuickColor");
	}
}


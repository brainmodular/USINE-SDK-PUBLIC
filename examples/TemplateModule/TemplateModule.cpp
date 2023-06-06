//-----------------------------------------------------------------------------
//@file  
//	TemplateModule.cpp
//
//@author
//	BrainModular team
//
//@brief 
//	Implementation of the TemplateModule class.
//
//  A template include to start from for new user module.
//
//@historic 
//	2013/05/15
//    first release for Hollyhock CPP SDK 6.00.226 
//
//@IMPORTANT
//	This file is part of the Usine Hollyhock CPP SDK
//
//  Please, report bugs and patch to Usine forum :
//  support@brainmodular.com 
//
// All dependencies are under there own licence.
//
//@LICENCE
// Copyright (C) 2013, 2014, 2015, 2019 BrainModular
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
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------
#include "TemplateModule.h"

//----------------------------------------------------------------------------
// create, general info and destroy methods
//----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Create
void CreateModule (void* &pModule, AnsiCharPtr optionalString, LongBool Flag, MasterInfo* pMasterInfo, AnsiCharPtr optionalContent)
{
	pModule = new TemplateModule();
}

// destroy
void DestroyModule(void* pModule) 
{
	// cast is important to call the good destructor
	delete ((TemplateModule*)pModule);
}

//-------------------------------------------------------------------------
// module constructors/destructors
//-------------------------------------------------------------------------

// constructor
TemplateModule::TemplateModule()
{
	//
}

// destructor
TemplateModule::~TemplateModule()
{
	// 
}

// module constants for browser info and module info
const AnsiCharPtr UserModuleBase::MODULE_NAME = "Template";
const AnsiCharPtr UserModuleBase::MODULE_DESC = "Template Description";
const AnsiCharPtr UserModuleBase::MODULE_VERSION = "1.0";

// browser info
void GetBrowserInfo(ModuleInfo* pModuleInfo)
{
	pModuleInfo->Name = UserModuleBase::MODULE_NAME;
	pModuleInfo->Description = UserModuleBase::MODULE_DESC;
	pModuleInfo->Version = UserModuleBase::MODULE_VERSION;
}


//void TemplateModule::onCreate(AinsiCharPtr optionalString);
//void TemplateModule::onDestroy(); 
void TemplateModule::onGetModuleInfo (TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo)
{
	pModuleInfo->Name				= MODULE_NAME;
	pModuleInfo->Description		= MODULE_DESC;
	pModuleInfo->ModuleType         = mtSimple;
	pModuleInfo->BackColor          = sdkGetUsineColor(clDataModuleColor);
	pModuleInfo->NumberOfParams     = 1;
	pModuleInfo->Version			= MODULE_VERSION;
	pModuleInfo->DontProcess		= false;
}

//-----------------------------------------------------------------------------
// query system and init
int  TemplateModule::onGetNumberOfParams( int QIdx) {return 0;}
void TemplateModule::onAfterQuery (TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo, int QIdx) {}
void TemplateModule::onInitModule (TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo) {}

//-----------------------------------------------------------------------------
// parameters and process
void TemplateModule::onGetParamInfo (int ParamIndex, TParamInfo* pParamInfo)
{	
	switch (ParamIndex) 
    {
        // Add all parameters declared in the module class, example :

        // m_txtfExample
        case 0:
	        pParamInfo->ParamType		= ptTextField;
	        pParamInfo->Caption			= "text";
            pParamInfo->TextValue       = "hello world";
	        pParamInfo->IsInput			= TRUE;
	        pParamInfo->IsOutput		= TRUE;
			pParamInfo->EventPtr        = &m_txtfExample;
	        break;

	        // default case
        default:
	        break;
	}
}

void TemplateModule::onCallBack (TUsineMessage *Message) {}
void TemplateModule::onProcess () {}

//-----------------------------------------------------------------------------
// midi out callbacks
void TemplateModule::onMidiSendOut (int DeviceID, TUsineMidiCode Code) {}
void TemplateModule::onMidiSysexSendOut (int DeviceID, char* Sysex) {}

//-----------------------------------------------------------------------------
// chunk system
int  TemplateModule::onGetChunkLen (LongBool Preset) {return 0;}
void TemplateModule::onGetChunk (void* chunk, LongBool Preset) {}
void TemplateModule::onSetChunk (const void* chunk, int sizeInBytes, LongBool Preset) {}

//-----------------------------------------------------------------------------
// layout
void TemplateModule::onCreateSettings() {}
void TemplateModule::onSettingsHasChanged() {}
void TemplateModule::onPaint () {}

//-----------------------------------------------------------------------------
// mouse and multi touch interaction
void TemplateModule::onMouseMove(TShiftState Shift, float X, float Y) {}
void TemplateModule::onMouseDown(TMouseButton MouseButton, TShiftState Shift, float X,float Y) {}
void TemplateModule::onMouseUp (TMouseButton MouseButton, TShiftState Shift, float X,float Y) {}
void TemplateModule::onMouseMoveMulti(TShiftState Shift, UsineEventPtr X, UsineEventPtr Y) {}
void TemplateModule::onMouseDownMulti(TMouseButton MouseButton, TShiftState Shift, UsineEventPtr X, UsineEventPtr Y) {}
void TemplateModule::onMouseUpMulti (TMouseButton MouseButton, TShiftState Shift,UsineEventPtr X, UsineEventPtr Y) {}
void TemplateModule::onOpenEditor() {}
void TemplateModule::onBringToFront() {}

//-----------------------------------------------------------------------------
// audio setup update
void TemplateModule::onBlocSizeChange (int BlocSize) {}
void TemplateModule::onSampleRateChange (double SampleRate) {}

//-----------------------------------------------------------------------------
// recording 
void TemplateModule::onSetRecordedValue (TPrecision X, TPrecision Y, TPrecision Z) {}

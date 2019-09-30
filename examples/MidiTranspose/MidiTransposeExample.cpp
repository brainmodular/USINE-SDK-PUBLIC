//-----------------------------------------------------------------------------
//@file  
//	MidiTransposeExample.cpp
//
//@author
//	Martin FLEURENT aka 'martignasse'
//
//@brief 
//	Definitions of the MidiTransposeExampleExample class.
//
//  Example user module to show how to process midi messages.
//
//@historic 
//	2015/02/23
//    first release for Hollyhock CPP SDK 6.04.001
//
//@IMPORTANT
//	This file is part of the Usine Hollyhock CPP SDK
//
//  Please, report bugs :
//  support@brainmodular.org
//
// All dependencies are under there own licence.
//
//@LICENCE
// Copyright (C) 2013, 2014, 2015 BrainModular
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
#include "MidiTransposeExample.h"

//----------------------------------------------------------------------------
// create, general info and destroy methodes
//----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Create
void CreateModule (void* &pModule, AnsiCharPtr optionalString, LongBool Flag, MasterInfo* pMasterInfo, AnsiCharPtr optionalContent)
{
	pModule = new MidiTransposeExampleExample ();
}

//-----------------------------------------------------------------------------
// destroy
void DestroyModule (void* pModule) 
{
    // cast is important to call the good destructor
	delete ((MidiTransposeExampleExample*)pModule);
}

// module constants for browser info and module info
const AnsiCharPtr UserModuleBase::MODULE_NAME = "midi transpose example";
const AnsiCharPtr UserModuleBase::MODULE_DESC = "midi transpose sdk module example";
const AnsiCharPtr UserModuleBase::MODULE_VERSION = "1.0";

// browser info
void GetBrowserInfo(ModuleInfo* pModuleInfo) 
{
	pModuleInfo->Name				= UserModuleBase::MODULE_NAME;
	pModuleInfo->Description		= UserModuleBase::MODULE_DESC;
	pModuleInfo->Version			= UserModuleBase::MODULE_VERSION;
}

//-----------------------------------------------------------------------------
// module description
void MidiTransposeExampleExample::onGetModuleInfo (MasterInfo* pMasterInfo, ModuleInfo* pModuleInfo) 
{

	//identification of the module
	pModuleInfo->Name				= MODULE_NAME;
	pModuleInfo->Description		= MODULE_DESC;
	pModuleInfo->ModuleType         = mtSimple;
	//pModuleInfo->BackColor          = sdkGetUsineColor(clMIDIModuleColor);
	pModuleInfo->NumberOfParams     = 3;
	pModuleInfo->Version			= MODULE_VERSION;
	pModuleInfo->CanBeSavedInPreset = FALSE;
}

//----------------------------------------------------------------------------
// parameters and process
//----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Parameters description
void MidiTransposeExampleExample::onGetParamInfo (int ParamIndex, TParamInfo* pParamInfo) 
{
	// all parameters declared in the module class		
	switch (ParamIndex) 
	{
	// midiIn
	case 0:
		pParamInfo->ParamType			= ptMidi;
		pParamInfo->Caption				= "in";
		pParamInfo->IsInput				= TRUE;
		pParamInfo->IsOutput			= FALSE;
		pParamInfo->CallBackType		= ctImmediate;
		pParamInfo->EventPtr            = &midiIn;
		break;

	// midiOut
	case 1:
		pParamInfo->ParamType			= ptMidi;
		pParamInfo->Caption				= "out";
		pParamInfo->IsInput				= FALSE;
		pParamInfo->IsOutput			= TRUE;
		pParamInfo->EventPtr            = &midiOut;
		break;

	// fdrPitch
	case 2:
		pParamInfo->ParamType			= ptDataFader;
		pParamInfo->Caption				= "pitch";
		pParamInfo->IsInput				= TRUE;
		pParamInfo->IsOutput			= FALSE;
		pParamInfo->MinValue			= -127.0f;
		pParamInfo->MaxValue			= 127.0f;
		pParamInfo->DefaultValue		= 0.0f;
		pParamInfo->Symbol				= "halftone";
		pParamInfo->Format				= "%.0f";
		pParamInfo->IsStoredInPreset	= TRUE;
		pParamInfo->CallBackType		= ctImmediate;
		pParamInfo->EventPtr            = &fdrPitch;
		break;

	// default case
	default:
		break;
	}
}

void MidiTransposeExampleExample::onProcess () 
{
    int sizeMidiIn = sdkGetEvtSize (midiIn);
    sdkSetEvtSize (midiOut, 0);

    if (sizeMidiIn > 0)
    {
        sdkCopyEvt (midiIn, midiOut);
        int pitch = (int)sdkGetEvtData (fdrPitch);

	    for (int i = 0; i < sizeMidiIn; i++)
        {
            UsineMidiCode code = sdkGetEvtArrayMidi (midiOut, i);
           if (code.Msg ==  MIDI_NOTEON || code.Msg ==  MIDI_NOTEOFF)
           {
                code.Data1 = std::min (127, std::max (0, (int)code.Data1 + pitch));
                sdkSetEvtArrayMidi (midiOut, i, code);
           }
        }
    }
}


//-----------------------------------------------------------------------------
//@file  
//	PhaseVocoderModule.cpp
//
//@date 
//
//@author
//	Martin FLEURENT aka 'martignasse'
//
//@brief 
//	Implementation of the PhaseVocoderModule class.
//
//HISTORIC 
//	2013/05/15
//    first release for Hollyhock CPP SDK 6.00.226 
//
//IMPORTANT
//	This file is part of the Usine Hollyhock CPP SDK
//
//  Please, report bugs and patch to Usine forum :
//  support@brainmodular.org 
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
#include "PhaseVocoder.h"

//----------------------------------------------------------------------------
// create, general info and destroy methodes
//----------------------------------------------------------------------------
// Create
void CreateModule (void* &pModule, AnsiCharPtr optionalString, LongBool Flag, MasterInfo* pMasterInfo, AnsiCharPtr optionalContent)
{
	pModule = new PhaseVocoderModule ();
}

//-----------------------------------------------------------------------------
// destroy
void DestroyModule (void* pModule) 
{
	// cast is important to call the good destructor
	delete ((PhaseVocoderModule*)pModule);
}

// module constants for browser info and module info
const AnsiCharPtr UserModuleBase::MODULE_NAME = "PhVocForw";
const AnsiCharPtr UserModuleBase::MODULE_DESC = "Phase Vocoder Forward";
const AnsiCharPtr UserModuleBase::MODULE_VERSION = "2.0";

// browser info
void GetBrowserInfo(ModuleInfo* pModuleInfo) 
{
	pModuleInfo->Name				= UserModuleBase::MODULE_NAME;
	pModuleInfo->Description		= UserModuleBase::MODULE_DESC;
	pModuleInfo->Version			= UserModuleBase::MODULE_VERSION;
}

//-------------------------------------------------------------------------
// module constructors/destructors
//-------------------------------------------------------------------------
// constructor
PhaseVocoderModule::PhaseVocoderModule()
    : sampleRate (44100.0f)
{

	//rampsamps = (float) RAMPSAMPS;
	spectralTransposer = new spectraltransposer();
}

// destructor
PhaseVocoderModule::~PhaseVocoderModule()
{
	if(spectralTransposer)  
    {
		delete spectralTransposer;
		spectralTransposer = nullptr;
	}
}

//
void PhaseVocoderModule::onGetModuleInfo (MasterInfo* pMasterInfo, ModuleInfo* pModuleInfo)
{
	pModuleInfo->Name				= MODULE_NAME;
	pModuleInfo->Description		= MODULE_DESC;
	pModuleInfo->ModuleType         = mtSimple;
	pModuleInfo->BackColor          = sdkGetUsineColor(clArrayModuleColor);
	pModuleInfo->NumberOfParams     = 3;
	pModuleInfo->DontProcess		= false;
	pModuleInfo->Version			= MODULE_VERSION;
	pModuleInfo->CanBeSavedInPreset = FALSE;
}

//-----------------------------------------------------------------------------
// query system and init
void PhaseVocoderModule::onInitModule (MasterInfo* pMasterInfo, ModuleInfo* pModuleInfo) 
{
    sampleRate = sdkGetSampleRate ();
	spectralTransposer->init (sampleRate, FFT_SIZE, FFT_SIZE / 2.0f, PVPP_STREAMING);
	
    int sampleFrames = FFT_SIZE / 2;
    sdkSetEvtSize (arrAmplitude, sampleFrames);
	sdkSetEvtSize (arrFrequency, sampleFrames);
}

//-----------------------------------------------------------------------------
// parameters and process
void PhaseVocoderModule::onGetParamInfo (int ParamIndex, TParamInfo* pParamInfo)
{	
	switch (ParamIndex) 
    {
		// audioInSignal
	case 0: 
		pParamInfo->ParamType		= ptAudio;
		pParamInfo->Caption			= "audio in";
		pParamInfo->IsInput			= TRUE;
		pParamInfo->IsOutput		= FALSE;
		pParamInfo->ReadOnly		= TRUE;
		break;

		// arrFrequency ouput
	case 1:
		pParamInfo->ParamType		= ptArray;
		pParamInfo->Caption			= "frequency";
		pParamInfo->IsInput			= FALSE;
		pParamInfo->IsOutput		= TRUE;
		pParamInfo->ReadOnly		= TRUE;
		pParamInfo->MinValue		= FLT_MIN;
		pParamInfo->MaxValue		= FLT_MAX;
		break;

		// arrAmplitude output
	case 2: 
		pParamInfo->ParamType		= ptArray;
		pParamInfo->Caption			= "amplitude";
		pParamInfo->IsInput			= FALSE;
		pParamInfo->IsOutput		= TRUE;
		pParamInfo->ReadOnly		= TRUE;
		//pParamInfo->MinValue		= -1;
		//pParamInfo->MaxValue		= 1;
		break;

		// default case
	default:
		break;
	}
}
void PhaseVocoderModule::onSetEventAddress (int ParamIndex, UsineEventPtr pEvent)
{
	switch (ParamIndex) 
    {
	// audioInSignal
	case 0: 
		audioInSignal = pEvent;
		break;

	// arrFrequency output
	case 1:
		arrFrequency = pEvent;
		break;

	// arrAmplitude output
	case 2: 
		arrAmplitude = pEvent;
		break;

	// default case
	default:
		break;
	}
}

void PhaseVocoderModule::onCallBack (UsineMessage *Message) {}

void PhaseVocoderModule::onProcess () 
{
	float *in1      = sdkGetEvtDataAddr (audioInSignal);    
    float *amp1     = spectralTransposer->amp;
    float *amp2     = sdkGetEvtDataAddr (arrAmplitude);
    float *freq1    = spectralTransposer->freq;
    float *freq2    = sdkGetEvtDataAddr (arrFrequency);
	 
    int  sampleFrames = sdkGetEvtSize (audioInSignal);
	
    while (--sampleFrames >= 0)
		spectralTransposer->tickForw(*in1++);            
	

	sampleFrames = (FFT_SIZE / 2) + 1;
	while (--sampleFrames >= 0)
    {
      *amp2++  = *amp1++;
      *freq2++ = *freq1++;
	}
}


//-----------------------------------------------------------------------------
// audio setup update
void PhaseVocoderModule::onBlocSizeChange (int BlocSize) 
{

}

void PhaseVocoderModule::onSampleRateChange (double SampleRate) 
{
    sampleRate = sdkGetSampleRate ();
	spectralTransposer->init ((long)sampleRate, FFT_SIZE, FFT_SIZE/2, PVPP_STREAMING);

}
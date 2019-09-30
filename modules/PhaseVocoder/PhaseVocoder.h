//-----------------------------------------------------------------------------
//@file  
//	PhaseVocoderModule.h
//
//@author
//	Martin FLEURENT aka 'martignasse'
//
//@brief 
//	Definitions of the PhaseVocoderModule class.
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

// include once, no more
#ifndef __PHASE_VOCODER_MODULE_H__
#define __PHASE_VOCODER_MODULE_H__

//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------
#include "../../sdk/UserDefinitions.h"  
#include "cdp/transform.h"

//-----------------------------------------------------------------------------
// defines and constantes
//-----------------------------------------------------------------------------
#define FFT_SIZE 1024
//#define RAMPSAMPS (128)
#define DENORMALISATION 0.000000000001f

//-----------------------------------------------------------------------------
// class definition
//-----------------------------------------------------------------------------
class PhaseVocoderModule : public UserModuleBase
{
    //-------------------------------------------------------------------------
	// module constructors/destructors
	//-------------------------------------------------------------------------
public:
    // constructor
	PhaseVocoderModule();

    // destructor
	virtual ~PhaseVocoderModule();

	//-------------------------------------------------------------------------
	// public methodes inherited from UserModule
	//-------------------------------------------------------------------------
public:
	//-----------------------------------------------------------------------------
	// module info
	void onGetModuleInfo (MasterInfo* pMasterInfo, ModuleInfo* pModuleInfo);

	//-----------------------------------------------------------------------------
	// query system and init
	//int  onGetNumberOfParams( int queryIndex);
	//void onAfterQuery (MasterInfo* pMasterInfo, ModuleInfo* pModuleInfo, int queryIndex);
	void onInitModule (MasterInfo* pMasterInfo, ModuleInfo* pModuleInfo);

	//-----------------------------------------------------------------------------
	// parameters and process
	void onGetParamInfo (int ParamIndex, TParamInfo* pParamInfo);
	void onSetEventAddress (int ParamIndex, UsineEventPtr pEvent);
	void onCallBack (UsineMessage *Message);
	void onProcess ();
    
	//-----------------------------------------------------------------------------
	// audio setup update
	void onBlocSizeChange (int BlocSize);
	void onSampleRateChange (double SampleRate);

	//-------------------------------------------------------------------------
	// protected members
	//-------------------------------------------------------------------------
protected:
	//-------------------------------------------------------------------------
	// parameters events
    UsineEventPtr arrFrequency;	    // used as input for PhaseVocoderForw and as output for PhaseVocoderIn
    UsineEventPtr arrAmplitude;	    // used as input for PhaseVocoderForw and as output for PhaseVocoderIn
    UsineEventPtr audioInSignal;
    UsineEventPtr audioOutSignal;
    //UsineEventPtr PSemitone;	// gain fader (-80 -->> +6 db pointer example  
	
    // internal variable
    spectraltransposer* spectralTransposer;
    //float rampsamps;
    //char programName[32];
    double sampleRate;
    
}; // class PhaseVocoderModule

#endif //__PHASE_VOCODER_MODULE_H__
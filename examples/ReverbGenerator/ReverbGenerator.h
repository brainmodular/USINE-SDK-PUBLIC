//-----------------------------------------------------------------------------
//@file  
//	AudioVolumeExample.h
//
//@author
//	Martin FLEURENT aka 'martignasse'
//
//@brief 
//	Definitions of the AudioVolumeExample class.
//
//  Example user module to show how to process audio buffers.
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

// include once, no more
#ifndef __FF_CONVOLVER_H__
#define __FF_CONVOLVER_H__

//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------
#include "../../sdk/UserDefinitions.h" 

#include "FFTConvolver.h"
//#include "TwoStageFFTConvolver.h"
#include "CookbookEq.h"
#include <vector>

//-----------------------------------------------------------------------------
// defines and constantes
//-----------------------------------------------------------------------------

static const NativeInt NOTIFY_START_LOAD_IR = 0x78AB0;
static const NativeInt NOTIFY_LOAD_IR_FINISHED = 0x78AB1;
typedef enum class TLoadingIRState
{
   lsProcessing,
   lsAsked,
   lsLoading,
   lsLoaded
};


//-----------------------------------------------------------------------------
// class definition
//-----------------------------------------------------------------------------
class FFConvolver : public UserModuleBase
{
    //-------------------------------------------------------------------------
	// module constructors/destructors
	//-------------------------------------------------------------------------
public:
    // constructor
	FFConvolver();

    // destructor
	virtual ~FFConvolver();

	//-------------------------------------------------------------------------
	// public methodes inherited from UserModule
	//-------------------------------------------------------------------------
public:
	//-----------------------------------------------------------------------------
	// module info
	void onGetModuleInfo (MasterInfo* pMasterInfo, ModuleInfo* pModuleInfo);

	//-----------------------------------------------------------------------------
	// query system and init
	int  onGetNumberOfParams( int queryIndex);
	void onAfterQuery (MasterInfo* pMasterInfo, ModuleInfo* pModuleInfo, int queryIndex);
	void onInitModule (MasterInfo* pMasterInfo, ModuleInfo* pModuleInfo);

	//-----------------------------------------------------------------------------
	// parameters and process
	void onGetParamInfo (int ParamIndex, TParamInfo* pParamInfo);
	void onCallBack (UsineMessage *Message);
	void onProcess ();

	//-----------------------------------------------------------------------------
	// engine changed
	void onBlocSizeChange(int BlocSize);
	void onSampleRateChange(double SampleRate);
	
	// called after the module is loaded
	void onAfterLoading();

	//-------------------------------------------------------------------------
	// private members
	//-------------------------------------------------------------------------
private:
	//-------------------------------------------------------------------------
	// parameters events
    UsineEventPtr audioInputs[AUDIO_INS_OUTS_MAX];     // audio input
    UsineEventPtr audioOutputs[AUDIO_INS_OUTS_MAX];    // audio output
	TPrecision audioOutTmp[MAX_AUDIO_EVT_SIZE];
	UsineEventPtr fdrDryWet;
	UsineEventPtr fdrOnOff;
	UsineEventPtr fdrDensity;
	UsineEventPtr fdrDampingFreq;
	UsineEventPtr fdrSize;
	UsineEventPtr fdrDecay;
	UsineEventPtr fdrHiPassFreq;
	UsineEventPtr fdrPreDelay;


	//-------------------------------------------------------------------------
    static const int numOfParamAfterAudiotInOut = 8;

	int queryIndex;
	int numOfAudiotInsOuts;

	fftconvolver::FFTConvolver  Convolver[AUDIO_INS_OUTS_MAX];
	
	int irSize;
	//int irNumChannels;
	
	void clearAudioOut();
	void createAndLoadIR();
	CookbookEq* HiPassFilter[AUDIO_INS_OUTS_MAX];
	CookbookEq* ImpulseLowPassFilter;

	TLoadingIRState loadingIRSate;
	int cptDeactivation;
	int irFinalSize;


}; // class AudioVolumeExample

#endif //__FF_CONVOLVER_H__

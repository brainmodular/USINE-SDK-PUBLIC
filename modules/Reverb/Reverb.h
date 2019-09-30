//-----------------------------------------------------------------------------
//@file  
//	Reverb.h
//
//@author
//	Martin FLEURENT aka 'martignasse'
//
//@brief 
//	Definitions of the Reverb class.
//
//  A reverb module based on the original freeverb algo.
//
//HISTORIC 
//	2013/08/27
//    first release for Hollyhock CPP SDK 6.02.011
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
#ifndef __TEMPLATE_MODULE_SIMPLE_H__
#define __TEMPLATE_MODULE_SIMPLE_H__

//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------
#include "../../sdk/UserDefinitions.h"  
#include "comb.hpp"
#include "allpass.hpp"
#include "tuning.h"
#include <vector>
//#include "revmodel.hpp"

//-----------------------------------------------------------------------------
// defines and constantes
//----------------------------------------------------------------------------
#define AUDIO_INS_OUTS_MAX 64

typedef std::vector <std::string>   ListString;
    
typedef std::vector <comb>          ListComb;
typedef std::vector <allpass>       ListAllPass;
typedef std::vector <float>         ListFloat;
typedef std::vector <ListFloat>     ListListFloat;
typedef std::vector <int>           ListInt;
//-----------------------------------------------------------------------------
// class definition
//-----------------------------------------------------------------------------
class Reverb : public UserModuleBase
{
    //-------------------------------------------------------------------------
	// module constructors/destructors
	//-------------------------------------------------------------------------
public:
    // constructor
	Reverb();

    // destructor
	virtual ~Reverb();

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
	void onSetEventAddress (int ParamIndex, UsineEventPtr pEvent);
	void onCallBack (UsineMessage *Message);
	void onProcess ();
    
	//-------------------------------------------------------------------------
	// protected methodes
	//-------------------------------------------------------------------------
protected:
    void reverbInit();
    void reverbParamsUpdate();

	//-------------------------------------------------------------------------
	// protected members
	//-------------------------------------------------------------------------
protected:
	//-------------------------------------------------------------------------
	// parameters events
    UsineEventPtr audioInputs[AUDIO_INS_OUTS_MAX];     // audio input
    UsineEventPtr audioOutputs[AUDIO_INS_OUTS_MAX];    // audio output
	UsineEventPtr swtchMode;
	UsineEventPtr fdrRoomSize;
	UsineEventPtr fdrDamping;
	UsineEventPtr fdrChannelsSeparation;
	UsineEventPtr gfdrWetLevel;
	UsineEventPtr gfdrDryLevel;
	
	//-------------------------------------------------------------------------
    int queryIndex;
	int numOfAudiotInsOuts;
	int numOfParamAfterAudiotInOut;
    
    TPrecision* audioInputsPointers[AUDIO_INS_OUTS_MAX];
    TPrecision* audioOutputsPointers[AUDIO_INS_OUTS_MAX];

    // reverb stuff
	//revmodel*	reverbModel;
    UsineEventPtr temp[AUDIO_INS_OUTS_MAX];
    UsineEventPtr temp2[AUDIO_INS_OUTS_MAX];

    UsineEventPtr processOutput[AUDIO_INS_OUTS_MAX];
    UsineEventPtr processInput[AUDIO_INS_OUTS_MAX];

    // default values for the reverb model param
    // !!!!! in module scale (not between 0 and 1)
    static const float initialWet;      // dB
    static const float initialDry;      // dB
    static const float initialRoom;     // %
    static const float initialDamp;     // %
    static const float initialChannelsSeparation;    // %
    static const float initialMode;     // switch

	float	gain;
	float	roomsize,roomsize1;
	float	damp,damp1;
	float	wet,wet1,wet2;
	float	dry;
	float	channelsSeparation;
	float	mode;
	float	currentwet1,currentwet2,currentdry;
    float   coeffwet1; 
    float   coeffwet2;

	// Comb filters
    ListComb   combvoices;
    
	// Allpass filters
	ListAllPass	allpassvoices;
    
    // tuning values for combs and allpass
    ListInt    combtuningvoices;
    ListInt    allpasstuningvoices;

	// Buffers for the combs
	//float	bufcombvoices[numcombs][numvoicesmax][1700];
	ListListFloat	bufcombvoices[numcombs];
    
	// Buffers for the allpasses
	//float	bufallpassvoices[numallpasses][numvoicesmax][600];
	ListListFloat	bufallpassvoices[numallpasses];

}; // class Reverb

#endif //__TEMPLATE_MODULE_SIMPLE_H__
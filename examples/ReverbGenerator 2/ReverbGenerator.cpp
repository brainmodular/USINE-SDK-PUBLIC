//-----------------------------------------------------------------------------
//@file  
//	ReverbGenerator.cpp V2
//@author
//	Olivier Sens aka 'senso'
//
//@brief 
//	Implementation of the convolution Reverb generator class.
//
//  Example user module to show how to process audio buffers.
//
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
// Copyright (C) 2013, 2014, 2015 Brainmodular
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
#include "ReverbGenerator.h"
#include "../libs/TwoStageFFTConvolver.h"
#include "../libs/CookbookEq.h"

#define LOW_CUT_FREQ_DEFAULT 20.0f

#define MAX_DURATION 10000
#define MAX_PREDELAY 150
#define MAX_EARLY 400
#define MAX_SAMPLERATE 192



//----------------------------------------------------------------------------
// create, general info and destroy methodes
//----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Create
void CreateModule (void* &pModule, AnsiCharPtr optionalString, LongBool Flag, MasterInfo* pMasterInfo, AnsiCharPtr optionalContent)
{
	pModule = new FFConvolver();

}

// destroy
void DestroyModule(void* pModule) 
{
	// cast is important to call the good destructor
	delete ((FFConvolver*)pModule);
}

// module constants for browser info and module info
const AnsiCharPtr UserModuleBase::MODULE_NAME = "Reverb Generator";
const AnsiCharPtr UserModuleBase::MODULE_DESC = "Real Time FFT Convolution";
const AnsiCharPtr UserModuleBase::MODULE_VERSION = "1.0";

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
FFConvolver::FFConvolver()    
{
	 

}

// destructor
FFConvolver::~FFConvolver()
{
	for (int i = 0; i < numOfAudiotInsOuts; i++)
	{
		delete HiPassFilter[i];
		delete finalIR[i];
		delete earlyIR[i];
		delete lateIR[i];
	}

}

void FFConvolver::onGetModuleInfo (MasterInfo* pMasterInfo, ModuleInfo* pModuleInfo)
{
	pModuleInfo->Name				= MODULE_NAME;
	pModuleInfo->Description		= MODULE_DESC;
	pModuleInfo->ModuleType         = mtSimple;
	pModuleInfo->BackColor          = sdkGetUsineColor(clAudioModuleColor);
	pModuleInfo->Version			= MODULE_VERSION;
    
	// query for multi-channels
	if (pMasterInfo != nullptr)
    {
	    pModuleInfo->QueryString		= sdkGetAudioQueryTitle();
	    pModuleInfo->QueryListValues	= sdkGetAudioQueryChannelList();
	    pModuleInfo->QueryDefaultIdx	= 1;
    }
	pModuleInfo->CanBeSavedInPreset = FALSE;

}

//-----------------------------------------------------------------------------
// query system and init methodes
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Get total parameters number of the module
int FFConvolver::onGetNumberOfParams (int queryIndex)
{
	int result = 0;
    this->queryIndex = queryIndex;
    numOfAudiotInsOuts = sdkGetAudioQueryToNbChannels (queryIndex);

    // we want 1 in 1 out per channels
	result = (numOfAudiotInsOuts * 2) + numOfParamAfterAudiotInOut;

    return result;
}

//-----------------------------------------------------------------------------
// Called after the query popup
void FFConvolver::onAfterQuery (MasterInfo* pMasterInfo, ModuleInfo* pModuleInfo, int queryIndex)
{
}

//-----------------------------------------------------------------------------
// Called after loading
void FFConvolver::onAfterLoading()
{
	calculateContants();
	createEarly();
	createLate();
	mixAndLoadIR();
	loadingIRSate = TLoadingIRState::lsProcessing;
}


//-----------------------------------------------------------------------------
// initialisation
void FFConvolver::onInitModule (MasterInfo* pMasterInfo, ModuleInfo* pModuleInfo) 
{
	for (int i = 0; i < numOfAudiotInsOuts; i++)
	{
		HiPassFilter[i] = new CookbookEq(CookbookEq::HiPass2, LOW_CUT_FREQ_DEFAULT, 1);
		HiPassFilter[i]->prepareToPlay(float(sdkGetSampleRate()), sdkGetBlocSize());
		HiPassFilter[i]->setType(CookbookEq::HiPass2);
		HiPassFilter[i]->setFreq(LOW_CUT_FREQ_DEFAULT);
		finalIR[i] = new TPrecision[MAX_SAMPLERATE * (MAX_DURATION + MAX_PREDELAY)];
		earlyIR[i] = new TPrecision[MAX_SAMPLERATE * MAX_EARLY];
		lateIR[i] = new TPrecision[MAX_SAMPLERATE * MAX_DURATION];
	}
	ImpulseLowPassFilter = new CookbookEq(CookbookEq::LoPass2, 100.0f, 1);
	ImpulseLowPassFilter->prepareToPlay(float(sdkGetSampleRate()), sdkGetBlocSize());
	ImpulseLowPassFilter->setType(CookbookEq::LoPass2);
	

	cptDeactivation = 0;
	irFinalSizeSp = 0;

	calculateContants();
	createEarly();
	createLate();
	mixAndLoadIR();
	loadingIRSate = TLoadingIRState::lsProcessing;
}

//----------------------------------------------------------------------------
void FFConvolver::onBlocSizeChange(int BlocSize)
{
	sdkLockPatch();
	for (int i = 0; i < numOfAudiotInsOuts; i++)
	{
		HiPassFilter[i]->prepareToPlay(float(sdkGetSampleRate()), sdkGetBlocSize());
	}
	ImpulseLowPassFilter->prepareToPlay(float(sdkGetSampleRate()), sdkGetBlocSize());
	sdkUnLockPatch();
};

//----------------------------------------------------------------------------
void FFConvolver::onSampleRateChange(double SampleRate)
{
	sdkLockPatch();
	for (int i = 0; i < numOfAudiotInsOuts; i++)
	{
		HiPassFilter[i]->prepareToPlay(float(sdkGetSampleRate()), sdkGetBlocSize());
	}
	ImpulseLowPassFilter->prepareToPlay(float(sdkGetSampleRate()), sdkGetBlocSize());
	sdkUnLockPatch();
};

//----------------------------------------------------------------------------
// parameters and process
//----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Parameters description
void FFConvolver::onGetParamInfo (int ParamIndex, TParamInfo* pParamInfo)
{	
    // audioInputs
    if (ParamIndex < numOfAudiotInsOuts)
    {
		pParamInfo->ParamType		= ptAudio;
		pParamInfo->Caption			= sdkGetAudioQueryChannelNames ( "in ", ParamIndex + 1, queryIndex);
		pParamInfo->IsInput			= TRUE;
		pParamInfo->IsOutput		= FALSE;
		pParamInfo->ReadOnly		= FALSE;
		pParamInfo->EventPtr        = &audioInputs[ParamIndex];

        if (ParamIndex == 0)
        {
            pParamInfo->IsSeparator     = TRUE;
            pParamInfo->SeparatorCaption = "audio in";
        }
    }
    // audioOutputs
    else if (ParamIndex >= numOfAudiotInsOuts && ParamIndex < (numOfAudiotInsOuts*2))
    {
		pParamInfo->ParamType		= ptAudio;
		pParamInfo->Caption			= sdkGetAudioQueryChannelNames ( "out ", ParamIndex - numOfAudiotInsOuts + 1, queryIndex);
		pParamInfo->IsInput			= FALSE;
		pParamInfo->IsOutput		= TRUE;
		pParamInfo->ReadOnly		= TRUE;
		pParamInfo->EventPtr        = &audioOutputs[ParamIndex - numOfAudiotInsOuts];

        if (ParamIndex == numOfAudiotInsOuts)
        {
            pParamInfo->IsSeparator     = TRUE;
            pParamInfo->SeparatorCaption = "audio out";
        }
    }

	// fdrOnOff
	else if (ParamIndex == (numOfAudiotInsOuts * 2) )
	{
		pParamInfo->ParamType = ptLeftLed;
		pParamInfo->Caption = "on";
		pParamInfo->IsInput = TRUE;
		pParamInfo->IsOutput = FALSE;
		pParamInfo->IsSeparator = TRUE;
		pParamInfo->CallBackType = ctImmediate;
		pParamInfo->CallBackId = 0xFFD;
		pParamInfo->EventPtr = &fdrOnOff;
		pParamInfo->DefaultValue = 1.0f;
		pParamInfo->SeparatorCaption = "settings";
	}

	// fdrDryWet
	else if (ParamIndex == (numOfAudiotInsOuts * 2) +1 )
	{
		pParamInfo->ParamType = ptDataFader;
		pParamInfo->Caption = "dry-wet";
		pParamInfo->IsInput = TRUE;
		pParamInfo->IsOutput = FALSE;
		pParamInfo->IsSeparator = FALSE;
		pParamInfo->CallBackType = ctNormal;
		pParamInfo->CallBackId = 0xFFA;
		pParamInfo->EventPtr = &fdrDryWet;
		pParamInfo->DefaultValue = 1.0f;

	}
	
	// fdrLowCutFreq
	else if (ParamIndex == (numOfAudiotInsOuts * 2) + 2)
	{
		pParamInfo->ParamType = ptDataFader;
		pParamInfo->Caption = "low-cut freq";
		pParamInfo->IsInput = TRUE;
		pParamInfo->IsOutput = FALSE;
		pParamInfo->IsSeparator = FALSE;
		pParamInfo->CallBackType = ctImmediate;
		pParamInfo->CallBackId = 0xFFC;
		pParamInfo->EventPtr = &fdrLowCutFreq;
		pParamInfo->DefaultValue = LOW_CUT_FREQ_DEFAULT;
		pParamInfo->Scale = scLog;
		pParamInfo->MinValue = LOW_CUT_FREQ_DEFAULT;
		pParamInfo->MaxValue = 4000.0f;
		pParamInfo->Symbol = "Hz";
		pParamInfo->Format = "%.1f";
	}
	// fdrPreDelay
	else if (ParamIndex == (numOfAudiotInsOuts * 2) + 3)
	{
		pParamInfo->ParamType = ptDataFader;
		pParamInfo->Caption = "pre-delay";
		pParamInfo->IsInput = TRUE;
		pParamInfo->IsOutput = FALSE;
		pParamInfo->IsSeparator = FALSE;
		pParamInfo->CallBackType = ctNormal;
		pParamInfo->CallBackId = 0xFFB1;
		pParamInfo->EventPtr = &fdrPreDelay;
		pParamInfo->DefaultValue = 0.0f;
		pParamInfo->Scale = scLog;
		pParamInfo->MinValue = 0.0f;
		pParamInfo->MaxValue = MAX_PREDELAY;
		pParamInfo->Symbol = "ms";
		pParamInfo->Format = "%.1f";
	}

	// fdrDuration
	else if (ParamIndex == (numOfAudiotInsOuts * 2) + 4)
	{
		pParamInfo->ParamType = ptDataFader;
		pParamInfo->Caption = "duration";
		pParamInfo->IsInput = TRUE;
		pParamInfo->IsOutput = FALSE;
		pParamInfo->IsSeparator = TRUE;
		pParamInfo->CallBackType = ctNormal;
		pParamInfo->CallBackId = 0xFFB3;
		pParamInfo->EventPtr = &fdrDuration;
		pParamInfo->DefaultValue = 2000.0f;
		pParamInfo->Scale = scLog;
		pParamInfo->MinValue = 100.0f;
		pParamInfo->MaxValue = MAX_DURATION;
		pParamInfo->Symbol = "ms";
		pParamInfo->Format = "%.0f";
	}

	// fdrEarlyLevel
	else if (ParamIndex == (numOfAudiotInsOuts * 2) + 5)
	{
		pParamInfo->ParamType = ptDataFader;
		pParamInfo->Caption = "early level";
		pParamInfo->IsInput = TRUE;
		pParamInfo->IsOutput = FALSE;
		pParamInfo->IsSeparator = TRUE;
		pParamInfo->CallBackType = ctNormal;
		pParamInfo->CallBackId = 0xFFB1;
		pParamInfo->EventPtr = &fdrEarlyLevel;
		pParamInfo->DefaultValue = 1.0f;
		pParamInfo->Scale = scLog;
		pParamInfo->MinValue = 0.0f;
		pParamInfo->MaxValue = 1.0f;
		pParamInfo->Symbol = "";
		pParamInfo->Format = "%.3f";
	}
	// fdrEarlyDuration
	else if (ParamIndex == (numOfAudiotInsOuts * 2) + 6)
	{
	pParamInfo->ParamType = ptDataFader;
	pParamInfo->Caption = "early duration";
	pParamInfo->IsInput = TRUE;
	pParamInfo->IsOutput = FALSE;
	pParamInfo->IsSeparator = FALSE;
	pParamInfo->CallBackType = ctNormal;
	pParamInfo->CallBackId = 0xFFB2;
	pParamInfo->EventPtr = &fdrEarlyDuration;
	pParamInfo->DefaultValue = 10.0f;
	pParamInfo->Scale = scLog;
	pParamInfo->MinValue = 0.0f;
	pParamInfo->MaxValue = MAX_EARLY;
	pParamInfo->Symbol = "ms";
	pParamInfo->Format = "%.1f";
	}
	// fdrEarlyDensity
	else 
	if (ParamIndex == (numOfAudiotInsOuts * 2) + 7)
	{
	pParamInfo->ParamType = ptDataFader;
	pParamInfo->Caption = "early density";
	pParamInfo->IsInput = TRUE;
	pParamInfo->IsOutput = FALSE;
	pParamInfo->IsSeparator = FALSE;
	pParamInfo->CallBackType = ctNormal;
	pParamInfo->CallBackId = 0xFFB2;
	pParamInfo->EventPtr = &fdrEarlyDensity;
	pParamInfo->DefaultValue = 0.3f;
	pParamInfo->Scale = scLog;
	pParamInfo->MinValue = 0.0f;
	pParamInfo->MaxValue = 1.0f;
	pParamInfo->Symbol = "";
	pParamInfo->Format = "%.3f";
	}

	// fdrLateLevel
	else if (ParamIndex == (numOfAudiotInsOuts * 2) + 8)
	{
	pParamInfo->ParamType = ptDataFader;
	pParamInfo->Caption = "late level";
	pParamInfo->IsInput = TRUE;
	pParamInfo->IsOutput = FALSE;
	pParamInfo->IsSeparator = TRUE;
	pParamInfo->CallBackType = ctNormal;
	pParamInfo->CallBackId = 0xFFB1;
	pParamInfo->EventPtr = &fdrLateLevel;
	pParamInfo->DefaultValue = 0.5f;
	pParamInfo->Scale = scLog;
	pParamInfo->MinValue = 0.0f;
	pParamInfo->MaxValue = 1.0f;
	pParamInfo->Symbol = "";
	pParamInfo->Format = "%.3f";
	}

	// fdrLateSlope
	else if (ParamIndex == (numOfAudiotInsOuts * 2) + 9)
	{
	pParamInfo->ParamType = ptDataFader;
	pParamInfo->Caption = "late slope";
	pParamInfo->IsInput = TRUE;
	pParamInfo->IsOutput = FALSE;
	pParamInfo->IsSeparator = FALSE;
	pParamInfo->CallBackType = ctNormal;
	pParamInfo->CallBackId = 0xFFB3;
	pParamInfo->EventPtr = &fdrLateSlope;
	pParamInfo->DefaultValue = 0.5f;
	pParamInfo->Scale = scLinear;
	pParamInfo->MinValue = 0.0f;
	pParamInfo->MaxValue = 1.0f;
	pParamInfo->Symbol = "";
	pParamInfo->Format = "%.3f";
	}

	// fdrLateColor
	else if (ParamIndex == (numOfAudiotInsOuts * 2) + 10)
	{
	pParamInfo->ParamType = ptDataFader;
	pParamInfo->Caption = "late color";
	pParamInfo->IsInput = TRUE;
	pParamInfo->IsOutput = FALSE;
	pParamInfo->IsSeparator = FALSE;
	pParamInfo->CallBackType = ctNormal;
	pParamInfo->CallBackId = 0xFFB3;
	pParamInfo->EventPtr = &fdrLateColor;
	pParamInfo->DefaultValue = 0.5f;
	pParamInfo->Scale = scLinear;
	pParamInfo->MinValue = 0.0f;
	pParamInfo->MaxValue = 1.0f;
	pParamInfo->Symbol = "";
	pParamInfo->Format = "%.3f";
	}

}


//-----------------------------------------------------------------------------
// White Noise Generator

#define PI 3.1415926536

float AWGN_generator()
{/* Generates additive white Gaussian Noise samples with zero mean and a standard deviation of 1. */

	float temp1;
	float temp2;
	float result;
	int p;

	p = 1;

	while (p > 0)
	{
		temp2 = (rand() / ((float)RAND_MAX)); /*  rand() function generates an
														   integer between 0 and  RAND_MAX,
														   which is defined in stdlib.h.
													   */

		if (temp2 == 0)
		{// temp2 is >= (RAND_MAX / 2)
			p = 1;
		}// end if
		else
		{// temp2 is < (RAND_MAX / 2)
			p = -1;
		}// end else

	}// end while()

	temp1 = cosf((2.0f * (float)PI) * rand() / ((float)RAND_MAX));
	result = sqrtf(-2.0f * log(temp2)) * temp1;

	return result;	// return the generated random sample to the caller

}// end AWGN_generator()


//-----------------------------------------------------------------------------
// Calculate Conatants
void FFConvolver::calculateContants()
{
	try
	{
		preDelayDurationMs = int(sdkGetEvtData(fdrPreDelay));

		durationMs = sdkGetEvtData(fdrDuration);

		earlyDensity = sdkGetEvtData(fdrEarlyDensity);
		earlyDurationMs = sdkGetEvtData(fdrEarlyDuration);
		earlyLevel = sdkGetEvtData(fdrEarlyLevel);

		lateLevel = sdkGetEvtData(fdrLateLevel);

		lateSlope = sdkGetEvtData(fdrLateSlope);
		lateColor = sdkGetEvtData(fdrLateColor);

		lateDurationSp = int(sdkGetSampleRate() * std::max(durationMs, earlyDurationMs) / 1000.0f);
		preDelayDurationSp = int(preDelayDurationMs * sdkGetSampleRate() / 1000.0f);
		earlyDurationSp = int(earlyDurationMs * sdkGetSampleRate() / 1000.0f);

		irFinalSizeSp = preDelayDurationSp + lateDurationSp;
	}
	catch (...)
	{
		sdkTraceErrorChar("error in calculate constants");
	}
}


//-----------------------------------------------------------------------------
// Create Early
void FFConvolver::createEarly()
{
	try
	{
		for (int ch = 0; ch < numOfAudiotInsOuts; ch++)
		{
			::memset(earlyIR[ch], 0, earlyDurationSp * sizeof(TPrecision));
			// generate EARLY
			if (earlyDurationSp > 0)
			{
				float dens = float(earlyDurationSp);
				float incrdes = 1.0f - dens;
				// first 
				earlyIR[ch][0] = 1.0f;
				for (int i = 1; i < earlyDurationSp; i++)
				{					
					if (abs(AWGN_generator()) < (dens+incrdes*float(i/ earlyDurationSp)))
					{
						float slope = sqr((float(earlyDurationSp - i)) / float(earlyDurationSp));
						earlyIR[ch][i] = AWGN_generator() * slope;
					}
				}
			}
		}
	}
	catch (...)
	{
        	sdkTraceErrorChar("unable to load convolution file");
	}
}


//-----------------------------------------------------------------------------
// LoadIR
void FFConvolver::createLate()
{
	try
	{


		for (int ch = 0; ch < numOfAudiotInsOuts; ch++)
		{

			::memset(lateIR[ch], 0, lateDurationSp * sizeof(TPrecision));

			float freqMax = 100.0f + lateColor * lateColor * 10000.0f;
			float freqMin = 100.0f;
			float freq;
			float slopeLinearEarly, slopeLinear, slope;
			float powerSlope = lateSlope * 4 + 1;

			ImpulseLowPassFilter->setType(CookbookEq::LoPass1);
			ImpulseLowPassFilter->cleanup();


			// generate LATE calculate after the Eraly fade in
			for (int i = 0; i < lateDurationSp; i++)
			{
				slopeLinear = (float(lateDurationSp - i)) / float(lateDurationSp);
				if (i < earlyDurationSp)
				{
					slopeLinearEarly = sqr(1.0f - float(earlyDurationSp - i) / float(earlyDurationSp));
				}
				else slopeLinearEarly = 1;
				slope = powf(slopeLinear, powerSlope * (slopeLinear + 1));
				freq = freqMin + (freqMax - freqMin) * slopeLinear;

				lateIR[ch][i] = AWGN_generator() * slope * slopeLinearEarly;
				ImpulseLowPassFilter->setFreq(freq);
				ImpulseLowPassFilter->filterOut(&lateIR[ch][i], 1);
			}
		}

	}
	catch (...)
	{
		sdkTraceErrorChar("unable to load convolution file");
	}
	
}



//-----------------------------------------------------------------------------
// LoadIR
void FFConvolver::mixAndLoadIR()
{
	try
	{

		int blocSize;
		if (sdkGetSampleRate() >= 176400.0)
		{
			blocSize = sdkGetBlocSize() * int(32);
		}
		else
		if (sdkGetSampleRate() >= 88200.0)
		{
			blocSize = sdkGetBlocSize() * int(16);
		}
		else
		{
			blocSize = sdkGetBlocSize() * int(8);
		}


		for (int ch = 0; ch < numOfAudiotInsOuts; ch++)
		{

 			   ::memset(finalIR[ch], 0, irFinalSizeSp * sizeof(TPrecision));
			   
			   for (int i = 0; i < earlyDurationSp; i++)
			   {
				   finalIR[ch][i+preDelayDurationSp] = earlyIR[ch][i] * earlyLevel;
			   }
			   for (int i = 0; i < lateDurationSp; i++)
			   {
				   finalIR[ch][i+ preDelayDurationSp] += lateIR[ch][i] * lateLevel;
			   }
			   // normalization
			   float sumofsquare = 0.0f;
			   for (int i = 0; i < irFinalSizeSp; i++)
			   {
				   sumofsquare += finalIR[ch][i] * finalIR[ch][i];
			   }

			   if (sumofsquare > 0)
			   {
				   sumofsquare = 1 / sqrt(sumofsquare);
				   for (int i = 0; i < irFinalSizeSp; i++)
				   {
					   finalIR[ch][i] *= sumofsquare;
				   }
			   }
			   Convolver[ch].init(blocSize, &finalIR[ch][0], irFinalSizeSp, true);

		}
  		    
	}
	catch(...)
	{
		sdkTraceErrorChar("unable to set convolution");
	}
	sdkNotifyUsine(NOTIFY_TARGET_USER_MODULE, NOTIFY_MSG_USINE_CALLBACK, NOTIFY_LOAD_IR_FINISHED);
}


//-----------------------------------------------------------------------------
// Parameters callback
void FFConvolver::onCallBack (UsineMessage *Message)
{
    // filter only message specific to this user module
    if (Message->message == NOTIFY_MSG_USINE_CALLBACK)
    {
		
		// FileName,SizeFactor,Predelay
		if ((Message->wParam == 0xFFB1) && (Message->lParam == MSG_CHANGE))
		{
			if (loadingIRSate == TLoadingIRState::lsProcessing)
			{
				loadingIRSate = TLoadingIRState::lsAskedMix;
			}
			else
			{
				sdkNotifyUsine(NOTIFY_TARGET_USER_MODULE, NOTIFY_MSG_USINE_CALLBACK, 0xFFB1);
			}
		}
		else
			if ((Message->wParam == 0xFFB2) && (Message->lParam == MSG_CHANGE))
			{
				if (loadingIRSate == TLoadingIRState::lsProcessing)
				{
					loadingIRSate = TLoadingIRState::lsAskedEarly;
				}
				else
				{
					sdkNotifyUsine(NOTIFY_TARGET_USER_MODULE, NOTIFY_MSG_USINE_CALLBACK, 0xFFB2);
				}
			}
		else
		if ((Message->wParam == 0xFFB3) && (Message->lParam == MSG_CHANGE))
		{
			if (loadingIRSate == TLoadingIRState::lsProcessing)
			{
				loadingIRSate = TLoadingIRState::lsAskedLate;
			}
			else
			{
				sdkNotifyUsine(NOTIFY_TARGET_USER_MODULE, NOTIFY_MSG_USINE_CALLBACK, 0xFFB3);
			}
		}
		else
		// HiPassFreq
		if ((Message->wParam == 0xFFC) && (Message->lParam == MSG_CHANGE))
		{
			for (int i = 0; i < numOfAudiotInsOuts; i++)
			{
				HiPassFilter[i]->setFreq(sdkGetEvtData(fdrLowCutFreq));
			}
		}
		else
		if (Message->wParam == NOTIFY_START_CREATE_IR_EARLY)
			{
			    calculateContants();
				createEarly();
				mixAndLoadIR();
			}
		else
		if (Message->wParam == NOTIFY_START_CREATE_IR_LATE)
			{
			    calculateContants();
			    createLate();
				mixAndLoadIR();
			}
	    else
		if (Message->wParam == NOTIFY_START_CREATE_IR_MIX)
		{
			calculateContants();
			mixAndLoadIR();
		}
	if (Message->wParam == NOTIFY_LOAD_IR_FINISHED)
		{
			loadingIRSate = TLoadingIRState::lsLoaded;
		}
		else
		// OnOFF
		if ((Message->wParam == 0xFFD) && (Message->lParam == MSG_CHANGE))
		{
			cptDeactivation = irFinalSizeSp;
		}
    }
}

//-----------------------------------------------------------------------------
// Clear Audio Out
void FFConvolver::clearAudioOut()
{
	for (int i = 0; i < numOfAudiotInsOuts; i++)
	{
		sdkClearAudioEvt(audioOutputs[i]);
	}
}



//-----------------------------------------------------------------------------
// Process
void FFConvolver::onProcess ()
{

	if ((numOfAudiotInsOuts > 0) && (loadingIRSate != TLoadingIRState::lsLoading))
	{
		if (sdkPatchJustActivated())
		{
			for (int i = 0; i < numOfAudiotInsOuts; i++)
			{
				Convolver[i].clear();
			}

		}
		float dryWet = sdkGetEvtData(fdrDryWet);
		float onoffstate = sdkGetEvtData(fdrOnOff);

		// denormalization
		for (int i = 0; i < numOfAudiotInsOuts; i++)
		{
			sdkDenormalizeAudioEvt(audioInputs[i]);
		}

		if ((onoffstate == 1.0f) || (cptDeactivation > 0))
		{
			for (int i = 0; i < numOfAudiotInsOuts; i++)
			{
				sdkCopyEvt(audioInputs[i], audioOutputs[i]);
				//sdkTraceInt(sdkGetEvtSize(audioInputs[i]));
				if (onoffstate != 1.0f)
				{
					sdkClearAudioEvt(audioOutputs[i]);
				}
				if (sdkGetEvtData(fdrLowCutFreq) > LOW_CUT_FREQ_DEFAULT)
				{
					HiPassFilter[i]->filterOut(sdkGetEvtDataAddr(audioOutputs[i]), sdkGetBlocSize());
				}

				Convolver[i].process(sdkGetEvtDataAddr(audioOutputs[i]), &audioOutTmp[0], sdkGetBlocSize());
				::memcpy(sdkGetEvtDataAddr(audioOutputs[i]), &audioOutTmp[0], sdkGetBlocSize() * sizeof(TPrecision));

				if (dryWet < 1.0f)
				{
					sdkMultEvt1(1.0f - dryWet, audioInputs[i]);
					sdkMultEvt1(dryWet, audioOutputs[i]);
					sdkAddEvt2(audioInputs[i], audioOutputs[i]);
				}
			}
			if (onoffstate != 1.0f)
			{
				cptDeactivation -= sdkGetBlocSize();
			}
		}
		else
		{
			for (int i = 0; i < numOfAudiotInsOuts; i++)
			{
				if (dryWet < 1.0)
				{
					sdkCopyEvt(audioInputs[i], audioOutputs[i]);
					sdkMultEvt1(1.0f - dryWet, audioOutputs[i]);
				}
				else sdkClearAudioEvt(audioOutputs[i]);
			}
		}
	}
	else clearAudioOut();
	
	switch (loadingIRSate)
	{
		case TLoadingIRState::lsAskedEarly:
			{
			for (int i = 0; i < numOfAudiotInsOuts; i++)
				{
					sdkFadeOutAudioEvt(audioOutputs[i]);
				}
 				sdkNotifyUsine(NOTIFY_TARGET_USER_MODULE, NOTIFY_MSG_USINE_CALLBACK, NOTIFY_START_CREATE_IR_EARLY);
				loadingIRSate = TLoadingIRState::lsLoading;
				break;
			}
		case TLoadingIRState::lsAskedLate:
		{
			for (int i = 0; i < numOfAudiotInsOuts; i++)
			{
				sdkFadeOutAudioEvt(audioOutputs[i]);
			}
			sdkNotifyUsine(NOTIFY_TARGET_USER_MODULE, NOTIFY_MSG_USINE_CALLBACK, NOTIFY_START_CREATE_IR_LATE);
			loadingIRSate = TLoadingIRState::lsLoading;
			break;
		}
		case TLoadingIRState::lsAskedMix:
		{
			for (int i = 0; i < numOfAudiotInsOuts; i++)
			{
				sdkFadeOutAudioEvt(audioOutputs[i]);
			}
			sdkNotifyUsine(NOTIFY_TARGET_USER_MODULE, NOTIFY_MSG_USINE_CALLBACK, NOTIFY_START_CREATE_IR_MIX);
			loadingIRSate = TLoadingIRState::lsLoading;
			break;
		}

		case TLoadingIRState::lsLoaded:
		{
			for (int i = 0; i < numOfAudiotInsOuts; i++)
			{
				sdkFadeInAudioEvt(audioOutputs[i]);
			}
			loadingIRSate = TLoadingIRState::lsProcessing;
			break;
		}
		case TLoadingIRState::lsLoading:
		{
			clearAudioOut();
			break;
		}
	}

}

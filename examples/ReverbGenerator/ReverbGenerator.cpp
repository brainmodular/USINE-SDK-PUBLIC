//-----------------------------------------------------------------------------
//@file  
//	ReverbGenerator.cpp
//
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
#include "TwoStageFFTConvolver.h"
#include "CookbookEq.h"

#define LOW_CUT_FREQ_DEFAULT 20.0f
#define DAMPING_FREQ_MAX 15000.0f
#define DAMPING_FREQ_MIN 200.0f
#define LN4 1.386294361119f
#define EARLYDENSITY 0.01f



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
	createAndLoadIR();
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
	}
	ImpulseLowPassFilter = new CookbookEq(CookbookEq::LoPass2, DAMPING_FREQ_MAX, 1);
	ImpulseLowPassFilter->prepareToPlay(float(sdkGetSampleRate()), sdkGetBlocSize());
	ImpulseLowPassFilter->setType(CookbookEq::LoPass2);
	ImpulseLowPassFilter->setFreq(DAMPING_FREQ_MAX);
	cptDeactivation = 0;
	irFinalSize = 0;
	
	createAndLoadIR();
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
	
	// fdrHiPassFreq
	else if (ParamIndex == (numOfAudiotInsOuts * 2) + 2)
	{
		pParamInfo->ParamType = ptDataFader;
		pParamInfo->Caption = "low-cut freq";
		pParamInfo->IsInput = TRUE;
		pParamInfo->IsOutput = FALSE;
		pParamInfo->IsSeparator = FALSE;
		pParamInfo->CallBackType = ctImmediate;
		pParamInfo->CallBackId = 0xFFC;
		pParamInfo->EventPtr = &fdrHiPassFreq;
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
		pParamInfo->CallBackId = 0xFFB;
		pParamInfo->EventPtr = &fdrPreDelay;
		pParamInfo->DefaultValue = 0.0f;
		pParamInfo->Scale = scLog;
		pParamInfo->MinValue = 0.0f;
		pParamInfo->MaxValue = 200.0f;
		pParamInfo->Symbol = "ms";
		pParamInfo->Format = "%.1f";
	}

	
	// fdrDamping
	else if (ParamIndex == (numOfAudiotInsOuts * 2) + 4)
	{
		pParamInfo->ParamType = ptDataFader;
		pParamInfo->Caption = "damping freq";
		pParamInfo->IsInput = TRUE;
		pParamInfo->IsOutput = FALSE;
		pParamInfo->IsSeparator = TRUE;
		pParamInfo->CallBackType = ctNormal;
		pParamInfo->CallBackId = 0xFFB;
		pParamInfo->EventPtr = &fdrDampingFreq;
		pParamInfo->DefaultValue = 3000;
		pParamInfo->Scale = scLog;
		pParamInfo->MinValue = DAMPING_FREQ_MIN;
		pParamInfo->MaxValue = DAMPING_FREQ_MAX;
		pParamInfo->Symbol = "Hz";
		pParamInfo->Format = "%.0f";
	}
	// fdrDecay
	else if (ParamIndex == (numOfAudiotInsOuts * 2) + 5)
	{
		pParamInfo->ParamType = ptDataFader;
		pParamInfo->Caption = "decay curve";
		pParamInfo->IsInput = TRUE;
		pParamInfo->IsOutput = FALSE;
		pParamInfo->IsSeparator = FALSE;
		pParamInfo->CallBackType = ctNormal;
		pParamInfo->CallBackId = 0xFFB;
		pParamInfo->EventPtr = &fdrDecay;
		pParamInfo->DefaultValue = 0.5f;
		pParamInfo->Scale = scLinear;
		pParamInfo->MinValue = 0.0f;
		pParamInfo->MaxValue = 1.0f;
		pParamInfo->Symbol = "";
		pParamInfo->Format = "%.3f";
	}	
	// fdrDensity
	else if (ParamIndex == (numOfAudiotInsOuts * 2) + 6)
	{
		pParamInfo->ParamType = ptDataFader;
		pParamInfo->Caption = "density";
		pParamInfo->IsInput = TRUE;
		pParamInfo->IsOutput = FALSE;
		pParamInfo->IsSeparator = FALSE;
		pParamInfo->CallBackType = ctNormal;
		pParamInfo->CallBackId = 0xFFB;
		pParamInfo->EventPtr = &fdrDensity;
		pParamInfo->DefaultValue = 0.5f;
		pParamInfo->Scale = scLinear;
		pParamInfo->MinValue = 0.001f;
		pParamInfo->MaxValue = 1.0f;
		pParamInfo->Symbol = "";
		pParamInfo->Format = "%.3f";
	}
	// fdrSize
	else if (ParamIndex == (numOfAudiotInsOuts * 2) + 7)
	{
		pParamInfo->ParamType = ptDataFader;
		pParamInfo->Caption = "duration";
		pParamInfo->IsInput = TRUE;
		pParamInfo->IsOutput = FALSE;
		pParamInfo->IsSeparator = FALSE;
		pParamInfo->CallBackType = ctNormal;
		pParamInfo->CallBackId = 0xFFB;
		pParamInfo->EventPtr = &fdrSize;
		pParamInfo->DefaultValue = 1000.0f;
		pParamInfo->Scale = scLog;
		pParamInfo->MinValue = 100.0f;
		pParamInfo->MaxValue = 10000.0f;
		pParamInfo->Symbol = "ms";
		pParamInfo->Format = "%.0f";
	}
	//fdrAirAbsorption
	else if (ParamIndex == (numOfAudiotInsOuts * 2) + 8)
	{
	pParamInfo->ParamType = ptDataFader;
	pParamInfo->Caption = "air absorption";
	pParamInfo->IsInput = TRUE;
	pParamInfo->IsOutput = FALSE;
	pParamInfo->IsSeparator = FALSE;
	pParamInfo->CallBackType = ctNormal;
	pParamInfo->CallBackId = 0xFFB;
	pParamInfo->EventPtr = &fdrAirAbsorption;
	pParamInfo->DefaultValue = 1.0f;
	pParamInfo->Scale = scLinear;
	pParamInfo->MinValue = 0.0f;
	pParamInfo->MaxValue = 1.0f;
	pParamInfo->Symbol = "";
	pParamInfo->Format = "%.2f";
	}

	//fdrEarlyAmount
	else 
	if (ParamIndex == (numOfAudiotInsOuts * 2) + 9)
	{
	pParamInfo->ParamType = ptDataFader;
	pParamInfo->Caption = "early size";
	pParamInfo->IsInput = TRUE;
	pParamInfo->IsOutput = FALSE;
	pParamInfo->IsSeparator = FALSE;
	pParamInfo->CallBackType = ctNormal;
	pParamInfo->CallBackId = 0xFFB;
	pParamInfo->EventPtr = &fdrEarlySize;
	pParamInfo->DefaultValue = 0.05f;
	pParamInfo->Scale = scLog;
	pParamInfo->MinValue = 0.0f;
	pParamInfo->MaxValue = 1.0f;
	pParamInfo->Symbol = "";
	pParamInfo->Format = "%.2f";
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

//float b0, b1, b2;
//
//float PinkNoise()
//{
//	float white = AWGN_generator();
//    b0 = 0.99765 * b0 + white * 0.0990460;
//    b1 = 0.96300 * b1 + white * 0.2965164;
//    b2 = 0.57000 * b2 + white * 1.0526913;
//    return (b0 + b1 + b2 + white * 0.1848) / 4.0;
//
//}


//-----------------------------------------------------------------------------
// LoadIR
void FFConvolver::createAndLoadIR()
{
	try
	{

			irSize = int(sdkGetSampleRate()*sdkGetEvtData(fdrSize)/1000.0f);

			int preDelaySize = int(sdkGetEvtData(fdrPreDelay) * sdkGetSampleRate() / 1000.0f);
			irFinalSize = preDelaySize + irSize;
			int earlySize = int(float(irSize) * sdkGetEvtData(fdrEarlySize));

			float airAbsorption = sdkGetEvtData(fdrAirAbsorption);
			// air absorption formula power coeff =exp(2*fdrAirAbsorption-1)*ln(4)
			float powcoeff = expf((2.0f * airAbsorption - 1) * LN4);

			TPrecision* IRFiltered = new TPrecision[irSize];
			TPrecision* IRNoFiltered = new TPrecision[irSize];


			TPrecision* finalIR = new TPrecision[irFinalSize];

			float decaycoeff = (sdkGetEvtData(fdrDecay));
			float density =sdkGetEvtData(fdrDensity);
			float exponentialFactor = float(decaycoeff) + 1.71828182845904f;
			float endvalue = 1.0f / powf(exponentialFactor, 10.0f);
			

			for (int ch = 0; ch < numOfAudiotInsOuts; ch++)
			{
				
				::memset(IRNoFiltered, 0, irSize * sizeof(TPrecision));
				::memset(finalIR, 0, irFinalSize * sizeof(TPrecision));
				::memset(IRFiltered, 0, irSize * sizeof(TPrecision));
				

				float v;
				float dens;
				// generate impulse
				for (int i = 0; i < irSize; i++)
				{	
					float iss = float(i) / float(irSize);
					dens = density + (1.0f - density) * iss;
					dens = dens * dens;

					v = 0;
					if (i < earlySize)						
					{
						float earlycurve = float(i) / float(earlySize);
						float rnd = AWGN_generator();
						if (abs(rnd) < EARLYDENSITY)
						{
							v = 16.0f*(1 - earlycurve);
							if (rnd < 0)
							{
								v = -v;
							}								
						}
						else
						{
							if (abs(AWGN_generator()) < dens)
							{
								v = AWGN_generator() *  earlycurve;
							}
						}																				
					}
					else
					{
						if (abs(AWGN_generator()) < dens)
						{
							v = AWGN_generator();
						}
					}
					v = v / powf(exponentialFactor,10.0f*iss)-endvalue;
					IRNoFiltered[i] = v;					
				}

				// one impulse is filtered 6db/oct
				ImpulseLowPassFilter->setType(CookbookEq::LoPass1);
				ImpulseLowPassFilter->cleanup();
				ImpulseLowPassFilter->setFreq(sdkGetEvtData(fdrDampingFreq));
				ImpulseLowPassFilter->filterOut(&IRNoFiltered[0], irSize);

				// apply a smooth on the IR
				// the smooth is stronger at the end of the IR
				float val,oldv,smooth;
				oldv = IRNoFiltered[0];
				for (int i = 1; i < irSize; i++)
				{
					smooth = float(irSize - i) / float(irSize);
					smooth = powf(smooth, powcoeff);
					
					val = oldv * (1.0f - smooth);
					val = val + float(IRNoFiltered[i]) * smooth;
					oldv = val;
					IRFiltered[i] = float(val);	
				}
				IRFiltered[irSize - 1] = 0.0f;

				// normalization
				float sumofsquare = 0.0f;
				for (int i = 0; i < irSize; i++)
				{
					sumofsquare += IRFiltered[i] * IRFiltered[i];
				}

				if (sumofsquare > 0)
				{
					sumofsquare = 1 / sqrt(sumofsquare);
					for (int i = 0; i < irSize; i++)
					{
						IRFiltered[i] = IRFiltered[i]*sumofsquare;
					}
				}
				::memcpy(&finalIR[preDelaySize], &IRFiltered[0], irSize * sizeof(TPrecision));
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


				Convolver[ch].init(blocSize, &finalIR[0], irFinalSize, true);

			}
  		    
			delete[] finalIR;
			delete[] IRFiltered;
			delete[] IRNoFiltered;

	}
	catch(...)
	{
		sdkTraceErrorChar("unable to load convolution file");
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
		if ((Message->wParam == 0xFFB) && (Message->lParam == MSG_CHANGE))
		{
			if (loadingIRSate == TLoadingIRState::lsProcessing)
			{
				loadingIRSate = TLoadingIRState::lsAsked;
			}
			else
			{
				sdkNotifyUsine(NOTIFY_TARGET_USER_MODULE, NOTIFY_MSG_USINE_CALLBACK, 0xFFB);
			}
		}
		else
		// HiPassFreq
		if ((Message->wParam == 0xFFC) && (Message->lParam == MSG_CHANGE))
		{
			for (int i = 0; i < numOfAudiotInsOuts; i++)
			{
				HiPassFilter[i]->setFreq(sdkGetEvtData(fdrHiPassFreq));
			}
		}
		else
		if (Message->wParam == NOTIFY_START_LOAD_IR)
		{
			createAndLoadIR();
		}
		else
    	if (Message->wParam == NOTIFY_LOAD_IR_FINISHED)
		{
			loadingIRSate = TLoadingIRState::lsLoaded;
		}
		else
		// OnOFF
		if ((Message->wParam == 0xFFD) && (Message->lParam == MSG_CHANGE))
		{
			cptDeactivation = irFinalSize;
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
		float dryWet = sdkGetEvtData(fdrDryWet);
		float onoffstate = sdkGetEvtData(fdrOnOff);

		if (sdkPatchJustActivated())
		{
			for (int i = 0; i < numOfAudiotInsOuts; i++)
			{
				Convolver[i].clear();
			}

		}
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
				if (onoffstate != 1.0f)
				{
					sdkClearAudioEvt(audioOutputs[i]);
				}
				if (sdkGetEvtData(fdrHiPassFreq) > LOW_CUT_FREQ_DEFAULT)
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
		case TLoadingIRState::lsAsked:
			{
			for (int i = 0; i < numOfAudiotInsOuts; i++)
				{
					sdkFadeOutAudioEvt(audioOutputs[i]);
				}
 				sdkNotifyUsine(NOTIFY_TARGET_USER_MODULE, NOTIFY_MSG_USINE_CALLBACK, NOTIFY_START_LOAD_IR);
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

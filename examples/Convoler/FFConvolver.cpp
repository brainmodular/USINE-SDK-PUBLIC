//-----------------------------------------------------------------------------
//@file  
//	AudioConvolverExample.cpp
//
//@author
//	Olivier Sens aka 'senso'
//
//@brief 
//	Implementation of the convolution Reverb class.
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
#include "FFConvolver.h"
#include "TwoStageFFTConvolver.h"
#include "CookbookEq.h"

#define LOW_CUT_FREQ_DEFAULT 20.0f

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
const AnsiCharPtr UserModuleBase::MODULE_NAME = "Reverb Convolver";
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

	sdkDestroyAudioFile(irFile);

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
// initialisation
void FFConvolver::onInitModule (MasterInfo* pMasterInfo, ModuleInfo* pModuleInfo) 
{
	irFile = sdkCreateAudioFile();
	for (int i = 0; i < numOfAudiotInsOuts; i++)
	{
		HiPassFilter[i] = new CookbookEq(CookbookEq::HiPass2, LOW_CUT_FREQ_DEFAULT, 1);
		HiPassFilter[i]->prepareToPlay(float(sdkGetSampleRate()), sdkGetBlocSize());
		HiPassFilter[i]->setType(CookbookEq::HiPass2);
		HiPassFilter[i]->setFreq(LOW_CUT_FREQ_DEFAULT);
	}
	cptDeactivation = 0;
	irFinalSize = 0;
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
	sdkUnLockPatch();
};

//-----------------------------------------------------------------------------
// Called after loading
void FFConvolver::onAfterLoading()
{
	loadIR();
	loadingIRSate = TLoadingIRState::lsProcessing;
}


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
	else if (ParamIndex == (numOfAudiotInsOuts * 2))
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
	else if (ParamIndex == (numOfAudiotInsOuts * 2) + 1)
	{
		pParamInfo->ParamType = ptDataFader;
		pParamInfo->Caption = "dry-wet";
		pParamInfo->IsInput = TRUE;
		pParamInfo->IsOutput = FALSE;
		pParamInfo->IsSeparator = TRUE;
		pParamInfo->CallBackType = ctNormal;
		pParamInfo->CallBackId = 0xFFA;
		pParamInfo->SeparatorCaption = "settings";
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
	// fdrFileName
	else if (ParamIndex == (numOfAudiotInsOuts * 2) + 4)
	{
		pParamInfo->ParamType = ptFileName;
		pParamInfo->Caption = "file name";
		pParamInfo->IsInput = TRUE;
		pParamInfo->IsOutput = FALSE;
		pParamInfo->IsSeparator = TRUE;
		pParamInfo->CallBackType = ctNormal;
		pParamInfo->CallBackId = 0xFFB;
		pParamInfo->SeparatorCaption = "Impulse";
		pParamInfo->EventPtr = &fdrFileName;
	}
	// fdrSizeFactor
	else if (ParamIndex == (numOfAudiotInsOuts * 2) + 5)
	{
		pParamInfo->ParamType = ptDataFader;
		pParamInfo->Caption = "impulse size";
		pParamInfo->IsInput = TRUE;
		pParamInfo->IsOutput = FALSE;
		pParamInfo->IsSeparator = FALSE;
		pParamInfo->CallBackType = ctNormal;
		pParamInfo->CallBackId = 0xFFB;
		pParamInfo->EventPtr = &fdrSizeFactor;
		pParamInfo->DefaultValue = 1.0f;
		pParamInfo->Scale = scLinear;
		pParamInfo->MinValue = 0.01f;
		pParamInfo->MaxValue = 1.0f;
		pParamInfo->Symbol = "";
		pParamInfo->Format = "%.2f";
	}
	// fdrImpulseDur
	else if (ParamIndex == (numOfAudiotInsOuts * 2) + 6)
	{
	pParamInfo->ParamType = ptDataField;
	pParamInfo->Caption = "impulse duration";
	pParamInfo->IsInput = FALSE;
	pParamInfo->IsOutput = TRUE;
	pParamInfo->IsSeparator = FALSE;
	pParamInfo->CallBackType = ctNormal;
	pParamInfo->CallBackId = -1;
	pParamInfo->EventPtr = &fdrImpulseDur;
	pParamInfo->DefaultValue = 0.0f;
	pParamInfo->Scale = scLinear;
	pParamInfo->MinValue = 0.0f;
	pParamInfo->MaxValue = float(INT32_MAX);
	pParamInfo->Symbol = "ms";
	pParamInfo->Format = "%.1f";
	}
	// fdrNormalize
	else if (ParamIndex == (numOfAudiotInsOuts * 2) + 7)
	{
	pParamInfo->ParamType = ptRightLed;
	pParamInfo->Caption = "normalize";
	pParamInfo->IsInput = TRUE;
	pParamInfo->IsOutput = FALSE;
	pParamInfo->IsSeparator = FALSE;
	pParamInfo->CallBackType = ctNormal;
	pParamInfo->CallBackId = 0xFFB;
	pParamInfo->EventPtr = &fdrNormalize;
	pParamInfo->DefaultValue = 1.0f;
	pParamInfo->Scale = scLinear;
	}

}

//-----------------------------------------------------------------------------
// Parameters callback
void FFConvolver::loadIR()
{
	try
	{
		AnsiCharPtr fn = sdkGetEvtPChar(fdrFileName);
		
		sdkLoadInMemoryAudioFile(irFile, fn);
		if (sdkGetSizeAudioFile(irFile) > 0)
		{

			sdkResampleAudioFile(irFile, float(sdkGetSampleRate()) / float(sdkGetSampleRateAudioFile(irFile)));

			irFileSize = sdkGetSizeAudioFile(irFile);
			irNumChannels = sdkGetChannelAudioFile(irFile);


			TPrecision coeffcurve = 2.0f * sqr(sdkGetEvtData(fdrSizeFactor));
			TPrecision curvevalue;
			TPrecision* psample;
			float suminitial;
			float sumfinal;
			float sumofsquare;
			TPrecision correctionsum;

			sdkSetEvtData(fdrImpulseDur, sdkGetEvtData(fdrSizeFactor)*1000.0f * float(irFileSize) / float(sdkGetSampleRate()));

			for (int ch = 0; ch < irNumChannels; ch++)
			{
				irDataPointer[ch] = sdkGetSampleArrayAudioFile(irFile, ch + 1);


				if (coeffcurve != 2.0f)
				{
					// calcul de la somme des samples
					suminitial = 0;
					psample = irDataPointer[ch];
					for (int i = 0; i < irFileSize; i++)
					{
						suminitial += abs(*psample);
						psample++;
					}

					// apply curve
					psample = irDataPointer[ch];
					for (int i = 0; i < irFileSize; i++)
					{
						curvevalue = std::max(0.0f, float(-i / coeffcurve / irFileSize) + 1.0f);
						*psample = *psample * curvevalue;
						psample++;
					}

					// calcul de la somme des sample
					sumfinal = 0;
					psample = irDataPointer[ch];
					for (int i = 0; i < irFileSize; i++)
					{
						sumfinal += abs(*psample);
						psample++;
					}

					// appliquer la correction d'intensité
					correctionsum = TPrecision(sqrt(suminitial / sumfinal));
					if (correctionsum != 0)
					{
						psample = irDataPointer[ch];
						for (int i = 0; i < irFileSize; i++)
						{
							*psample = *psample * correctionsum;
							psample++;
						}
					}
				}
				if (sdkGetEvtData(fdrNormalize) == 1.0f)
				{
					// normalization
					sumofsquare = 0;
					psample = irDataPointer[ch];
					for (int i = 0; i < irFileSize; i++)
					{
						sumofsquare += (*psample) * (*psample);
						psample++;
					}

					if (sumofsquare > 0)
					{
						sumofsquare = 1 / sqrt(sumofsquare);
						psample = irDataPointer[ch];
						for (int i = 0; i < irFileSize; i++)
						{
							*psample *= sumofsquare;
							psample++;
						}
					}
				}
			}


			//sdkLockPatch();
			try
			{

				if (irFileSize > 0)
				{
					int preDelaySize = int(sdkGetEvtData(fdrPreDelay) * sdkGetSampleRate() / 1000.0f);
					irFinalSize = preDelaySize + irFileSize;
					TPrecision* finalIR = new TPrecision[irFinalSize];
					::memset(finalIR, 0, irFinalSize * sizeof(TPrecision));
					for (int ch = 0; ch < numOfAudiotInsOuts; ch++)
					{
						int j = ch % irNumChannels;
						::memcpy(&finalIR[preDelaySize], irDataPointer[j], irFileSize * sizeof(TPrecision));
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

				}
			}
			catch (...)
			{
				sdkTraceErrorChar("error on init convolution engine");
			}
			//sdkUnLockPatch();
		}
		else
			// filesize =0
		{
		    //sdkLockPatch();
		    try
			{
				for (int ch = 0; ch < irNumChannels; ch++)
				{
					Convolver[ch].reset();
				}
			}
			catch (...)
			{
				sdkTraceErrorChar("error on reset convolution engine");
			}
			//sdkUnLockPatch();

		}
		sdkClearAudioFile(irFile);

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
			loadIR();
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
	
	if ((irNumChannels > 0) && (loadingIRSate != TLoadingIRState::lsLoading))
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
				if (dryWet < 1.0)
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

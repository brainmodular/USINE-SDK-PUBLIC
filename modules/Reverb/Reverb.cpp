//-----------------------------------------------------------------------------
//@file  
//	Reverb.cpp
//
//@author
//	Martin FLEURENT aka 'martignasse'
//
//@brief 
//	Implementation of the Reverb class.
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
//  http://www.sensomusic.com/wiki2/doku.php?id=hollyhock:bugsreport 
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
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------
#include "Reverb.h"

//----------------------------------------------------------------------------
// create, general info and destroy methodes
//----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Create
void CreateModule (void* &pModule, AnsiCharPtr optionalString, LongBool Flag, MasterInfo* pMasterInfo, AnsiCharPtr optionalContent)
{
	pModule = new Reverb();
}

// destroy
void DestroyModule(void* pModule) 
{
	// cast is important to call the good destructor
	delete ((Reverb*)pModule);
}

// module constants for browser info and module info
const AnsiCharPtr UserModuleBase::MODULE_NAME = "Reverb";
const AnsiCharPtr UserModuleBase::MODULE_DESC = "Reverb";
const AnsiCharPtr UserModuleBase::MODULE_VERSION = "1.1";

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

const float Reverb::initialWet = 0.0f;
const float Reverb::initialDry = -200.0f;
const float Reverb::initialRoom = 95.0f; 
const float Reverb::initialDamp = 75.0f; 
const float Reverb::initialChannelsSeparation = 50.0f;
const float Reverb::initialMode = 0.0f;  



// constructor
Reverb::Reverb()
    : numOfAudiotInsOuts (2)
    , numOfParamAfterAudiotInOut (6)
//    , reverbModel (nullptr)
    , queryIndex (0)
{    
    for(int v = 0; v < AUDIO_INS_OUTS_MAX; ++v)
    {
       temp[v] = nullptr;
       temp2[v] = nullptr;
       processInput[v] = nullptr;
       processOutput[v] = nullptr;
    }
}

// destructor
Reverb::~Reverb()
{
    for(int v = 0; v < AUDIO_INS_OUTS_MAX; ++v)
    {
        if (temp[v] != nullptr)
            sdkDestroyEvt (temp[v]);
        
        if (temp2[v] != nullptr)
            sdkDestroyEvt (temp2[v]);
        
        if (processInput[v] != nullptr)
            sdkDestroyEvt (processInput[v]);
        
        if (processOutput[v] != nullptr)
            sdkDestroyEvt (processOutput[v]);
    }
}



//void Reverb::onCreate(AnsiCharPtr optionalString);
//void Reverb::onDestroy(); 
void Reverb::onGetModuleInfo (MasterInfo* pMasterInfo, ModuleInfo* pModuleInfo)
{
	pModuleInfo->Name				= MODULE_NAME;
	pModuleInfo->Description		= MODULE_DESC;
	pModuleInfo->ModuleType         = mtSimple;
	pModuleInfo->BackColor          = sdkGetUsineColor(clAudioModuleColor);
	pModuleInfo->NumberOfParams     = (numOfAudiotInsOuts * 2) + numOfParamAfterAudiotInOut;
	pModuleInfo->DontProcess		= FALSE;
	pModuleInfo->Version			= MODULE_VERSION;
	pModuleInfo->CanBeSavedInPreset = FALSE;
	// query ask for number of audio channels
	if (pMasterInfo != nullptr)
    {
	    pModuleInfo->QueryString		= sdkGetAudioQueryTitle();
	    pModuleInfo->QueryListValues	= sdkGetAudioQueryChannelList();
	    pModuleInfo->QueryDefaultIdx	= 1;
    }
}

//-----------------------------------------------------------------------------
// query system and init methodes
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Get total parameters number of the module
int Reverb::onGetNumberOfParams (int queryIndex)
{
	int result = 0;
    this->queryIndex = queryIndex;
    numOfAudiotInsOuts = sdkGetAudioQueryToNbChannels (queryIndex);
	result = (numOfAudiotInsOuts * 2) + numOfParamAfterAudiotInOut;
    return result;
}

//-----------------------------------------------------------------------------
// Called after the query popup
void Reverb::onAfterQuery (MasterInfo* pMasterInfo, ModuleInfo* pModuleInfo, int queryIndex)
{
}


void Reverb::onInitModule (MasterInfo* pMasterInfo, ModuleInfo* pModuleInfo) 
{
	
    for(int v = 0; v < AUDIO_INS_OUTS_MAX; ++v)
    {
        sdkCreateEvt (temp[v], 1);
        sdkCreateEvt (temp2[v], 1);
        sdkCreateEvt (processInput[v], 1);
        sdkCreateEvt (processOutput[v], 1);
    }

    reverbInit ();
    mode = initialMode;
    roomsize = initialRoom / 100.0f;
    damp = initialDamp / 100.0f;
    channelsSeparation = initialChannelsSeparation / 100.0f;
    wet = sdkDbToCoeff (initialWet);
    dry = sdkDbToCoeff (initialDry);
    reverbParamsUpdate ();
}

//-----------------------------------------------------------------------------
// parameters and process
void Reverb::onGetParamInfo (int ParamIndex, TParamInfo* pParamInfo)
{
    // audioInputs
    if (ParamIndex < numOfAudiotInsOuts)
    {

		pParamInfo->ParamType		= ptAudio;
		pParamInfo->Caption			= sdkGetAudioQueryChannelNames ( "in ", ParamIndex + 1, queryIndex);
		pParamInfo->IsInput			= TRUE;
		pParamInfo->IsOutput		= FALSE;
		pParamInfo->ReadOnly		= FALSE;
    }
    // audioOutputs
    else if (ParamIndex >= numOfAudiotInsOuts && ParamIndex < (numOfAudiotInsOuts*2))
    {
		pParamInfo->ParamType		= ptAudio;
		pParamInfo->Caption			= sdkGetAudioQueryChannelNames ( "out ", ParamIndex - numOfAudiotInsOuts + 1, queryIndex);
		pParamInfo->IsInput			= FALSE;
		pParamInfo->IsOutput		= TRUE;
		pParamInfo->ReadOnly		= TRUE;
    }
    // swtchMode
    else if (ParamIndex == (numOfAudiotInsOuts*2))
	{
		pParamInfo->ParamType		= ptSwitch;
		pParamInfo->Caption			= "freeze";
		pParamInfo->IsInput			= TRUE;
		pParamInfo->IsOutput		= FALSE;
		pParamInfo->DefaultValue    = initialMode;
	} 
    // fdrRoomSize
    else if (ParamIndex == (numOfAudiotInsOuts*2) + 1)
	{
        pParamInfo->ParamType       = ptDataFader;
		pParamInfo->Caption         = "room size";
		pParamInfo->IsInput         = TRUE;
		pParamInfo->IsOutput        = FALSE;
		pParamInfo->MinValue        = 0.1f;
		pParamInfo->MaxValue        = 100.0f;
		pParamInfo->DefaultValue    = initialRoom;
		pParamInfo->Symbol          = "%";
		pParamInfo->Format          = "%.2f";
        pParamInfo->Scale           = scExp;
	} 
    // fdrDamping
    else if (ParamIndex == (numOfAudiotInsOuts*2) + 2)
	{
		pParamInfo->ParamType       = ptDataFader;
		pParamInfo->Caption         = "damping";
		pParamInfo->IsInput         = TRUE;
		pParamInfo->IsOutput        = FALSE;
		pParamInfo->MinValue        = 0.0f;
		pParamInfo->MaxValue        = 100.0f;
		pParamInfo->DefaultValue    = initialDamp;
        pParamInfo->Symbol          = "%";
		pParamInfo->Format          = "%.2f";
	} 
    // fdrChannelsSeparation
    else if (ParamIndex == (numOfAudiotInsOuts*2) + 3)
	{
		pParamInfo->ParamType       = ptDataFader;
		pParamInfo->Caption         = "channels separation";
		pParamInfo->IsInput         = TRUE;
		pParamInfo->IsOutput        = FALSE;
		pParamInfo->MinValue        = 0.0f;
		pParamInfo->MaxValue        = 100.0f;
		pParamInfo->DefaultValue    = initialChannelsSeparation;
		pParamInfo->Symbol          = "%";
		pParamInfo->Format          = "%.2f";
	} 
    // gfdrWetLevel
    else if (ParamIndex == (numOfAudiotInsOuts*2) + 4)
	{
		pParamInfo->ParamType       = ptGainFader;
		pParamInfo->Caption         = "wet level";
		pParamInfo->IsInput         = TRUE;
		pParamInfo->IsOutput        = FALSE;
		pParamInfo->DefaultValue    = initialWet;
		pParamInfo->Format          = "%.2f";
	} 
    // gfdrDryLevel
    else if (ParamIndex == (numOfAudiotInsOuts*2) + 5)
	{
		pParamInfo->ParamType       = ptGainFader;
		pParamInfo->Caption         = "dry level";
		pParamInfo->IsInput         = TRUE;
		pParamInfo->IsOutput        = FALSE;
		pParamInfo->DefaultValue    = initialDry;
    }
}


void Reverb::onSetEventAddress (int ParamIndex, UsineEventPtr pEvent)
{
    // audioInputs
    if (ParamIndex < numOfAudiotInsOuts)
    {
		audioInputs[ParamIndex] = pEvent;
    }
    // audioOutputs
    else if (ParamIndex >= numOfAudiotInsOuts && ParamIndex < (numOfAudiotInsOuts*2))
    {
		audioOutputs[ParamIndex - numOfAudiotInsOuts] = pEvent;
    }
    // swtchMode
    else if (ParamIndex == (numOfAudiotInsOuts*2))
	{
		swtchMode = pEvent;
    }
    // fdrRoomSize
    else if (ParamIndex == (numOfAudiotInsOuts*2) + 1)
	{
		fdrRoomSize = pEvent;		
	}
    // fdrDamping
    else if (ParamIndex == (numOfAudiotInsOuts*2) + 2)
	{
		fdrDamping = pEvent;
	}  
    // fdrChannelsSeparation
    else if (ParamIndex == (numOfAudiotInsOuts*2) + 3)
	{
		fdrChannelsSeparation = pEvent;
	}
    // gfdrWetLevel
    else if (ParamIndex == (numOfAudiotInsOuts*2) + 4)
	{
		gfdrWetLevel = pEvent;
	}
    // gfdrDryLevel
    else if (ParamIndex == (numOfAudiotInsOuts*2) + 5)
	{
		gfdrDryLevel = pEvent;
	}
}

void Reverb::onCallBack (UsineMessage *Message) 
{
    //  lboxMode
	if ( (Message->wParam == (numOfAudiotInsOuts*2)) && (Message->lParam == MSG_CHANGE) ) 
	{
        mode = sdkGetEvtData (swtchMode);
        reverbParamsUpdate ();
	}
    //  fdrRoomSize
	else if ( (Message->wParam == (numOfAudiotInsOuts*2) + 1) && (Message->lParam == MSG_CHANGE) ) 
	{
        roomsize = sdkGetEvtData (fdrRoomSize) / 100.0f;
        reverbParamsUpdate ();
	}
    // fdrDamping
	else if ( (Message->wParam == (numOfAudiotInsOuts*2) + 2) && (Message->lParam == MSG_CHANGE) ) 
	{
        damp = sdkGetEvtData (fdrDamping) / 100.0f;
        reverbParamsUpdate ();
	}   
	// fdrChannelsSeparation
	else if ( (Message->wParam == (numOfAudiotInsOuts*2) + 3) && (Message->lParam == MSG_CHANGE) ) 
	{
        channelsSeparation = sdkGetEvtData (fdrChannelsSeparation) / 100.0f;
        reverbParamsUpdate ();
	}   
	// gfdrWetLevel
	else if ( (Message->wParam == (numOfAudiotInsOuts*2) + 4) && (Message->lParam == MSG_CHANGE) ) 
	{
        wet = sdkDbToCoeff (sdkGetEvtData (gfdrWetLevel));
        reverbParamsUpdate ();
	}
	// gfdrDryLevel
	else if ( (Message->wParam == (numOfAudiotInsOuts*2) + 5) && (Message->lParam == MSG_CHANGE) ) 
	{
        dry = sdkDbToCoeff (sdkGetEvtData (gfdrDryLevel));
        reverbParamsUpdate ();
	}
}

void Reverb::onProcess () 
{ 	
    int  sampleFrames = sdkGetBlocSize ();
    for (int num = 0; num < numOfAudiotInsOuts; num++)
	{        
        if (sdkGetEvtSize (audioInputs[num]) != sampleFrames)
	        sdkClearAudioEvt(audioInputs[num]);
        
        if (sdkGetEvtSize (audioOutputs[num]) != sampleFrames)
	        sdkClearAudioEvt(audioOutputs[num]);
    }


    currentwet1 = currentwet1 * smooth + (1.0f - smooth) * wet1;
    currentwet2 = currentwet2 * smooth + (1.0f - smooth) * wet2;
    currentdry = currentdry * smooth +  (1.0f - smooth) * dry;

    coeffwet1 = currentwet1 * gain;

    if (numOfAudiotInsOuts > 2)
        coeffwet2 = currentwet2 * gain / (float)(numOfAudiotInsOuts - 1);
    else
        coeffwet2 = currentwet2 * gain;

    for(int v = 0; v < numOfAudiotInsOuts; ++v)
    {
        sdkCopyEvt (audioInputs[v], processInput[v]);
        sdkMultEvt1 (coeffwet1, processInput[v]);
        sdkClearAudioEvt (processOutput[v]);

        // partie dry 
        if (currentdry != 0)
        {
            sdkCopyEvt (audioInputs[v], audioOutputs[v]);
            sdkMultEvt1 (currentdry, audioOutputs[v]);
        }
        else
        {
            sdkClearAudioEvt (audioOutputs[v]);
        }

    }

    if (coeffwet2 !=0)
    {
        for(int v = 0; v < numOfAudiotInsOuts; ++v)
        {
            sdkCopyEvt (audioInputs[v], temp[v]);
            sdkMultEvt1 (coeffwet2, temp[v]);       
        }
        for(int v = 0; v < numOfAudiotInsOuts; ++v)
        {
            for(int n=0; n < numOfAudiotInsOuts; ++n)
            {
                if(v != n)
                    sdkAddEvt2(temp[n], processInput[v]);
            }
        }
    }
    

    for(int v = 0; v < numOfAudiotInsOuts; ++v)
    {
        int vc = v * numcombs;   
        int va = v * numallpasses; 
        TPrecision* processInputPtr = sdkGetEvtDataAddr (processInput[v]);
        TPrecision* processOutputPtr = sdkGetEvtDataAddr (processOutput[v]);

        for (int s = 0; s < sampleFrames; ++s)
        {
            // Accumulate comb filters in parallel
            for(int c = 0; c < numcombs; ++c)
            {
                // sdkSetEvtArrayData (processOutput[v], s, *(processOutputPtr + s) + combvoices[vc + c].process(*(processInputPtr + s)));
                *(processOutputPtr + s) = *(processOutputPtr + s) + combvoices[vc + c].process(*(processInputPtr + s));
            }
            
            // Feed through allpasses in series
            for(int a = 0; a < numallpasses; ++a)
            {
                // sdkSetEvtArrayData (processOutput[v], s, allpassvoices[va + a].process(*(processOutputPtr + s)));
                *(processOutputPtr + s) = allpassvoices[va + a].process(*(processOutputPtr + s));
            }
        }
    }
    
    
    for(int v = 0; v < numOfAudiotInsOuts; ++v)
    {
        sdkMultEvt1 (currentwet1, processOutput[v]);
        sdkAddEvt2 (processOutput[v], audioOutputs[v]);
    }
    
    
    if (currentwet2 !=0)
    {

        for(int v = 0; v < numOfAudiotInsOuts; ++v)
        {
            sdkCopyEvt (audioOutputs[v],temp[v]);
            sdkMultEvt1 (currentwet2, temp[v]);
        }

        for(int v = 0; v < numOfAudiotInsOuts; ++v)
        {
            for(int n=0;n<numOfAudiotInsOuts; n++)
            {
                if(v != n)
                    sdkAddEvt2 (temp[n],audioOutputs[v]);
            }
        }
    }
    





    /*
    int  sampleFrames = sdkGetBlocSize ();
 	for (int num = 0; num < numOfAudiotInsOuts; num++)
	{        
        if (sdkGetEvtSize (audioInputs[num]) != sampleFrames)
	        sdkClearAudioEvt(audioInputs[num]);
        
        if (sdkGetEvtSize (audioOutputs[num]) != sampleFrames)
	        sdkClearAudioEvt(audioOutputs[num]);

		audioInputsPointers[num] = sdkGetEvtDataAddr(audioInputs[num]);   
		audioOutputsPointers[num] = sdkGetEvtDataAddr(audioOutputs[num]);
    }
    
    //reverbModel->process(audioInputsPointers, audioOutputsPointers, sampleFrames);
    float out[numvoicesmax];
    float processinput[numvoicesmax];
    
    currentwet1 = currentwet1 * smooth + (1.0f - smooth) * wet1;
    currentwet2 = currentwet2 * smooth + (1.0f - smooth) * wet2;
    currentdry = currentdry * smooth +  (1.0f - smooth) * dry;
    
    float coeffwet1 = currentwet1 * gain;
    float coeffwet2 = currentwet2 * gain / (float)(numvoices - 1);

    //int s = numsamples;

    for (int s = 0; s < sampleFrames; ++s)
    {
        for(int v = 0; v < numvoices; ++v)
        {
            out[v] = 0;
            processinput[v] = audioInputsPointers[v][s] * coeffwet1;
            for(int n=0;n<numvoices; n++)
            {
                if(v != n)
                    processinput[v] += audioInputsPointers[n][s] * coeffwet2;
            }
        }

        for(int v = 0; v < numvoices; ++v)
        {
            
            // Accumulate comb filters in parallel
            for(int c = 0; c < numcombs; ++c)
            {
                out[v] += combvoices[v * numcombs + c].process(processinput[v]);
            }
            
            // Feed through allpasses in series
            for(int a = 0; a < numallpasses; ++a)
            {
                out[v] = allpassvoices[v * numallpasses + a].process(out[v]);
            }
        }

        // Calculate output REPLACING anything already there

        for(int v = 0; v < numvoices; ++v)
        {
            audioOutputsPointers[v][s] = out[v] * currentwet1 + audioInputsPointers[v][s] * currentdry;
            
            for(int n=0;n<numvoices; n++)
            {
                if(v != n)
                   audioOutputsPointers[v][s] += out[n] * currentwet2 / (float)(numvoices -1);
            }
            
        }
    }
    */
}

void Reverb::reverbInit()
{
    currentwet1 = 0.0f;
    currentwet2 = 0.0f;
    currentdry = 0.0f;

    // setting size from number off voices
    combvoices.resize(numcombs * numOfAudiotInsOuts);
    allpassvoices.resize(numallpasses * numOfAudiotInsOuts);

    combtuningvoices.resize(numcombs * numOfAudiotInsOuts);
    allpasstuningvoices.resize(numallpasses * numOfAudiotInsOuts);


    // populate all list
    for (int v = 0; v < numOfAudiotInsOuts; ++v)
    {
        for (int c = 0; c < numcombs; ++c)
        {
            int wichcomb = v * numcombs + c;
            combtuningvoices[wichcomb] = combtuning[c] + (((float)stereospread / (float)numOfAudiotInsOuts) * v);
            bufcombvoices[c].resize(numOfAudiotInsOuts);
            bufcombvoices[c][v].resize(combtuningvoices[wichcomb]);
            combvoices[wichcomb].setbuffer(&bufcombvoices[c][v][0],combtuningvoices[wichcomb]);
            combvoices[wichcomb].mute ();
        }
        
        for (int a = 0; a < numallpasses; ++a)
        {
            int wichallpass = v * numallpasses + a;
            allpasstuningvoices[wichallpass] = allpasstuning[a] + (((float)stereospread / (float)numOfAudiotInsOuts) * v);
            bufallpassvoices[a].resize(numOfAudiotInsOuts);
            bufallpassvoices[a][v].resize(allpasstuningvoices[wichallpass]);
            allpassvoices[wichallpass].setbuffer(&bufallpassvoices[a][v][0],allpasstuningvoices[wichallpass]);
            allpassvoices[wichallpass].setfeedback(0.5);
            allpassvoices[wichallpass].mute ();

        }
    }
}

void Reverb::reverbParamsUpdate()
{
// Recalculate internal values after parameter change

	wet1 = wet*(channelsSeparation/2 + 0.5f);
	wet2 = wet*((1-channelsSeparation)/2);

	if (mode >= freezemode)
	{
		roomsize1 = 1;
		damp1 = 0;
		gain = muted;
	}
	else
	{
		roomsize1 = roomsize;
		damp1 = damp;
		gain = fixedgain;
	}

    for (int v = 0; v < numOfAudiotInsOuts; ++v)
    {
        for (int c = 0; c < numcombs; ++c)
        {
            int wichcomb = v * numcombs + c;
            combvoices[wichcomb].setfeedback(roomsize1);
            combvoices[wichcomb].setdamp(damp1);
        }
    }
}

//-----------------------------------------------------------------------------
//@file  
//	Reverb.cpp
//
//@author
//	BrainModular team
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
#include "Reverb.h"

//----------------------------------------------------------------------------
// create, general info and destroy methods
//----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Create
void CreateModule (void* &pModule, AnsiCharPtr optionalString, LongBool Flag, TMasterInfo* pMasterInfo, AnsiCharPtr optionalContent)
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
    : numOfAudiotInsOuts(2)
    , numOfParamAfterAudiotInOut(6)
{}

// destructor
Reverb::~Reverb()
{
    for(int v = 0; v < AUDIO_INS_OUTS_MAX; ++v)
    {
        if (temp[v] != nullptr)
            temp[v].destroyEvent();
        
        if (temp2[v] != nullptr)
            temp2[v].destroyEvent();
        
        if (processInput[v] != nullptr)
            processInput[v].destroyEvent();
        
        if (processOutput[v] != nullptr)
            processOutput[v].destroyEvent();
    }
}



//void Reverb::onCreate(AnsiCharPtr optionalString);
//void Reverb::onDestroy(); 
void Reverb::onGetModuleInfo (TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo)
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
	    pModuleInfo->QueryListString        = sdkGetAudioQueryTitle();
	    pModuleInfo->QueryListValues	    = sdkGetAudioQueryChannelList();
	    pModuleInfo->QueryListDefaultIdx	= 1;
    }
}

//-----------------------------------------------------------------------------
// query system and init methods
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Get total parameters number of the module
int Reverb::onGetNumberOfParams (int queryResult1, int queryResult2)
{
	int result = 0;
    this->queryResult = queryResult1;
    numOfAudiotInsOuts = sdkGetAudioQueryToNbChannels (queryResult1);
	result = (numOfAudiotInsOuts * 2) + numOfParamAfterAudiotInOut;
    return result;
}

//-----------------------------------------------------------------------------
// Called after the query popup
void Reverb::onAfterQuery (TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo, int queryResult1, int queryResult2)
{
}


void Reverb::onInitModule (TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo) 
{
	
    for(int v = 0; v < AUDIO_INS_OUTS_MAX; ++v)
    {
        temp[v].createEvent(1);
        temp2[v].createEvent(1);
        processInput[v].createEvent(1);
        processOutput[v].createEvent(1);
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
		pParamInfo->Caption			= sdkGetAudioQueryChannelNames ( "in ", ParamIndex + 1, queryResult);
		pParamInfo->IsInput			= TRUE;
		pParamInfo->IsOutput		= FALSE;
		pParamInfo->ReadOnly		= FALSE;
        pParamInfo->setEventClass   (audioInputs[ParamIndex]);
    }
    // audioOutputs
    else if (ParamIndex >= numOfAudiotInsOuts && ParamIndex < (numOfAudiotInsOuts*2))
    {
		pParamInfo->ParamType		= ptAudio;
		pParamInfo->Caption			= sdkGetAudioQueryChannelNames ( "out ", ParamIndex - numOfAudiotInsOuts + 1, queryResult);
		pParamInfo->IsInput			= FALSE;
		pParamInfo->IsOutput		= TRUE;
		pParamInfo->ReadOnly		= TRUE;
        pParamInfo->setEventClass   (audioOutputs[ParamIndex - numOfAudiotInsOuts]);
	}
    // swtchMode
    else if (ParamIndex == (numOfAudiotInsOuts*2))
	{
		pParamInfo->ParamType		= ptSwitch;
		pParamInfo->Caption			= "freeze";
		pParamInfo->IsInput			= TRUE;
		pParamInfo->IsOutput		= FALSE;
		pParamInfo->DefaultValue    = initialMode;
        pParamInfo->setEventClass   (swtchMode);
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
        pParamInfo->setEventClass   (fdrRoomSize);
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
        pParamInfo->setEventClass   (fdrDamping);
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
        pParamInfo->setEventClass   (fdrChannelsSeparation);
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
        pParamInfo->setEventClass   (gfdrWetLevel);
	}
    // gfdrDryLevel
    else if (ParamIndex == (numOfAudiotInsOuts*2) + 5)
	{
		pParamInfo->ParamType       = ptGainFader;
		pParamInfo->Caption         = "dry level";
		pParamInfo->IsInput         = TRUE;
		pParamInfo->IsOutput        = FALSE;
		pParamInfo->DefaultValue    = initialDry;
        pParamInfo->setEventClass   (gfdrDryLevel);
	}
}


void Reverb::onCallBack (TUsineMessage *Message) 
{
    //  lboxMode
	if ( (Message->wParam == (numOfAudiotInsOuts*2)) && (Message->lParam == MSG_CHANGE) ) 
	{
        mode = swtchMode.getData();
        reverbParamsUpdate ();
	}
    //  fdrRoomSize
	else if ( (Message->wParam == (numOfAudiotInsOuts*2) + 1) && (Message->lParam == MSG_CHANGE) ) 
	{
        roomsize = fdrRoomSize.getData() / 100.0f;
        reverbParamsUpdate ();
	}
    // fdrDamping
	else if ( (Message->wParam == (numOfAudiotInsOuts*2) + 2) && (Message->lParam == MSG_CHANGE) ) 
	{
        damp = fdrDamping.getData() / 100.0f;
        reverbParamsUpdate ();
	}   
	// fdrChannelsSeparation
	else if ( (Message->wParam == (numOfAudiotInsOuts*2) + 3) && (Message->lParam == MSG_CHANGE) ) 
	{
        channelsSeparation = fdrChannelsSeparation.getData() / 100.0f;
        reverbParamsUpdate ();
	}   
	// gfdrWetLevel
	else if ( (Message->wParam == (numOfAudiotInsOuts*2) + 4) && (Message->lParam == MSG_CHANGE) ) 
	{
        wet = sdkDbToCoeff (gfdrWetLevel.getData());
        reverbParamsUpdate ();
	}
	// gfdrDryLevel
	else if ( (Message->wParam == (numOfAudiotInsOuts*2) + 5) && (Message->lParam == MSG_CHANGE) ) 
	{
        dry = sdkDbToCoeff (gfdrDryLevel.getData());
        reverbParamsUpdate ();
	}
}

void Reverb::onProcess () 
{ 	
	 
    int  sampleFrames = sdkGetBlocSize ();
    for (int num = 0; num < numOfAudiotInsOuts; num++)
	{        
        if (audioInputs[num].getSize() != sampleFrames)
            audioInputs[num].clearAudio();
        
        if (audioOutputs[num].getSize() != sampleFrames)
	        audioOutputs[num].clearAudio();
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
        processInput[v].copy(audioInputs[v]);
        processInput[v].mult(coeffwet1);
        processOutput[v].clearAudio();

        // dry part
        if (currentdry != 0)
        {
            audioOutputs[v].copy(audioInputs[v]);
            audioOutputs[v].mult(currentdry);
        }
        else
        {
            audioOutputs[v].clearAudio();
        }

    }

    if (coeffwet2 !=0)
    {
        for(int v = 0; v < numOfAudiotInsOuts; ++v)
        {
            temp[v].copy(audioInputs[v]);
            temp[v].mult(coeffwet2);       
        }
        for(int v = 0; v < numOfAudiotInsOuts; ++v)
        {
            for(int n=0; n < numOfAudiotInsOuts; ++n)
            {
                if(v != n)
                    processInput[v].add(temp[n]);
            }
        }
    }
    

    for(int v = 0; v < numOfAudiotInsOuts; ++v)
    {
        int vc = v * numcombs;   
        int va = v * numallpasses; 
        TPrecision* processInputPtr = processInput[v].getDataAddr();
        TPrecision* processOutputPtr = processOutput[v].getDataAddr();
        if ((processOutputPtr != NULL) && (processInputPtr != NULL))
        {
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
    }
    
    
    for(int v = 0; v < numOfAudiotInsOuts; ++v)
    {
        processOutput[v].mult(currentwet1);
        audioOutputs[v].add(processOutput[v]);
    }
    
    
    if (currentwet2 !=0)
    {

        for(int v = 0; v < numOfAudiotInsOuts; ++v)
        {
            temp[v].copy(audioOutputs[v]);
            temp[v].mult(currentwet2);
        }

        for(int v = 0; v < numOfAudiotInsOuts; ++v)
        {
            for(int n=0;n<numOfAudiotInsOuts; n++)
            {
                if(v != n)
                    audioOutputs[v].add(temp[n]);
            }
        }
    }
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
            combtuningvoices[wichcomb] = combtuning[c] + (int)(((float)stereospread / (float)numOfAudiotInsOuts) * v);
            bufcombvoices[c].resize(numOfAudiotInsOuts);
            bufcombvoices[c][v].resize(combtuningvoices[wichcomb]);
            combvoices[wichcomb].setbuffer(&bufcombvoices[c][v][0],combtuningvoices[wichcomb]);
            combvoices[wichcomb].mute ();
        }
        
        for (int a = 0; a < numallpasses; ++a)
        {
            int wichallpass = v * numallpasses + a;
            allpasstuningvoices[wichallpass] = allpasstuning[a] + (int)(((float)stereospread / (float)numOfAudiotInsOuts) * v);
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

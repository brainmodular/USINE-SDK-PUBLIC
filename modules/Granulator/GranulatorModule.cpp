//-----------------------------------------------------------------------------
//@file  
//	GranulatorModule.cpp
//
//@author
//	Martin FLEURENT aka 'martignasse'
//
//@brief 
//	Implementation of the GranulatorModule class.
//
//  Inspired from the granulator part of the Fragmental-v1.02 VST plugin from niall moody.
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
#include "GranulatorModule.h"

//----------------------------------------------------------------------------
// create, general info and destroy methodes
//----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Create
void CreateModule (void* &pModule, AnsiCharPtr optionalString, LongBool Flag, MasterInfo* pMasterInfo, AnsiCharPtr optionalContent)
{
	pModule = new GranulatorModule();
}

//-----------------------------------------------------------------------------
// destroy
void DestroyModule (void* pModule) 
{
	// cast is important to call the good destructor
	delete ((GranulatorModule*)pModule);
}

// module constants for browser info and module info
const AnsiCharPtr UserModuleBase::MODULE_NAME = "Granulator";
const AnsiCharPtr UserModuleBase::MODULE_DESC = "Granulator";
const AnsiCharPtr UserModuleBase::MODULE_VERSION = "2.3";

// browser info
void GetBrowserInfo(ModuleInfo* pModuleInfo) 
{
	pModuleInfo->Name				= UserModuleBase::MODULE_NAME;
	pModuleInfo->Description		= UserModuleBase::MODULE_DESC;
	pModuleInfo->Version			= UserModuleBase::MODULE_VERSION;
}

//-----------------------------------------------------------------------------
// module description
void GranulatorModule::onGetModuleInfo (MasterInfo* pMasterInfo, ModuleInfo* pModuleInfo) 
{
	pModuleInfo->Name				= MODULE_NAME;
	pModuleInfo->Description		= MODULE_DESC;
	pModuleInfo->ModuleType         = mtControl;
	pModuleInfo->BackColor          = sdkGetUsineColor(clAudioModuleColor);
	pModuleInfo->DefaultWidth		= 100;
	pModuleInfo->DefaultHeight		= 100;
	pModuleInfo->NumberOfParams     = 10;
	pModuleInfo->DontProcess		= FALSE;
	pModuleInfo->Version			= MODULE_VERSION;
	pModuleInfo->CanRecord			= TRUE;
	
	// query ask for number of audio channels
	if (pMasterInfo != nullptr)
    {
	    pModuleInfo->QueryString		= sdkGetAudioQueryTitle();
	    pModuleInfo->QueryListValues	= sdkGetAudioQueryChannelList();
	    pModuleInfo->QueryDefaultIdx	= 1;
    }
	pModuleInfo->CanBeSavedInPreset = TRUE;

}

//-----------------------------------------------------------------------------
// query system and init methodes
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Get total parameters number of the module
int GranulatorModule::onGetNumberOfParams (int queryIndex)
{
	int result = 0;
    this->queryIndex = queryIndex;
    numOfAudiotInsOuts = sdkGetAudioQueryToNbChannels (queryIndex);
	result = (numOfAudiotInsOuts * 2) + numOfParamAfterAudiotInOut;
    return result;
}

//-----------------------------------------------------------------------------
// Called after the query popup
void GranulatorModule::onAfterQuery (MasterInfo* pMasterInfo, ModuleInfo* pModuleInfo, int queryIndex)
{
}


//-----------------------------------------------------------------------------
// initialisation
void GranulatorModule::onInitModule (MasterInfo* pMasterInfo, ModuleInfo* pModuleInfo) 
{
	m_LineColor = sdkGetUsineColor(clAudioFlow);
	m_mouseLastButtonDown = -1;
	m_mouseLastCoords.x = 0;
	m_mouseLastCoords.y = 0;
	m_unusedY = 0;
	m_unusedZ = 0;
    
    dimPitch.left = 0.25f;
    dimPitch.top = 0.5f;
    dimPitch.right = 0.75f;
    dimPitch.bottom = 0.75f;
    
    dimGrainDuration.left = 0.25f;
    dimGrainDuration.top = 0.75f;
    dimGrainDuration.right = 0.75f;
    dimGrainDuration.bottom = 1.0f;
    	
	for (int num = 0; num < numOfAudiotInsOuts; num++)
	{
		currentGrain.push_back(new Grain[NUM_OF_GRAIN]);
		        
		for (int i = 0; i < NUM_OF_GRAIN; i++)
        {
            currentGrain[num][i].setModule(this);
        }
	}

    onSampleRateChange(sdkGetVstTimeInfo()->sampleRate);
    changeDuration();
    changePitch();
}

//----------------------------------------------------------------------------
// parameters and process
//----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Parameters description
void GranulatorModule::onGetParamInfo (int ParamIndex, TParamInfo* pParamInfo) 
{	
    // audio input
    if (ParamIndex < numOfAudiotInsOuts)
    {
		pParamInfo->ParamType		= ptAudio;
		pParamInfo->Caption			= sdkGetAudioQueryChannelNames ( "in ", ParamIndex + 1, queryIndex);
		pParamInfo->IsInput			= TRUE;
		pParamInfo->IsOutput		= FALSE;
		pParamInfo->ReadOnly		= FALSE;
    }
    // audio output
    else if (ParamIndex >= numOfAudiotInsOuts && ParamIndex < (numOfAudiotInsOuts*2))
    {
		pParamInfo->ParamType		= ptAudio;
		pParamInfo->Caption			= sdkGetAudioQueryChannelNames ( "out ", ParamIndex - numOfAudiotInsOuts + 1, queryIndex);
		pParamInfo->IsInput			= FALSE;
		pParamInfo->IsOutput		= TRUE;
		pParamInfo->ReadOnly		= TRUE;
    }
    // grain on trigger led
    else if (ParamIndex == (numOfAudiotInsOuts*2))
	{
		pParamInfo->ParamType		= ptTriggerLed;
		pParamInfo->Caption			= "trig out";
		pParamInfo->Symbol			= "";
		pParamInfo->Format			= "%.0f";
		pParamInfo->IsInput			= FALSE;
		pParamInfo->IsOutput		= TRUE;
		pParamInfo->ReadOnly		= TRUE;
        pParamInfo->IsSeparator     = TRUE;
    }
    // next grain interval fader
    else if (ParamIndex == (numOfAudiotInsOuts*2) + 1)
	{
		pParamInfo->ParamType			= ptDataFader;
		pParamInfo->Caption				= "interval";
		pParamInfo->IsInput				= TRUE;
		pParamInfo->IsOutput			= FALSE;
		pParamInfo->MinValue			= -1000.0f; // can put negatif interval, ex : -1000.0f
		pParamInfo->MaxValue			= 20000.0f;
		pParamInfo->DefaultValue		= 100.0f;
        pParamInfo->Scale				= scLog;
		pParamInfo->Symbol				= " ms";
		pParamInfo->Format				= "%.3f";
		pParamInfo->ReadOnly			= FALSE;
		pParamInfo->IsStoredInPreset	= TRUE;
		pParamInfo->CallBackType		= ctImmediate;
	}
    // grain envelope listbox
    else if (ParamIndex == (numOfAudiotInsOuts*2) + 2)
	{
		pParamInfo->ParamType			= ptListBox;
		pParamInfo->Caption				= "grain env";
		pParamInfo->IsInput				= TRUE;
		pParamInfo->IsOutput			= FALSE;
		pParamInfo->ListBoxStrings		= "\"Linear ASR\",\"Bezier\",\"Raised Cosine Bell\",\"Hanning\",\"Gaussian\"";
		pParamInfo->DefaultValue		= 0;  // set to 'Linear ASR' value
		pParamInfo->ReadOnly			= FALSE;
		pParamInfo->IsStoredInPreset	= TRUE;
		pParamInfo->CallBackType		= ctImmediate;
	}  
    // grain envelope variable
    else if (ParamIndex == (numOfAudiotInsOuts*2) + 3)
	{
		pParamInfo->ParamType			= ptDataFader;
		pParamInfo->Caption				= "env var";
		pParamInfo->IsInput				= TRUE;
		pParamInfo->IsOutput			= FALSE;
		pParamInfo->MinValue			= ENV_VAR_MIN;
		pParamInfo->MaxValue			= ENV_VAR_MAX;
		pParamInfo->DefaultValue		= 0.5f;
		pParamInfo->Format				= "%.3f";
		pParamInfo->ReadOnly			= FALSE;
		pParamInfo->IsStoredInPreset	= TRUE;
		pParamInfo->CallBackType		= ctImmediate;
	}
    // grain duration fader
    else if (ParamIndex == (numOfAudiotInsOuts*2) + 4)
	{
		pParamInfo->ParamType			= ptDataFader;
		pParamInfo->Caption				= "duration";
		pParamInfo->IsInput				= TRUE;
		pParamInfo->IsOutput			= FALSE;
		pParamInfo->MinValue			= GRAIN_DUR_MIN;
		pParamInfo->MaxValue			= GRAIN_DUR_MAX;
		pParamInfo->DefaultValue		= 100.0f;
        pParamInfo->Scale				= scLog;
		pParamInfo->Symbol				= " ms";
		pParamInfo->Format				= "%.3f";
		pParamInfo->ReadOnly			= FALSE;
		pParamInfo->IsStoredInPreset	= TRUE;
		pParamInfo->CallBackType		= ctImmediate;
	}
    // pitch fader
    else if (ParamIndex == (numOfAudiotInsOuts*2) + 5)
	{
		pParamInfo->ParamType			= ptDataFader;
		pParamInfo->Caption				= "pitch";
		pParamInfo->IsInput				= TRUE;
		pParamInfo->IsOutput			= FALSE;
		pParamInfo->MinValue			= GRAIN_PITCH_MIN;
		pParamInfo->MaxValue			= GRAIN_PITCH_MAX;
		pParamInfo->DefaultValue		= 0.0f;
		pParamInfo->Symbol				= " ht";
		pParamInfo->Format				= "%.0f";
		pParamInfo->ReadOnly			= FALSE;
		pParamInfo->IsStoredInPreset	= TRUE;
		pParamInfo->CallBackType		= ctImmediate;
	}
    // grain mode listbox
    else if (ParamIndex == (numOfAudiotInsOuts*2) + 6)
	{
		pParamInfo->ParamType			= ptListBox;
		pParamInfo->Caption				= "grain mode";
		pParamInfo->IsInput				= TRUE;
		pParamInfo->IsOutput			= FALSE;
		pParamInfo->ListBoxStrings		= "\"Interval\",\"Play Button\"";
		pParamInfo->DefaultValue		= 0;  // set to 'Interval' value
		pParamInfo->ReadOnly			= FALSE;
		pParamInfo->IsStoredInPreset	= TRUE;
		pParamInfo->CallBackType		= ctImmediate;
	}  
    // grain go button // used in Button mode
    else if (ParamIndex == (numOfAudiotInsOuts*2) + 7)
	{
		pParamInfo->ParamType		= ptButton;
		pParamInfo->Caption			= "play";
		pParamInfo->IsInput			= TRUE;
		pParamInfo->IsOutput		= TRUE;
		pParamInfo->CallBackType    = ctImmediate;
    }
	// default case
    else
    {
	}
}

//-----------------------------------------------------------------------------
// set the parameters events address
void GranulatorModule::onSetEventAddress (int ParamIndex, UsineEventPtr pEvent) 
{
    // audio input
    if (ParamIndex < numOfAudiotInsOuts)
    {
		m_audioInputs[ParamIndex] = pEvent;
    }
    // audio output
    else if (ParamIndex >= numOfAudiotInsOuts && ParamIndex < (numOfAudiotInsOuts*2))
    {
		m_audioOutputs[ParamIndex - numOfAudiotInsOuts] = pEvent;
    }
    // grain on trigger led
    else if (ParamIndex == (numOfAudiotInsOuts*2))
	{
		m_ledGrainStart = pEvent;
    }
    // next grain interval fader
    else if (ParamIndex == (numOfAudiotInsOuts*2) + 1)
	{
		m_fdrGrainInterval = pEvent;		
	}
    // grain envelope listbox
    else if (ParamIndex == (numOfAudiotInsOuts*2) + 2)
	{
		m_lboxGrainEnvelope = pEvent;
	}  
    // grain envelope variable
    else if (ParamIndex == (numOfAudiotInsOuts*2) + 3)
	{
		m_fdrGrainEnvelopeSkrew = pEvent;
	}
    // grain duration fader
    else if (ParamIndex == (numOfAudiotInsOuts*2) + 4)
	{
		m_fdrGrainDuration = pEvent;
	}
    // pitch fader
    else if (ParamIndex == (numOfAudiotInsOuts*2) + 5)
	{
		m_fdrGrainPitch = pEvent;
	}
    // grain mode listbox
    else if (ParamIndex == (numOfAudiotInsOuts*2) + 6)
	{
		m_lboxGrainMode = pEvent;
	}  
    // grain go button // used in Button mode
    else if (ParamIndex == (numOfAudiotInsOuts*2) + 7)
	{
		m_btnGrainGo = pEvent;
    }
	// default case
    else
    {
	}
}

//-----------------------------------------------------------------------------
// Parameters callback
void GranulatorModule::onCallBack (UsineMessage* Message) 
{ 
    //  grain interval fader
	 if ( (Message->wParam == (numOfAudiotInsOuts*2) + 1) && (Message->lParam == MSG_CHANGE) ) 
	{
        changeInterval();
	}
    // grain envelope listbox
	else if ( (Message->wParam == (numOfAudiotInsOuts*2) + 2) && (Message->lParam == MSG_CHANGE) ) 
	{
        changeGrainEnvelope();
	}   
	// grain envelope variable
	else if ( (Message->wParam == (numOfAudiotInsOuts*2) + 3) && (Message->lParam == MSG_CHANGE) ) 
	{
        changeEnvelopeVar();

	}   
	// duration fader input changed
	else if ( (Message->wParam == (numOfAudiotInsOuts*2) + 4) && (Message->lParam == MSG_CHANGE) ) 
	{
        changeDuration();

	}
	// pitch fader input changed
	else if ( (Message->wParam == (numOfAudiotInsOuts*2) + 5) && (Message->lParam == MSG_CHANGE) ) 
	{
        changePitch();

	}
    // grain mode listbox
    else if ( (Message->wParam == (numOfAudiotInsOuts*2) + 6) && (Message->lParam == MSG_CHANGE) ) 
	{
        changeGrainMode();
	}  
    // grain go button // used in Button mode
    else if ( (Message->wParam == (numOfAudiotInsOuts*2) + 7) && (Message->lParam == MSG_CHANGE) ) 
	{
        grainGo();
    }
}

//-----------------------------------------------------------------------------
// Main process
void GranulatorModule::onProcess () 
{
    int i, num;

	if (sdkGetEvtData(m_ledGrainStart) == 1.0f)
		sdkSetEvtData(m_ledGrainStart, 0.0f);
    
	blockSize = sdkGetBlocSize();

	for (num = 0; num < numOfAudiotInsOuts; num++)
    {
        sdkSetEvtSize(m_audioOutputs[num], blockSize);
        tempInputBlock[num] = sdkGetEvtDataAddr(m_audioInputs[num]);
        tempOutputBlock[num] = sdkGetEvtDataAddr(m_audioOutputs[num]);
    }

	//Calculate grain scheduling stuff.
	for (i = 0; i < blockSize; ++i)
	{		        
        if (currentGrainMode == 0)
		{
			//Check onset.
			--audioLineNextOnset;

			if(audioLineNextOnset < 0)
			{
				audioLineNextOnset = grainSize + grainInterval - 1;
				wantNewGrain = true;  
			}
		}

        for (num = 0; num < numOfAudiotInsOuts; num++)
        {
            if (sdkGetEvtSize (m_audioInputs[num]) == blockSize)
            {
		        //TPrecision val = sdkGetEvtArrayData(m_audioInputs[num], i);
		        circularBuffer[num][cbufReadPos] = *(tempInputBlock[num] + i);

		        //Now clear outputs.
		        tempOutputBlock[num][i] = 0.0f;

		        // Interval mode
		        if (wantNewGrain && currentGrainMode == 0)
		        {
				   generateNewGrain(num); 
		        }
		        // PlayButton mode
		        else if (wantNewGrain && i == 0)
		        {
				        generateNewGrain(num);
		        }

		        for (int j = 0; j < NUM_OF_GRAIN; j++)
		        {
			        if(currentGrain[num][j].getIsActive())
			        {
				        currentGrain[num][j].getSample(tempOutputBlock[num] + i);
			        }
		        } 
            }
            else
            {
                //sdkSetEvtArrayData (m_audioOutputs[num], i, 0);
                *(tempOutputBlock[num] + i) = 0.0f;
            }
        }
        if (wantNewGrain)
		    wantNewGrain = false;

		// update buffer read pos
		++cbufReadPos;
		if (cbufReadPos >= (cbufSize))
			cbufReadPos -= cbufSize;
		assert(cbufReadPos <(cbufSize));
	}

    if (currentGrainMode == 0)
        nextOnset =  audioLineNextOnset;
}

//----------------------------------------------------------------------------
// layout
//----------------------------------------------------------------------------

void GranulatorModule::onCreateSettings()
{
	sdkAddSettingLineCaption(DESIGN_TAB_NAME, "line", 0);
	sdkAddSettingLineSingle(DESIGN_TAB_NAME,  &m_LineThickness, "line width", 0, 100, scLog, "pix", DEFAULT_FORMAT_FLOAT_2, 2.0f );
	sdkAddSettingLineColor(DESIGN_TAB_NAME,  &m_LineColor, "line color" );
}

void GranulatorModule::onSettingsHasChanged()
{
	// ask to repaint the panel
	sdkRepaintPanel();
} 

//-----------------------------------------------------------------------------
// paint the module panel
void GranulatorModule::onPaint () 
{
	switch (currentGrainEnvType)
	{
			
	default:
	case Grain::ENV_ASR:
		sdkDrawPolyLine(coordEnvAsr, ENV_ASR_SIZE, m_LineColor,  m_LineThickness );
		break;

	case Grain::ENV_BEZIER:
		sdkDrawPolyLine(coordEnvBezier, ENV_BEZIER_SIZE, m_LineColor, m_LineThickness );
		break;

	case Grain::ENV_RCB:
		sdkDrawPolyLine(coordEnvRcb, ENV_RCB_SIZE, m_LineColor, m_LineThickness );
		break;

	case Grain::ENV_HANN:
		sdkDrawPolyLine(coordEnvHanning, ENV_HANNING_SIZE, m_LineColor, m_LineThickness );
		break;

	case Grain::ENV_GAUSS:
		sdkDrawPolyLine(coordEnvGaussian, ENV_GAUSSIAN_SIZE, m_LineColor, m_LineThickness );
		break;
			
	}
    
    // grain pitch info
    sdkFillText( dimPitch, (AnsiCharPtr)((caPitch+ " ht").c_str ()), sdkGetUsineColor(clFontLight), 0.12f);
    // grain duration info
    sdkFillText( dimGrainDuration, (AnsiCharPtr)((caGrainDuration + " ms").c_str ()), sdkGetUsineColor(clFontLight), 0.12f);
}

//-----------------------------------------------------------------------------
// mouse and multi touch interaction
void GranulatorModule::onMouseDown(TMouseButton MouseButton, TShiftState Shift, float X,float Y)
{
	if (MouseButton == mbLeft)
	{
		isRecording = true;
		m_mouseLastButtonDown = (int)mbLeft;
	}

	m_mouseLastCoords.x = X;
	m_mouseLastCoords.y = Y;
}

void GranulatorModule::onMouseMove(TShiftState Shift, float X, float Y)
{
	if (isRecording && (m_mouseLastButtonDown == (int)mbLeft) )
	{
        float pitch = sdkGetEvtData( m_fdrGrainPitch);
        float dur = sdkGetEvtData( m_fdrGrainDuration);
        
        if (Shift & ssShift)
        {
            // Shift Y slide change the pitch
            pitch = std::min(GRAIN_PITCH_MAX, std::max(GRAIN_PITCH_MIN, pitch - ((Y - m_mouseLastCoords.y)*5)));
            sdkSetEvtData( m_fdrGrainPitch, pitch);
            changePitch();
        }
        else
        {
            // Y slide change the envVar
            grainEnvelopeVar = std::min(1.0f, std::max(0.0f, grainEnvelopeVar - (Y - m_mouseLastCoords.y)));
            sdkSetEvtData( m_fdrGrainEnvelopeSkrew, grainEnvelopeVar);
            changeEnvelopeVar();
        }
        // X slide change the grain duration
		dur = std::min(GRAIN_DUR_MAX, std::max(GRAIN_DUR_MIN, dur + (X - m_mouseLastCoords.x)*10));
		sdkSetEvtData( m_fdrGrainDuration, dur);
		changeDuration();
        
		try
		{
			sdkProcessRecord(dur, grainEnvelopeVar, pitch );
		}
		catch (std::exception& /*e*/) 
		{
			//const char* result(e.what());
			//TraceErrorPChar ((AnsiCharPtr)NormalizeString(result).c_str());
		}
	}	
	m_mouseLastCoords.x = X;
	m_mouseLastCoords.y = Y;
}

void GranulatorModule::onMouseUp (TMouseButton MouseButton, TShiftState Shift, float X,float Y)
{
	if (isRecording )
	{
		isRecording = false;
		//StopRecord ();
		
		try
		{
			sdkStopRecord ();
		}
		catch (std::exception& /*e*/) 
		{
			//const char* result(e.what());
			//TraceErrorPChar ((AnsiCharPtr)NormalizeString(result).c_str());
		}
	}
	if (m_mouseLastButtonDown == mbLeft)
	{
		// mouse is no more down
		m_mouseLastButtonDown = -1;
	}
}

//-----------------------------------------------------------------------------
// audio setup update
void GranulatorModule::onSampleRateChange (double SampleRate)
{
    usineSamplerate = (float)SampleRate; 
    cbufSize = (int)(2.1 * 16 * usineSamplerate);
	
	for (int num = 0; num < numOfAudiotInsOuts; num++)
	{
		if (circularBuffer[num] != nullptr)
		{
			delete[] circularBuffer[num];
			circularBuffer[num] = nullptr;
		}

		circularBuffer[num] = new TPrecision[cbufSize];
		memset(circularBuffer[num], 0, (sizeof(float) * cbufSize));

		for (int i = 0; i < NUM_OF_GRAIN; i++)
		{
			currentGrain[num][i].setInputBuffer(circularBuffer[num], cbufSize);
		}
	}
}

//-----------------------------------------------------------------------------
// recording 
void GranulatorModule::onSetRecordedValue (TPrecision X, TPrecision Y, TPrecision Z)
{
    // X = grain dur, Y = env skrew, Z = grain pitch
    float dur = std::min(GRAIN_DUR_MAX, std::max(GRAIN_DUR_MIN, X));
    sdkSetEvtData( m_fdrGrainDuration, dur);
    changeDuration();
    
    grainEnvelopeVar = std::min(1.0f, std::max(0.0f, Y));
    sdkSetEvtData( m_fdrGrainEnvelopeSkrew, grainEnvelopeVar);
    changeEnvelopeVar();
    
    float pitch = std::min(GRAIN_PITCH_MAX, std::max(GRAIN_PITCH_MIN, Z));
    sdkSetEvtData( m_fdrGrainPitch, pitch);
    changePitch();
}

//-------------------------------------------------------------------------
// private methodes implementation
//-------------------------------------------------------------------------
//
void GranulatorModule::changeGrainEnvelope()
{
    currentGrainEnvType = (Grain::EnvType)(int)sdkGetEvtData( m_lboxGrainEnvelope);
    changeEnvelopeVar();
}

//-------------------------------------------------------------------------
//
void GranulatorModule::changeEnvelopeVar()
{
    grainEnvelopeVar = (float)sdkGetEvtData( m_fdrGrainEnvelopeSkrew);

    switch (currentGrainEnvType)
    {
    default:
    case Grain::ENV_ASR:
        currentGrain[0][0].getGraphPoints( Grain::ENV_ASR, grainEnvelopeVar, coordEnvAsr, ENV_ASR_SIZE);
        break;

    case Grain::ENV_BEZIER:
        currentGrain[0][0].getGraphPoints( Grain::ENV_BEZIER, grainEnvelopeVar, coordEnvBezier, ENV_BEZIER_SIZE);
        break;

    case Grain::ENV_RCB:
        currentGrain[0][0].getGraphPoints( Grain::ENV_RCB, grainEnvelopeVar, coordEnvRcb, ENV_RCB_SIZE);
        break;

    case Grain::ENV_HANN:
        currentGrain[0][0].getGraphPoints( Grain::ENV_HANN, grainEnvelopeVar, coordEnvHanning, ENV_HANNING_SIZE);
        break;

    case Grain::ENV_GAUSS:
        currentGrain[0][0].getGraphPoints( Grain::ENV_GAUSS, grainEnvelopeVar, coordEnvGaussian, ENV_GAUSSIAN_SIZE);
        break;
    }

    // finally ask a repaint
	sdkRepaintPanel();  
}

//-------------------------------------------------------------------------
//
void GranulatorModule::changeInterval()
{
	// converstion from milliseconds to sample
    grainInterval = static_cast<int>(floor((sdkGetEvtData(m_fdrGrainInterval)* usineSamplerate * 0.001f )+ 0.5f ));

}

//-------------------------------------------------------------------------
//
void GranulatorModule::changeDuration()
{
	// converstion from milliseconds to sample
    float sizeInMs = sdkGetEvtData( m_fdrGrainDuration);
    grainSize = static_cast<int>(floor((sizeInMs * usineSamplerate * 0.001f ) + 0.5f ));
    caGrainDuration = sdkFloatToString (sizeInMs, 2, 8);
	sdkRepaintPanel();
}   
    
//-------------------------------------------------------------------------
// 
void GranulatorModule::changePitch()
{
    float tempf;
                
    tempf = (float)sdkGetEvtData( m_fdrGrainPitch);
    pitchIncrement = pow(2.0f, tempf/12.0f);
    caPitch = sdkFloatToString (tempf, 0, 8);
	sdkRepaintPanel();
}
    
//-------------------------------------------------------------------------
// 
void GranulatorModule::changeGrainMode()
{
	// 0 mean Interval mode
	// 1 mean PlayButton mode
	currentGrainMode = (int)sdkGetEvtData( m_lboxGrainMode);
}
    
//-------------------------------------------------------------------------
// 
void GranulatorModule::grainGo()
{
	wantNewGrain = true;
}

void GranulatorModule::generateNewGrain(int num)
{

    currentGrain[num][nextNewGrainIndex].setEnvelope(currentGrainEnvType); 
    currentGrain[num][nextNewGrainIndex].activate( 
            grainEnvelopeVar
        , cbufReadPos
		, grainSize
        , pitchIncrement
        );
  
	//grainToggle = (grainToggle + 1) % 2;
	nextNewGrainIndex = (nextNewGrainIndex + 1) % NUM_OF_GRAIN;

    sdkSetEvtData(m_ledGrainStart, 1.0f);    
}


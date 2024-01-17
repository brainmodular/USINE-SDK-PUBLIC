//-----------------------------------------------------------------------------
//@file  
//	GranulatorModule.h
//
//@author
//	BrainModular team
//
//@brief 
//	Definitions of the GranulatorModule class.
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

// include once, no more
#ifndef INCLUDED_GRANULATORMODULE_H
#define INCLUDED_GRANULATORMODULE_H

//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------
//#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
//#include <crtdbg.h>

#include "../../sdk/UserDefinitions.h"  
//#ifdef _DEBUG
//   #ifndef DBG_NEW
//      #define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
//      #define new DBG_NEW
//   #endif
//#endif  // _DEBUG

#include "Grain.h"

#include <string>
#include <iostream>
#include <cassert>
#include <cmath>
#include <vector>
#include <limits>


//-----------------------------------------------------------------------------
// defines and constantes
//-----------------------------------------------------------------------------
#define NUM_OF_GRAIN 16
#define ENV_ASR_SIZE 4
#define ENV_BEZIER_SIZE 61
#define ENV_RCB_SIZE 40
#define ENV_HANNING_SIZE 61
#define ENV_GAUSSIAN_SIZE 61


#define GRAIN_DUR_MIN 0.05f
#define GRAIN_DUR_MAX 2000.0f

#define GRAIN_PITCH_MIN -48.0f
#define GRAIN_PITCH_MAX 48.0f


// module constant
const int MODULE_HEIGHT_DEFAULT = 134; 

//-----------------------------------------------------------------------------
// structures and typedef
//-----------------------------------------------------------------------------

// structures and typedef goes here

//-----------------------------------------------------------------------------
// class definition
//-----------------------------------------------------------------------------
class GranulatorModule : public UserModuleBase
{

    //-------------------------------------------------------------------------
	// public constructors/destructors
	//-------------------------------------------------------------------------
public:
    // constructor
	GranulatorModule()
        : cbufReadPos(0)
        , audioLineNextOnset(0)
        , cbufSize(0)
//        , grainToggle(0)
		, numOfAudiotInsOuts (2)
		, numOfParamAfterAudiotInOut (9)
        , usineSamplerate(0)
        , pitchIncrement(1.0f)
		, currentGrainEnvType(Grain::ENV_ASR)
        , grainEnvelopeVar(0.0f)
        , grainInterval(0)
        , grainSize(4410)
        , nextOnset(0)
        , m_LineThickness(2)
		, wantNewGrain(false)
		, currentGrainMode (0)
		, nextNewGrainIndex (0)
		, isRecording (false)
	{
		//_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );

        for (int i = 0; i < ENV_ASR_SIZE; i++)
        {
            coordEnvAsr[i].x = (1.0f / static_cast<float>(ENV_ASR_SIZE)) * static_cast<float>(i);
            coordEnvAsr[i].y = 1.0f;
        }

        for (int i = 0; i < ENV_BEZIER_SIZE; i++)
        {
            coordEnvBezier[i].x = (1.0f / static_cast<float>(ENV_BEZIER_SIZE)) * static_cast<float>(i);
            coordEnvBezier[i].y = 1.0f;
        }

        for (int i = 0; i < ENV_RCB_SIZE; i++)
        {
            coordEnvRcb[i].x = (1.0f / static_cast<float>(ENV_RCB_SIZE)) * static_cast<float>(i);
            coordEnvRcb[i].y = 1.0f;
        }
			
		for (int num = 0; num < USINE_MAX_AUDIO_INPUTS; num++)
        {
			circularBuffer[num] = nullptr;
        }
	};

    // destructor
	~GranulatorModule()
	{
		currentGrain.clear();
		for (int num = 0; num < numOfAudiotInsOuts; num++)
		{
			delete [] circularBuffer[num]; 
		}
	};


	//-------------------------------------------------------------------------
	// public methods inherited from UserModule
	//-------------------------------------------------------------------------
public:
	//-----------------------------------------------------------------------------
	// module info
	void onGetModuleInfo (MasterInfo* pMasterInfo, TModuleInfo* pModuleInfo);

	//-----------------------------------------------------------------------------
	// query system and init
	int  onGetNumberOfParams(int queryResult1, int queryResult2);
	void onAfterQuery (MasterInfo* pMasterInfo, TModuleInfo* pModuleInfo, int queryResult1, int queryResult2);
	void onInitModule (MasterInfo* pMasterInfo, TModuleInfo* pModuleInfo);

	//-----------------------------------------------------------------------------
	// parameters and process
	void onGetParamInfo (int ParamIndex, TParamInfo* pParamInfo);
	void onCallBack (TUsineMessage *Message);
	void onProcess ();

	//-----------------------------------------------------------------------------
	// midi out callbacks
	//void onMidiSendOut (int DeviceID, UsineMidiCode Code);
	//void onMidiSysexSendOut (int DeviceID, char* Sysex);

	//-----------------------------------------------------------------------------
	// chunk system
	//int  onGetChunkLen (LongBool Preset);
	//void onGetChunk (void* chunk, LongBool Preset);
	//void onSetChunk (const void* chunk, int sizeInBytes, LongBool Preset);

	//-----------------------------------------------------------------------------
	// layout
	void onCreateSettings();
	void onSettingsHasChanged();
    //void onCreateCommands();
	void onPaint ();

	//-----------------------------------------------------------------------------
	// mouse and multi touch interaction
	void onMouseMove(TShiftState Shift, float X, float Y);
	void onMouseDown(TMouseButton MouseButton, TShiftState Shift, float X,float Y);
	void onMouseUp (TMouseButton MouseButton, TShiftState Shift, float X,float Y);
	//void onMouseMoveMulti(TShiftState Shift, UsineEventPtr X, UsineEventPtr Y);
	//void onMouseDownMulti(TMouseButton MouseButton, TShiftState Shift, UsineEventPtr X, UsineEventPtr Y);
	//void onMouseUpMulti (TMouseButton MouseButton, TShiftState Shift,UsineEventPtr X, UsineEventPtr Y);
	//void onOpenEditor();
	//void onBringToFront();

	//-----------------------------------------------------------------------------
	// audio setup update
	//void onBlocSizeChange (int BlocSize);
	void onSampleRateChange (double SampleRate);
	
	//-----------------------------------------------------------------------------
	// recording 
	 void onSetRecordedValue (TPrecision X, TPrecision Y, TPrecision Z);


	//-------------------------------------------------------------------------
	// private members
	//-------------------------------------------------------------------------
private:

	//-------------------------------------------------------------------------
	// parameters events
    UsineEventClass m_audioInputs [USINE_MAX_AUDIO_INPUTS];     // audio input
    UsineEventClass m_audioOutputs [USINE_MAX_AUDIO_OUTPUTS];    // audio output
	UsineEventClass m_fdrGrainInterval;
	UsineEventClass m_ledGrainStart;
	UsineEventClass m_lboxGrainEnvelope;
	UsineEventClass m_fdrGrainEnvelopeSkrew;
	UsineEventClass m_fdrGrainDuration;
	UsineEventClass m_fdrGrainPitch;
	UsineEventClass m_lboxGrainMode;
	UsineEventClass m_btnGrainGo;
    	
	//-------------------------------------------------------------------------
	int queryResult;
	int numOfAudiotInsOuts;
	int numOfParamAfterAudiotInOut;

	// Graphic attributes of the envelope curve
    float m_LineThickness;
	TUsineColor m_LineColor;


    // list of points to paint the grain envelope 
    TPointF coordEnvAsr[ENV_ASR_SIZE];
    TPointF coordEnvBezier[ENV_BEZIER_SIZE];
    TPointF coordEnvRcb[ENV_RCB_SIZE];
    TPointF coordEnvHanning[ENV_HANNING_SIZE];
    TPointF coordEnvGaussian[ENV_GAUSSIAN_SIZE];
    
    TRectF  dimGrainDuration;
    std::string caGrainDuration;
    TRectF  dimPitch;
    std::string caPitch;
    
    /// 
    float usineSamplerate;

    
	///	Current read position in the delay line.
    int cbufReadPos;

    /// 
    int blockSize;

	
	///	Our delay line.
	TPrecision* circularBuffer[USINE_MAX_AUDIO_INPUTS];
    int cbufSize;

    /// Pointer to a temporary block of memory 
    TPrecision* tempInputBlock[USINE_MAX_AUDIO_INPUTS];
    TPrecision* tempOutputBlock[USINE_MAX_AUDIO_OUTPUTS];
    int audioLineNextOnset;
	///	Number of samples to the next onset.
	int nextOnset;

    /// 
//    int grainToggle;

	///	Our currentGrain.
	//Grain currentGrain[NUM_OF_GRAIN];
	std::vector <Grain*> currentGrain;
	int nextNewGrainIndex;

    /// current envelope used
    Grain::EnvType currentGrainEnvType;

    float grainEnvelopeVar;

	///	Current pitchIncrement (used to pitch the currentGrain).
	float pitchIncrement;

	///	Current size of the currentGrain.
	int grainSize;

    /// interval between current grain and next grain
    int grainInterval;


	int currentGrainMode;
	bool wantNewGrain;
	
	bool isRecording;
	int			m_mouseLastButtonDown;	    // -1 = no button, other see TMouseButton in UsineDefinition.h
	TPointF		m_mouseLastCoords;
	TPrecision	m_unusedY;
	TPrecision	m_unusedZ;

	//-------------------------------------------------------------------------
	// private methods
	//-------------------------------------------------------------------------
private:

	//-------------------------------------------------------------------------
    //
    void changeGrainEnvelope();
    void changeEnvelopeVar();
    void changeInterval();
    void changeDuration();
    void changePitch();
    void changeGrainMode();
    
	//-------------------------------------------------------------------------
    // 
    void grainGo();
	void generateNewGrain(int num);

}; // class GranulatorModule

#endif //INCLUDED_GRANULATORMODULE_H

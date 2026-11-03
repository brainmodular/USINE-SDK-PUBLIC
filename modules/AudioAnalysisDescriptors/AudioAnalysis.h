//-----------------------------------------------------------------------------
//@file  
//	AudioAnalysis.h
//
//@author
//	BrainModular team
//
//@brief 
//	Definitions of the AudioAnalysis class.
//
//  A template include to start from for new user module.
//
//@historic 
//	2013/05/15
//    first release for Hollyhock CPP SDK 6.00.226 
//
//@IMPORTANT
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
#pragma once

//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------

#include <iostream>
#include <cmath>
#include <limits>
#include <numeric>
#include <vector>
#include <complex>
#include <chrono>

#include "Hanalysis/Hanalysis.h"
#include "Hanalysis/HaudioBuffer.h"

#include "UserDefinitions.h"
//#include "UsineFunctions.h"


//-----------------------------------------------------------------------------
// defines and constantes
//-----------------------------------------------------------------------------

constexpr NativeInt PROCESS_BUFFER_SIZE = 20480;
constexpr NativeInt PROCESS_FFT_SIZE    = 4096;

constexpr int SIMPLE_ANALYSIS  = 0;
constexpr int COMPLEX_ANALYSIS = 1;

//-----------------------------------------------------------------------------
// class definition
//-----------------------------------------------------------------------------
class AudioAnalysis : public UserModuleBase
{
public:
    // constructor
    AudioAnalysis();

    // destructor
	virtual ~AudioAnalysis();

	//-------------------------------------------------------------------------
	// public methods inherited from UserModule
	//-------------------------------------------------------------------------
	//-----------------------------------------------------------------------------
	// module info
	void onGetModuleInfo(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo) override;
    //-----------------------------------------------------------------------------
    // query system and init
    int  onGetNumberOfParams(int queryResult1, int queryResult2) override;
    void onAfterQuery(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo, int queryResult1, int queryResult2) override;
	void onInitModule(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo) override;
    
	//-----------------------------------------------------------------------------
	// parameters and process
	void onGetParamInfo(int ParamIndex, TParamInfo* pParamInfo) override;
	void onCallBack(TUsineMessage *Message) override;
	void onProcess() override;
    void onAnalysis();  //  triggered when the buffer is ready to be analyzed
    
    //-----------------------------------------------------------------------------
    // audio setup update
    void onBlocSizeChange(int BlocSize) override;
    void onSampleRateChange(double SampleRate) override;

private:
	//-------------------------------------------------------------------------
	// UsineEventClass members
	//-------------------------------------------------------------------------
    UsineEventClass toggleGlobal;               //  global toggle
    UsineEventClass audioInput;                 //  audio input
    // Spectral
    UsineEventClass toggleSpectral;             //  toggle spectral analysis descriptors
    UsineEventClass spectralCentroid;           //  spectral centroid
    UsineEventClass spectralSpread;             //  spectral spread
    UsineEventClass spectralSkewness;           //  spectral skewness
    UsineEventClass spectralKurtosis;           //  spectral kurtosis
    UsineEventClass spectralSlope;              //  spectral slope
    UsineEventClass spectralRolloff;            //  spectral rolloff
    UsineEventClass rolloffThreshold;           //  rolloff threshold
    UsineEventClass spectralFlatness;           //  spectral flatness
    UsineEventClass spectralCrest;              //  spectral crest
    UsineEventClass tonalityCoeff;              //  tonality coefficient (10log(SFM)/-60)
    UsineEventClass spectralDecrease;           //  spectral decrease
    UsineEventClass spectralEntropy;            //  spectral entropy
    UsineEventClass peaksAmplitudes;            //  spectral peaks amplitudes array
    UsineEventClass peaksFrequencies;           //  spectral peaks frequencies array
    // Harmonic
    UsineEventClass toggleHarmonic;             //  toggle Harmonic Analysis
    UsineEventClass mfcc;                       //  mfcc array
    UsineEventClass fundFrequency;              //  estimated fondamental frequency
    UsineEventClass frequencyProba;             //  estimated frequency probability
    UsineEventClass inharmonicity;              //  inharmonicity
    UsineEventClass oddToEvenRatio;             //  Odd to even ratio
    UsineEventClass tristimulus;                //  Tristimulus 1, 2 and 3 array
    // Temporal
    UsineEventClass toggleTemporal;             //  toggle Temporal Analysis
    UsineEventClass rms;                        //  rms
    // Perceptual
    UsineEventClass togglePerceptual;           //  toggle Perceptual Analysis
    UsineEventClass loudness;                   //  total loudness
    UsineEventClass specificLoudness;           //  specific loudness array
    UsineEventClass sharpness;                  //  sharpness
    // Other
    //UsineEventClass autocorrelation;          //  autocorrelation
    UsineEventClass fftSize;                    //  change fft size 
    UsineEventClass overlap;                    //  change clock time
    UsineEventClass fftDuration;                //  duration of the fft (every X ms we do the calculations)
    

    //-------------------------------------------------------------------------
    // member classes

    AudioBuffer<float> audioBuffer;                                 //  buffer to analyze
    Hanalysis hanalysis;                                            //  Analysis class

    //-------------------------------------------------------------------------
    // variables
    float m_fSampleRate = static_cast<float>(sdkGetSampleRate());   //  sample rate
    int m_iBlocSize = sdkGetBlocSize();                             //  bloc size
    int m_FFTSize = 0;                                              //  fft size
    int m_overlap = 1;                                              //  overlap factor
    std::vector<SpectralPeak> peaksArray;

    //-------------------------------------------------------------------------

    // internal variables thread and query related

    TCriticalSectionPtr criticalSection;                            //  critical section
    bool isProcessing = false;                                      //  boolean of processing state

    TThreadPtr thread;                                              //  thread
    int m_QueryResult = 0;                                          //  query result

    // private methods
    void setFFTsize(int new_size);                                  //  change fft size

    static void processThread(void* pModule, TThreadPtr pThread);


}; // class AudioAnalysis

//-----------------------------------------------------------------------------
//@file  
//	AudioAnalysis.cpp
//
//@author
//	BrainModular team
//
//@brief 
//	Implementation of the AudioAnalysis class.
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

//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------
#include "AudioAnalysis.h"


//----------------------------------------------------------------------------
// callback ids
//----------------------------------------------------------------------------

constexpr NativeInt TOGGLE_ON_CBID          = 0x001000F0;
constexpr NativeInt FFT_SIZE_CBID           = 0x001000F1;
constexpr NativeInt OVERLAP_CBID            = 0x001000F2;
constexpr NativeInt TOGGLE_SPECTRAL_CBID    = 0x001000F3;
constexpr NativeInt ROLLOFF_THRES_CBID      = 0x001000F4;
constexpr NativeInt TOGGLE_HARMONIC_CBID    = 0x001000F5;
constexpr NativeInt TOGGLE_TEMPORAL_CBID    = 0x001000F6;
constexpr NativeInt TOGGLE_PERCEPTUAL_CBID  = 0x001000F7;


//----------------------------------------------------------------------------
// create, general info and destroy methods
//----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

void CreateModule(void*& pModule, AnsiCharPtr optionalString, LongBool Flag, TMasterInfo* pMasterInfo, AnsiCharPtr optionalContent)
{
    pModule = new AudioAnalysis();
}

void DestroyModule(void* pModule)
{
    delete ((AudioAnalysis*)pModule);
}

//-------------------------------------------------------------------------
// module constructors/destructors
//-------------------------------------------------------------------------

AudioAnalysis::AudioAnalysis() :
    audioBuffer(PROCESS_BUFFER_SIZE),
    hanalysis(PROCESS_FFT_SIZE, PROCESS_FFT_SIZE/2 , (float)sdkGetSampleRate(), this)
{
    hanalysis.initFFT(PROCESS_FFT_SIZE);
    
    // default values, will be overriden by parameters callbacks
    audioBuffer.setFFTSize(PROCESS_FFT_SIZE);
    audioBuffer.setClockTime(1);
}

AudioAnalysis::~AudioAnalysis()
{
    try
    {
        sdkCriticalSectionLock(criticalSection);
        sdkThreadDestroy(thread);
    }
    catch (...)
    {
        sdkTraceLogChar("error in AudioAnalysis destructor");
    }
    sdkCriticalSectionUnLock(criticalSection);
    sdkCriticalSectionDestroy(criticalSection);
}

constexpr AnsiCharPtr UserModuleBase::MODULE_NAME = "Audio Analysis";
constexpr AnsiCharPtr UserModuleBase::MODULE_DESC = "Audio Analysis";
constexpr AnsiCharPtr UserModuleBase::MODULE_VERSION = "1.0";

void GetBrowserInfo(TModuleInfo* pModuleInfo)
{
    pModuleInfo->Name = UserModuleBase::MODULE_NAME;
    pModuleInfo->Description = UserModuleBase::MODULE_DESC;
    pModuleInfo->Version = UserModuleBase::MODULE_VERSION;
}

void AudioAnalysis::onGetModuleInfo(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo)
{
    pModuleInfo->Name = MODULE_NAME;
    pModuleInfo->Description = MODULE_DESC;
    pModuleInfo->ModuleType = mtSimple;
    pModuleInfo->BackColor = sdkGetUsineColor(clDataModuleColor);
    pModuleInfo->Version = MODULE_VERSION;
    pModuleInfo->DontProcess = FALSE;

    if (pMasterInfo != nullptr)
    {
        pModuleInfo->QueryListString = "Audio analysis type";
        pModuleInfo->QueryListValues = "Basic,Advanced";
        pModuleInfo->QueryListDefaultIdx = 0;
        pModuleInfo->QueryDefaultValue1 = 0;
        pModuleInfo->QueryMaxValue1 = 1;
    }
    pModuleInfo->CanBeSavedInPreset = false;
}

int AudioAnalysis::onGetNumberOfParams(int queryResult1, int queryResult2)
{
    int result = 0;
    if (m_QueryResult == SIMPLE_ANALYSIS)
    {
        result = 13;
    }
    else if (m_QueryResult == COMPLEX_ANALYSIS)
    {
        result = 33;
    }
    return result;
}

void AudioAnalysis::onAfterQuery(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo, int queryResult1, int queryResult2)
{
    if (queryResult1 == 0 || queryResult1 == 1)
    {
        m_QueryResult = queryResult1;
        hanalysis.m_AnalysisType = queryResult1;
    }
}

// Thread function
void AudioAnalysis::processThread(void* pModule, TThreadPtr pThread)
{
    AudioAnalysis* module = static_cast<AudioAnalysis*>(pModule);
    try {
        module->isProcessing = TRUE;
        module->sdkCriticalSectionLock(module->criticalSection);
        
        module->onAnalysis();
    
        // time tracking
        static auto lastTime = std::chrono::steady_clock::now();
        auto currentTime = std::chrono::steady_clock::now();
        
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastTime).count();
        module->fftDuration.setData((float)duration);
        lastTime = currentTime;
        
    }
    catch (...)
    {
        sdkTraceLogChar("error in AudioAnalysis processThread");
    }
    module->isProcessing = FALSE;
    module->sdkCriticalSectionUnLock(module->criticalSection);
}

void AudioAnalysis::onInitModule(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo)
{
    criticalSection = sdkCriticalSectionCreate("critical section", true);
    thread = sdkThreadCreate("process thread", &processThread, tpMedium, UINT32_MAX);
}

//-----------------------------------------------------------------------------
// parameters and process
void AudioAnalysis::onGetParamInfo(int ParamIndex, TParamInfo* pParamInfo)
{
    switch (ParamIndex)
    {
    case 0:
        pParamInfo->ParamType = ptSwitch;
        pParamInfo->Caption = "on analysis";
        pParamInfo->IsInput = TRUE;
        pParamInfo->IsOutput = FALSE;
        pParamInfo->DefaultValue = true;
        pParamInfo->CallBackType = ctNormal;
        pParamInfo->CallBackId  = TOGGLE_ON_CBID;
        pParamInfo->setEventClass(toggleGlobal);
        break;

    case 1: // audio input
        pParamInfo->ParamType = ptAudio;
        pParamInfo->Caption = "in";
        pParamInfo->IsInput = TRUE;
        pParamInfo->IsOutput = FALSE;
        pParamInfo->CallBackType = ctNone;
        pParamInfo->setEventClass(audioInput);
        break;

    case 2:
        pParamInfo->ParamType = ptDataField;
        pParamInfo->Caption = "estimated frequency";
        pParamInfo->IsInput = FALSE;
        pParamInfo->IsOutput = TRUE;
        pParamInfo->IsSeparator = TRUE;
        pParamInfo->SeparatorCaption = "Frequency Estimation";
        pParamInfo->DefaultValue = 0.0f;
        pParamInfo->MinValue = 0.0f;
        pParamInfo->MaxValue = (float)(sdkGetSampleRate() / 2.0);
        pParamInfo->Format = "%.2f";
        pParamInfo->Symbol = "Hz";
        pParamInfo->CallBackType = ctNone;
        pParamInfo->setEventClass(fundFrequency);
        break;

    case 3:
        pParamInfo->ParamType = ptDataField;
        pParamInfo->Caption = "frequency score";
        pParamInfo->IsInput = FALSE;
        pParamInfo->IsOutput = TRUE;
        pParamInfo->DefaultValue = 0.0f;
        pParamInfo->MinValue = 0.0f;
        pParamInfo->MaxValue = 1.0f;
        pParamInfo->Format = "%.2f";
        pParamInfo->Symbol = "";
        pParamInfo->CallBackType = ctNone;
        pParamInfo->setEventClass(frequencyProba);
        break;

    case 4:
        pParamInfo->ParamType = ptDataField;
        pParamInfo->Caption = "spectral centroid";
        pParamInfo->IsInput = FALSE;
        pParamInfo->IsOutput = TRUE;
        pParamInfo->IsSeparator = TRUE;
        pParamInfo->SeparatorCaption = "Spectral Metrics";
        pParamInfo->DefaultValue = 0.0f;
        pParamInfo->MinValue = 0.0f;
        pParamInfo->MaxValue = (float)(sdkGetSampleRate() / 2.0);
        pParamInfo->Format = "%.2f";
        pParamInfo->Symbol = "Hz";
        pParamInfo->CallBackType = ctNone;
        pParamInfo->setEventClass(spectralCentroid);
        break;

    case 5:
        pParamInfo->ParamType = ptDataField;
        pParamInfo->Caption = "spectral spread";
        pParamInfo->IsInput = FALSE;
        pParamInfo->IsOutput = TRUE;
        pParamInfo->DefaultValue = 0.0f;
        pParamInfo->MinValue = 0.0f;
        pParamInfo->MaxValue = (float)(sdkGetSampleRate() / 2.0);
        pParamInfo->Format = "%.2f";
        pParamInfo->Symbol = "Hz";
        pParamInfo->CallBackType = ctNone;
        pParamInfo->setEventClass(spectralSpread);
        break;

    case 6:
        pParamInfo->ParamType = ptDataField;
        pParamInfo->Caption = "spectral flatness";
        pParamInfo->IsInput = FALSE;
        pParamInfo->IsOutput = TRUE;
        pParamInfo->DefaultValue = 0.0f;
        pParamInfo->MinValue = 0.0f;
        pParamInfo->MaxValue = 1.0f;
        pParamInfo->Format = "%.2f";
        pParamInfo->CallBackType = ctNone;
        pParamInfo->setEventClass(spectralFlatness);
        break;

    case 7:
        pParamInfo->ParamType = ptDataField;
        pParamInfo->Caption = "RMS";
        pParamInfo->IsInput = FALSE;
        pParamInfo->IsOutput = TRUE;
        pParamInfo->DefaultValue = 0.0f;
        pParamInfo->MinValue = -INFINITY;
        pParamInfo->MaxValue = 12.0f;
        pParamInfo->IsSeparator = TRUE;
        pParamInfo->SeparatorCaption = "Level Metrics";
        pParamInfo->Format = "%.2f";
        pParamInfo->Symbol = "dB";
        pParamInfo->CallBackType = ctNone;
        pParamInfo->setEventClass(rms);
        break;

    case 8:
        pParamInfo->ParamType = ptDataField;
        pParamInfo->Caption = "total loudness";
        pParamInfo->IsInput = FALSE;
        pParamInfo->IsOutput = TRUE;
        pParamInfo->DefaultValue = 0.0f;
        pParamInfo->MinValue = 0.0f;
        pParamInfo->MaxValue = 1.0f;
        pParamInfo->Format = "%.2f";
        pParamInfo->CallBackType = ctNone;
        pParamInfo->setEventClass(loudness);
        break;

    case 9:
        pParamInfo->ParamType = ptArray;
        pParamInfo->Caption = "loudness per bands";
        pParamInfo->IsInput = FALSE;
        pParamInfo->IsOutput = TRUE;
        pParamInfo->DefaultValue = 0.0f;
        pParamInfo->MinValue = 0.0f;
        pParamInfo->MaxValue = 1.0f;
        pParamInfo->Format = "%.2f";
        pParamInfo->CallBackType = ctNone;
        pParamInfo->setEventClass(specificLoudness);
        break;

    case 10:
        pParamInfo->ParamType = ptListBox;
        pParamInfo->Caption = "fft size";
        pParamInfo->IsInput = TRUE;
        pParamInfo->IsOutput = FALSE;
        pParamInfo->DefaultValue = 2;
        pParamInfo->MinValue = 0;
        pParamInfo->MaxValue = 3;
        pParamInfo->IsSeparator = TRUE;
        pParamInfo->SeparatorCaption = "Analysis Parameters";
        pParamInfo->ListBoxStrings = "1024,2048,4096,8192";
        pParamInfo->CallBackType = ctNormal;
        pParamInfo->CallBackId  = FFT_SIZE_CBID;
        pParamInfo->setEventClass(fftSize);
        break;

    case 11:
        pParamInfo->ParamType = ptListBox;
        pParamInfo->Caption = "overlap";
        pParamInfo->IsInput = TRUE;
        pParamInfo->IsOutput = FALSE;
        pParamInfo->DefaultValue = 0;
        pParamInfo->MinValue = 0;
        pParamInfo->MaxValue = 2;
        pParamInfo->ListBoxStrings = "1,2,4";
        pParamInfo->CallBackType = ctNormal;
        pParamInfo->CallBackId  = OVERLAP_CBID;
        pParamInfo->setEventClass(overlap);
        break;

    case 12:
        pParamInfo->ParamType = ptDataField;
        pParamInfo->Caption = "latency";
        pParamInfo->IsInput = FALSE;
        pParamInfo->IsOutput = TRUE;
        pParamInfo->DefaultValue = 0.0f;
        pParamInfo->MinValue = 0.0f;
        pParamInfo->MaxValue = 1000.0f;
        pParamInfo->Format = "%.2f";
        pParamInfo->Symbol = "ms";
        pParamInfo->CallBackType = ctNone;
        pParamInfo->setEventClass(fftDuration);
        break;

    case 13:
        pParamInfo->ParamType = ptSwitch;
        pParamInfo->Caption = "on spectral";
        pParamInfo->IsInput = TRUE;
        pParamInfo->IsOutput = FALSE;
        pParamInfo->IsSeparator = TRUE;
        pParamInfo->DefaultValue = true;
        pParamInfo->SeparatorCaption = "Spectral";
        pParamInfo->CallBackType = ctNormal;
        pParamInfo->CallBackId  = TOGGLE_SPECTRAL_CBID;
        pParamInfo->setEventClass(toggleSpectral);
        break;

    case 14:
        pParamInfo->ParamType = ptDataField;
        pParamInfo->Caption = "spectral skewness";
        pParamInfo->IsInput = FALSE;
        pParamInfo->IsOutput = TRUE;
        pParamInfo->DefaultValue = 0.0f;
        pParamInfo->MinValue = -100.0f;
        pParamInfo->MaxValue = 100.0f;
        pParamInfo->Format = "%.2f";
        pParamInfo->setEventClass(spectralSkewness);
        break;

    case 15:
        pParamInfo->ParamType = ptDataField;
        pParamInfo->Caption = "spectral kurtosis";
        pParamInfo->IsInput = FALSE;
        pParamInfo->IsOutput = TRUE;
        pParamInfo->DefaultValue = 0.0f;
        pParamInfo->MinValue = -100.0f;
        pParamInfo->MaxValue = 100.0f;
        pParamInfo->Format = "%.2f";
        pParamInfo->setEventClass(spectralKurtosis);
        break;

    case 16:
        pParamInfo->ParamType = ptDataField;
        pParamInfo->Caption = "spectral slope";
        pParamInfo->IsInput = FALSE;
        pParamInfo->IsOutput = TRUE;
        pParamInfo->DefaultValue = 0.0f;
        pParamInfo->MinValue = 0.0f;
        pParamInfo->MaxValue = 10.0f;
        pParamInfo->Format = "%.2f";
        pParamInfo->Symbol = "1/Hz";
        pParamInfo->setEventClass(spectralSlope);
        break;

    case 17:
        pParamInfo->ParamType = ptDataField;
        pParamInfo->Caption = "spectral rolloff";
        pParamInfo->IsInput = FALSE;
        pParamInfo->IsOutput = TRUE;
        pParamInfo->DefaultValue = 0.0f;
        pParamInfo->MinValue = 0.0f;
        pParamInfo->MaxValue = (float)(sdkGetSampleRate() / 2.0);
        pParamInfo->Format = "%.2f";
        pParamInfo->Symbol = "Hz";
        pParamInfo->setEventClass(spectralRolloff);
        break;

    case 18:
        pParamInfo->ParamType = ptDataFader;
        pParamInfo->Caption = "rolloff threshold";
        pParamInfo->IsInput = TRUE;
        pParamInfo->IsOutput = FALSE;
        pParamInfo->DefaultValue = 0.95f;
        pParamInfo->MinValue = 0.50f;
        pParamInfo->MaxValue = 0.99f;
        pParamInfo->Format = "%.2f";
        pParamInfo->Symbol = "%";
        pParamInfo->CallBackType = ctNormal;
        pParamInfo->CallBackId  = ROLLOFF_THRES_CBID;
        pParamInfo->setEventClass(rolloffThreshold);
        break;


    case 19:
        pParamInfo->ParamType = ptDataField;
        pParamInfo->Caption = "spectral crest";
        pParamInfo->IsInput = FALSE;
        pParamInfo->IsOutput = TRUE;
        pParamInfo->DefaultValue = 0.0f;
        pParamInfo->MinValue = 0.0f;
        pParamInfo->MaxValue = (float)(sdkGetSampleRate() / 2.0);
        pParamInfo->Format = "%.2f";
        pParamInfo->setEventClass(spectralCrest);
        break;

    case 20:
        pParamInfo->ParamType = ptDataField;
        pParamInfo->Caption = "tonality coeff";
        pParamInfo->IsInput = FALSE;
        pParamInfo->IsOutput = TRUE;
        pParamInfo->DefaultValue = 0.0f;
        pParamInfo->MinValue = 0.0f;
        pParamInfo->MaxValue = 1.0f;
        pParamInfo->Format = "%.2f";
        pParamInfo->Symbol = "%";
        pParamInfo->setEventClass(tonalityCoeff);
        break;

    case 21:
        pParamInfo->ParamType = ptDataField;
        pParamInfo->Caption = "spectral decrease";
        pParamInfo->IsInput = FALSE;
        pParamInfo->IsOutput = TRUE;
        pParamInfo->DefaultValue = 0.0f;
        pParamInfo->MinValue = 0.0f;
        pParamInfo->MaxValue = 1.0f;
        pParamInfo->Format = "%.2f";
        pParamInfo->Symbol = "1/Hz";
        pParamInfo->setEventClass(spectralDecrease);
        break;

    case 22:
        pParamInfo->ParamType = ptDataField;
        pParamInfo->Caption = "spectral entropy";
        pParamInfo->IsInput = FALSE;
        pParamInfo->IsOutput = TRUE;
        pParamInfo->DefaultValue = 0.0f;
        pParamInfo->MinValue = 0.0f;
        pParamInfo->MaxValue = (float)std::log(PROCESS_FFT_SIZE);
        pParamInfo->Format = "%.2f";
        pParamInfo->setEventClass(spectralEntropy);
        break;

    case 23:
        pParamInfo->ParamType = ptArray;
        pParamInfo->Caption = "peaks amplitudes";
        pParamInfo->IsInput = FALSE;
        pParamInfo->IsOutput = TRUE;
        pParamInfo->DefaultValue = 0.0f;
        pParamInfo->MinValue = 0.0f;
        pParamInfo->MaxValue = 1.0f;
        pParamInfo->Format = "%.2f";
        pParamInfo->setEventClass(peaksAmplitudes);
        break;

    case 24:
        pParamInfo->ParamType = ptArray;
        pParamInfo->Caption = "peaks frequencies";
        pParamInfo->IsInput = FALSE;
        pParamInfo->IsOutput = TRUE;
        pParamInfo->DefaultValue = 0.0f;
        pParamInfo->MinValue = 0.0f;
        pParamInfo->Format = "%.2f";
        pParamInfo->MaxValue = (float)(sdkGetSampleRate() / 2.0);
        pParamInfo->setEventClass(peaksFrequencies);
        break;

    case 25:
        pParamInfo->ParamType = ptSwitch;
        pParamInfo->Caption = "on harmonic";
        pParamInfo->IsInput = TRUE;
        pParamInfo->IsOutput = FALSE;
        pParamInfo->IsSeparator = TRUE;
        pParamInfo->DefaultValue = true;
        pParamInfo->SeparatorCaption = "Harmonic";
        pParamInfo->CallBackType = ctNormal;
        pParamInfo->CallBackId  = TOGGLE_HARMONIC_CBID;
        pParamInfo->setEventClass(toggleHarmonic);
        break;

    case 26:
        pParamInfo->ParamType = ptArray;
        pParamInfo->Caption = "MFCC";
        pParamInfo->IsInput = FALSE;
        pParamInfo->IsOutput = TRUE;
        pParamInfo->MinValue = -5;
        pParamInfo->MaxValue = 10;
        pParamInfo->Format = "%.2f";
        pParamInfo->setEventClass(mfcc);
        break;

    case 27:
        pParamInfo->ParamType = ptDataField;
        pParamInfo->Caption = "inharmonicity";
        pParamInfo->IsInput = FALSE;
        pParamInfo->IsOutput = TRUE;
        pParamInfo->DefaultValue = 0.0f;
        pParamInfo->MinValue = 0.0f;
        pParamInfo->MaxValue = 1.0f;
        pParamInfo->Format = "%.2f";
        pParamInfo->setEventClass(inharmonicity);
        break;

    case 28:
        pParamInfo->ParamType = ptDataField;
        pParamInfo->Caption = "odd to even ratio";
        pParamInfo->IsInput = FALSE;
        pParamInfo->IsOutput = TRUE;
        pParamInfo->DefaultValue = 0.0f;
        pParamInfo->MinValue = -1.0f;
        pParamInfo->MaxValue = 1.0f;
        pParamInfo->Format = "%.2f";
        pParamInfo->setEventClass(oddToEvenRatio);
        break;

    case 29:
        pParamInfo->ParamType = ptArray;
        pParamInfo->Caption = "tristimulus";
        pParamInfo->IsInput = FALSE;
        pParamInfo->IsOutput = TRUE;
        pParamInfo->DefaultValue = 0.0f;
        pParamInfo->MinValue = 0.0f;
        pParamInfo->MaxValue = 100.0f;
        pParamInfo->Format = "%.1f";
        pParamInfo->setEventClass(tristimulus);
        break;

    case 30:
        pParamInfo->ParamType = ptSwitch;
        pParamInfo->Caption = "on temporal";
        pParamInfo->IsInput = TRUE;
        pParamInfo->IsOutput = FALSE;
        pParamInfo->IsSeparator = TRUE;
        pParamInfo->DefaultValue = true;
        pParamInfo->SeparatorCaption = "Temporal";
        pParamInfo->CallBackType = ctNormal;
        pParamInfo->CallBackId  = TOGGLE_TEMPORAL_CBID;
        pParamInfo->setEventClass(toggleTemporal);
        break;

    case 31:
        pParamInfo->ParamType = ptSwitch;
        pParamInfo->Caption = "on perceptual";
        pParamInfo->IsInput = TRUE;
        pParamInfo->IsOutput = FALSE;
        pParamInfo->IsSeparator = TRUE;
        pParamInfo->DefaultValue = true;
        pParamInfo->SeparatorCaption = "Perceptual";
        pParamInfo->CallBackType = ctNormal;
        pParamInfo->CallBackId  = TOGGLE_PERCEPTUAL_CBID;
        pParamInfo->setEventClass(togglePerceptual);
        break;

    case 32:
        pParamInfo->ParamType = ptDataFader;
        pParamInfo->Caption = "sharpness";
        pParamInfo->IsInput = FALSE;
        pParamInfo->IsOutput = TRUE;
        pParamInfo->DefaultValue = 0.0f;
        pParamInfo->MinValue = 0.0f;
        pParamInfo->MaxValue = 100.0f;
        pParamInfo->Format = "%.2f";
        pParamInfo->setEventClass(sharpness);
        break;

    default:
        break;
    }
}

void AudioAnalysis::onCallBack(TUsineMessage* Message)
{
    try {
        if (Message->message == NOTIFY_MSG_USINE_CALLBACK && Message->lParam == MSG_CHANGE)
        {
            switch (Message->wParam)
            {
                // Toggles
                // global on/off
            case TOGGLE_ON_CBID:
                hanalysis.globalBool = toggleGlobal.getData();
                break;
                // on/off spectral
            case TOGGLE_SPECTRAL_CBID:
                hanalysis.spectralBool = toggleSpectral.getData();
                break;
                // Parameters change
            case ROLLOFF_THRES_CBID:
                if (rolloffThreshold.getData() < 1.0f && rolloffThreshold.getData() >= 0.5f)
                {
                    hanalysis.features.rolloffThreshold = rolloffThreshold.getData();
                }
                break;
                // on/off harmonic
            case TOGGLE_HARMONIC_CBID:
                hanalysis.harmonicBool = toggleHarmonic.getData();
                break;
                // on/off temporal
            case TOGGLE_TEMPORAL_CBID:
                hanalysis.temporalBool = toggleTemporal.getData();
                break;
                // on/off perceptual
            case TOGGLE_PERCEPTUAL_CBID:
                hanalysis.perceptualBool = togglePerceptual.getData();
                break;
                // onChange FFT size
            case FFT_SIZE_CBID:
                ////////// TODO CHECK ALTERNATIVE 1024=1024...
                switch((int)fftSize.getData()) {
                    case 1 : m_FFTSize = 2048; break;
                    case 2 : m_FFTSize = 4096; break;
                    case 3 : m_FFTSize = 8192; break;
                    default: m_FFTSize = 1024; break;
                }
                try {
                    sdkCriticalSectionLock(criticalSection);
                    setFFTsize(m_FFTSize);
                }
                catch (...)
                {
                    sdkTraceLogChar("Error while changing the fft size");
                }
                sdkCriticalSectionUnLock(criticalSection);
                break;
                // onChange Clock Time
            case OVERLAP_CBID:
                switch((int)overlap.getData()) {
                    case 1: m_overlap = 2; break;
                    case 2: m_overlap = 4; break;
                    default: m_overlap = 1; break;
                }
                try {
                    sdkCriticalSectionLock(criticalSection);
                    audioBuffer.setClockTime(m_overlap);
                }
                catch (...) {
                    sdkTraceLogChar("Error while changing the clock time");
                }
                sdkCriticalSectionUnLock(criticalSection);
                break;
            default:
                // do nothing
                break;
            }
        }
    }
    catch (...) {
        sdkTraceErrorChar("error on callback");
    }
}

void AudioAnalysis::onProcess()
{
    if (sdkPatchJustActivated())
    {
        audioBuffer.reset();
    }

    if (audioBuffer.isFull())
    {
        audioBuffer.copyToBeginning();
    }

    audioBuffer.add(audioInput.getDataAddr(), sdkGetBlocSize());

    if (audioBuffer.isReadyToProcess() && hanalysis.globalBool && !isProcessing)
    {
        sdkThreadRestart(thread);
    }
}

void AudioAnalysis::onAnalysis()
{
    if (audioBuffer.getProcessBufferPtr())
    {
        hanalysis.process(audioBuffer.getProcessBufferPtr(), m_FFTSize, (float)sdkGetSampleRate());
    }

    if (m_QueryResult == SIMPLE_ANALYSIS)
    {
        // frequency estimation
        if (hanalysis.results.harmonicF0_YIN.frequency > 40.0f && hanalysis.results.harmonicF0_YIN.frequency < 3000.0f)
        {
            fundFrequency.setData(hanalysis.results.harmonicF0_YIN.frequency);
        }
        frequencyProba.setData(hanalysis.results.harmonicF0_YIN.probability);

        // spectral metrics
        spectralCentroid.setData(hanalysis.results.spectralCentroid);
        spectralSpread.setData(hanalysis.results.spectralSpread);
        spectralFlatness.setData(hanalysis.results.spectralFlatness);

        // level metrics
        rms.setData(hanalysis.results.temporalRMS);
        loudness.setData(hanalysis.results.perceptLoudness.totalLoudness);
        specificLoudness.setSize(25);
        memcpy(specificLoudness.getDataAddr(), hanalysis.results.perceptLoudness.specificLoudness.data(), sizeof(float) * 25);

    }
    else if (m_QueryResult == COMPLEX_ANALYSIS)
    {
        // spectral
        spectralCentroid.setData(hanalysis.results.spectralCentroid);
        spectralSpread.setData(hanalysis.results.spectralSpread);
        spectralSkewness.setData(hanalysis.results.spectralSkewness);
        spectralKurtosis.setData(hanalysis.results.spectralKurtosis);
        spectralSlope.setData(hanalysis.results.spectralSlope);
        spectralRolloff.setData(hanalysis.results.spectralRolloff);
        spectralFlatness.setData(hanalysis.results.spectralFlatness);
        spectralCrest.setData(hanalysis.results.spectralCrest);
        tonalityCoeff.setData(hanalysis.results.spectralTonality);
        spectralDecrease.setData(hanalysis.results.spectralDecrease);
        spectralEntropy.setData(hanalysis.results.spectralEntropy);
        peaksArray.clear();
        peaksArray = hanalysis.results.spectralPeaks;
        peaksAmplitudes.setSize(int(peaksArray.size()));
        peaksFrequencies.setSize(int(peaksArray.size()));
        for (int i = 0; i < peaksArray.size(); i++)
        {
            peaksFrequencies.setArrayData(i, peaksArray[i].frequency);
            peaksAmplitudes.setArrayData(i, peaksArray[i].magnitude);
        }

        // harmonic
        mfcc.setSize(13);
        memcpy(mfcc.getDataAddr(), hanalysis.results.harmonicMfcc.data(), sizeof(float) * 13);
        if (hanalysis.results.harmonicF0_YIN.frequency > 40.0f && hanalysis.results.harmonicF0_YIN.frequency < 3000.0f)
        {
            fundFrequency.setData(hanalysis.results.harmonicF0_YIN.frequency);
        }
        frequencyProba.setData(hanalysis.results.harmonicF0_YIN.probability);
        inharmonicity.setData(hanalysis.results.harmonicInharmonicity);
        oddToEvenRatio.setData(hanalysis.results.harmonicOTERatio);
        tristimulus.setSize(3);
        memcpy(tristimulus.getDataAddr(), hanalysis.results.harmonicTristimulus.data(), sizeof(float) * 3);

        // temporal
        rms.setData(hanalysis.results.temporalRMS);

        // perceptual
        loudness.setData(hanalysis.results.perceptLoudness.totalLoudness);
        specificLoudness.setSize(25);
        memcpy(specificLoudness.getDataAddr(), hanalysis.results.perceptLoudness.specificLoudness.data(), sizeof(float) * 25);
        sharpness.setData(hanalysis.results.perceptSharpness);
    }
}

void AudioAnalysis::setFFTsize(int new_size)
{
    audioBuffer.setFFTSize(new_size);
    hanalysis.setFFTSize(new_size, (float)sdkGetSampleRate());
}


//-----------------------------------------------------------------------------
// audio setup update

void AudioAnalysis::onBlocSizeChange(int BlocSize) {
    try{
        sdkCriticalSectionLock(criticalSection);
        m_iBlocSize = BlocSize;
    } catch (...)
    {
        sdkTraceLogChar("Error while changing bloc size");
    }
    sdkCriticalSectionUnLock(criticalSection);
}

void AudioAnalysis::onSampleRateChange(double SampleRate) {
    
    m_fSampleRate = float(SampleRate);
    hanalysis.setSampleRate(m_fSampleRate);
}

#pragma once

#include <vector>
#include <cmath>
#include <complex>
#include <limits>
#include <algorithm>

#include "Hfeatures.h"
#include "Hdescriptors.h"
#include "UsineDefinitions.h"


constexpr int NUM_BARK_BANDS  = 25;


// forward declaration
class AudioAnalysis;


class Hanalysis
{
public:

    Hanalysis(int buffer_size, int fft_size, float sample_rate, AudioAnalysis* modulePtr);

    ~Hanalysis();

    void initializeHanningWindow();

    void initFFT(int buffer_size);
    void destroyFFT();


    void applyHanningWindow(float* ptrAudioInput, int buffer_size);

    void computeFeatures(float* ptrAudioInput, const int buffer_size, const float sample_rate);
    void computeSimpleFeatures(float* ptrAudioInput, const int buffer_size, const float sample_rate);
    void computeDescriptors(Features& features, Results& results);
    void computeSimpleDescriptors(Features& features, Results& results);

    void reset(Features& features);


    void process(float* ptrAudioInput, const int buffer_size, const float sample_rate);
    
    void setFFTSize(int new_size, float sample_rate);
    
    void setSampleRate(float new_sample_rate);


    Features features;
    Results results;
    Descriptors descriptors;

    bool globalBool = false;
    bool spectralBool = false;
    bool harmonicBool = false;
    bool temporalBool = false;
    bool perceptualBool = false;

    int m_AnalysisType = 0;

private:
    int m_BufferSize = 4096;
    int m_FftSize = m_BufferSize / 2 + 1;
    float m_SampleRate = 44100.0f;

    std::vector<float> m_HanningWindow;

    TUsineFFTPtr m_FftPtr = nullptr;
    std::vector<TPrecision> m_Re;
    std::vector<TPrecision> m_Im;

}; // class Hanalysis

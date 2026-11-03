#pragma once

#include <vector>
#include <cmath>

static constexpr int meanSpectrumSize = 8192;


struct PitchProbability {
    float frequency = 0.0f;
    float probability = 0.0f;
};

struct Features {
    int fft_size;
    float sample_rate;
    int buffer_size;
        // Precompute values
    float* ptrAudio = nullptr;
    std::vector<float> windowedAudioBuffer;
    std::vector<float> magnitudes;
    std::vector<float> unnormalized_magnitudes;
    std::vector<float> frequencies;
    std::vector<float> amplitudes;
    float sum_amplitudes = 0.0f;
    float sum_magnitudes = 0.0f;
    float sum_magnitudes_times_freqs = 0.0f;
    float sum_amplitudes_times_freqs = 0.0f;

        // Results values
    float spectralCentroid = 0.0f;
    float spectralSpread = 0.0f;
    float spectralFlatness = 0.0f;
    std::vector<float> harmonicMfcc;
    std::vector<float> harmonicF0_YIN;
    float temporalRMS = 0.0f;
    float loudness = 0.0f;
    std::vector<float> realPart;
    std::vector<float> imPart;
    std::vector<float> fftMags;
    
    Features(int buffer_size, int fft_size, float sample_rate) :
    windowedAudioBuffer(buffer_size,0.0f),
    magnitudes(fft_size, 0.0f),
    amplitudes(fft_size,0.0f),
    frequencies(fft_size, 0.0f),
    harmonicMfcc(13,0.0f),
    realPart(8192,0.0f),
    imPart(8192,0.0f),
    fftMags(8192,0.0f)
    {
        this->fft_size = fft_size;
        this->buffer_size = buffer_size;
        this->sample_rate = sample_rate;
        
    }
    
};


struct FeaturesResults
{
    float spectralCentroid = 0.0f;
    float spectralSpread = 0.0f;
    float spectralFlatness = 0.0f;
    std::vector<float> harmonicMfcc;
    std::vector<float> harmonicF0_YIN;
    float temporalRMS = 0.0f;
    float loudness = 0.0f;
    std::vector<float> fftMags;
    int frameCounter = 0;
    
};






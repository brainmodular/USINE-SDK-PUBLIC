#pragma once

#include <vector>
#include <cmath>
#include "UsineDefinitions.h"

// <cmath> already defines M_PI as a macro on macOS/Linux; only provide it where it is missing (MSVC).
#ifndef M_PI
constexpr TPrecision M_PI = 3.14159265358979323846264338327f;
#endif

struct SpectralPeak {
    float frequency = 0.0f;
    float magnitude = 0.0f;
};

struct Loudness_s {
    float totalLoudness = 0.0f;
    std::vector<float> specificLoudness;
    
    Loudness_s(int size) : specificLoudness(size,0.0f)
    {
    };
};

struct PitchProbability {
    float frequency = 0.0f;
    float probability = 0.0f;
};

struct Features {
        // Parameters
    int buffer_size;
    int fft_size;
    float sample_rate;
    
    float rolloffThreshold = 0.95f;
    
        // Precompute values
    float* ptrAudio = nullptr;
    std::vector<float> windowedAudioBuffer;
    std::vector<float> unnormalized_magnitudes;
    std::vector<float> magnitudes;
    std::vector<float> amplitudes;
    std::vector<float> frequencies;
    float sum_magnitudes = 0.0f;
    float sum_amplitudes = 0.0f;
    float sum_magnitudes_times_freqs = 0.0f;
    float sum_frequencies = 0.0f;
    float sum_frequencies_squared = 0.0f;
    float prod_magnitudes = 0.0f;
    float max_magnitude = 0.0f;
    float max_frequency = 0.0f;
    
    Features(int buffer_size, int fft_size, float sample_rate) :
    buffer_size(buffer_size),
    fft_size(fft_size),
    sample_rate(sample_rate),
    windowedAudioBuffer(buffer_size,0.0f),
    unnormalized_magnitudes(fft_size, 0.0f),
    magnitudes(fft_size, 0.0f),
    amplitudes(fft_size, 0.0f),
    frequencies(fft_size, 0.0f)
    {
    }
    
};

struct Results {
        // Results values
    float spectralCentroid = 0.0f;
    float spectralSpread = 0.0f;
    float spectralSkewness = 0.0f;
    float spectralKurtosis = 0.0f;
    float spectralSlope = 0.0f;
    float spectralRolloff = 0.0f;
    float spectralFlatness = 0.0f;
    float spectralCrest = 0.0f;
    float spectralTonality = 0.0f;
    float spectralDecrease = 0.0f;
    float spectralEntropy = 0.0f;
    std::vector<SpectralPeak> spectralPeaks;
    std::vector<float> harmonicMfcc;
    PitchProbability harmonicF0_YIN;
    std::vector<float> autocorr;
    float harmonicInharmonicity = 0.0f;
    float harmonicOTERatio = 0.0f;
    std::vector<float> harmonicTristimulus;
    float temporalRMS = 0.0f;
    Loudness_s perceptLoudness;
    float perceptSharpness = 0.0f;
    
    Results(int buffer_size) :
    autocorr(buffer_size,0.0f),
    harmonicTristimulus(3,0.0f),
    harmonicMfcc(13,0.0f),
    perceptLoudness(25)
    {
        
    }
};

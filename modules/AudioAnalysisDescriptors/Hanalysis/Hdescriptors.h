#pragma once

#include "functions/f0_YIN.h"
#include "functions/RMS.h"
#include "functions/Loudness.h"
#include "functions/Sharpness.h"
#include "functions/SpectralCentroid.h"
#include "functions/SpectralSpread.h"
#include "functions/SpectralKurtosis.h"
#include "functions/SpectralSkewness.h"
#include "functions/SpectralSlope.h"
#include "functions/SpectralRolloff.h"
#include "functions/SpectralFlatness.h"
#include "functions/SpectralCrest.h"
#include "functions/SpectralTonality.h"
#include "functions/SpectralDecrease.h"
#include "functions/SpectralEntropy.h"
#include "functions/SpectralPeaks.h"
#include "functions/MFCC.h"
#include "functions/Tristimulus.h"
#include "functions/OddToEvenRatio.h"
#include "functions/Inharmonicity.h"


struct Descriptors {
    SpectralCentroid spectralCentroid;
    SpectralSpread spectralSpread;
    SpectralKurtosis spectralKurtosis;
    SpectralSkewness spectralSkewness;
    SpectralSlope spectralSlope;
    SpectralRolloff spectralRolloff;
    SpectralFlatness spectralFlatness;
    SpectralCrest spectralCrest;
    SpectralTonality spectralTonality;
    SpectralDecrease spectralDecrease;
    SpectralEntropy spectralEntropy;
    SpectralPeaks spectralPeaks;
    MFCC mfcc;
    F0_YIN f0_YIN;
    Inharmonicity inharmonicity;
    OddToEvenRatio oddToEvenRatio;
    Tristimulus tristimulus;
    RMS rms;
    Loudness loudness;
    Sharpness sharpness;
    
    void setFftSize(int new_size, float sample_rate)
    {
        f0_YIN.setFFTSize(new_size);
        mfcc.setFftSize(new_size, sample_rate);
        loudness.setFftSize(new_size, sample_rate);
    }
    
    void setSampleRate(int fft_size, float sample_rate)
    {
        f0_YIN.setSampleRate(fft_size,sample_rate);
        mfcc.setFftSize(fft_size, sample_rate);
        loudness.setFftSize(fft_size, sample_rate);
    }
    
    Descriptors(int buffer_size, int fft_size, float sample_rate, int num_bark_bands) :
    mfcc(fft_size, sample_rate),
    f0_YIN(buffer_size, sample_rate),
    loudness(num_bark_bands, buffer_size, sample_rate),
    sharpness(num_bark_bands)
    {
    }
    
};

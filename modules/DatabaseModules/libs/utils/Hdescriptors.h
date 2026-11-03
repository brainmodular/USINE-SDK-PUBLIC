#pragma once

#include "functions/RMS.h"
#include "functions/SpectralCentroid.h"
#include "functions/SpectralSpread.h"
#include "functions/SpectralFlatness.h"
#include "functions/MFCC.h"
#include "functions/f0_YIN.h"
#include "functions/Loudness.h"

struct Descriptors {
    SpectralCentroid spectralCentroid;
    SpectralSpread spectralSpread;
    SpectralFlatness spectralFlatness;
    MFCC mfcc;
    RMS rms;
    F0_YIN f0_yin;
    Loudness loudness;
    
    void setSampleRate(int fft_size, float sample_rate)
    {
        mfcc.setFftSize(fft_size, sample_rate);
        f0_yin.setSampleRate(fft_size, sample_rate);
        loudness.setFftSize(fft_size, sample_rate);
    }
    
    Descriptors(int buffer_size, int fft_size, float sample_rate, int num_bark_bands) :
    mfcc(fft_size, sample_rate),
    f0_yin(buffer_size, sample_rate),
    loudness(num_bark_bands,buffer_size,sample_rate)
    {
        
    }
    
};

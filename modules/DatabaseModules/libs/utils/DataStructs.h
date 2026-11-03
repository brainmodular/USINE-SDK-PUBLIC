#pragma once
#include <vector>
#include "functions/MFCC.h"
#include "functions/Rms.h"
#include "functions/SpectralCentroid.h"
#include "functions/SpectralFlatness.h"
#include "functions/SpectralSpread.h"
#include "functions/HarmonicF0.h"
#include "functions/Loudness.h"


struct FileStatsResults
{
    // struct of results for the file :
    // mean
    // std dev
    
    
    std::vector<float> statsCentroid = std::vector<float>(2,0.0f);
    std::vector<float> statsSpread = std::vector<float>(2,0.0f);
    std::vector<float> statsFlatness = std::vector<float>(2,0.0f);
    std::vector<std::vector<float>> statsMfcc = std::vector<std::vector<float>>(2, std::vector<float>(13,0.0f));
    std::vector<float> statsRms = std::vector<float>(2,0.0f);
    

    
};



/*
struct Descriptors
{
    SpectralCentroid spectralCentroid;
    SpectralFlatness spectralFlatness;
    SpectralSpread spectralSpread;
    Rms rms;
    MFCC mfcc;
    //freq

    Descriptors(int fft_size, float sample_rate) :
        spectralCentroid(),
        spectralFlatness(),
        spectralSpread(),
        rms(),
        mfcc(fft_size, sample_rate)
    {
    }

}
*/

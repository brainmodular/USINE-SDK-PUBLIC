#include "SpectralFlatness.h"

namespace
{
    // floor of the normalized power of a bin (log(0) guard): -120 dB of the total power, below the precision
    // of the float fft. A higher floor bounds the flatness of a tonal sound (float epsilon: -36 dB at 2049 bins,
    // so the tonality coefficient, flatness in dB / -60, could not exceed 0.6)
    constexpr float FLATNESS_FLOOR = 1e-12f;
}

void SpectralFlatness::process(Features& features, Results& results)
{
    float result = 0.0f;
    
    if(features.sum_magnitudes != 0.0f)
    {
        float geometricMean = 0.0f;
        
        
        for(int i = 0; i < features.fft_size ; i++)
        {
            geometricMean += logf(features.magnitudes[i] + FLATNESS_FLOOR);
            
        }
        geometricMean = expf(geometricMean / features.fft_size);
        
        if(geometricMean > 0.0f)
        {
            // divided by sum_mags / fft_size with mags normalized so 1.0f
            result = geometricMean / (1.0f / static_cast<float>(features.fft_size));
        }
        
        if(result < 0.0f)
            result = 0.0f;
        
    }
    
    results.spectralFlatness = result;
}


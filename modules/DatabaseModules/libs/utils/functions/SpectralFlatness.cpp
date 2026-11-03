#include "SpectralFlatness.h"



void SpectralFlatness::process(Features& features)
{
    float result = 0.0f;
    
    if(features.sum_magnitudes != 0.0f)
    {
        float geometricMean = 0.0f;
        
        
        for(int i = 0; i < features.fft_size ; i++)
        {
            geometricMean += logf(features.magnitudes[i] + std::numeric_limits<float>::epsilon());
            
        }
        geometricMean = expf(geometricMean / features.fft_size);
        
        if(geometricMean > 0.0f)
        {
            // divided by sum_mags / fft_size with mags normalized so 1.0f
            result = geometricMean / (1.0f/float(features.fft_size));
        }
        
        if(result < 0.0f)
            result = 0.0f;
        
    }
    
    features.spectralFlatness = result;
}


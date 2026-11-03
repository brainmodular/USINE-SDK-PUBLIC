#include "SpectralDecrease.h"

void SpectralDecrease::process(Features& features, Results& results)
{
    float result = 0.0f;
    float num = 0.0f;
    float denum = 0.0f;
    
    float firstMagnitude = features.magnitudes[0];
    
    for(int i = 1; i < features.fft_size; i++)
    {
        num += (features.magnitudes[i] - firstMagnitude) / float(i);
        denum += features.magnitudes[i];
    }
    
    if(denum != 0.0f)
        result = num / denum;
    
    results.spectralDecrease = result;
}


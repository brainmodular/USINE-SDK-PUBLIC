#include "SpectralSpread.h"

void SpectralSpread::process(Features& features)
{
    float result = 0.0f;
    float tmp;
    for(int i = 0; i < features.fft_size - 1; i++)
    {
        tmp = (features.frequencies[i] - features.spectralCentroid);
        result += tmp * tmp * features.magnitudes[i];
    }
    
    // normalized magnitudes, so we divide by sum_magnitudes = 1
    result = std::sqrtf(result);
    
    if(result < 0.0f || std::isnan(result))
    result = 0.0f;
    else if(result > features.sample_rate/2.0f)
    result = features.sample_rate/2.0f;
    
    
    features.spectralSpread = result;
}



#include "SpectralCrest.h"

void SpectralCrest::process(Features& features, Results& results)
{
    float result = 0.0f;
    
    if(features.sum_magnitudes > 0.0f && features.fft_size > 0.0f && features.max_magnitude > 0.0f)
    {
        result = features.max_magnitude / (features.sum_magnitudes / float(features.fft_size));
    }
    
    results.spectralCrest = result;
}

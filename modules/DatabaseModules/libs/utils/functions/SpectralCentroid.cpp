#include "SpectralCentroid.h"

void SpectralCentroid::process(Features& features)
{
    float result = 0.0f;
     
    if(features.sum_amplitudes > 0.0f && features.sum_amplitudes_times_freqs > 0.0f)
    {
        result = features.sum_amplitudes_times_freqs/features.sum_amplitudes;
    } else {
        result = 0.0f;
    }
    

    if(result > features.sample_rate / 2.0f)
        result = features.sample_rate/2.0f;
    
    features.spectralCentroid = result;

}

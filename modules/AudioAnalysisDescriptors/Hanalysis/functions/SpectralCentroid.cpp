#include "SpectralCentroid.h"

void SpectralCentroid::process(Features& features, Results& results)
{
    float result = 0.0f;
     
    if(features.sum_magnitudes > 0.0f && features.sum_magnitudes_times_freqs > 0.0f)
    {
        result = features.sum_magnitudes_times_freqs/features.sum_magnitudes;
    } else {
        result = 0.0f;
    }
    

    if(result > features.sample_rate / 2.0f)
        result = features.sample_rate/2.0f;
    
    results.spectralCentroid = result;

}

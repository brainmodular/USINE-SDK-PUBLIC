#include "SpectralRolloff.h"


void SpectralRolloff::process(Features& features, Results& results)
{
    float acc = 0.0f;
    float result = 0.0f;

    for(int i = 0; i < features.fft_size; i++)
    {
        acc += features.magnitudes[i];
        if(acc >= features.rolloffThreshold)
        {
            result = (i * features.sample_rate) / (2.0f * (features.fft_size-1));
            break;
        }
    }
    
    results.spectralRolloff = result;
}

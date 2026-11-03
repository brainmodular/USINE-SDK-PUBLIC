#include "SpectralEntropy.h"

void SpectralEntropy::process(Features& features, Results& results)
{
    float result = 0.0f;
    
    for (int i = 0; i < features.fft_size; i++)
    {
        if (features.magnitudes[i] > 0.0f)
        {
            result -= features.magnitudes[i] * log2f(features.magnitudes[i]);
        }
    }
    
    if(result < 0.0f)
        result = 0.0f;
    else if(result > (int)logf(static_cast<float>(features.fft_size)))
        result = logf(static_cast<float>(features.fft_size));
    
    results.spectralEntropy = result;
}

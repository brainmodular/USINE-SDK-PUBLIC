#include "SpectralSkewness.h"


void SpectralSkewness::process(Features& features, Results& results)
{
    float result = 0.0f;
    if(results.spectralSpread != 0)
    {
        float thirdMoment = 0.0f;
        float tmp = 0.0f;
        
        for(int i = 0; i < features.fft_size; i++)
        {
            tmp = features.frequencies[i] - results.spectralCentroid;
            thirdMoment += tmp * tmp * tmp * features.magnitudes[i];
        }
        
        result = thirdMoment/(results.spectralSpread*results.spectralSpread*results.spectralSpread);
        
        if(std::isnan(result))
            result = 0.0f;
    }
    
    results.spectralSkewness = result;
}

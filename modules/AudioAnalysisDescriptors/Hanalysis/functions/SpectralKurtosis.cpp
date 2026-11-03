#include "SpectralKurtosis.h"



void SpectralKurtosis::process(Features& features, Results& results)
{
    float result = 0.0f;
    if(results.spectralSpread != 0)
    {
        float fourthMoment = 0.0f;
        float tmp = 0.0f;
        
        for(int i = 0; i < features.fft_size; i++)
        {
            tmp = features.frequencies[i] - results.spectralCentroid;
            tmp = tmp * tmp;
            tmp = tmp * tmp;
            fourthMoment += tmp * features.magnitudes[i];
        }
        
        tmp = results.spectralSpread*results.spectralSpread;
        tmp = tmp * tmp;

        result = fourthMoment/tmp;
        
        if(std::isnan(result))
            result = 0.0f;
    }
    
    results.spectralKurtosis = result;

}

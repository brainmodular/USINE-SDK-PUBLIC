#include "SpectralTonality.h"

void SpectralTonality::process(Features& features, Results& results)
{
    float result = 0.0f;
    float flatnessdB = 0.0f;
    
    if(results.spectralFlatness > 0.0f)
    {
        flatnessdB = 10.0f * log10f(results.spectralFlatness);
        result = std::min(flatnessdB/-60.0f,1.0f);
    }
    
    results.spectralTonality = result;
}

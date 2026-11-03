#include "Sharpness.h"

Sharpness::Sharpness(int num_bands) :
    m_iNumBands(num_bands)
{
}


void Sharpness::process(Results& results)
{

    float result = 0.0f;
    
    if(results.perceptLoudness.totalLoudness > 0.0f)
    {
        float num = 0.0f;
        float Gfunc = 0.0f;
        // Calculate sharpness
        for (int band = 0; band < m_iNumBands; band++) {
            
            Gfunc = (band < 15) ? 1.0f : 0.066f * std::expf(0.171f*band);
            
            num += band * Gfunc * results.perceptLoudness.specificLoudness[band];
        }
        result = 0.11f * num / results.perceptLoudness.totalLoudness;
    }
        

    results.perceptSharpness = result;
}

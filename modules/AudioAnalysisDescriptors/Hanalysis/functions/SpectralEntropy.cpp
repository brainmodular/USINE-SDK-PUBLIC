#include "SpectralEntropy.h"
#include <algorithm>
#include <cmath>

void SpectralEntropy::process(Features& features, Results& results)
{
    // Shannon entropy of the normalized power spectrum in nats (natural log), from 0 when the energy is in a
    // single bin to ln(number of bins) for a flat spectrum
    float result = 0.0f;
    
    for (int i = 0; i < features.fft_size; i++)
    {
        if (features.magnitudes[i] > 0.0f)
        {
            result -= features.magnitudes[i] * std::log(features.magnitudes[i]);
        }
    }
    
    const float maxEntropy = std::log(static_cast<float>(std::max(features.fft_size, 1)));
    results.spectralEntropy = std::min(std::max(result, 0.0f), maxEntropy);
}

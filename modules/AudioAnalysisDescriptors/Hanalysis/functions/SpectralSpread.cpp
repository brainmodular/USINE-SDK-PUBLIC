#include "SpectralSpread.h"

void SpectralSpread::process(Features& features, Results& results)
{
    float result = 0.0f;
    float tmp;
    // every bin from DC to Nyquist, as the centroid
    for (int i = 0; i < features.fft_size; i++)
    {
        tmp = (features.frequencies[i] - results.spectralCentroid);
        result += tmp * tmp * features.magnitudes[i];
    }

    // normalized magnitudes, so we divide by sum_magnitudes = 1
    result = sqrtf(result);

    if (result < 0.0f || std::isnan(result))
        result = 0.0f;
    else if (result > features.sample_rate/2.0f)
        result = features.sample_rate/2.0f;

    results.spectralSpread = result;
}
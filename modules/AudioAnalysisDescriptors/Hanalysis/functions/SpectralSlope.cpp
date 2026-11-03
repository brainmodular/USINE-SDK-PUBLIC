#include "SpectralSlope.h"

void SpectralSlope::process(Features& features, Results& results)
{
    float result = 0.0f;

    result = (features.fft_size * features.sum_magnitudes_times_freqs - features.sum_frequencies * features.sum_magnitudes) /
             (features.fft_size * features.sum_frequencies_squared    - features.sum_frequencies * features.sum_frequencies);

    results.spectralSlope = result;
}

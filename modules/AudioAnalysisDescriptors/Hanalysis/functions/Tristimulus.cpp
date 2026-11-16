#include "Tristimulus.h"
#include <algorithm>
#include <cmath>
#include <cstring>

namespace
{
    // harmonics taken into account, as in the Timbre Toolbox (Peeters et al. 2011)
    constexpr int TRISTIMULUS_MAX_HARMONICS = 20;
}

void Tristimulus::process(Features &features, Results& results)
{
    // tristimulus after Pollard and Jansson (1982): the share of the fundamental (T1), of harmonics 2 to 4 (T2)
    // and of harmonics 5 and above (T3) in the sum of the amplitudes of the harmonics 1 to 20 below Nyquist
    float sums[3] = {0.0f, 0.0f, 0.0f};
    float total = 0.0f;

    std::fill(m_vResult.begin(), m_vResult.end(), 0.0f);
    
    const float f0 = results.harmonicF0_YIN.frequency;
    if(f0 > 50.0f && f0 < 5000.0f)
    {
        // fundamental in bins, at least 2 bins between the harmonics
        const float f0Bin = f0 / features.sample_rate * features.buffer_size;
        if(f0Bin >= 2.0f)
        {
            // amplitude of a harmonic: highest bin around its position (the window spreads it over several
            // bins), searching 1 bin around when the harmonics are 3 bins apart or more
            const int halfWidth = (f0Bin >= 3.0f) ? 1 : 0;
            const int lastBin = features.fft_size - 1;
            for(int h = 1; h <= TRISTIMULUS_MAX_HARMONICS; h++)
            {
                const int bin = static_cast<int>(std::lround(h * f0Bin));
                if(bin + halfWidth > lastBin)
                    break;
                float amplitude = 0.0f;
                for(int i = bin - halfWidth; i <= bin + halfWidth; i++)
                {
                    amplitude = std::max(amplitude, features.amplitudes[i]);
                }
                sums[(h == 1) ? 0 : ((h <= 4) ? 1 : 2)] += amplitude;
                total += amplitude;
            }
        }
    }

    if(total > 0.0f)
    {
        for(int i = 0; i < 3; i++)
        {
            m_vResult[i] = sums[i] / total;
        }
    }

    memcpy(results.harmonicTristimulus.data(), m_vResult.data(), sizeof(float) * 3);

}


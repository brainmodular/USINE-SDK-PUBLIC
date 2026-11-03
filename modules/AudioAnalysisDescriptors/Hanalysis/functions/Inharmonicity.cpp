#include "Inharmonicity.h"

void Inharmonicity::process(Features &features, Results& results)
{
    float result = 0.0f;
    int fundFreqBin = 0;

    if (results.harmonicF0_YIN.frequency > 40.0f && results.harmonicF0_YIN.frequency < 5000.0f)
    {
        fundFreqBin = int(results.harmonicF0_YIN.frequency * features.buffer_size / features.sample_rate);
    }
    if (fundFreqBin > 1 && fundFreqBin < features.fft_size)
    {
        int peaks_size = int(results.spectralPeaks.size());

        float ratio = 0.0f;
        float num = 0.0f;
        float denum = 0.0f;

        for (int i = 0; i < peaks_size;i++)
        {
            ratio = std::round(results.spectralPeaks[i].frequency/results.harmonicF0_YIN.frequency);
            num += fabs(results.spectralPeaks[i].frequency - ratio * results.harmonicF0_YIN.frequency) * results.spectralPeaks[i].magnitude;
            denum += results.spectralPeaks[i].magnitude;
        }
        if (denum > 0.0f && num > 0.0f)
        {
            result = 2.0f * num / (results.harmonicF0_YIN.frequency * denum);
        }
    }
    results.harmonicInharmonicity = result;
}
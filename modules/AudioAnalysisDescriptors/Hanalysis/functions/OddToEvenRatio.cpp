#include "OddToEvenRatio.h"

void OddToEvenRatio::process(Features &features, Results& results)
{
    float oddSum = 0.0f;
    float evenSum = 0.0f;
    float result = 0.0f;
    float ratio = 0.0f;
    float step = 0.0f;
    int number = 0;
    int fundFreqBin = 0;

    if(results.harmonicF0_YIN.frequency > 50.0f && results.harmonicF0_YIN.frequency < 5000.0f)
    {
        fundFreqBin = static_cast<int>(results.harmonicF0_YIN.frequency / features.sample_rate * features.buffer_size);
    }
    
    if(fundFreqBin > 1 && fundFreqBin < int(features.frequencies.size()))
    {
        int peaks_size = int(results.spectralPeaks.size());
        for(int i = 0; i < peaks_size ;i++)
        {
            ratio = results.spectralPeaks[i].frequency/results.harmonicF0_YIN.frequency;
            
            step = (features.sample_rate / features.buffer_size) / results.harmonicF0_YIN.frequency;
            
            number = (int)ratio;
            
            
            //check if peak is an harmonic of fund freq
            if(std::fabs( ratio - float(number) ) <= step )
            {
                //check if ratio is even or odd
                if(number % 2 == 0 && number > 1)
                {
                    evenSum += results.spectralPeaks[i].magnitude;
                    
                }
                else if (number > 1)
                {
                    oddSum += results.spectralPeaks[i].magnitude;
                }
            }
        }
        if(evenSum > 0.0f || oddSum > 0.0f)
        {
            result = (oddSum - evenSum) / (evenSum + oddSum);    
        } 
    }

    results.harmonicOTERatio = result;

}

#include "Tristimulus.h"


void Tristimulus::process(Features &features, Results& results)
{
    float num1 = 0.0f;
    float num2 = 0.0f;
    float num3 = 0.0f;
    float sum = 0.0f;

    int fundFreqBin = 0;

    std::fill(m_vResult.begin(), m_vResult.end(), 0.0f);
    
    if(results.harmonicF0_YIN.frequency > 50.0f && results.harmonicF0_YIN.frequency < 5000.0f)
    {
        fundFreqBin = static_cast<int>(results.harmonicF0_YIN.frequency / features.sample_rate * features.buffer_size);
    }
    
    if(fundFreqBin > 1 && fundFreqBin < features.fft_size/2)
    {
        
        num1 = features.magnitudes[fundFreqBin*2];
        for(int i = 2; i<features.fft_size/fundFreqBin;i++)
        {
            sum += features.magnitudes[fundFreqBin * i];
            if(i > 2 && i < 6)
            {
                num2 += features.magnitudes[fundFreqBin * i];
            }
            if(i > 6)
            {
                num3 += features.magnitudes[fundFreqBin * i];
            }
        }
        if(sum > 0.0f)
        {
                // Tristimulus 1
            if(num1 > 0.0f)
                m_vResult[0] = num1/sum;
                // Tristimulus 2
            if(num2 > 0.0f)
                m_vResult[1] = num2/sum;
                // Tristimulus 3
            if(num3 > 0.0f)
                m_vResult[2] = num3/sum;
        } 
        

    }

    memcpy(results.harmonicTristimulus.data(), m_vResult.data(), sizeof(float) * 3);

}


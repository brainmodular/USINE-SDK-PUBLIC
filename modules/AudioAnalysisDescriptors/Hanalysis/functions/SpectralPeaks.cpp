#include "SpectralPeaks.h"


void SpectralPeaks::process(Features &features, Results& results)
{
    m_vPeaks.clear();
    m_pPeak.frequency = 0.0f;
    m_pPeak.magnitude = 0.0f;
    
    float leftMin = 0.0f;
    float rightMin = 0.0f;
    float localMin = 0.0f;
    float prominence = 0.0f;
    
    
    
    float currentMag = 0.0f;
    
    for (int i = SEARCH_WINDOW; i < features.fft_size - SEARCH_WINDOW; i++)
    {
        currentMag = features.magnitudes[i];
        
        // skip if not local maximum
        if (currentMag <= features.magnitudes[i - 1] || currentMag <= features.magnitudes[i + 1])
            continue;
        
        leftMin = *std::min_element(features.magnitudes.begin() + i - SEARCH_WINDOW, features.magnitudes.begin() + i);
        rightMin = *std::min_element(features.magnitudes.begin() + i + 1, features.magnitudes.begin() + i + 1 + SEARCH_WINDOW);
        
        localMin = std::max(leftMin, rightMin);
        
        prominence = currentMag - localMin;
        
        if (prominence >= PROM_THRESHOLD && currentMag > MAG_THRESHOLD)
        {
            // currentMag = detected peak
            // i = bin of detected peak
            m_pPeak.frequency = features.frequencies[i];
            m_pPeak.magnitude = currentMag;
            m_vPeaks.push_back(m_pPeak);
        }
    }
    
    int peakCount = int(m_vPeaks.size());
    results.spectralPeaks.resize(peakCount);
    memcpy(results.spectralPeaks.data(), m_vPeaks.data(), peakCount * sizeof(SpectralPeak));
    
}
 

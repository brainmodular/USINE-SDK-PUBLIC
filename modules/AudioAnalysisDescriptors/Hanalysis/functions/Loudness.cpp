#include "Loudness.h"


Loudness::Loudness(int num_bands, int buffer_size, float sample_rate) :
    m_iNumBands(num_bands)
{
    init(buffer_size,sample_rate);
}

void Loudness::init(int buffer_size, float sample_rate)
{
    // Initialize the filter bank
    m_vBarkEdges = {0, 100, 200, 300, 400, 510, 630, 770, 920, 1080, 1270, 1480, 1720, 2000, 2320, 2700, 3150, 3700, 4400, 5300, 6400, 7700, 9500, 12000, 15500, 20500, 27000};
    // edges in bins
    if(m_iNumBands + 1 <= m_vBarkEdges.size())
    {
        m_vBarkLimits.resize(m_iNumBands + 1, 0);
    }
    else 
    {
        // too much bands, we fix to 25
        m_iNumBands = 25;
        m_vBarkLimits.resize(26, 0);
    }
    for(int i = 0; i < m_iNumBands + 1 ;i++)
    {
        m_vBarkLimits[i] = static_cast<int>(m_vBarkEdges[i] / sample_rate * buffer_size);
    }

    // resize vectors 
    m_vBarkCoefficients.resize(m_iNumBands);
    m_vSpecificLoudness.resize(m_iNumBands);
    m_vSpecificResult.resize(m_iNumBands);
}

void Loudness::setFftSize(int new_size, float new_sample_rate)
{
    reset();
    std::fill(m_vBarkLimits.begin(), m_vBarkLimits.end(), 0);
    init(new_size, new_sample_rate);
}


void Loudness::computeCoefficients(Features& features)
{
    for(int band = 0; band < m_iNumBands; band++)
    {
        for(int i = m_vBarkLimits[band]; i < m_vBarkLimits[band+1]; i++)
        {
            m_vBarkCoefficients[band] += features.magnitudes[i];
        }
    }
}

std::vector<float> Loudness::computeSpecificLoudness()
{
    for(int band = 0; band < m_iNumBands; band++)
    {
        m_vSpecificLoudness[band] = std::powf(m_vBarkCoefficients[band], 0.23f);
    }
    return m_vSpecificLoudness;
}

void Loudness::reset()
{
    std::fill(m_vSpecificLoudness.begin(), m_vSpecificLoudness.end(), 0.0f);
    std::fill(m_vBarkCoefficients.begin(), m_vBarkCoefficients.end(), 0.0f);
}

void Loudness::process(Features& features, Results& results)
{
    reset();
    float totalResult = 0.0f;
    // compute coefficients
    computeCoefficients(features);
    // compute specific loudness
    m_vSpecificResult = computeSpecificLoudness();
    // compute total loudness
    for(int i = 0; i < m_iNumBands; i++)
    {
        totalResult += m_vSpecificLoudness[i];
    }

    totalResult /= 10.0f;

    results.perceptLoudness.totalLoudness = totalResult;
    results.perceptLoudness.specificLoudness = m_vSpecificResult;
}

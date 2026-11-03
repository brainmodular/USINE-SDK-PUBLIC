#include "MFCC.h"


MFCC::MFCC(int buffer_size, float sample_rate) :
    m_iNumMelFilters(NUM_MEL_FILTERS),
    m_vMelFilterBank(NUM_MEL_FILTERS, std::vector<float>(buffer_size)),
    m_vMfccEnergy(NUM_MEL_FILTERS,0.0f),
    m_vMfcc(NUM_COEFF_MFCC,0.0f),
    m_vLiftering(NUM_COEFF_MFCC-1,0.0f),
    m_vMelPoints(NUM_MEL_FILTERS + 2, 0.0f),
    m_vResult(NUM_COEFF_MFCC,0.0f),
    m_vV1(NUM_COEFF_MFCC+1,0.0f),
    m_vV2(NUM_MEL_FILTERS,0.0f),
    m_vTemp(NUM_MEL_FILTERS,0.0f),
    m_fC(std::sqrtf(2.0f/NUM_MEL_FILTERS))
{
    initializeMelFilterBank(buffer_size,sample_rate);
    initializeDct();
   
    
    for(int i = 0; i < NUM_COEFF_MFCC - 1; i++)
    {
        m_vLiftering[i] = (1.0f + (LIFT_COEFF_S2)*std::sinf(PI_S_LIFT_COEFF * float(i)));
    }

}

MFCC::~MFCC() 
{
    
    
}


void MFCC::setFftSize(int new_size, float new_sample_rate)
{
    m_vMelFilterBank.clear();
    m_vMelFilterBank.resize(NUM_MEL_FILTERS);
    std::fill(m_vMelFilterBank.begin(), m_vMelFilterBank.end(), std::vector<float>(new_size,0.0f));
    
    m_fMin = 0.0f;
    m_fMax = 0.0f;
    m_fMelMin = 0.0f;
    m_fMelMax = 0.0f;
    m_fMelStep = 0.0f;
    
    initializeMelFilterBank(new_size, new_sample_rate);
    
}

void MFCC::process(Features& features, Results& results)
{
    reset();
    // compute MFCC energy and take the log of it
    computeMfccEnergy(features);
    // apply dct
        
    applyDct();
    
    // apply liftering
    applyLiftering(m_vMfcc);
    
    int resultSize = int(m_vResult.size());
    
    memcpy(results.harmonicMfcc.data(), m_vResult.data(), resultSize * sizeof(float));
}

float MFCC::Hz2Mel(const float& hz)
{
    return 2595.0f * std::log10f(1.0f + (hz / 700.0f));
}

float MFCC::Mel2Hz(const float& mel)
{
    return 700.0f * (std::powf(10.0f,mel/2595.0f) - 1.0f);
}

void MFCC::reset()
{
    std::fill(m_vResult.begin(), m_vResult.end(), 0.0f);
    std::fill(m_vMfccEnergy.begin(), m_vMfccEnergy.end(), 0.0f);
    std::fill(m_vMfcc.begin(),m_vMfcc.end(),0.0f);
}

void MFCC::initializeDct()
{
    // create DCT II matrix, can be better
    for (int i=0; i < NUM_COEFF_MFCC; i++)
    {
        m_vV1[i] = static_cast<float>(i);
    }
    for (int i=0; i < m_iNumMelFilters; i++)
    {
        m_vV2[i] = static_cast<float>(i) + 0.5f;
    }
    
    for (int i = 0; i < NUM_COEFF_MFCC; i++) {
        for (int j=0; j<m_iNumMelFilters; j++)
        {
            //Check if ok ?
            m_vTemp[j] = (m_fC * std::cosf(M_PI / (float) m_iNumMelFilters * m_vV1[i] * m_vV2[j]));
        }
        m_vDctMatrix.push_back(m_vTemp);
    }
    
}

void MFCC::initializeMelFilterBank(int buffer_size, float sample_rate)
{
    m_fMin = 0.0f;
    m_fMax = sample_rate / 2.0f;
    m_fMelMin = Hz2Mel(m_fMin);
    m_fMelMax = Hz2Mel(m_fMax);
    m_fMelStep = (m_fMelMax - m_fMelMin) / float(m_iNumMelFilters + 1);
    m_fMelBandWidth = 0.0f;
    int fft_size = buffer_size / 2 + 1;

    for(int i = 0; i < int(m_vMelPoints.size()); i++)
    {
        m_vMelPoints[i] = std::floor(Mel2Hz(m_fMelMin + i * m_fMelStep) * fft_size / sample_rate);
    }

    for (int j = 0; j < m_iNumMelFilters; j++)
    {
            // for bandwidth normalization
        m_fMelBandWidth = m_vMelPoints[j + 1] - m_vMelPoints[j];
        
            // Create first half of triangle
        for (int i= int(m_vMelPoints[j]); i<int(m_vMelPoints[j+1]); i++)
        {
            if(m_vMelPoints[j+1]-m_vMelPoints[j] == 0 || m_fMelBandWidth == 0)
            {
                m_vMelFilterBank[j][i] = 0.0f;
            } else {
                m_vMelFilterBank[j][i] = (i - m_vMelPoints[j]) / (m_vMelPoints[j+1]-m_vMelPoints[j]);
            }
        }
            // Create second half of triangle
        for (int i=int(m_vMelPoints[j+1]); i<int(m_vMelPoints[j+2]); i++)
        {
            if(m_vMelPoints[j+2]-m_vMelPoints[j+1] == 0 || m_fMelBandWidth == 0 )
            {
                m_vMelFilterBank[j][i] = 0.0f;
            } else {
                m_vMelFilterBank[j][i] = (m_vMelPoints[j+2]-i) / (m_vMelPoints[j+2]-m_vMelPoints[j+1]);
            }
            
        }
        float norm = 0.0f;
        for (int i = 0; i < fft_size; ++i) {
            norm += m_vMelFilterBank[j][i];
        }
        if (norm > 0.0f) {
            for (int i = 0; i < fft_size; ++i) {
                m_vMelFilterBank[j][i] /= norm;
            }
        }
    }

}

std::vector<float> MFCC::computeMfccEnergy(Features& features)
{
    m_fLogEnergy = std::logf(features.sum_magnitudes/features.fft_size);
    
    for(int filterCount = 0; filterCount < m_vMelFilterBank.size(); filterCount++)
    {
        for(int j = 0; j < features.fft_size; j++)
        {
                // energy by mel filter band
            m_vMfccEnergy[filterCount] += m_vMelFilterBank[filterCount][j] * features.amplitudes[j];
        }
            
        // Mel-flooring
        if(m_vMfccEnergy[filterCount] < 1e-10f)
        {
            m_vMfccEnergy[filterCount] = 1e-10f;
        }
        // apply log to the sum of energy to simulate human perception
        m_vMfccEnergy[filterCount] = std::log10f((m_vMfccEnergy[filterCount]));
    }
    
     return m_vMfccEnergy;
}

void MFCC::applyDct()
{
    for (int i=0; i < NUM_COEFF_MFCC; i++) {
        for (int j=0; j< m_iNumMelFilters; j++)
            m_vMfcc[i] += m_vDctMatrix[i][j] * m_vMfccEnergy[j];
    }
}

void MFCC::applyLiftering(std::vector<float> &mfcc)
{
    // liftering to reduce the effect of high frequencies and noise
    m_vResult[0] = mfcc[0];
    for(int i = 1; i < NUM_COEFF_MFCC; i++)
    {
        m_vResult[i] = mfcc[i] * m_vLiftering[i-1];
    }
}



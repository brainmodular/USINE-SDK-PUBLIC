#pragma once

#ifdef  _WIN32
#include "../Hfeatures.h"
#endif // Windows

#ifdef __APPLE__
#include "Hfeatures.h"
#endif // MacOs

#include <iostream>
#include <cmath>
#include <vector>
#include <stdlib.h>


constexpr TPrecision LIFT_COEFF      = 0.6f;
constexpr TPrecision LIFT_COEFF_S2   = 0.3f;
constexpr TPrecision PI_S_LIFT_COEFF = M_PI / LIFT_COEFF;

constexpr int NUM_MEL_FILTERS = 30;
constexpr int NUM_COEFF_MFCC  = 13;

class MFCC
{
public:

    MFCC(int fft_size, float sample_rate);

    ~MFCC();

    void process(Features& features, Results& results);

    void setFftSize(int new_size, float new_sample_rate);

private:
    //variables
    int m_iNumMelFilters = 0;

    //dct plan

    void applyDct();
    std::vector<std::vector<float>> m_vDctMatrix;
    std::vector<float> m_vV1;
    std::vector<float> m_vV2;
    std::vector<float> m_vTemp;
    float m_fC;

    //mel filter bank
    std::vector<std::vector<float>> m_vMelFilterBank;
    std::vector<float> m_vMelPoints;
    float m_fMin = 0.0f;
    float m_fMax = 0.0f;
    float m_fMelMin = 0.0f;
    float m_fMelMax = 0.0f;
    float m_fMelStep = 0.0f;
    float m_fMelBandWidth = 0.0f;

    std::vector<float> m_vMfccEnergy;
    float m_fLogEnergy = 0.0f;
    std::vector<float> m_vMfcc;
    std::vector<float> m_vLiftering;
    std::vector<float> m_vResult;

    //methods 
    float Hz2Mel(const float& hz);
    float Mel2Hz(const float& mel);
    void reset();

    void initializeDct();
    void initializeMelFilterBank(int buffer_size, float sample_rate);
    std::vector<float> computeMfccEnergy(Features& features);
    void applyLiftering(std::vector<float>& mfcc_energy);
};
#ifndef __F0_YIN_H__
#define __F0_YIN_H__

/*
 This implementation comes from this repo by Sevagh : https://github.com/sevagh/pitch-detection
 
 Original YIN Algorithm paper :
 De Cheveigne,A. Kawahara,I. (2002) YIN, a fundamental frequency estimator for speech and music : http://audition.ens.fr/adc/pdf/2002_JASA_YIN.pdf
 
 Original pYIN Algorithm paper :
 Mauch, M., & Dixon, S. (2014). pYIN: A fundamental frequency estimator using probabilistic threshold distributions.
 Proceedings of the IEEE International Conference on Acoustics, Speech and Signal Processing (ICASSP). https://www.eecs.qmul.ac.uk/~simond/pub/2014/MauchDixon-PYIN-ICASSP2014.pdf
 */

#include "../Hfeatures.h"

#ifdef __APPLE__
#include "Hfeatures.h"
#endif // MacOs

#include <vector>
#include <tuple>
#include <iostream>
#include <map>
#include "UserDefinitions.h"

constexpr TPrecision YIN_THRESHOLD      = 0.5f;
constexpr TPrecision PYIN_PA            = 0.01f;
constexpr int        PYIN_N_THRESHOLDS  = 100;
constexpr TPrecision PYIN_MIN_THRESHOLD = 0.025f;
constexpr TPrecision PROBA_THRESHOLD    = 0.0f;

// 570.29 // 243.66

class F0_YIN
{
public:
    F0_YIN(int buffer_size, float sample_rate);
    ~F0_YIN();

    void process(Features& features, Results& results);
    std::vector<float> autocorrelation();
    void setFFTSize(int new_size);
    void setSampleRate(int fft_size, float sample_rate);
    
private:
    // variables
    // cumulative mean normalized difference, for tau from 0 to buffer_size / 2 - 1
    std::vector<float> m_vYinBuffer;
    // the centered frame
    std::vector<float> m_vTempBuffer;

    std::vector<PitchProbability> m_vPitchProbability;
    
    //DEBUG
    std::vector<float> m_vAutoCorr;
    //DEBUG

    float m_fRe = 0.0f;
    float m_fIm = 0.0f;
    float m_fMean = 0.0f;
    

    TUsineFFTPtr m_FftPtr = nullptr;
    // spectrum of the first half of the frame zero padded, then the cross spectrum
    std::vector<TPrecision> m_Re;
    std::vector<TPrecision> m_Im;
    // spectrum of the whole frame
    std::vector<TPrecision> m_Re2;
    std::vector<TPrecision> m_Im2;
    // first half of the frame zero padded
    std::vector<TPrecision> m_vPadded;
    // cross correlation of the first half of the frame with the frame
    std::vector<TPrecision> m_vCorr;
    // size of m_FftPtr and of the buffers
    int m_iSize = 0;

    
    // methods
    void difference(const float* audioInput, const int buffer_size);
    void cumulative_mean_normalized_difference(std::vector<float>& yinBuffer);
    int absolute_threshold(std::vector<float>& yinBuffer, const int buffer_size);
    std::vector<PitchProbability> probabilistic_threshold(const std::vector<float>& yinBuffer, int sample_rate);
    
    std::pair<float,float> parabolic_interpolation(const std::vector<float>& yinBuffer, int x);

    void reset();

};




#endif // __F0_YIN_H__

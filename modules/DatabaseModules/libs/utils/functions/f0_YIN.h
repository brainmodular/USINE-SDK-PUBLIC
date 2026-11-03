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

#ifdef  _WIN32
#include "Hfeatures.h"
//#include "../../../sdk/UserDefinitions.h"
#include "UserDefinitions.h"
#endif // Windows

#ifdef __APPLE__
#include "Hfeatures.h"
#include "UserDefinitions.h"
#endif // MacOs

#include <vector>
#include <tuple>
#include <iostream>
#include <map>


#define YIN_THRESHOLD 0.5f
#define PYIN_PA 0.01f
#define PYIN_N_THRESHOLDS 100
#define PYIN_MIN_THRESHOLD 0.025f
#define PROBA_THRESHOLD 0.7f



static const float Beta_Distribution_0[100] = {0.000000f, 0.029069f, 0.048836f,
    0.061422f, 0.068542f, 0.071571f, 0.071607f, 0.069516f, 0.065976f, 0.061512f,
    0.056523f, 0.051309f, 0.046089f, 0.041021f, 0.036211f, 0.031727f, 0.027608f,
    0.023871f, 0.020517f, 0.017534f, 0.014903f, 0.012601f, 0.010601f, 0.008875f,
    0.007393f, 0.006130f, 0.005059f, 0.004155f, 0.003397f, 0.002765f, 0.002239f,
    0.001806f, 0.001449f, 0.001157f, 0.000920f, 0.000727f, 0.000572f, 0.000448f,
    0.000349f, 0.000271f, 0.000209f, 0.000160f, 0.000122f, 0.000092f, 0.000070f,
    0.000052f, 0.000039f, 0.000029f, 0.000021f, 0.000015f, 0.000011f, 0.000008f,
    0.000006f, 0.000004f, 0.000003f, 0.000002f, 0.000001f, 0.000001f, 0.000001f,
    0.000000f, 0.000000f, 0.000000f, 0.000000f, 0.000000f, 0.000000f, 0.000000f,
    0.000000f, 0.000000f, 0.000000f, 0.000000f, 0.000000f, 0.000000f, 0.000000f,
    0.000000f, 0.000000f, 0.000000f, 0.000000f, 0.000000f, 0.000000f, 0.000000f,
    0.000000f, 0.000000f, 0.000000f, 0.000000f, 0.000000f, 0.000000f, 0.000000f,
    0.000000f, 0.000000f, 0.000000f, 0.000000f, 0.000000f, 0.000000f, 0.000000f,
    0.000000f, 0.000000f, 0.000000f, 0.000000f, 0.000000f, 0.000000f};

// 570.29 // 243.66

class F0_YIN
{
public:
    F0_YIN(int buffer_size, float sample_rate);
    ~F0_YIN();

    void process(Features& features);
    std::vector<float> autocorrelation();
    void setFFTSize(int new_size);
    void setSampleRate(int fft_size, float sample_rate);
    
private:
    // variables
    std::vector<float> m_vYinBuffer;
    std::vector<float> m_vTempBuffer;

    std::vector<PitchProbability> m_vPitchProbability;
    
    //DEBUG
    std::vector<float> m_vAutoCorr;
    //DEBUG

    float m_fRe = 0.0f;
    float m_fIm = 0.0f;
    float m_fMean = 0.0f;
    

    TUsineFFTPtr m_FftPtr = nullptr;
    std::vector<TPrecision> m_Re;
    std::vector<TPrecision> m_Im;

    
    // methods
    void autocorr(const std::vector<float>& audioInput, const int buffer_size);
    void difference(std::vector<float>& yinBuffer, std::vector<float>& tempBuffer, const int buffer_size);
    void cumulative_mean_normalized_difference(std::vector<float>& yinBuffer);
    int absolute_threshold(std::vector<float>& yinBuffer, const int buffer_size);
    std::vector<PitchProbability> probabilistic_threshold(const std::vector<float>& yinBuffer, int sample_rate);
    
    std::pair<float,float> parabolic_interpolation(const std::vector<float>& yinBuffer, int x);

    void reset();

};




#endif // __F0_YIN_H__

#include "f0_YIN.h"

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

F0_YIN::F0_YIN(int buffer_size, float sample_rate) : 
    m_vYinBuffer(buffer_size, 0.0f), 
    m_vTempBuffer(buffer_size, 0.0f),

    m_Re(buffer_size,0.0f),
    m_Im(buffer_size,0.0f),


    m_vAutoCorr(buffer_size,0.0f)
{
    
    m_FftPtr = sdkFFTCreate(buffer_size);

    
}

F0_YIN::~F0_YIN()
{

    if(m_FftPtr != nullptr)
    {
        sdkFFTDestroy(m_FftPtr);
    }

}

std::vector<float> F0_YIN::autocorrelation()
{
    return m_vAutoCorr;
}


void F0_YIN::process(Features& features, Results& results)
{
    float estimatedFreq = 0.0f;
    
    float highestProba = 0.0f;
    
    // Reset the buffers
    reset();
    // Compute the autocorrelation of audioBuffer via FFT-IFFT and put it in the m_vTempBuffer
    autocorr(features.windowedAudioBuffer,features.buffer_size);

    // Compute the difference
    difference(m_vYinBuffer, m_vTempBuffer, features.buffer_size);
    // Compute the cumulative mean normalized difference
    cumulative_mean_normalized_difference(m_vYinBuffer);
    
    auto freq = probabilistic_threshold(m_vYinBuffer, static_cast<int>(features.sample_rate));
    PitchProbability pitchProba;
    for(const auto pitch_candidates : freq)
    {
        if(pitch_candidates.probability > PROBA_THRESHOLD && pitch_candidates.probability > highestProba)
        {
            highestProba = pitch_candidates.probability;
            estimatedFreq = pitch_candidates.frequency;
        }
    }
    pitchProba.frequency = estimatedFreq;
    pitchProba.probability = highestProba;
    
    results.harmonicF0_YIN = pitchProba;
}


void F0_YIN::autocorr(const std::vector<float>& audioInput, const int buffer_size)
{
    
        // We copy the audioBuffer into our temp buffer
    std::memcpy(m_vTempBuffer.data(), audioInput.data(), sizeof(float) * audioInput.size());
    
        // Compute mean of buffer for centering
    for(int i = 0; i < buffer_size; i++)
    {
        m_fMean += m_vTempBuffer[i];
    }
    m_fMean /= buffer_size;
    
        // Compute FFT of signal
    for(int i = 0; i < buffer_size; i++)
    {
        m_Re[i] = m_vTempBuffer[i] - m_fMean;
        m_Im[i] = 0.0f;
    }
    
    sdkFFTForward(m_FftPtr, m_Re.data(), m_Re.data(), m_Im.data());
    
        // Compute Power spectrum on output of FFT
    int nb_bins = buffer_size / 2 ;
    for(int i = 0; i < nb_bins; i++)
    {
        m_fRe = m_Re[i];
        m_fIm = m_Im[i];
        m_Re[i] = (m_fRe * m_fRe + m_fIm * m_fIm);
        m_Im[i] = 0.0f;
    }
        // Zero the rest of m_pIn (important for inverse FFT if using full-size plan)
    for (int i = nb_bins; i < buffer_size; ++i) {
        m_Re[i] = 0.0f;
        m_Im[i] = 0.0f;
    }
        // Execute IFFT
    sdkFFTInverse(m_FftPtr, m_vTempBuffer.data(), m_Re.data(), m_Im.data());
}


void F0_YIN::difference(std::vector<float> &yinBuffer, std::vector<float> &tempBuffer, const int buffer_size)
{
    float tempBufferZero = 2 * tempBuffer[0];
    for (int tau = 0; tau < buffer_size; tau++)
    {
        yinBuffer[tau] = tempBufferZero - 2 * tempBuffer[tau]; 
    }
}

void F0_YIN::cumulative_mean_normalized_difference(std::vector<float> &yinBuffer) 
{
    yinBuffer[0] = 1;
    float runningSum = 0.0f;
    
    for (int tau = 1; tau < signed(yinBuffer.size()); tau++) {
        runningSum += yinBuffer[tau];
        yinBuffer[tau] *= tau / runningSum;
    }
    m_vAutoCorr = yinBuffer;
}

int F0_YIN::absolute_threshold(std::vector<float> &yinBuffer, const int buffer_size)
{
    int tau = 0;
    for (tau = 2; tau < buffer_size; tau++) {
        if (yinBuffer[tau] < YIN_THRESHOLD) {
            while (tau + 1 < buffer_size && yinBuffer[tau + 1] < yinBuffer[tau]) {
                tau++;
            }
            break;
        }
    }
    
    return (tau == buffer_size || yinBuffer[tau] >= YIN_THRESHOLD) ? -1 : tau;
}


std::vector<PitchProbability> F0_YIN::probabilistic_threshold(const std::vector<float> &yin_buffer, int sample_rate)
{
    size_t size = yin_buffer.size();
    int tau;
    PitchProbability pitchProbability;
    
    std::map<int, float> t0_with_probability;
    std::vector<PitchProbability> f0_with_probability;
    
    float threshold = PYIN_MIN_THRESHOLD;
    
    for (int n = 0; n < PYIN_N_THRESHOLDS; ++n) {
        threshold = (n + 1) * PYIN_MIN_THRESHOLD;
        for (tau = 2; tau < size - 1; tau++) {
            if (yin_buffer[tau] < threshold) {
                while (
                       tau + 1 < size && yin_buffer[tau + 1] < yin_buffer[tau]) {
                           tau++;
                       }
                break;
            }
        }
        auto a = yin_buffer[tau] < threshold ? 1 : PYIN_PA;
        t0_with_probability[tau] += a * Beta_Distribution_0[n];
    }
    
    for (auto tau_estimate : t0_with_probability) {
        auto f0 =
        (tau_estimate.first != 0)
        ? sample_rate / std::get<0>(parabolic_interpolation(yin_buffer, tau_estimate.first))
        : -1.0f;
        
        if (f0 != -1.0) {
            pitchProbability.frequency = f0;
            pitchProbability.probability = tau_estimate.second;
            f0_with_probability.push_back(pitchProbability);
        }
    }
    
    return f0_with_probability;
}



std::pair<float, float>
F0_YIN::parabolic_interpolation(const std::vector<float> &yinBuffer,
                              int x) // the input is a float, output of FFTS
{
    int xAdjusted = 0;
    float den = 0.0f;
    float delta = 0.0f;
    
    if (x < 1) {
        xAdjusted = (yinBuffer[x] <= yinBuffer[x + 1]) ? x : x + 1;
    } else if (x > signed(yinBuffer.size()) - 2) {
        xAdjusted = (yinBuffer[x] <= yinBuffer[x - 1]) ? x : x - 1;
    } else {
        den = yinBuffer[x + 1] + yinBuffer[x - 1] - 2 * yinBuffer[x];
        delta = yinBuffer[x - 1] - yinBuffer[x + 1];
        return (!den)
        ? std::make_pair(static_cast<float>(x), static_cast<float>(yinBuffer[x]))
        : std::make_pair(static_cast<float>(x) + delta / (2 * den),
                         yinBuffer[x] - delta * delta / (8 * den));
    }
    return std::make_pair(static_cast<float>(xAdjusted), yinBuffer[xAdjusted]);
}

void F0_YIN::reset()
{
    // reset all member variables
    m_fRe = 0.0f;
    m_fIm = 0.0f;
    m_fMean = 0.0f;
    std::fill(m_vTempBuffer.begin(), m_vTempBuffer.end(), 0.0f);
    std::fill(m_vYinBuffer.begin(),m_vYinBuffer.end(),0.0f);
    

    std::fill(m_Re.begin(), m_Re.end(), 0.0f);
    std::fill(m_Im.begin(), m_Im.end(), 0.0f);


}

void F0_YIN::setFFTSize(int new_size)
{

    sdkFFTDestroy(m_FftPtr);
    m_Re.clear();
    m_Im.clear();

    
    m_vYinBuffer.clear();
    m_vTempBuffer.clear();
    m_vAutoCorr.clear();
    
    m_vYinBuffer.resize(new_size);
    m_vTempBuffer.resize(new_size);
    m_vAutoCorr.resize(new_size);
    

    m_FftPtr = sdkFFTCreate(new_size);
    m_Re.resize(new_size);
    m_Im.resize(new_size);

}

void F0_YIN::setSampleRate(int fft_size, float sample_rate)
{
    m_Re.clear();
    m_Im.clear();
    
    m_vYinBuffer.clear();
    m_vTempBuffer.clear();
    m_vAutoCorr.clear();
    
    m_vYinBuffer.resize(fft_size);
    m_vTempBuffer.resize(fft_size);
    m_vAutoCorr.resize(fft_size);
    
    m_Re.resize(fft_size);
    m_Im.resize(fft_size);
    
}

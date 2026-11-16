#include "f0_YIN.h"
#include <algorithm>
#include <iterator>

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
    m_vYinBuffer(buffer_size / 2, 0.0f), 
    m_vTempBuffer(buffer_size, 0.0f),
    m_vAutoCorr(buffer_size / 2,0.0f),
    m_Re(buffer_size,0.0f),
    m_Im(buffer_size,0.0f),
    m_Re2(buffer_size,0.0f),
    m_Im2(buffer_size,0.0f),
    m_vPadded(buffer_size,0.0f),
    m_vCorr(buffer_size,0.0f),
    m_iSize(buffer_size)
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
    // the buffers and the fft must have the size of the frame
    if (features.ptrAudio == nullptr)
    {
        results.harmonicF0_YIN = PitchProbability();
        return;
    }
    if (features.buffer_size != m_iSize)
    {
        setFFTSize(features.buffer_size);
    }
    // Compute the difference function on the frame, not windowed: a window changes the period of the signal
    difference(features.ptrAudio, features.buffer_size);
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


// difference function of YIN (de Cheveigne and Kawahara 2002, equation 7) with an integration window of half
// the frame: d(tau) = sum over j < W of (x[j] - x[j + tau])^2 = e(0) + e(tau) - 2 c(tau), for tau < W = buffer_size / 2,
// e(tau) being the energy of x[tau .. tau + W) and c(tau) the correlation of x[0 .. W) with x[tau .. tau + W).
// c is computed by fft: c(tau) = ifft(conj(fft(x[0 .. W) zero padded)) * fft(x))(tau) / buffer_size, without circular
// wrap since tau + j < buffer_size. A periodic signal gives d(T) = 0 at its period T
void F0_YIN::difference(const float* audioInput, const int buffer_size)
{
    const int halfSize = buffer_size / 2;

    // centered frame: a constant offset doesn't change d, removing it keeps the precision of the correlation
    double mean = 0.0;
    for(int i = 0; i < buffer_size; i++)
    {
        mean += audioInput[i];
    }
    mean /= buffer_size;
    m_fMean = static_cast<float>(mean);
    for(int i = 0; i < buffer_size; i++)
    {
        m_vTempBuffer[i] = audioInput[i] - m_fMean;
    }
    std::copy_n(m_vTempBuffer.begin(), halfSize, m_vPadded.begin());
    std::fill(m_vPadded.begin() + halfSize, m_vPadded.end(), 0.0f);

    sdkFFTForward(m_FftPtr, m_vPadded.data(), m_Re.data(), m_Im.data());
    sdkFFTForward(m_FftPtr, m_vTempBuffer.data(), m_Re2.data(), m_Im2.data());

    // cross spectrum conj(A) * B, bins 0 to buffer_size / 2 (the imaginary parts of DC and Nyquist are 0)
    for(int i = 0; i <= halfSize; i++)
    {
        m_fRe = m_Re[i] * m_Re2[i] + m_Im[i] * m_Im2[i];
        m_fIm = m_Re[i] * m_Im2[i] - m_Im[i] * m_Re2[i];
        m_Re[i] = m_fRe;
        m_Im[i] = m_fIm;
    }
    m_Im[0] = 0.0f;
    m_Im[halfSize] = 0.0f;
    // the inverse fft of the host is not scaled: buffer_size * c(tau)
    sdkFFTInverse(m_FftPtr, m_vCorr.data(), m_Re.data(), m_Im.data());

    // energies by running sums, in double
    double energyZero = 0.0;
    for(int j = 0; j < halfSize; j++)
    {
        energyZero += double(m_vTempBuffer[j]) * m_vTempBuffer[j];
    }
    double energyTau = energyZero;
    for(int tau = 0; tau < halfSize; tau++)
    {
        if(tau > 0)
        {
            energyTau += double(m_vTempBuffer[tau + halfSize - 1]) * m_vTempBuffer[tau + halfSize - 1]
                       - double(m_vTempBuffer[tau - 1]) * m_vTempBuffer[tau - 1];
        }
        const double correlation = double(m_vCorr[tau]) / buffer_size;
        // d >= 0, the rounding errors of the fft could make it slightly negative
        m_vYinBuffer[tau] = static_cast<float>(std::max(0.0, energyZero + energyTau - 2.0 * correlation));
    }
}

void F0_YIN::cumulative_mean_normalized_difference(std::vector<float> &yinBuffer) 
{
    yinBuffer[0] = 1;
    float runningSum = 0.0f;
    
    for (int tau = 1; tau < signed(yinBuffer.size()); tau++) {
        runningSum += yinBuffer[tau];
        // a silence gives a null difference: no period rather than 0/0
        yinBuffer[tau] = (runningSum > 0.0f) ? yinBuffer[tau] * tau / runningSum : 1.0f;
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
    const int size = static_cast<int>(yin_buffer.size());
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
        // no dip below the threshold, or a dip going down to the end of the search range (the period is longer):
        // the global minimum, with the low probability PYIN_PA (pYIN), rather than the end of the search range
        // which would give a fixed frequency
        const bool dipFound = (tau < size - 1) && (yin_buffer[tau] < threshold);
        if (!dipFound && size > 3) {
            tau = static_cast<int>(std::distance(yin_buffer.begin(), std::min_element(yin_buffer.begin() + 2, yin_buffer.end() - 1)));
        }
        auto a = dipFound ? 1 : PYIN_PA;
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
    std::fill(m_Re2.begin(), m_Re2.end(), 0.0f);
    std::fill(m_Im2.begin(), m_Im2.end(), 0.0f);


}

void F0_YIN::setFFTSize(int new_size)
{

    sdkFFTDestroy(m_FftPtr);
    m_Re.clear();
    m_Im.clear();

    
    m_vYinBuffer.clear();
    m_vTempBuffer.clear();
    m_vAutoCorr.clear();
    
    m_vYinBuffer.resize(new_size / 2);
    m_vTempBuffer.resize(new_size);
    m_vAutoCorr.resize(new_size / 2);
    

    m_FftPtr = sdkFFTCreate(new_size);
    m_Re.resize(new_size);
    m_Im.resize(new_size);
    m_Re2.assign(new_size, 0.0f);
    m_Im2.assign(new_size, 0.0f);
    m_vPadded.assign(new_size, 0.0f);
    m_vCorr.assign(new_size, 0.0f);
    m_iSize = new_size;

}

void F0_YIN::setSampleRate(int fft_size, float sample_rate)
{
    // the sample rate is read in the features at each analysis: only a new size matters, and the fft
    // must always have the size of the buffers (the host fft reads and writes fft size values)
    if (fft_size != m_iSize)
    {
        setFFTSize(fft_size);
    }
}

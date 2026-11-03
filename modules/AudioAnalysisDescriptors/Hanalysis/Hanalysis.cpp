#include "Hanalysis.h"
#include "UserModule.h"
#include "UsineFunctions.h"

Hanalysis::Hanalysis(int buffer_size, int fft_size, float sample_rate, AudioAnalysis* modulePtr) :
m_BufferSize(buffer_size),
m_FftSize(fft_size),
m_SampleRate(sample_rate),
m_HanningWindow(buffer_size,0.0f),
features(buffer_size,fft_size,sample_rate),
descriptors(buffer_size,fft_size,sample_rate,NUM_BARK_BANDS),
results(buffer_size)
{
    initFFT(buffer_size);
}

Hanalysis::~Hanalysis()
{
    destroyFFT();
}


void Hanalysis::destroyFFT()
{
    m_HanningWindow.clear();
    if(m_FftPtr != nullptr)
    {
        sdkFFTDestroy(m_FftPtr);
        m_Re.clear();
        m_Im.clear();
    }
}

void Hanalysis::initFFT(int buffer_size)
{
    m_FftPtr = sdkFFTCreate(buffer_size);
    
    m_Re.resize(buffer_size/2+1);
    m_Im.resize(buffer_size/2+1);
    
    initializeHanningWindow();
}

void Hanalysis::initializeHanningWindow()
{
    if (m_HanningWindow.size() != m_BufferSize)
        m_HanningWindow.resize(m_BufferSize);
    for (int i = 0; i < m_BufferSize; ++i)
    {
        m_HanningWindow[i] = 0.5f * (1 - std::cos(2 * M_PI * i / (m_BufferSize - 1)));
    }
}

void Hanalysis::applyHanningWindow(float *ptrAudioInput, int buffer_size)
{
    for (int i = 0; i < buffer_size; ++i)
    {
        features.windowedAudioBuffer[i] = ptrAudioInput[i] * m_HanningWindow[i];
    }
}

void Hanalysis::computeFeatures(float* ptrAudioInput, const int buffer_size, const float sample_rate)
{
    features.ptrAudio = ptrAudioInput;
    if(spectralBool || harmonicBool || perceptualBool)
    {
        applyHanningWindow(ptrAudioInput, buffer_size);
        

        sdkFFTForward(m_FftPtr, features.windowedAudioBuffer.data(), m_Re.data(), m_Im.data());

        
        for (int i = 0; i < m_FftSize; ++i) {
            

            features.magnitudes[i] = m_Re[i] * m_Re[i] + m_Im[i] * m_Im[i];

            
            features.amplitudes[i] = std::sqrtf(features.magnitudes[i]);
            features.frequencies[i] = i * sample_rate / buffer_size;
            features.sum_magnitudes += features.magnitudes[i];
            features.sum_amplitudes += features.amplitudes[i];
            features.sum_magnitudes_times_freqs += features.magnitudes[i] * features.frequencies[i];
            features.sum_frequencies += features.frequencies[i];
            features.sum_frequencies_squared += features.frequencies[i] * features.frequencies[i];
            features.prod_magnitudes += std::logf(features.magnitudes[i] + std::numeric_limits<float>::epsilon());
        }
        
        features.prod_magnitudes = std::expf(features.prod_magnitudes);
        
        if (!features.magnitudes.empty()) {
            auto itMaxAmp = std::max_element(features.magnitudes.begin(), features.magnitudes.end());
            features.max_magnitude = *itMaxAmp;
            features.max_frequency = features.frequencies[std::distance(features.magnitudes.begin(), itMaxAmp)];
        } else {
            features.max_magnitude = 0.0f;
            features.max_frequency = 0.0f;
        }
        
        features.unnormalized_magnitudes = features.magnitudes;
        if (features.sum_magnitudes != 0.0f)
        {
            for (size_t i = 0; i < m_FftSize; i++)
            {
                features.magnitudes[i] /= features.sum_magnitudes;
            }
        }
    }
}

void Hanalysis::computeSimpleFeatures(float *ptrAudioInput, const int buffer_size, const float sample_rate)
{
    if(ptrAudioInput != nullptr)
    {
        features.ptrAudio = ptrAudioInput;
        applyHanningWindow(ptrAudioInput, buffer_size);

        sdkFFTForward(m_FftPtr, features.windowedAudioBuffer.data(), m_Re.data(), m_Im.data());

        
        for (int i = 0; i < m_FftSize; ++i) {
            

            features.magnitudes[i] = m_Re[i] * m_Re[i] + m_Im[i] * m_Im[i];
   
            features.frequencies[i] = i * sample_rate / buffer_size;
            features.sum_magnitudes += features.magnitudes[i];
            features.sum_magnitudes_times_freqs += features.magnitudes[i] * features.frequencies[i];
        }

        if (features.sum_magnitudes != 0.0f)
        {
            for (size_t i = 0; i < m_FftSize; i++)
            {
                features.magnitudes[i] /= features.sum_magnitudes;
            }
        }

    }
}

void Hanalysis::computeDescriptors(Features& features, Results& results)
{
    if (spectralBool)
    {
        descriptors.spectralCentroid.process(features,results);
        descriptors.spectralSpread.process(features,results);
        descriptors.spectralSkewness.process(features,results);
        descriptors.spectralKurtosis.process(features,results);
        descriptors.spectralSlope.process(features,results);
        descriptors.spectralRolloff.process(features,results);
        descriptors.spectralFlatness.process(features,results);
        descriptors.spectralCrest.process(features,results);
        descriptors.spectralTonality.process(features,results);
        descriptors.spectralDecrease.process(features,results);
        descriptors.spectralEntropy.process(features,results);
        descriptors.spectralPeaks.process(features,results);
    }
    if (harmonicBool)
    {
        descriptors.mfcc.process(features,results);
        descriptors.f0_YIN.process(features,results);
        descriptors.inharmonicity.process(features,results);
        descriptors.oddToEvenRatio.process(features,results);
        descriptors.tristimulus.process(features,results);
    }
    if (temporalBool)
    {
        descriptors.rms.process(features,results);
    }
    if (perceptualBool)
    {
        descriptors.loudness.process(features,results);
        descriptors.sharpness.process(results);
    }
}

void Hanalysis::computeSimpleDescriptors(Features &features, Results& results)
{
    // frequency estimation
    descriptors.f0_YIN.process(features,results);
    // spectral metrics
    descriptors.spectralCentroid.process(features,results);
    descriptors.spectralSpread.process(features,results);
    descriptors.spectralFlatness.process(features,results);
    // level metrics
    descriptors.rms.process(features,results);
    descriptors.loudness.process(features,results);
}

void Hanalysis::reset(Features& features)
{
    features.sum_amplitudes = 0.0f;
    features.sum_magnitudes = 0.0f;
    features.sum_frequencies = 0.0f;
    features.sum_frequencies_squared = 0.0f;
    features.sum_magnitudes_times_freqs = 0.0f;
    features.prod_magnitudes = 0.0f;
    features.max_frequency = 0.0f;
    features.max_magnitude = 0.0f;
    
    if(int(features.amplitudes.size()) != m_FftSize )
    {
        features.amplitudes.resize(m_FftSize);
    }
    if(int(features.magnitudes.size()) != m_FftSize )
    {
        features.magnitudes.resize(m_FftSize);
    }
    if(int(features.frequencies.size()) != m_FftSize )
    {
        features.frequencies.resize(m_FftSize);
    }
    if(int(features.windowedAudioBuffer.size()) != m_BufferSize )
    {
        features.windowedAudioBuffer.resize(m_BufferSize);
    }
    if(int(features.unnormalized_magnitudes.size()) != m_FftSize )
    {
        features.unnormalized_magnitudes.resize(m_FftSize);
    }
    
}

void Hanalysis::process(float* ptrAudioInput, const int buffer_size, const float sample_rate)
{
    if (ptrAudioInput != nullptr)
    {
        if (m_AnalysisType == 0)
        {
            computeSimpleFeatures(ptrAudioInput, buffer_size, sample_rate);
            computeSimpleDescriptors(features, results);
        }
        else if (m_AnalysisType == 1)
        {
            computeFeatures(ptrAudioInput, buffer_size, sample_rate);
            computeDescriptors(features, results);
        }
    }
    else {
        sdkTraceLogChar("error in audio analysis process");
    }
    reset(features);
}



void Hanalysis::setFFTSize(int new_size, float sample_rate)
{
    m_BufferSize = new_size;
    m_FftSize = new_size / 2 + 1;
    reset(features);
    destroyFFT();
    initFFT(new_size);
    features.buffer_size = new_size;
    features.fft_size = new_size/2 + 1;
    features.sample_rate = sample_rate;
    m_SampleRate = sample_rate;
    descriptors.setSampleRate(new_size, sample_rate);
}


void Hanalysis::setSampleRate(float new_sample_rate)
{
    m_SampleRate = new_sample_rate;
    features.sample_rate = new_sample_rate;
    descriptors.setSampleRate(m_BufferSize, new_sample_rate);
}



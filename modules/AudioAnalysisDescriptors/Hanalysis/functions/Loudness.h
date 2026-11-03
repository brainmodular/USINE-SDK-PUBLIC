#ifndef __LOUDNESS_H__
#define __LOUDNESS_H__

#ifdef  _WIN32
#include "../Hfeatures.h"
#endif // Windows

#ifdef __APPLE__
#include "Hfeatures.h"
#endif // MacOs


#include <vector>

constexpr TPrecision TEMPORARY_GAIN = 1.0f;

class Loudness
{
public:
    Loudness(int num_bands, int buffer_size, float sample_rate);
    ~Loudness() {};

    void computeCoefficients(Features& features);
    std::vector<float> computeSpecificLoudness();
    void process(Features& features, Results& results);

    void setFftSize(int new_size, float new_sample_rate);
    
private:

    int m_iNumBands;

    std::vector<float> m_vBarkEdges;
    std::vector<int> m_vBarkLimits;
    std::vector<float> m_vBarkCoefficients;
    std::vector<float> m_vSpecificLoudness;
    std::vector<float> m_vSpecificResult;

    void init(int buffer_size, float sample_rate);
    void reset();
};

#endif // __LOUDNESS_H__

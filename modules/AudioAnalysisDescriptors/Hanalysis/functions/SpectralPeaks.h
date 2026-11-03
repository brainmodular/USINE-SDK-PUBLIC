#ifndef __SPECTRALPEAKS_H__
#define __SPECTRALPEAKS_H__

#ifdef  _WIN32
#include "../Hfeatures.h"
#endif // Windows

#ifdef __APPLE__
#include "Hfeatures.h"
#endif // MacOs

#include <vector>

constexpr TPrecision MAG_THRESHOLD  = 1E-2f;
constexpr TPrecision PROM_THRESHOLD = 1E-5f;
constexpr int        SEARCH_WINDOW  = 6;

class SpectralPeaks
{
public:

    SpectralPeaks() {};
    ~SpectralPeaks() {};

    void process(Features &features, Results& results);

private:
    std::vector<SpectralPeak> m_vPeaks;
    SpectralPeak m_pPeak;
};

#endif // __SPECTRALPEAKS_H__

#ifndef __SPECTRAL_FLATNESS_H__
#define __SPECTRAL_FLATNESS_H__

#ifdef  _WIN32
#include "../Hfeatures.h"
#endif // Windows

#ifdef __APPLE__
#include "Hfeatures.h"
#endif // MacOs

class SpectralFlatness{
public:

    SpectralFlatness() {};

    ~SpectralFlatness() {};

    void process(Features& features, Results& results);

};

#endif //__SPECTRAL_FLATNESS_H__

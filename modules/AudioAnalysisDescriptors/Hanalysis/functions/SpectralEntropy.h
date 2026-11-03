#ifndef __SPECTRAL_ENTROPY_H__
#define __SPECTRAL_ENTROPY_H__

#ifdef  _WIN32
#include "../Hfeatures.h"
#endif // Windows

#ifdef __APPLE__
#include "Hfeatures.h"
#endif // MacOs

#include <cmath>

class SpectralEntropy{
public:

    SpectralEntropy() {};

    ~SpectralEntropy() {};

    void process(Features& features, Results& results);
   
};
#endif //__SPECTRAL_ENTROPY_H__

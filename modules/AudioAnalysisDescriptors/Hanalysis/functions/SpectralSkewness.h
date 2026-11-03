#ifndef __SPECTRAL_SKEWNESS_H__
#define __SPECTRAL_SKEWNESS_H__

#ifdef  _WIN32
#include "../Hfeatures.h"
#endif // Windows

#ifdef __APPLE__
#include "Hfeatures.h"
#endif // MacOs
#include <cmath>

using namespace std;

class SpectralSkewness{
public:

    SpectralSkewness() {};

    ~SpectralSkewness() {};

    void process(Features& features, Results& results);
    
};


#endif //__SPECTRAL_SKEWNESS_H__

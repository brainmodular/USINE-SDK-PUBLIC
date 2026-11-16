#ifndef __SPECTRAL_SLOPE_H__
#define __SPECTRAL_SLOPE_H__

#include "../Hfeatures.h"

#ifdef __APPLE__
#include "Hfeatures.h"
#endif // MacOs

#include <cmath>

using namespace std;

class SpectralSlope{
public:

    SpectralSlope() {};

    ~SpectralSlope() {};

    void process(Features& features, Results& results);

};
#endif //__SPECTRAL_SLOPE_H__

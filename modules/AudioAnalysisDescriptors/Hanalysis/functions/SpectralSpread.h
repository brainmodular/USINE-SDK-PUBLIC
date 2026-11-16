#ifndef __SPECTRAL_SPREAD_H__
#define __SPECTRAL_SPREAD_H__

#include "../Hfeatures.h"

#ifdef __APPLE__
#include "Hfeatures.h"
#endif // MacOs

#include <cmath>

using namespace std;

class SpectralSpread{
public:

    SpectralSpread() {};

    ~SpectralSpread() {};

    void process(Features& features, Results& results);


    
};
#endif //__SPECTRAL_SPREAD_H__

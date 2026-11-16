#ifndef __SPECTRAL_DECREASE_H__
#define __SPECTRAL_DECREASE_H__

#include "../Hfeatures.h"

#ifdef __APPLE__
#include "Hfeatures.h"
#endif // MacOs



class SpectralDecrease{
public:

    SpectralDecrease() {};

    ~SpectralDecrease() {};

    void process(Features& features, Results& results);
    
};

#endif //__SPECTRAL_DECREASE_H__
    
    

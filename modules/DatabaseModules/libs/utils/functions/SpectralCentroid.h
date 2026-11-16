#ifndef __SPECTRAL_CENTROID_H__
#define __SPECTRAL_CENTROID_H__


#include "HFeatures.h"

class SpectralCentroid{
public:

    SpectralCentroid() {};

    ~SpectralCentroid() {};

    
    void process(Features& features);
    
};

#endif //__SPECTRAL_CENTROID_H__

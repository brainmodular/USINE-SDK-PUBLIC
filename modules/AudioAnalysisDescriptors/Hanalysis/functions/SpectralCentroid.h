#ifndef __SPECTRAL_CENTROID_H__
#define __SPECTRAL_CENTROID_H__


#include "../Hfeatures.h"

#ifdef __APPLE__
#include "Hfeatures.h"
#endif // MacOs

class SpectralCentroid{
public:

    SpectralCentroid() {};

    ~SpectralCentroid() {};

    
    void process(Features& features, Results& results);
    
};

#endif //__SPECTRAL_CENTROID_H__

#ifndef __SPECTRAL_CENTROID_H__
#define __SPECTRAL_CENTROID_H__


#ifdef  _WIN32
#include "Hfeatures.h"
#endif // Windows

#ifdef __APPLE__
#include "Hfeatures.h"
#endif // MacOs

class SpectralCentroid{
public:

    SpectralCentroid() {};

    ~SpectralCentroid() {};

    
    void process(Features& features);
    
};

#endif //__SPECTRAL_CENTROID_H__

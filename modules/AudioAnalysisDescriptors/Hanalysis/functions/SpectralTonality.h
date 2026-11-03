#ifndef __SPECTRAL_TONALITY_H__
#define __SPECTRAL_TONALITY_H__

#ifdef  _WIN32
#include "../Hfeatures.h"
#endif // Windows

#ifdef __APPLE__
#include "Hfeatures.h"
#endif // MacOs


class SpectralTonality{
public:
    
    SpectralTonality() {};

    ~SpectralTonality() {};
    

    
    void process(Features& features, Results& results);
    
};



#endif //__SPECTRAL_TONALITY_H__

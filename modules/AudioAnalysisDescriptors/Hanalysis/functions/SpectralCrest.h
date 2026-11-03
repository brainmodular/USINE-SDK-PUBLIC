#ifndef __SPECTRAL_CREST_H__
#define __SPECTRAL_CREST_H__

#ifdef  _WIN32
#include "../Hfeatures.h"
#endif // Windows

#ifdef __APPLE__
#include "Hfeatures.h"
#endif // MacOs

class SpectralCrest{
public:

    SpectralCrest() {};

    ~SpectralCrest() {};

    void process(Features& features, Results& results);

};


#endif //__SPECTRAL_CREST_H__

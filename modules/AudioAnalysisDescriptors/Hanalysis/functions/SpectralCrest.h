#ifndef __SPECTRAL_CREST_H__
#define __SPECTRAL_CREST_H__

#include "../Hfeatures.h"

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

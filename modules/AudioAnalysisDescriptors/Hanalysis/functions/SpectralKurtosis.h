#ifndef __SPECTRAL_KURTOSIS_H__
#define __SPECTRAL_KURTOSIS_H__

#ifdef  _WIN32
#include "../Hfeatures.h"
#endif // Windows

#ifdef __APPLE__
#include "Hfeatures.h"
#endif // MacOs

#include <cmath>


using namespace std;


class SpectralKurtosis{
public:
    SpectralKurtosis() {};
    ~SpectralKurtosis() {};

    void process(Features& features, Results& results);
    
};



#endif //__SPECTRAL_KURTOSIS_H__

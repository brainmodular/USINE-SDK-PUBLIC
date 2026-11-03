#ifndef __SPECTRAL_ROLLOFF_H__
#define __SPECTRAL_ROLLOFF_H__

#ifdef  _WIN32
#include "../Hfeatures.h"
#endif // Windows

#ifdef __APPLE__
#include "Hfeatures.h"
#endif // MacOs

#include <cmath>

using namespace std;
class SpectralRolloff{
public:

    SpectralRolloff() {};

    ~SpectralRolloff() {};

    void process(Features& features, Results& results);
   
};
#endif //__SPECTRAL_ROLLOFF_H__

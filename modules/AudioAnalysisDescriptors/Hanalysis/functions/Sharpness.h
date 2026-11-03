#ifndef __SHARPNESS_H__
#define __SHARPNESS_H__

#ifdef  _WIN32
#include "../Hfeatures.h"
#endif // Windows

#ifdef __APPLE__
#include "Hfeatures.h"
#endif // MacOs

#include <cmath>


class Sharpness
{
public:

    Sharpness(int num_bands);
    ~Sharpness() {};

    void process(Results& results);
private:

    int m_iNumBands;

};

#endif // __SHARPNESS_H__

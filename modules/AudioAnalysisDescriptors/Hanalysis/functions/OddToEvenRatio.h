#ifndef __ODDTOEVENRATIO_H__
#define __ODDTOEVENRATIO_H__

#ifdef  _WIN32
#include "../Hfeatures.h"
#endif // Windows

#ifdef __APPLE__
#include "Hfeatures.h"
#endif // MacOs

#include <iostream>
#include <cmath>

class OddToEvenRatio
{
public:
    OddToEvenRatio() {};
    ~OddToEvenRatio() {};

    void process(Features &features, Results& results);
private:



};

#endif // __ODDTOEVENRATIO_H__

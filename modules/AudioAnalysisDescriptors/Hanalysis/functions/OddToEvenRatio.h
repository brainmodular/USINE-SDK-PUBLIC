#ifndef __ODDTOEVENRATIO_H__
#define __ODDTOEVENRATIO_H__

#include "../Hfeatures.h"

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

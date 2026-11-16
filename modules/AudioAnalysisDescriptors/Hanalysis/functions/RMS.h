#ifndef __RMS_H__
#define __RMS_H__

#include "../Hfeatures.h"

#ifdef __APPLE__
#include "Hfeatures.h"
#endif // MacOs

#include <cmath>

class RMS
{
public:
    RMS() {};
    ~RMS() {};

    void process(Features& features, Results& results);

};





#endif // RMS.h

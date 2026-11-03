#ifndef __RMS_H__
#define __RMS_H__

#ifdef  _WIN32
#include "Hfeatures.h"
#endif // Windows

#ifdef __APPLE__
#include "Hfeatures.h"
#endif // MacOs

#include <cmath>

class RMS
{
public:
    RMS() {};
    ~RMS() {};

    void process(Features& features);

};





#endif // RMS.h

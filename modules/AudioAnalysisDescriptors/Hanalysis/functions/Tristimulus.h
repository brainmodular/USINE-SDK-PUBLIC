#ifndef __TRISTIMULUS_H__
#define __TRISTIMULUS_H__

#ifdef  _WIN32
#include "../Hfeatures.h"
#endif // Windows

#ifdef __APPLE__
#include "Hfeatures.h"
#endif // MacOs


#include <vector>


class Tristimulus
{
public:
    Tristimulus() {};
    ~Tristimulus() {};

    void process(Features& features, Results& results);
    
private:
    std::vector<float> m_vResult = {0.0f, 0.0f, 0.0f};
};

#endif // __TRISTIMULUS_H__

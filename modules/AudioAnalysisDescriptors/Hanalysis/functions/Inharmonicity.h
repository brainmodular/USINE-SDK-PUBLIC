#ifndef __INHARMONICITY_H__
#define __INHARMONICITY_H__

#ifdef  _WIN32
#include "../Hfeatures.h"
#endif // Windows

#ifdef __APPLE__
#include "Hfeatures.h"
#endif // MacOs

class Inharmonicity
{
public:
    Inharmonicity() {};

    ~Inharmonicity() {};

    void process(Features &features, Results& results);
    
};

#endif // __INHARMONICITY_H__

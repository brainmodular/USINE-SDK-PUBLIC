#ifndef __INHARMONICITY_H__
#define __INHARMONICITY_H__

#include "../Hfeatures.h"

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

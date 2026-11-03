#include "RMS.h"

void RMS::process(Features& features, Results& results)
{
    // Calculate the RMS value
    float result = 0.0f;
    for(int i = 0; i < features.buffer_size; i++)
    {
        result += features.ptrAudio[i] * features.ptrAudio[i];
    }
    
    result = std::sqrtf(result / features.buffer_size);
    
    // Convert to dB
    result = 20.0f * std::log10f(std::max(result, 1e-12f));
    
    
    results.temporalRMS = result;
}

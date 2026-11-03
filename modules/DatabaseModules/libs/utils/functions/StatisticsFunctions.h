#pragma once

#include <vector>
#include <cmath>

inline float CalcMeanValue (const std::vector<FeaturesResults>& vec, const float FeaturesResults::*member)
{
    if(vec.empty()) return 0.0f;
    
    size_t vec_size = vec.size();
    float result = 0.0f;
    for(size_t i = 0; i < vec_size; i++)
    {
        result += vec[i].*member;
    }
    
    return result / static_cast<float>(vec_size);
}

inline std::vector<float> CalcMeanVector(const std::vector<FeaturesResults>& vec, const std::vector<float> FeaturesResults::*member, int normal_member_size)
{
    if (vec.empty())
        return {};
    
    size_t vec_size = vec.size();
    size_t member_size = normal_member_size;
    
    std::vector<float> result(member_size, 0.0f);
    
    for (size_t i = 0; i < vec_size; ++i)
    {
        const auto& currentVec = vec[i].*member;
        
        if (currentVec.size() != member_size)
            continue;
        
        for (size_t j = 0; j < member_size; ++j)
        {
            result[j] += currentVec[j];
        }
    }
    
    for (size_t j = 0; j < member_size; ++j)
    {
        result[j] /= static_cast<float>(vec_size);
    }
    
    return result;
}

inline float CalcStdDev (const std::vector<FeaturesResults>& vec, const float FeaturesResults::*member)
{
    size_t vec_size = vec.size();
    if (vec_size == 0) return 0.0f;
    
    float mean = 0.0f;
    for (size_t i = 0; i < vec_size; ++i)
    {
        mean += vec[i].*member;
    }
    mean /= static_cast<float>(vec_size);
    
    float variance = 0.0f;
    for (size_t i = 0; i < vec_size; ++i)
    {
        float diff = vec[i].*member - mean;
        variance += diff * diff;
    }
    variance /= static_cast<float>(vec_size);
    
    return std::sqrt(variance);
}

inline std::vector<float> CalcStdDevVector(const std::vector<FeaturesResults>& vec, const std::vector<float> FeaturesResults::*member, int normal_member_size)
{
    if (vec.empty() || (vec[0].*member).empty()) return {};
    
    size_t vec_size = vec.size();
    size_t dim = normal_member_size;
    
    std::vector<float> mean(dim, 0.0f);
    std::vector<float> variance(dim, 0.0f);
    
    for (size_t i = 0; i < vec_size; ++i)
    {
        const std::vector<float>& current = vec[i].*member;
        if (current.size() != normal_member_size) continue;
        
        for (size_t j = 0; j < normal_member_size; ++j)
        {
            mean[j] += current[j];
        }
    }
    
    for (size_t j = 0; j < dim; ++j)
    {
        mean[j] /= static_cast<float>(vec_size);
    }
    
    for (size_t i = 0; i < vec_size; ++i)
    {
        const std::vector<float>& current = vec[i].*member;
        if (current.size() != normal_member_size) continue;
        
        for (size_t j = 0; j < normal_member_size; ++j)
        {
            float diff = current[j] - mean[j];
            variance[j] += diff * diff;
        }
    }
    
    for (size_t j = 0; j < normal_member_size; ++j)
    {
        variance[j] = std::sqrt(variance[j] / static_cast<float>(vec_size));
    }
    
    return variance;
}

inline float CalcMaxMeanVector(const std::vector<FeaturesResults>& vec, const std::vector<float> FeaturesResults::*member, int normal_member_size)
{
    if (vec.empty())
        return 0.0f;
    
    size_t vec_size = vec.size();
    size_t member_size = normal_member_size;
    
    float maxValue = 0.0f;
    
    for (size_t i = 0; i < vec_size; ++i)
    {
        const auto& currentVec = vec[i].*member;
        float currentValue = 0.0f;
        
        if (currentVec.size() != member_size)
            continue;
        
        for (size_t j = 0; j < member_size; ++j)
        {
            currentValue += currentVec[j];
        }
        currentValue /= (float)member_size;
        if(currentValue >= maxValue)
        {
            maxValue = currentValue;
        }
        
    }
    
    return maxValue;
}

inline float CalcMaxVector(const std::vector<FeaturesResults>& vec, const std::vector<float> FeaturesResults::*member, int normal_member_size)
{
    if (vec.empty())
        return 0.0f;
    
    size_t vec_size = vec.size();
    size_t member_size = normal_member_size;
    
    float maxValue = 0.0f;
    
    for (size_t i = 0; i < vec_size; ++i)
    {
        const auto& currentVec = vec[i].*member;
        
        if (currentVec.size() != member_size)
            continue;
        
        for (size_t j = 0; j < member_size; ++j)
        {
            if(currentVec[j] >= maxValue)
            {
                maxValue = currentVec[j];
            }
        }
    }
    
    return maxValue;
}

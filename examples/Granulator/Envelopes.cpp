
#include "../../sdk/UserUtils.h"
#include "Envelopes.h"
#include <cmath>
#include <algorithm> 

//-------------------------------------------------------------------------
//
AsrEnvelope::AsrEnvelope(int size)
{
    setSize(size);
    setVariables(1.0f);
}

//
void AsrEnvelope::setVariables(float skew)
{
    envelopeVar = skew;
    if (envelopeVar <= ENV_VAR_MIN)
    {
        asrAttackSize = 0.0f;
        asrSustainSize = (float)envelopeSize;

        asrInvAttackSize = 10.0f;  // fake value, because not used if asrAttackSize = 0;
        asrInvSustainSize = 1.0f/asrSustainSize;
    }
    else
    {
	    asrAttackSize = std::max(0.0001f, (static_cast<float>(envelopeSize)/2.0f) * skew);
        asrSustainSize = std::max(0.0001f,static_cast<float>(envelopeSize) - asrAttackSize);

        asrInvAttackSize = 1.0f/asrAttackSize;
        asrInvSustainSize = 1.0f/asrSustainSize;
    }
}

//
void AsrEnvelope::getGraphPoints(TPointF Points[], int NumOfPoints)
{
    float amp = 0;
    if(NumOfPoints == 4)
    {
        Points[0].x = 0.0f;
        Points[0].y = 1.0f; 

        processEnvelope(static_cast<int>(asrAttackSize + 1), amp);
        Points[1].x = (asrAttackSize - 1.0f) / static_cast<float>(envelopeSize);
        Points[1].y = 1.0f - amp;  

        processEnvelope(static_cast<int>(asrSustainSize - 1), amp);
        Points[2].x = (asrSustainSize - 1.0f) / static_cast<float>(envelopeSize);
        Points[2].y = 1.0f - amp;      
        
        Points[3].x = 0.999f;
        Points[3].y = 1.0f;       
    }
}

//
void AsrEnvelope::processEnvelope(int index, float& amp)
{
	//Calculate envelope.
	if(index < asrAttackSize)
		amp = (index * asrInvAttackSize);
	else if(index >= envelopeSize)
    {
        amp = 0.0f;
    }
	else if(index > (asrSustainSize))
		amp = (envelopeSize-index) * asrInvAttackSize;
	else
		amp = 1.0f;
}

//-------------------------------------------------------------------------
//
RcbEnvelope::RcbEnvelope(int size)
{
    setSize(size);
    setVariables(1.0f);
}

//
void RcbEnvelope::setVariables(float skew)
{
    envelopeVar = skew;

    rcbSectionSize = (static_cast<float>(envelopeSize)/2.0f) * envelopeVar;
    attack_angle = PI;
    release_angle = 0.0f;
    //angle_increment = PI / rcbSectionSize/2.0f;
    angle_increment = PI / rcbSectionSize;
}

//
void RcbEnvelope::getGraphPoints(TPointF Points[], int NumOfPoints)
{
    float amp = 0.0f;
    float tmp = 0.0f;
    float tmpsize = (NumOfPoints-2.0f);
    int halfsize = static_cast<int>(tmpsize)/2;
    int i;

    Points[0].x = 0.0f;
    Points[0].y = 0.999f; 

    for (i = 1; i <= halfsize; i++)
    {
        tmp = static_cast<float>(i)/static_cast<float>(halfsize);
        processEnvelope(static_cast<int>(tmp*rcbSectionSize), amp);
        Points[i].x = (static_cast<float>(i)/tmpsize) * envelopeVar;
        Points[i].y = 1.0f - amp;        
    }

    Points[halfsize+1].x = 0.5f + ((1.0f- envelopeVar)*0.499f);
    Points[halfsize+1].y = 0.0f ; 

    for (i = 1; i < halfsize; i++)
    {
        tmp = static_cast<float>(i)/static_cast<float>(halfsize);
        processEnvelope(static_cast<int>((envelopeSize - rcbSectionSize) + (tmp*rcbSectionSize)), amp);
        Points[halfsize+1 +i].x = 0.5f + ((1.0f- envelopeVar)*0.499f) + ((static_cast<float>(i)/tmpsize) * envelopeVar);
        Points[halfsize+1 +i].y = 1.0f - amp;        
    }
    
    Points[NumOfPoints - 1].x = 0.999f;
    Points[NumOfPoints - 1].y = 0.999f ;        
}

//
void RcbEnvelope::processEnvelope(int index, float& amp)
{
	//Calculate envelope.
	if(index < rcbSectionSize)
    {
        attack_angle = PI + (angle_increment * index);
		amp = (0.5f + 0.5f * fastCosine(attack_angle));
    }
	else if(index >= envelopeSize)
    {
        amp = 0.0f;
    }
	else if(index >= (envelopeSize - rcbSectionSize))
    {
        release_angle = (index - (envelopeSize - rcbSectionSize))*(angle_increment);
		amp = (0.5f + 0.5f * fastCosine(release_angle));
    }
	else
    {
		amp = 1.0f;
    }
}

//-------------------------------------------------------------------------
//
BezierEnvelope::BezierEnvelope(int size)
    : left0(0.0f) 
    , left1(0.0f) 
    , left2(0.0f) 
    , center(0.0f)  
{
    setSize(size);
}

//
void BezierEnvelope::setVariables(float skew)
{
    envelopeVar = skew; 

    left0 = 0.0f; 
    center = 1.0f; 

    if (skew < 0.5f)
    {
        left1 = 0.0f; 
        left2 = envelopeVar*2.0f; 
    }
    else
    {
        left1 = (envelopeVar- 0.5f)*2.0f; 
        left2 = 1.0f; 
    }  
}

//
void BezierEnvelope::getGraphPoints(TPointF Points[], int size)
{
    float amp = 0.0f;
    float tmp = 0.0f;
    for (int i = 0; i < size-1; i++)
    {
        tmp = static_cast<float>(i)/static_cast<float>(size-1);
        processEnvelope(static_cast<int>(tmp*envelopeSize), amp);
        Points[i].x = static_cast<float>(i)/static_cast<float>(size-1);
        Points[i].y = 1.0f - amp;        
    }  
        Points[size-1].x = 0.999f;
        Points[size-1].y = 1.0f;       
}

//
void BezierEnvelope::processEnvelope(int index, float& amp)
{

    if(index >= envelopeSize)
    {
        amp = 0.0f;
        return;
    }

    float fraction = static_cast<float>(index)/static_cast<float>(envelopeSize);

    if (fraction >= 0.5f)
    {
        fraction = 1.0f - fraction;
    }

    fraction *= 2.0f;
    fraction = (sqrtf(fraction)* (envelopeVar)) + ((fraction*fraction)*(1.0f - (envelopeVar)));
    amp = sdkBezier4(left0, left1, left2, center, fraction);
}


//-------------------------------------------------------------------------
//
GaussianEnvelope::GaussianEnvelope(int size)
{
    setSize(size);
}

//
void GaussianEnvelope::setVariables(float skew)
{
    envelopeVar = skew;
    gaussMax = envelopeSize/2.0f;
    float coeff = (envelopeSize/(4.9f + (10.0f * envelopeVar)));
    gaussSqrDev = coeff*coeff;
}

//
void GaussianEnvelope::getGraphPoints(TPointF Points[], int size)
{
    float amp = 0.0f;
    float tmp = 0.0f;
    for (int i = 0; i < size - 1; i++)
    {
        tmp = static_cast<float>(i)/static_cast<float>(size);
        processEnvelope(static_cast<int>(tmp*envelopeSize), amp);
        Points[i].x = static_cast<float>(i)/static_cast<float>(size);
        Points[i].y = 1.0f - amp;        
    }
    Points[size - 1].x = 0.9999f;
    Points[size - 1].y = 0.9999f ;        
}

//
void GaussianEnvelope::processEnvelope(int index, float& amp)
{
    float x = index - gaussMax;

    amp = exp(-(x*x/gaussSqrDev));

    if(index >= envelopeSize)
    {
        amp = 0.0f;
    }
}

//-------------------------------------------------------------------------
//
HanningEnvelope::HanningEnvelope(int size)
{
    setSize(size);
}

//
void HanningEnvelope::setVariables(float skew)
{
    envelopeVar = skew;
}


//
void HanningEnvelope::getGraphPoints(TPointF Points[], int size)
{
    float amp = 0.0f;
    float tmp = 0.0f;
    for (int i = 0; i < size - 1; i++)
    {
        tmp = static_cast<float>(i)/static_cast<float>(size);
        processEnvelope(static_cast<int>(tmp*envelopeSize), amp);
        Points[i].x = static_cast<float>(i)/static_cast<float>(size);
        Points[i].y = 1.0f - amp;        
    }
    Points[size - 1].x = 0.9999f;
    Points[size - 1].y = 0.9999f ;  
}

//
void HanningEnvelope::processEnvelope(int index, float& amp)
{
    amp = 0.5f * (1.0f - fastCosine(2.0f*PI*(float)index/envelopeSize));
    //amp = 0.5f * (1.0f - cosf(2.0f*PI*(float)index/envelopeSize));

    if(index >= envelopeSize)
    {
        amp = 0.0f;
    }
}


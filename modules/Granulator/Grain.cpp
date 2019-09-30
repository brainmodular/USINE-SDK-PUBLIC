//	Grain.cpp - Simple class representing a single grain.
//	---------------------------------------------------------------------------
//	Copyright (c) 2006-2007 Niall Moody
//
//	Permission is hereby granted, free of charge, to any person obtaining a
//	copy of this software and associated documentation files (the "Software"),
//	to deal in the Software without restriction, including without limitation
//	the rights to use, copy, modify, merge, publish, distribute, sublicense,
//	and/or sell copies of the Software, and to permit persons to whom the
//	Software is furnished to do so, subject to the following conditions:
//
//	The above copyright notice and this permission notice shall be included in
//	all copies or substantial portions of the Software.
//
//	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
//	THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
//	FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
//	DEALINGS IN THE SOFTWARE.
//	---------------------------------------------------------------------------

#include "../../sdk/UserUtils.h"
#include "GranulatorModule.h"
#include "Grain.h"
#include <cassert>
#include <cmath>

std::auto_ptr<Envelope> Grain::asrEnvelope(new AsrEnvelope(10000));
std::auto_ptr<Envelope> Grain::rcbEnvelope(new RcbEnvelope(10000));
std::auto_ptr<Envelope> Grain::bezierEnvelope(new BezierEnvelope(10000));
std::auto_ptr<Envelope> Grain::hanningEnvelope(new HanningEnvelope(10000));
std::auto_ptr<Envelope> Grain::gaussianEnvelope(new GaussianEnvelope(10000));
//-----------------------------------------------------------------------------
Grain::Grain()
    : isActive(false)
    , buffer(0)
    , bufferSize(0)
    , index(0.0f)
    , grainIndex(0.0f)
    , pitchIncrement(1.0f)
    , currentEnvelope(0)
    , startPosition(0)
    , grainSize(0)
{
    currentEnvelope = new AsrEnvelope(10);
}

//-----------------------------------------------------------------------------
Grain::~Grain()
{
    delete currentEnvelope;
}

//-----------------------------------------------------------------------------
void Grain::setModule(GranulatorModule* parent)
{
    module = parent;
}

//-----------------------------------------------------------------------------
void Grain::setInputBuffer(TPrecision* inputBuffer, int size)
{
	buffer = inputBuffer;
	bufferSize = size;
}

//-----------------------------------------------------------------------------
void Grain::setEnvelope(EnvType envType)
{
    delete currentEnvelope;

    switch (envType)
    {
    case ENV_ASR:
    default:
        currentEnvelope = new AsrEnvelope(grainSize);
        break;
    case ENV_BEZIER:
        currentEnvelope = new BezierEnvelope(grainSize);
        break;
    case ENV_RCB:
        currentEnvelope = new RcbEnvelope(grainSize);
        break;
    case ENV_HANN:
        currentEnvelope = new HanningEnvelope(grainSize);
        break;
    case ENV_GAUSS:
        currentEnvelope = new GaussianEnvelope(grainSize);
        break;
    }   
}

//-----------------------------------------------------------------------------
void Grain::getGraphPoints(EnvType envType, float envVar, TPointF Points[], int NumOfPoints)
{
    switch (envType)
    {
    case ENV_ASR:
        asrEnvelope->setVariables(envVar);
        asrEnvelope->getGraphPoints(Points, NumOfPoints);
        break;

    case ENV_BEZIER:
        bezierEnvelope->setVariables(envVar);
        bezierEnvelope->getGraphPoints(Points, NumOfPoints);
        break;

    case ENV_RCB:
        rcbEnvelope->setVariables(envVar);
        rcbEnvelope->getGraphPoints(Points, NumOfPoints);
        break;

    case ENV_HANN:
        hanningEnvelope->setVariables(envVar);
        hanningEnvelope->getGraphPoints(Points, NumOfPoints);
        break;

    case ENV_GAUSS:
        gaussianEnvelope->setVariables(envVar);
        gaussianEnvelope->getGraphPoints(Points, NumOfPoints);
        break;
    }
}

//-----------------------------------------------------------------------------
void Grain::activate(float envVar, int startPos, int size, float inc)
{
    pitchIncrement = inc;
	grainSize = size;
    currentEnvelope->setSize(grainSize);
    currentEnvelope->setVariables(envVar);

	if(pitchIncrement > 1.0f)
	{
		startPosition = startPos - (int)((pitchIncrement-1.0f)* grainSize);
		while(startPosition < 0.0f)
			startPosition += grainSize;
	}
	else
        startPosition = startPos;

	index = 0.0f;
	grainIndex = 0.0f;
	isActive = true;
}
//-----------------------------------------------------------------------------
void Grain::getSample(TPrecision* audioBlock)
{
	//int tempint;
	float amp = 0;

    // compute gain from envelope
    currentEnvelope->processEnvelope( (int)grainIndex, amp);

	//Generate audio.
	if(isActive)
	{
        // no pitch
        if (pitchIncrement == 1)
        {
            // no interpolation
            //tempint = (static_cast<int>(startPosition + index)) % (bufferSize);

            int i1 = (startPosition + (int)index) % bufferSize;
            audioBlock[0] += buffer[i1] * amp; 
        }
        // pitch from -24 to -48
		else if (pitchIncrement <= 0.25)
        {
            // cubic interpolation
            int i0 = (int)index;
            float x = index - i0;            
            int im1 = (i0 + startPosition - 1 + bufferSize) % bufferSize;
            int i1 = (i0 + startPosition) % bufferSize;
            int i2 = (i1 + 1) % bufferSize;
            int i3 = (i2 + 1) % bufferSize;

            audioBlock[0] += module->sdkCubicInterpolation (x , buffer[im1], buffer[i1], buffer[i2], buffer[i3]) * amp;
        }
        // pitch from -24 to +48
        else
        {
            // linear interpolation
            int i0 = (int)index;
            float x = index - i0;
            int i1 = (i0 + startPosition) % bufferSize;
            int i2 = (i1 + 1) % bufferSize;

            audioBlock[0] += ((x*buffer[i2]) * amp + ((1.0f-x)*buffer[i1]) * amp); 
        }
       
		index += pitchIncrement;
        ++grainIndex;
	}
    
    if (grainIndex >= grainSize)
    {
        isActive = false;
        return;
    }
}
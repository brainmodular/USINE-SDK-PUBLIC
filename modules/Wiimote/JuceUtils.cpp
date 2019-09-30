//-----------------------------------------------------------------------------
//@file  
//	DeviceAudio.cpp
//
//@author
//	Martin FLEURENT aka 'martignasse'
//
//@brief 
//  Utils defs and methodes to glue juce and usine types.
//
//@historic 
//	2013/05/15
//    first release for Hollyhock CPP SDK 6.00.226 
//
//@IMPORTANT
//	This file is part of the Usine Hollyhock CPP SDK
//
//  Please, report bugs and patch to Usine forum :
//  http://www.sensomusic.com/wiki2/doku.php?id=hollyhock:bugsreport 
//
// All dependencies are under there own licence.
//
//@LICENCE
// Copyright (C) 2013 Sensomusic
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy of 
// this software and associated documentation files (the "Software"), 
// to deal in the Software without restriction, including without limitation 
// the rights to use, copy, modify, merge, publish, distribute, sublicense, 
// and/or sell copies of the Software, and to permit persons to whom the Software 
// is furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all 
//     copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
// SOFTWARE.
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------
#include "JuceUtils.h"

//----------------------------------------------------------------------------
// AudioSampleBufferFifo implementation
int AudioSampleBufferFifo::addToFifo (const float** data, int numSamples)
{
    prepareToWrite (numSamples, start1, size1, start2, size2);

	if (size1 > 0)
	{
		for (int i = numChannels; --i >= 0;)
			buffer.copyFrom (i, start1, data[i], size1);
	}

	if (size2 > 0)
	{
		for (int i = numChannels; --i >= 0;)
			buffer.copyFrom (i, start2, data[i] + size1, size2);
	}

    finishedWrite (size1 + size2);
    return size1 + size2;
}

// return the num of samples effectivly read from the buffer
int AudioSampleBufferFifo::delFromFifo ( float** data,int startSample, int numSamples)
{
	prepareToRead (numSamples, start1, size1, start2, size2);

	AudioSampleBuffer outputsBuffer(data, numChannels, startSample, numSamples);

	return read (outputsBuffer, start1, size1, start2, size2);
}

// return the num of samples effectivly read from the buffer
int AudioSampleBufferFifo::delFromFifo ( AudioSampleBuffer &outputsBuffer, int numSamples)
{
	prepareToRead (numSamples, start1, size1, start2, size2);

	return read (outputsBuffer, start1, size1, start2, size2);
}

int AudioSampleBufferFifo::read ( AudioSampleBuffer &outputsBuffer, int start1, int size1, int start2, int size2)
{
	if (size1 > 0)
	{
		for (int i = numChannels; --i >= 0;)
			outputsBuffer.copyFrom (i, 0, buffer, i, start1, size1);
	}

	if (size2 > 0)
	{
		for (int i = numChannels; --i >= 0;)
			outputsBuffer.copyFrom (i, size1, buffer, i, start2, size2);
	}

	finishedRead (size1 + size2);
	return size1 + size2;
}



//-----------------------------------------------------------------------------
//@file  
//	JuceUtils.h
//
//@author
//	Martin FLEURENT aka 'martignasse'
//
//@brief 
//  Utils defs and methodes to glue juce and usine types.
//
//HISTORIC 
//	2013/05/15
//    first release for Hollyhock CPP SDK 6.00.226 
//
//IMPORTANT
//	This file is part of the Usine Hollyhock CPP SDK
//
//  Please, report bugs and patch to Usine forum :
//  support@brainmodular.org 
//
// All dependencies are under there own licence.
//
//@LICENCE
// Copyright (C) 2013, 2014, 2015, 2019 BrainModular
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

// include once, no more
#ifndef __JUCEUTILS_H__
#define __JUCEUTILS_H__

//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------
//#include "../SDK/UserDefinitions.h"  
//#include <iostream>
//#include <cmath>
//#include <limits>
#include "JuceHeader.h"

//-----------------------------------------------------------------------------
// defines and constantes
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// structures and typedef
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// class definition
//-----------------------------------------------------------------------------
//
class AudioSampleBufferFifo : public AbstractFifo
{
public:
	AudioSampleBufferFifo(int numChannels_, int numSamples)  
		: AbstractFifo (numSamples)
		, buffer (numChannels_, numSamples)
		, numChannels (numChannels_)
	{
	}

	// return the num of samples effectivly wrote in the buffer
	int addToFifo (const float** data, int numSamples);

	// return the num of samples effectivly read from the buffer
	int delFromFifo ( float** data,int startSample, int numSamples);

	// return the num of samples effectivly read from the buffer
	int delFromFifo ( AudioSampleBuffer &outputsBuffer, int numSamples);

//-----------------------------------------------------------------------------
private:
	inline int read ( AudioSampleBuffer &outputsBuffer, int start1, int size1, int start2, int size2);

//-----------------------------------------------------------------------------
private:
	int numChannels;
	int start1, size1, start2, size2;
	AudioSampleBuffer buffer;

	JUCE_DECLARE_NON_COPYABLE (AudioSampleBufferFifo)
};

//-------------------------------------------------------------------------
static void  StringArrayToCommaString (String& result, StringArray list, bool noneatfirst = true)
{
	StringArray tmpList(list);
	if (noneatfirst)
		tmpList.insert (0, "none");
		
	result = tmpList.joinIntoString("\",\"").quoted();
};


//-------------------------------------------------------------------------
// crc system

#define CRC_INIT_VALUE 0xffff
#define CRC_XOR_VALUE 0x0000

void CRC_Init(unsigned short int *crcvalue);
void CRC_ProcessByte(unsigned short int *crcvalue, const char *sData, size_t iDataLength);
unsigned short CRC_Value(unsigned short int crcvalue);

#endif //__JUCEUTILS_H__

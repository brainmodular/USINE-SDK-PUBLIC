//	Grain.h - Simple class representing a single grain.
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


#ifndef GRAIN_H_
#define GRAIN_H_

#ifdef WIN32
#define strictinline __forceinline
#elif defined (__GNUC__)
#define strictinline inline __attribute__((always_inline))
#else
#define strictinline inline
#endif

#include "Envelopes.h"
#include <memory>

class GranulatorModule;

///	Simple class representing a single grain.
class Grain
{
  public:
	///	Constructor.
	Grain();

	///	Destructor.
	~Grain();

    enum EnvType
    {
        ENV_ASR,        // Linear Attack/Sustain/Release
        ENV_BEZIER,      // Bezier
        ENV_RCB,         // Raised Cosine Bell 
        ENV_HANN,       // Hanning
        ENV_GAUSS      // Gaussian
    };

	///	Used to set the buffer and it's size in the plugin's constructor.
	/*!
		\param delayLine Pointer to the (mono) buffer where the input audio
		is stored.
		\param size Size of delayLine.
	 */
	void setInputBuffer(TPrecision* inputBuffer, int size);

    void setModule(GranulatorModule* parent);

	///	Call this when the Grain is activated to reset various internal parameters.
	/*!
		\param startPos Position in buffer to start reading from.
		\param size Size of the Grain in samples.
		\param inc The pitchIncrement to use (to change the grain's pitch).
	 */
	void activate( float envVar, int startPos, int size, float inc);
	//void activate(EnvType envType, int startPos, int size, float inc);

    void setEnvelope(EnvType envType);

    void getGraphPoints(EnvType envType, float envVar, TPointF Points[], int NumOfPoints);

	///	Returns whether this block is active or not.
	strictinline bool getIsActive() const {return isActive;};

	///	Returns the next block of audio for this Grain.
	/*!
		\param audioBlock Pointer to a mono array of a block of audio
		samples to write to.
		\param numSamples Number of samples in audioBlock.
	 */
	//void getBlock(float *audioBlock, int numSamples);
	void getSample(TPrecision* audioBlock);
    int getActualPos() {return (startPosition + static_cast<int>(index)) % bufferSize;};

  private:
    GranulatorModule* module;

    Envelope* currentEnvelope;

    static std::auto_ptr<Envelope> asrEnvelope;
    static std::auto_ptr<Envelope> rcbEnvelope;
    static std::auto_ptr<Envelope> bezierEnvelope;
    static std::auto_ptr<Envelope> hanningEnvelope;
    static std::auto_ptr<Envelope> gaussianEnvelope;
   // static Envelope* gaussEnvelope;

	///	Whether the grain is active or not.
	bool isActive;

	///	Pointer to the (mono) buffer where the input audio is stored.
	TPrecision* buffer;

	///	Size of the buffer.
	int bufferSize;

	///	Current index in the buffer.
	float index;

	///	current read position in the grain buffer.
	float grainIndex;

	///	Increment controlling how fast the buffer is played back.
	float pitchIncrement;

	///	Start point in the buffer.
	int startPosition;

	///	Size of the current Grain.
	int grainSize;

};

#endif

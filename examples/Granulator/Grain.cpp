//-----------------------------------------------------------------------------
// @file    Grain.cpp
// @author  Niall Moody, adapted by BrainModular team
// @brief   A single grain, reads the delay line of the Granulator through an envelope.
//
// This file is part of the Usine Hollyhock CPP SDK.
// Adapted from the Fragmental VST plugin, see the original notice below.
// Copyright (C) 2013-2026 BrainModular, BeSpline, Adamson
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
// Original notice:
//
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
//-----------------------------------------------------------------------------
#include <array>
#include <cmath>
#include "../../sdk/UserDefinitions.h"
#include "Grain.h"

namespace
{
	// one shared envelope per type, only used to compute the painted curves
	Envelope& graphEnvelope(EnvelopeType type)
	{
		static const std::array<std::unique_ptr<Envelope>, ENVELOPE_TYPE_COUNT> envelopes = {
			makeEnvelope(EnvelopeType::Asr, 10000),
			makeEnvelope(EnvelopeType::Bezier, 10000),
			makeEnvelope(EnvelopeType::RaisedCosineBell, 10000),
			makeEnvelope(EnvelopeType::Hanning, 10000),
			makeEnvelope(EnvelopeType::Gaussian, 10000)
		};
		return *envelopes[static_cast<size_t>(type)];
	}
}

void Grain::setInputBuffer(std::span<TPrecision> inputBuffer)
{
	buffer = inputBuffer;
}

void Grain::setEnvelope(EnvelopeType type)
{
	currentEnvelope = makeEnvelope(type, grainSize);
}

void Grain::getGraphPoints(EnvelopeType type, float envVar, std::span<TPointF> points)
{
	Envelope& envelope = graphEnvelope(type);
	envelope.setVariables(envVar);
	envelope.getGraphPoints(points);
}

void Grain::activate(float envVar, int startPos, int size, float inc)
{
	pitchIncrement = inc;
	grainSize      = size;
	currentEnvelope->setSize(grainSize);
	currentEnvelope->setVariables(envVar);

	// a pitched up grain reads faster than the delay line is written:
	// it starts earlier so it never passes the write position
	const int bufferSize = static_cast<int>(buffer.size());
	startPosition = startPos;
	if (pitchIncrement > 1.0f && bufferSize > 0)
	{
		startPosition -= static_cast<int>((pitchIncrement - 1.0f) * grainSize);
		startPosition  = ((startPosition % bufferSize) + bufferSize) % bufferSize;
	}

	index      = 0.0f;
	grainIndex = 0.0f;
	isActive   = true;
}

void Grain::addSampleTo(TPrecision& sample)
{
	if (!isActive)
		return;

	float amp = 0.0f;
	currentEnvelope->processEnvelope(static_cast<int>(grainIndex), amp);

	const int bufferSize = static_cast<int>(buffer.size());
	const int i0         = static_cast<int>(index);
	if (pitchIncrement == 1.0f)
	{
		// original pitch: no interpolation
		sample += buffer[(startPosition + i0) % bufferSize] * amp;
	}
	else if (pitchIncrement <= 0.25f)
	{
		// pitch from -48 to -24 semitones: cubic interpolation
		const float x   = index - i0;
		const int   im1 = (i0 + startPosition - 1 + bufferSize) % bufferSize;
		const int   i1  = (i0 + startPosition) % bufferSize;
		const int   i2  = (i1 + 1) % bufferSize;
		const int   i3  = (i2 + 1) % bufferSize;
		sample += sdkCubicInterpolation(x, buffer[im1], buffer[i1], buffer[i2], buffer[i3]) * amp;
	}
	else
	{
		// pitch from -24 to +48 semitones: linear interpolation
		const float x  = index - i0;
		const int   i1 = (i0 + startPosition) % bufferSize;
		const int   i2 = (i1 + 1) % bufferSize;
		sample += std::lerp(buffer[i1], buffer[i2], x) * amp;
	}

	index += pitchIncrement;
	++grainIndex;

	if (grainIndex >= grainSize)
		isActive = false;
}

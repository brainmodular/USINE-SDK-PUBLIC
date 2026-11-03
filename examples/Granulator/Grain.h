//-----------------------------------------------------------------------------
// @file    Grain.h
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
#pragma once

#include <memory>
#include <span>
#include "Envelopes.h"

class Grain
{
public:
	/// Sets the (mono) buffer where the input audio is stored.
	void setInputBuffer(std::span<TPrecision> inputBuffer);

	/// Resets the grain and starts it.
	/// @param envVar The envelope variable.
	/// @param startPos Position in the buffer to start reading from.
	/// @param size Size of the grain in samples.
	/// @param inc The pitch increment (1 = original pitch).
	void activate(float envVar, int startPos, int size, float inc);

	void setEnvelope(EnvelopeType type);

	/// Fills points with the shape of an envelope, to paint it.
	static void getGraphPoints(EnvelopeType type, float envVar, std::span<TPointF> points);

	[[nodiscard]] bool getIsActive() const { return isActive; }

	/// Adds the next sample of this grain to sample.
	void addSampleTo(TPrecision& sample);

private:
	std::unique_ptr<Envelope> currentEnvelope = makeEnvelope(EnvelopeType::Asr, 10);
	std::span<TPrecision>     buffer;

	bool  isActive       = false;
	// read position relative to startPosition, moves by pitchIncrement each sample
	float index          = 0.0f;
	// position in the envelope, moves by 1 each sample
	float grainIndex     = 0.0f;
	float pitchIncrement = 1.0f;
	int   startPosition  = 0;
	int   grainSize      = 0;
};

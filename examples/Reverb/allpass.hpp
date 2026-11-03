//-----------------------------------------------------------------------------
// @file    allpass.hpp
// @author  Jezar at Dreampoint, adapted by BrainModular team
// @brief   Allpass filter of the Freeverb reverb model.
//
// This file is part of the Usine Hollyhock CPP SDK.
// Based on Freeverb, written by Jezar at Dreampoint in June 2000 and placed in the public domain.
// Copyright (C) 2013-2026 BrainModular, BeSpline, Adamson
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
#pragma once

#include <span>
#include "denormals.h"

class allpass
{
public:
	void  setbuffer(std::span<float> buf);
	float process(float input);
	void  mute();
	void  setfeedback(float val);
	[[nodiscard]] float getfeedback() const;

private:
	float            feedback = 0.0f;
	std::span<float> buffer;
	size_t           bufidx = 0;
};

// inlined because it is called for every sample
inline float allpass::process(float input)
{
	float bufout = buffer[bufidx];
	undenormalise(bufout);

	const float output = -input + bufout;
	buffer[bufidx]     = input + bufout * feedback;

	if (++bufidx >= buffer.size())
		bufidx = 0;

	return output;
}

//-----------------------------------------------------------------------------
// @file    comb.hpp
// @author  Jezar at Dreampoint, adapted by BrainModular team
// @brief   Lowpass feedback comb filter of the Freeverb reverb model.
//
// This file is part of the Usine Hollyhock CPP SDK.
// Based on Freeverb, written by Jezar at Dreampoint in June 2000 and placed in the public domain.
// Copyright (C) 2013-2026 BrainModular, BeSpline, Adamson
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
#pragma once

#include <span>
#include "denormals.h"

class comb
{
public:
	void  setbuffer(std::span<float> buf);
	float process(float input);
	void  mute();
	void  setdamp(float val);
	[[nodiscard]] float getdamp() const;
	void  setfeedback(float val);
	[[nodiscard]] float getfeedback() const;

private:
	float            feedback    = 0.0f;
	float            filterstore = 0.0f;
	float            damp1       = 0.0f;
	float            damp2       = 0.0f;
	std::span<float> buffer;
	size_t           bufidx      = 0;
};

// inlined because it is called for every sample
inline float comb::process(float input)
{
	float output = buffer[bufidx];
	undenormalise(output);

	filterstore = output * damp2 + filterstore * damp1;
	undenormalise(filterstore);

	buffer[bufidx] = input + filterstore * feedback;

	if (++bufidx >= buffer.size())
		bufidx = 0;

	return output;
}

//-----------------------------------------------------------------------------
// @file    comb.cpp
// @author  Jezar at Dreampoint, adapted by BrainModular team
// @brief   Lowpass feedback comb filter of the Freeverb reverb model.
//
// This file is part of the Usine Hollyhock CPP SDK.
// Based on Freeverb, written by Jezar at Dreampoint in June 2000 and placed in the public domain.
// Copyright (C) 2013-2026 BrainModular, BeSpline, Adamson
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
#include <algorithm>
#include "comb.hpp"

void comb::setbuffer(std::span<float> buf)
{
	buffer = buf;
	bufidx = 0;
}

void comb::mute()
{
	std::ranges::fill(buffer, 0.0f);
}

void comb::setdamp(float val)
{
	damp1 = val;
	damp2 = 1.0f - val;
}

float comb::getdamp() const
{
	return damp1;
}

void comb::setfeedback(float val)
{
	feedback = val;
}

float comb::getfeedback() const
{
	return feedback;
}

//-----------------------------------------------------------------------------
// @file    allpass.cpp
// @author  Jezar at Dreampoint, adapted by BrainModular team
// @brief   Allpass filter of the Freeverb reverb model.
//
// This file is part of the Usine Hollyhock CPP SDK.
// Based on Freeverb, written by Jezar at Dreampoint in June 2000 and placed in the public domain.
// Copyright (C) 2013-2026 BrainModular, BeSpline, Adamson
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
#include <algorithm>
#include "allpass.hpp"

void allpass::setbuffer(std::span<float> buf)
{
	buffer = buf;
	bufidx = 0;
}

void allpass::mute()
{
	std::ranges::fill(buffer, 0.0f);
}

void allpass::setfeedback(float val)
{
	feedback = val;
}

float allpass::getfeedback() const
{
	return feedback;
}

//-----------------------------------------------------------------------------
// @file    denormals.h
// @author  Jezar at Dreampoint, adapted by BrainModular team
// @brief   Flushes denormal floats to zero, they are very slow to process.
//
// This file is part of the Usine Hollyhock CPP SDK.
// Based on Freeverb, written by Jezar at Dreampoint in June 2000 and placed in
// the public domain, itself based on the IS_DENORMAL macro by Jon Watte.
// Copyright (C) 2013-2026 BrainModular, BeSpline, Adamson
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
#pragma once

#include <cstdint>
#include <cstring>

inline void undenormalise(float& sample)
{
	std::uint32_t bits;
	std::memcpy(&bits, &sample, sizeof(bits));
	if ((bits & 0x7f800000) == 0)
		sample = 0.0f;
}

//-----------------------------------------------------------------------------
// @file    tuning.h
// @author  Jezar at Dreampoint, adapted by BrainModular team
// @brief   Tuning values of the Freeverb reverb model.
//
// This file is part of the Usine Hollyhock CPP SDK.
// Based on Freeverb, written by Jezar at Dreampoint in June 2000 and placed in the public domain.
// Copyright (C) 2013-2026 BrainModular, BeSpline, Adamson
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
#pragma once

constexpr int   numcombs     = 8;
constexpr int   numallpasses = 4;
constexpr float muted        = 0.0f;
constexpr float fixedgain    = 0.015f;
constexpr float freezemode   = 0.5f;
// offset of the comb and allpass sizes between two channels, decorrelates them
constexpr int   stereospread = 23;
// smoothing of the wet and dry levels, per bloc
constexpr float smooth       = 0.97f;

// sizes in samples of the comb and allpass buffers, obtained by listening tests.
// They assume a 44.1 kHz sample rate, they are fine at 48 kHz but should be scaled for higher rates.
constexpr int combtuning[numcombs]        = { 1116, 1188, 1277, 1356, 1422, 1491, 1557, 1617 };
constexpr int allpasstuning[numallpasses] = { 556, 441, 341, 225 };

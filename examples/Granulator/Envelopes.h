//-----------------------------------------------------------------------------
// @file    Envelopes.h
// @author  BrainModular team
// @brief   Amplitude envelopes of the Granulator grains.
//
// This file is part of the Usine Hollyhock CPP SDK.
// Copyright (C) 2013-2026 BrainModular, BeSpline, Adamson
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
#pragma once

#include <memory>
#include <span>
#include "../../sdk/UsineDefinitions.h"

constexpr float ENV_VAR_MIN = 0.0002f;
constexpr float ENV_VAR_MAX = 1.0f;

// the order must match the items of the grain env list box
enum class EnvelopeType
{
	Asr,              // Linear Attack/Sustain/Release
	Bezier,
	RaisedCosineBell,
	Hanning,
	Gaussian
};
constexpr int ENVELOPE_TYPE_COUNT = 5;

// envelopeVar (0..1) shapes the envelope, its meaning depends on the envelope type
class Envelope
{
public:
	virtual ~Envelope() = default;

	virtual void setVariables(float skew) { envelopeVar = skew; }
	virtual void getGraphPoints(std::span<TPointF> points) {}

	/// Computes the amplitude at the sample index of the grain.
	virtual void processEnvelope(int index, float& amp) = 0;

	void setSize(int size) { envelopeSize = size; }

protected:
	int   envelopeSize = 0;
	float envelopeVar  = 0.0f;
};

class AsrEnvelope : public Envelope
{
public:
	explicit AsrEnvelope(int size);

	void setVariables(float skew) override;
	void getGraphPoints(std::span<TPointF> points) override;
	void processEnvelope(int index, float& amp) override;

private:
	float asrAttackSize     = 0.0f;
	float asrSustainSize    = 0.0f;
	float asrInvAttackSize  = 0.0f;
	float asrInvSustainSize = 0.0f;
};

class RcbEnvelope : public Envelope
{
public:
	explicit RcbEnvelope(int size);

	void setVariables(float skew) override;
	void getGraphPoints(std::span<TPointF> points) override;
	void processEnvelope(int index, float& amp) override;

private:
	float rcbSectionSize = 0.0f;
	float angleIncrement = 0.0f;
};

class BezierEnvelope : public Envelope
{
public:
	explicit BezierEnvelope(int size);

	void setVariables(float skew) override;
	void getGraphPoints(std::span<TPointF> points) override;
	void processEnvelope(int index, float& amp) override;

private:
	float left0  = 0.0f;
	float left1  = 0.0f;
	float left2  = 0.0f;
	float center = 0.0f;
};

class GaussianEnvelope : public Envelope
{
public:
	explicit GaussianEnvelope(int size);

	void setVariables(float skew) override;
	void getGraphPoints(std::span<TPointF> points) override;
	void processEnvelope(int index, float& amp) override;

private:
	float gaussMax    = 0.0f;
	float gaussSqrDev = 0.0f;
};

class HanningEnvelope : public Envelope
{
public:
	explicit HanningEnvelope(int size);

	void setVariables(float skew) override;
	void getGraphPoints(std::span<TPointF> points) override;
	void processEnvelope(int index, float& amp) override;
};

std::unique_ptr<Envelope> makeEnvelope(EnvelopeType type, int size);

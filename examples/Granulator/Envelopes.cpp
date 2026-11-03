//-----------------------------------------------------------------------------
// @file    Envelopes.cpp
// @author  BrainModular team
// @brief   Amplitude envelopes of the Granulator grains.
//
// This file is part of the Usine Hollyhock CPP SDK.
// Copyright (C) 2013-2026 BrainModular, BeSpline, Adamson
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
#include <algorithm>
#include <cmath>
#include <numbers>
#include "../../sdk/UserUtils.h"
#include "Envelopes.h"

namespace
{
	constexpr float pi = std::numbers::pi_v<float>;

	// parabolic approximation of cos(angle), angle in [-pi, pi]
	float fastCosine(float angle)
	{
		angle += pi / 2.0f;
		if (angle > pi)
			angle -= 2.0f * pi;

		if (angle < 0)
			return 1.273239545f * angle + 0.405284735f * angle * angle;
		return 1.273239545f * angle - 0.405284735f * angle * angle;
	}
}

AsrEnvelope::AsrEnvelope(int size)
{
	setSize(size);
	setVariables(1.0f);
}

void AsrEnvelope::setVariables(float skew)
{
	envelopeVar = skew;
	if (envelopeVar <= ENV_VAR_MIN)
	{
		asrAttackSize     = 0.0f;
		asrSustainSize    = static_cast<float>(envelopeSize);
		asrInvAttackSize  = 10.0f; // not used when asrAttackSize is 0
		asrInvSustainSize = 1.0f / asrSustainSize;
	}
	else
	{
		asrAttackSize     = std::max(0.0001f, (static_cast<float>(envelopeSize) / 2.0f) * skew);
		asrSustainSize    = std::max(0.0001f, static_cast<float>(envelopeSize) - asrAttackSize);
		asrInvAttackSize  = 1.0f / asrAttackSize;
		asrInvSustainSize = 1.0f / asrSustainSize;
	}
}

void AsrEnvelope::getGraphPoints(std::span<TPointF> points)
{
	const int numOfPoints = static_cast<int>(points.size());
	if (numOfPoints != 4)
		return;

	float amp = 0.0f;
	points[0].x = 0.0f;
	points[0].y = 1.0f;

	processEnvelope(static_cast<int>(asrAttackSize + 1), amp);
	points[1].x = (asrAttackSize - 1.0f) / static_cast<float>(envelopeSize);
	points[1].y = 1.0f - amp;

	processEnvelope(static_cast<int>(asrSustainSize - 1), amp);
	points[2].x = (asrSustainSize - 1.0f) / static_cast<float>(envelopeSize);
	points[2].y = 1.0f - amp;

	points[3].x = 0.999f;
	points[3].y = 1.0f;
}

void AsrEnvelope::processEnvelope(int index, float& amp)
{
	if (index < asrAttackSize)
		amp = index * asrInvAttackSize;
	else if (index >= envelopeSize)
		amp = 0.0f;
	else if (index > asrSustainSize)
		amp = (envelopeSize - index) * asrInvAttackSize;
	else
		amp = 1.0f;
}

RcbEnvelope::RcbEnvelope(int size)
{
	setSize(size);
	setVariables(1.0f);
}

void RcbEnvelope::setVariables(float skew)
{
	envelopeVar    = skew;
	rcbSectionSize = (static_cast<float>(envelopeSize) / 2.0f) * envelopeVar;
	angleIncrement = pi / rcbSectionSize;
}

void RcbEnvelope::getGraphPoints(std::span<TPointF> points)
{
	const int numOfPoints = static_cast<int>(points.size());
	float       amp      = 0.0f;
	const float tmpSize  = numOfPoints - 2.0f;
	const int   halfSize = static_cast<int>(tmpSize) / 2;

	points[0].x = 0.0f;
	points[0].y = 0.999f;

	for (int i = 1; i <= halfSize; i++)
	{
		const float ratio = static_cast<float>(i) / static_cast<float>(halfSize);
		processEnvelope(static_cast<int>(ratio * rcbSectionSize), amp);
		points[i].x = (static_cast<float>(i) / tmpSize) * envelopeVar;
		points[i].y = 1.0f - amp;
	}

	points[halfSize + 1].x = 0.5f + (1.0f - envelopeVar) * 0.499f;
	points[halfSize + 1].y = 0.0f;

	for (int i = 1; i < halfSize; i++)
	{
		const float ratio = static_cast<float>(i) / static_cast<float>(halfSize);
		processEnvelope(static_cast<int>((envelopeSize - rcbSectionSize) + ratio * rcbSectionSize), amp);
		points[halfSize + 1 + i].x = 0.5f + (1.0f - envelopeVar) * 0.499f + (static_cast<float>(i) / tmpSize) * envelopeVar;
		points[halfSize + 1 + i].y = 1.0f - amp;
	}

	points[numOfPoints - 1].x = 0.999f;
	points[numOfPoints - 1].y = 0.999f;
}

void RcbEnvelope::processEnvelope(int index, float& amp)
{
	if (index < rcbSectionSize)
	{
		const float attackAngle = pi + angleIncrement * index;
		amp = 0.5f + 0.5f * fastCosine(attackAngle);
	}
	else if (index >= envelopeSize)
	{
		amp = 0.0f;
	}
	else if (index >= envelopeSize - rcbSectionSize)
	{
		const float releaseAngle = (index - (envelopeSize - rcbSectionSize)) * angleIncrement;
		amp = 0.5f + 0.5f * fastCosine(releaseAngle);
	}
	else
	{
		amp = 1.0f;
	}
}

BezierEnvelope::BezierEnvelope(int size)
{
	setSize(size);
}

void BezierEnvelope::setVariables(float skew)
{
	envelopeVar = skew;
	left0       = 0.0f;
	center      = 1.0f;

	if (skew < 0.5f)
	{
		left1 = 0.0f;
		left2 = envelopeVar * 2.0f;
	}
	else
	{
		left1 = (envelopeVar - 0.5f) * 2.0f;
		left2 = 1.0f;
	}
}

void BezierEnvelope::getGraphPoints(std::span<TPointF> points)
{
	const int numOfPoints = static_cast<int>(points.size());
	float amp = 0.0f;
	for (int i = 0; i < numOfPoints - 1; i++)
	{
		const float ratio = static_cast<float>(i) / static_cast<float>(numOfPoints - 1);
		processEnvelope(static_cast<int>(ratio * envelopeSize), amp);
		points[i].x = ratio;
		points[i].y = 1.0f - amp;
	}
	points[numOfPoints - 1].x = 0.999f;
	points[numOfPoints - 1].y = 1.0f;
}

void BezierEnvelope::processEnvelope(int index, float& amp)
{
	if (index >= envelopeSize)
	{
		amp = 0.0f;
		return;
	}

	// symmetric envelope: the second half mirrors the first one
	float fraction = static_cast<float>(index) / static_cast<float>(envelopeSize);
	if (fraction >= 0.5f)
		fraction = 1.0f - fraction;

	fraction *= 2.0f;
	fraction  = std::sqrt(fraction) * envelopeVar + (fraction * fraction) * (1.0f - envelopeVar);
	amp       = sdkBezier4(left0, left1, left2, center, fraction);
}

GaussianEnvelope::GaussianEnvelope(int size)
{
	setSize(size);
}

void GaussianEnvelope::setVariables(float skew)
{
	envelopeVar = skew;
	gaussMax    = envelopeSize / 2.0f;
	const float deviation = envelopeSize / (4.9f + 10.0f * envelopeVar);
	gaussSqrDev = deviation * deviation;
}

void GaussianEnvelope::getGraphPoints(std::span<TPointF> points)
{
	const int numOfPoints = static_cast<int>(points.size());
	float amp = 0.0f;
	for (int i = 0; i < numOfPoints - 1; i++)
	{
		const float ratio = static_cast<float>(i) / static_cast<float>(numOfPoints);
		processEnvelope(static_cast<int>(ratio * envelopeSize), amp);
		points[i].x = ratio;
		points[i].y = 1.0f - amp;
	}
	points[numOfPoints - 1].x = 0.9999f;
	points[numOfPoints - 1].y = 0.9999f;
}

void GaussianEnvelope::processEnvelope(int index, float& amp)
{
	if (index >= envelopeSize)
	{
		amp = 0.0f;
		return;
	}

	const float x = index - gaussMax;
	amp = std::exp(-(x * x / gaussSqrDev));
}

HanningEnvelope::HanningEnvelope(int size)
{
	setSize(size);
}

void HanningEnvelope::setVariables(float skew)
{
	envelopeVar = skew;
}

void HanningEnvelope::getGraphPoints(std::span<TPointF> points)
{
	const int numOfPoints = static_cast<int>(points.size());
	float amp = 0.0f;
	for (int i = 0; i < numOfPoints - 1; i++)
	{
		const float ratio = static_cast<float>(i) / static_cast<float>(numOfPoints);
		processEnvelope(static_cast<int>(ratio * envelopeSize), amp);
		points[i].x = ratio;
		points[i].y = 1.0f - amp;
	}
	points[numOfPoints - 1].x = 0.9999f;
	points[numOfPoints - 1].y = 0.9999f;
}

void HanningEnvelope::processEnvelope(int index, float& amp)
{
	if (index >= envelopeSize)
	{
		amp = 0.0f;
		return;
	}

	amp = 0.5f * (1.0f - fastCosine(2.0f * pi * static_cast<float>(index) / envelopeSize));
}

std::unique_ptr<Envelope> makeEnvelope(EnvelopeType type, int size)
{
	switch (type)
	{
		case EnvelopeType::Bezier:
			return std::make_unique<BezierEnvelope>(size);
		case EnvelopeType::RaisedCosineBell:
			return std::make_unique<RcbEnvelope>(size);
		case EnvelopeType::Hanning:
			return std::make_unique<HanningEnvelope>(size);
		case EnvelopeType::Gaussian:
			return std::make_unique<GaussianEnvelope>(size);
		case EnvelopeType::Asr:
		default:
			return std::make_unique<AsrEnvelope>(size);
	}
}

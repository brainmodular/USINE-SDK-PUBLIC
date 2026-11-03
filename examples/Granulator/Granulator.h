//-----------------------------------------------------------------------------
// @file    Granulator.h
// @author  BrainModular team
// @brief   Granular synthesis on live audio inputs, with automation recording.
//          Inspired from the granulator of the Fragmental VST plugin by Niall Moody.
//
// This file is part of the Usine Hollyhock CPP SDK.
// Copyright (C) 2013-2026 BrainModular, BeSpline, Adamson
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
#pragma once

#include <array>
#include <string>
#include <vector>
#include "../../sdk/UserDefinitions.h"
#include "Grain.h"

constexpr int NUM_OF_GRAIN = 16;

// number of points of the painted curve, for each EnvelopeType
constexpr std::array<int, ENVELOPE_TYPE_COUNT> ENVELOPE_GRAPH_POINTS = { 4, 61, 40, 61, 61 };

constexpr float GRAIN_DUR_MIN   = 0.05f;
constexpr float GRAIN_DUR_MAX   = 2000.0f;
constexpr float GRAIN_PITCH_MIN = -48.0f;
constexpr float GRAIN_PITCH_MAX = 48.0f;

// the order must match the items of the grain mode list box
enum class GrainMode
{
	Interval,
	PlayButton
};

class Granulator : public UserModuleBase
{
public:
	void onGetModuleInfo(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo) override;
	void onAfterQuery(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo, int queryResult1, int queryResult2) override;
	void onInitModule(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo) override;
	void onCreateParameters(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo) override;
	void onCallBack(TUsineMessage* Message) override;
	void onProcess() override;

	void onCreateSettings() override;
	void onSettingsHasChanged() override;
	void onPaint() override;

	void onMouseMove(TShiftState Shift, float X, float Y) override;
	void onMouseDown(TMouseButton MouseButton, TShiftState Shift, float X, float Y) override;
	void onMouseUp(TMouseButton MouseButton, TShiftState Shift, float X, float Y) override;

	void onSampleRateChange(double SampleRate) override;
	void onSetRecordedValue(TPrecision X, TPrecision Y, TPrecision Z) override;

private:
	using AudioEvents  = std::array<UsineEventClass, USINE_AUDIO_CHANNEL_MODULE_MAX>;
	using ChannelFlags = std::array<bool, USINE_AUDIO_CHANNEL_MODULE_MAX>;

	AudioEvents     audioInputs;
	AudioEvents     audioOutputs;
	UsineEventClass grainStartSwOut;
	UsineEventClass grainIntervalFdrIn;
	UsineEventClass grainEnvelopeLboxIn;
	UsineEventClass grainEnvVarFdrIn;
	UsineEventClass grainDurationFdrIn;
	UsineEventClass grainPitchFdrIn;
	UsineEventClass grainModeLboxIn;
	UsineEventClass grainGoBtnIn;

	int queryResult       = 0;
	int numOfAudioInsOuts = 2;

	// settings of the 'edit layout' panel
	float       lineThickness = 2.0f;
	TUsineColor lineColor     = 0;

	// points of the envelope curves painted in the panel, one curve per EnvelopeType
	std::array<std::vector<TPointF>, ENVELOPE_TYPE_COUNT> envelopePoints;

	TRectF      dimPitch         { .left = 0.25f, .top = 0.5f, .right = 0.75f, .bottom = 0.75f };
	TRectF      dimGrainDuration { .left = 0.25f, .top = 0.75f, .right = 0.75f, .bottom = 1.0f };
	std::string caPitch;
	std::string caGrainDuration;

	float usineSampleRate = 0.0f;

	// one delay line per channel, the grains read in it
	std::array<std::vector<TPrecision>, USINE_AUDIO_CHANNEL_MODULE_MAX> circularBuffer;
	int                                                                 cbufSize    = 0;
	int                                                                 cbufReadPos = 0;

	// NUM_OF_GRAIN grains per channel, reused in a round robin
	std::vector<std::array<Grain, NUM_OF_GRAIN>> grains;
	int                                          nextNewGrainIndex  = 0;
	bool                                         grainOverlapWarned = false;

	EnvelopeType currentEnvelopeType = EnvelopeType::Asr;
	GrainMode    currentGrainMode    = GrainMode::Interval;
	float        grainEnvelopeVar    = 0.0f;
	float        pitchIncrement      = 1.0f;
	float        grainDurationMs     = 100.0f;
	float        grainIntervalMs     = 0.0f;
	// grainSize, grainInterval and audioLineNextOnset are in samples
	int          grainSize           = 4410;
	int          grainInterval       = 0;
	int          audioLineNextOnset  = 0;
	bool         wantNewGrain        = false;

	bool    isRecording         = false;
	bool    mouseLeftButtonDown = false;
	TPointF mouseLastCoords     = {};

	void changeGrainEnvelope();
	void changeEnvelopeVar();
	void changeInterval();
	void changeDuration();
	void changePitch();
	void changeGrainMode();
	void startNewGrain(const ChannelFlags& isConnected);

	[[nodiscard]] int                   msToSamples(float ms) const;
	[[nodiscard]] std::vector<TPointF>& currentEnvelopePoints();
};

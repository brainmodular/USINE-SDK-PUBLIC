//-----------------------------------------------------------------------------
// @file    Reverb.h
// @author  BrainModular team
// @brief   Multichannel reverb based on the Freeverb model, with cross-channel feeds.
//
// This file is part of the Usine Hollyhock CPP SDK.
// Copyright (C) 2013-2026 BrainModular, BeSpline, Adamson
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
#pragma once

#include <array>
#include <vector>
#include "../../sdk/UserDefinitions.h"
#include "allpass.hpp"
#include "comb.hpp"
#include "tuning.h"

class Reverb : public UserModuleBase
{
public:
	~Reverb() override;

	void onGetModuleInfo(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo) override;
	void onAfterQuery(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo, int queryResult1, int queryResult2) override;
	void onInitModule(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo) override;
	void onCreateParameters(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo) override;
	void onCallBack(TUsineMessage* Message) override;
	void onProcess() override;

private:
	using AudioEvents = std::array<UsineEventClass, USINE_AUDIO_CHANNEL_MODULE_MAX>;

	AudioEvents     audioInputs;
	AudioEvents     audioOutputs;
	UsineEventClass modeSwIn;
	UsineEventClass roomSizeFdrIn;
	UsineEventClass dampingFdrIn;
	UsineEventClass channelsSeparationFdrIn;
	UsineEventClass wetLevelFdrIn;
	UsineEventClass dryLevelFdrIn;

	// work buffers created with createEvent, destroyed in the destructor
	AudioEvents     temp;
	AudioEvents     processInput;
	AudioEvents     processOutput;

	int queryResult       = 0;
	int numOfAudioInsOuts = 2;

	// parameters of the model, see reverbParamsUpdate
	float mode               = 0.0f;
	float roomsize           = 0.0f;
	float damp               = 0.0f;
	float channelsSeparation = 0.0f;
	float wet                = 0.0f;
	float dry                = 0.0f;
	float gain               = 0.0f;
	float roomsize1          = 0.0f;
	float damp1              = 0.0f;
	// wet1 feeds a channel with its own reverb, wet2 with the reverb of the other channels
	float wet1               = 0.0f;
	float wet2               = 0.0f;
	// smoothed values used by onProcess
	float currentwet1        = 0.0f;
	float currentwet2        = 0.0f;
	float currentdry         = 0.0f;

	// numcombs combs and numallpasses allpasses per channel, with their buffers
	std::vector<comb>                    combvoices;
	std::vector<allpass>                 allpassvoices;
	std::vector<std::vector<float>>      bufcombvoices;
	std::vector<std::vector<float>>      bufallpassvoices;

	void reverbInit();
	void reverbParamsUpdate();
};

//-----------------------------------------------------------------------------
// @file    AudioVolume.h
// @author  BrainModular team
// @brief   Gain and mute applied to a user-chosen number of audio channels.
//
// This file is part of the Usine Hollyhock CPP SDK.
// Copyright (C) 2013-2026 BrainModular, BeSpline, Adamson
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
#pragma once

#include <array>
#include "../../sdk/UserDefinitions.h"

class AudioVolume : public UserModuleBase
{
public:
	void onGetModuleInfo(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo) override;
	void onAfterQuery(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo, int queryResult1, int queryResult2) override;
	void onCreateParameters(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo) override;
	void onCallBack(TUsineMessage* Message) override;
	void onProcess() override;

private:
	using AudioEvents = std::array<UsineEventClass, USINE_AUDIO_CHANNEL_MODULE_MAX>;

	AudioEvents     audioInputs;
	AudioEvents     audioOutputs;
	UsineEventClass gainFdrIn;
	UsineEventClass muteSwIn;

	int        queryResult       = 0;
	int        numOfAudioInsOuts = 0;
	TPrecision coeffGain         = 1.0f;
};

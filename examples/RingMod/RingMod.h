//-----------------------------------------------------------------------------
// @file    RingMod.h
// @author  BrainModular team
// @brief   Multiplies audio inputs by a modulator, shows how to process audio buffers.
//
// This file is part of the Usine Hollyhock CPP SDK.
// Copyright (C) 2015-2026 BrainModular, BeSpline, Adamson
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
#pragma once

#include <array>
#include "../../sdk/UserDefinitions.h"

class RingMod : public UserModuleBase
{
public:
	~RingMod() override;

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
	UsineEventClass modAudioIn;
	UsineEventClass mixFdrIn;

	// work buffer created with createEvent, destroyed in the destructor
	UsineEventClass bufferTemp;

	int        queryResult       = 0;
	int        numOfAudioInsOuts = 0;
	TPrecision coeffMix          = 0.5f;
};

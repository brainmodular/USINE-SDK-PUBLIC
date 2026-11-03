//-----------------------------------------------------------------------------
// @file    MidiTranspose.h
// @author  BrainModular team
// @brief   Transposes midi notes, shows how to process midi messages.
//
// This file is part of the Usine Hollyhock CPP SDK.
// Copyright (C) 2015-2026 BrainModular, BeSpline, Adamson
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
#pragma once

#include "../../sdk/UserDefinitions.h"

class MidiTranspose : public UserModuleBase
{
public:
	void onGetModuleInfo(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo) override;
	void onCreateParameters(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo) override;
	void onProcess() override;

private:
	UsineEventClass midiIn;
	UsineEventClass midiOut;
	UsineEventClass pitchFdrIn;
};

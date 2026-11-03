//-----------------------------------------------------------------------------
// @file    VideoDimmer.h
// @author  Olivier Sens (senso), BrainModular team
// @brief   Dims a video and draws a grid on it, shows how to process video frames.
//
// This file is part of the Usine Hollyhock CPP SDK.
// Copyright (C) 2015-2026 BrainModular, BeSpline, Adamson
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
#pragma once

#include "../../sdk/UserDefinitions.h"

class VideoDimmer : public UserModuleBase
{
public:
	void onGetModuleInfo(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo) override;
	void onInitModule(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo) override;
	void onCreateParameters(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo) override;
	void onProcessVideo() override;

private:
	UsineEventClass dimmerFdrIn;
	UsineEventClass pixelColIn;
	UsineEventClass spaceFdrIn;

	TUsineFrame frameIn;
	TUsineFrame frameOut;
};

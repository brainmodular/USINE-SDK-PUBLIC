//-----------------------------------------------------------------------------
// @file    ArrayDistance.h
// @author  BrainModular team
// @brief   Dynamic time warping (DTW) distance between two arrays.
//
// This file is part of the Usine Hollyhock CPP SDK.
// Copyright (C) 2013-2026 BrainModular, BeSpline, Adamson
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
#pragma once

#include "../../sdk/UserDefinitions.h"

class ArrayDistance : public UserModuleBase
{
public:
	void onGetModuleInfo(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo) override;
	void onCreateParameters(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo) override;
	void onCallBack(TUsineMessage* Message) override;

private:
	UsineEventClass array1ArrIn;
	UsineEventClass array2ArrIn;
	UsineEventClass distanceFdrOut;
	UsineEventClass minFdrIn;
	UsineEventClass maxFdrIn;
	UsineEventClass calculateBtnIn;

	void calculateDistance();
};

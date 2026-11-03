//-----------------------------------------------------------------------------
// @file    UsineChunks.h
// @author  BrainModular team
// @brief   Shares data with other modules through a Usine chunk and its notifications.
//
// This file is part of the Usine Hollyhock CPP SDK.
// Copyright (C) 2025-2026 BrainModular, BeSpline, Adamson
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
#pragma once

#include "../../sdk/UserDefinitions.h"
#include <vector>
#include <array>


class UsineChunks : public UserModuleBase
{
public:
	~UsineChunks() override;

	void onGetModuleInfo(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo) override;
	void onInitModule(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo) override;
	void onCreateParameters(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo) override;
	void onCallBack(TUsineMessage* Message) override;

private:
	UsineEventClass setItemArrIn;
	UsineEventClass setCommaTxtIn;
	UsineEventClass getItemArrOut;
	UsineEventClass getCommaTxtOut;
	UsineEventClass getChunkDataBtnIn;
	UsineEventClass setChunkDataBtnIn;

	TUsineChunkPtr chunkPtr = nullptr;

    static constexpr int kBulkSize = 10000;
    std::array<float, kBulkSize> data_bulk{};
	std::vector<float>	data_buffer;

	void	fillBulkTestData();
};

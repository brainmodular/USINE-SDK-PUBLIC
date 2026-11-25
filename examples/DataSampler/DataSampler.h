//-----------------------------------------------------------------------------
// @file    DataSampler.h
// @author  BrainModular team
// @brief   Records a data input and plays it back in a loop, in chunks allocated outside of the audio thread.
//
// This file is part of the Usine Hollyhock CPP SDK.
// Copyright (C) 2026 BrainModular, BeSpline, Adamson
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
#pragma once

#include <array>
#include <atomic>
#include <cstddef>
#include "../../sdk/UserDefinitions.h"

constexpr int DEFAULT_MODULE_SIZE = 134;

class DataSampler : public UserModuleBase
{
public:
	~DataSampler() override;

	void onGetModuleInfo(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo) override;
	void onInitModule(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo) override;
	void onCreateParameters(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo) override;
	void onCallBack(TUsineMessage* Message) override;
	void onProcess() override;

	void onCreateSettings() override;
	void onSettingsHasChanged() override;
	void onPaint() override;

private:
	UsineEventClass dataFdrIn;
	UsineEventClass recSwIn;
	UsineEventClass clearBtnIn;
	UsineEventClass boxColIn;
	UsineEventClass outFdrOut;

	TUsineColor colorBox = 0;

	// The recording is a list of chunks of 2000 values. The main thread allocates each chunk before the audio thread
	// reaches it, so onProcess never allocates, and the values never move while they are played back or drawn.
	// The values are atomics: the main thread draws them while the audio thread may write them.
	static constexpr size_t CHUNK_SIZE = 2000;
	static constexpr size_t MAX_CHUNKS = 4096; // 8 192 000 values: about 6 hours at 375 blocks per second
	using Chunk = std::array<std::atomic<TPrecision>, CHUNK_SIZE>;
	std::array<std::atomic<Chunk*>, MAX_CHUNKS> chunks {};
	size_t allocatedChunks = 0; // main thread

	// [toggle rec] and [clear rec] are ctImmediate: their callback runs in the thread that changes the value (the
	// audio thread through a wire, the main thread from the interface). It only stores the request, applied by
	// onProcess at the start of its next block.
	enum Request : int
	{
		NO_REQUEST,
		START_REC,
		STOP_REC,
		CLEAR_REC,
	};
	std::atomic<int> request {NO_REQUEST};

	// written by the audio thread, read by onPaint in the main thread
	std::atomic<bool>   recording {false};
	std::atomic<size_t> recordLength {0};
	std::atomic<size_t> playPosition {0};

	void allocateChunks();
	TPrecision recordedValue(size_t index) const;
	void notifyMainThread(NativeInt callbackId) const;
};

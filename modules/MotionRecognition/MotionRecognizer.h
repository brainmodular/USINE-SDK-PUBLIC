//-----------------------------------------------------------------------------
// @file    MotionRecognizer.h
// @author  BrainModular team
// @brief   Records movements on data inputs and recognizes them in real time.
//
// This file is part of the Usine Hollyhock CPP SDK.
// Copyright (C) 2013-2026 BrainModular, BeSpline, Adamson
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
#pragma once

#include <array>
#include <map>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>
#include "../../sdk/UserDefinitions.h"
#include "process/Handler.h"

// maximum number of data inputs, and of sample outputs
constexpr int DATA_INS_OUTS_MAX = 128;

class MotionRecognizer : public UserModuleBase
{
public:
	void onGetModuleInfo(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo) override;
	void onAfterQuery(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo, int queryResult1, int queryResult2) override;
	void onCreateParameters(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo) override;
	void onCallBack(TUsineMessage* Message) override;
	void onProcess() override;
	int  onGetStateChunkLen(LongBool Preset) override;
	void onGetStateChunk(void* chunk, LongBool Preset) override;
	void onSetStateChunk(const void* chunk, int sizeInBytes, LongBool Preset) override;

private:
	using DataEvents = std::array<UsineEventClass, DATA_INS_OUTS_MAX>;

	DataEvents      dataFdrIn;
	UsineEventClass detectionFdrOut;
	UsineEventClass samplingPeriodFdrIn;
	UsineEventClass thresholdFdrIn;
	UsineEventClass selectMovementLboxIn;
	UsineEventClass recordSwIn;
	UsineEventClass undoBtnIn;
	UsineEventClass clearBtnIn;
	DataEvents      sampleArrOut;
	UsineEventClass latencyFdrOut;

	int numOfInputs = 3;

	// audio thread {
	// last values of each input, in circular buffers, to normalize the input
	std::vector<std::vector<TPrecision>> normalizationBuffers;
	int                                  normalizationHead  = 0;
	// number of values in the buffers, at most their size
	int                                  normalizationCount = 0;
	// last finite value of each input
	std::vector<TPrecision>              lastValidValues;
	// last point read, normalized
	std::vector<TPrecision>              newPoint;
	// in ms, the time since the last point
	double                               timeSinceLastPoint = 0.0;
	bool                                 wasRecording       = false;
	// }

	// chunk taken by onGetStateChunkLen and written by onGetStateChunk, for each thread that saves
	std::mutex                                          chunkMutex;
	std::map<std::thread::id, std::vector<TPrecision>> chunks;

	// its destructor waits for the end of the recognition thread
	std::unique_ptr<process::Handler> handler;

	bool                    isPointDue();
	void                    readPoint();
	int                     selectedMovement() const;
	void                    storeRecordings();
	void                    drawSample();
	std::vector<TPrecision> buildChunk();
};

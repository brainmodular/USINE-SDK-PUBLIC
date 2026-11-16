//-----------------------------------------------------------------------------
// @file    MotionRecognizer.cpp
// @author  BrainModular team
// @brief   Records movements on data inputs and recognizes them in real time.
//
// This file is part of the Usine Hollyhock CPP SDK.
// Copyright (C) 2013-2026 BrainModular, BeSpline, Adamson
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
#include <algorithm>
#include <cmath>
#include <cstring>
#include <limits>
#include <string>
#include <utility>
#include <memory>
#include "MotionRecognizer.h"
#include "ModuleDefinition.h"

namespace
{
	// converts a value read in a chunk; false when it is NaN or out of the int range, where a cast is undefined
	bool toInt(const TPrecision value, int& result)
	{
		if (!((value >= static_cast<TPrecision>(std::numeric_limits<int>::min()))
		      && (value < static_cast<TPrecision>(std::numeric_limits<int>::max()))))
			return false;
		result = static_cast<int>(value);
		return true;
	}
}

void CreateModule(void* &pModule, AnsiCharPtr optionalString, LongBool Flag, TMasterInfo* pMasterInfo, AnsiCharPtr optionalContent)
{
	pModule = new MotionRecognizer();
}

void DestroyModule(void* pModule)
{
	// cast is important to call the good destructor
	delete static_cast<MotionRecognizer*>(pModule);
}

void MotionRecognizer::onGetModuleInfo(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo)
{
	pModuleInfo->ModuleType         = mtSimple;
	pModuleInfo->BackColor          = sdkGetUsineColor(clDataModuleColor);
	pModuleInfo->CanBeSavedInPreset = FALSE;

	// pMasterInfo is null when the module is only listed in the browser
	if (pMasterInfo != nullptr)
	{
		pModuleInfo->QueryLabel1        = "number of inputs";
		pModuleInfo->QueryDefaultValue1 = QUERY_DEFAULT_NB_INPUTS;
		pModuleInfo->QueryMinValue1     = QUERY_MIN_NB_INPUTS;
		pModuleInfo->QueryMaxValue1     = DATA_INS_OUTS_MAX;
	}
}

// called after the query popup, before onCreateParameters
void MotionRecognizer::onAfterQuery(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo, int queryResult1, int queryResult2)
{
	// the events of the inputs and of the sample outputs are arrays of DATA_INS_OUTS_MAX
	numOfInputs = std::clamp(queryResult1, QUERY_MIN_NB_INPUTS, DATA_INS_OUTS_MAX);

	handler = std::make_unique<process::Handler>(BATCH_SIZE, numOfInputs, MAX_RECORDED_POINTS);
	if (!handler->canRecognize())
		sdkRaiseError(ERROR_NO_RECOGNITION_THREAD);
	normalizationBuffers.assign(numOfInputs, std::vector<TPrecision>(NORMALIZATION_WINDOW, 0.0f));
	lastValidValues.assign(numOfInputs, 0.0f);
	newPoint.assign(numOfInputs, 0.0f);
}

void MotionRecognizer::onCreateParameters(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo)
{
	// Usine copies the caption when it creates the parameter
	for (int num = 0; num < numOfInputs; num++)
	{
		const std::string caption = "in " + std::to_string(num + 1);
		TParamInfo        dataIn  = dataFdrInParamInfo;
		dataIn.Caption            = caption.c_str();
		if (num == 0)
		{
			dataIn.IsSeparator      = TRUE;
			dataIn.SeparatorCaption = "data in";
		}
		sdkCreateParam(dataIn, dataFdrIn[num]);
	}

	sdkCreateParam(detectionFdrOutParamInfo, detectionFdrOut);
	sdkCreateParam(samplingPeriodFdrInParamInfo, samplingPeriodFdrIn);
	sdkCreateParam(thresholdFdrInParamInfo, thresholdFdrIn);
	sdkCreateParam(selectMovementLboxInParamInfo, selectMovementLboxIn);
	sdkCreateParam(recordSwInParamInfo, recordSwIn);
	sdkCreateParam(undoBtnInParamInfo, undoBtnIn);
	sdkCreateParam(clearBtnInParamInfo, clearBtnIn);

	for (int num = 0; num < numOfInputs; num++)
	{
		const std::string caption   = "sample out " + std::to_string(num + 1);
		TParamInfo        sampleOut = sampleArrOutParamInfo;
		sampleOut.Caption           = caption.c_str();
		sdkCreateParam(sampleOut, sampleArrOut[num]);
	}

	sdkCreateParam(latencyFdrOutParamInfo, latencyFdrOut);
}

// main thread: the recordings are locked at most for a copy, never for a whole recognition
void MotionRecognizer::onCallBack(TUsineMessage* Message)
{
	if ((Message->message != NOTIFY_MSG_USINE_CALLBACK) || (Message->lParam != MSG_CHANGE))
		return;

	try
	{
		switch (Message->wParam)
		{
			case SELECT_MOVEMENT_LBOX_CBID:
				drawSample();
				break;
			case UPDATE_RECORDINGS_CBID:
				storeRecordings();
				drawSample();
				break;
			case UNDO_BTN_CBID:
				if (!handler->undo())
					sdkRaiseError(WARNING_NOTHING_TO_UNDO);
				drawSample();
				break;
			case CLEAR_BTN_CBID:
				handler->clearSamples();
				drawSample();
				break;
			default:
				break;
		}
	}
	catch (...)
	{
		sdkRaiseError(ERROR_CALLBACK_FAILED);
	}
}

// audio thread: no lock, no allocation
void MotionRecognizer::onProcess()
{
	// read at each bloc: a recording starts and ends in the bloc where [record] is switched
	const bool recording = (recordSwIn.getData() != 0.0f);
	if (recording != wasRecording)
	{
		wasRecording = recording;
		if (recording)
			handler->startRecording();
		else if (handler->finishRecording(selectedMovement()))
			// storing the recording allocates: it is done in the main thread
			sdkNotifyUsine(NOTIFY_TARGET_USER_MODULE, NOTIFY_MSG_USINE_CALLBACK, UPDATE_RECORDINGS_CBID, MSG_CHANGE, TNotifyUsineMode::numMainThread);
	}

	if (!isPointDue())
		return;

	readPoint();
	if (recording)
	{
		handler->recordPoint(newPoint);
	}
	else
	{
		const process::Result result = handler->process(newPoint, LOF_THRESHOLD_MAX * thresholdFdrIn.getData());
		detectionFdrOut.setData(result.detected);
		latencyFdrOut.setData(result.latencyMs);
	}
}

// a point is read when the time since the last one reaches [sampling period]; the rest of the time is kept for the
// next point, so that the average period is exact whatever the bloc size
bool MotionRecognizer::isPointDue()
{
	const double blocMs = 1000.0 * sdkGetBlocSize() / sdkGetSampleRate();
	// no audio
	if (!(blocMs > 0.0) || !std::isfinite(blocMs))
		return false;

	double periodMs = samplingPeriodFdrIn.getData();
	// NaN included
	if (!(periodMs >= samplingPeriodFdrInParamInfo.MinValue))
		periodMs = samplingPeriodFdrInParamInfo.MinValue;

	timeSinceLastPoint += blocMs;
	if (timeSinceLastPoint < periodMs)
		return false;

	timeSinceLastPoint -= periodMs;
	// the period is shorter than a bloc, or it has just been shortened: one point per bloc at most
	if (timeSinceLastPoint >= periodMs)
		timeSinceLastPoint = std::fmod(timeSinceLastPoint, periodMs);
	return true;
}

// reads the inputs into newPoint, each one normalized with the mean and the standard deviation of its last values
void MotionRecognizer::readPoint()
{
	normalizationCount = std::min(normalizationCount + 1, NORMALIZATION_WINDOW);

	for (int i = 0; i < numOfInputs; ++i)
	{
		TPrecision rawVal = dataFdrIn[i].getData();
		// a NaN or an infinite value would stay in the statistics for NORMALIZATION_WINDOW points, and in the recordings
		if (std::isfinite(rawVal))
			lastValidValues[i] = rawVal;
		else
			rawVal = lastValidValues[i];

		std::vector<TPrecision>& buf = normalizationBuffers[i];
		buf[normalizationHead]       = rawVal;

		// over the values read so far only, the first normalizationCount ones of the buffer; in double, because in
		// float the rounding error of the mean of a still input of large magnitude was normalized to about +-1
		double mean = 0.0;
		for (int j = 0; j < normalizationCount; ++j)
			mean += buf[j];
		mean /= normalizationCount;

		double variance = 0.0;
		for (int j = 0; j < normalizationCount; ++j)
			variance += (buf[j] - mean) * (buf[j] - mean);
		variance /= normalizationCount;
		// the epsilon avoids a division by 0 when the input doesn't move
		const double stddev = std::sqrt(variance + 1e-6);

		newPoint[i] = static_cast<TPrecision>((rawVal - mean) / stddev);
	}
	normalizationHead = (normalizationHead + 1) % NORMALIZATION_WINDOW;
}

int MotionRecognizer::selectedMovement() const
{
	const TPrecision value = selectMovementLboxIn.getData();
	// clamped before the conversion, undefined for NaN or out of the int range
	if (!(value >= 0.0f))
		return 0;
	if (value >= static_cast<TPrecision>(N_MVT_MAX - 1))
		return N_MVT_MAX - 1;
	return static_cast<int>(value);
}

void MotionRecognizer::storeRecordings()
{
	const process::RecordingEvents events = handler->storeRecordings();
	if (events.empty > 0)
		sdkRaiseError(WARNING_EMPTY_RECORDING);
	if (events.truncated > 0)
		sdkRaiseError(WARNING_RECORDING_TRUNCATED);
	if (events.lateStart > 0)
		sdkRaiseError(WARNING_RECORDING_START_LOST);
	if (events.lost > 0)
		sdkRaiseError(WARNING_RECORDING_LOST);
}

// the chunk is an array of floats: a header of META_DATA_SIZE values (version, number of movements,
// number of recordings, number of inputs), then each recording: a header of META_DATA_SIZE values
// (movement, number of points) followed by its points; the unused values of the headers are 0
std::vector<TPrecision> MotionRecognizer::buildChunk()
{
	const auto movements = handler->getMovements();

	size_t size       = META_DATA_SIZE;
	int    recordings = 0;
	for (const auto& movement : movements)
	{
		for (const auto& sample : movement)
		{
			size += META_DATA_SIZE + sample.size() * static_cast<size_t>(numOfInputs);
			recordings++;
		}
	}

	std::vector<TPrecision> chunk(size, 0.0f);
	chunk[0] = static_cast<TPrecision>(CHUNK_VERSION);
	chunk[1] = static_cast<TPrecision>(N_MVT_MAX);
	chunk[2] = static_cast<TPrecision>(recordings);
	chunk[3] = static_cast<TPrecision>(numOfInputs);

	size_t writeHead = META_DATA_SIZE;
	for (int mvtNumber = 0; mvtNumber < N_MVT_MAX; ++mvtNumber)
	{
		for (const auto& sample : movements[mvtNumber])
		{
			chunk[writeHead]     = static_cast<TPrecision>(mvtNumber);
			chunk[writeHead + 1] = static_cast<TPrecision>(sample.size());
			writeHead += META_DATA_SIZE;

			for (const auto& point : sample)
			{
				std::copy_n(point.begin(), std::min(point.size(), static_cast<size_t>(numOfInputs)), chunk.begin() + static_cast<std::ptrdiff_t>(writeHead));
				writeHead += numOfInputs;
			}
		}
	}
	return chunk;
}

// the chunk is taken here, from the recordings locked once: onGetStateChunk writes exactly the announced size even
// if the recordings change in between. Usine calls both from the same thread (TpgUser.SaveChunk)
int MotionRecognizer::onGetStateChunkLen(LongBool Preset)
{
	try
	{
		std::vector<TPrecision> chunk = buildChunk();
		if (chunk.size() > static_cast<size_t>(std::numeric_limits<int>::max()) / sizeof(TPrecision))
		{
			sdkRaiseError(ERROR_SAVE_FAILED);
			return 0;
		}
		const int                   len = static_cast<int>(chunk.size() * sizeof(TPrecision));
		const std::lock_guard<std::mutex> lock(chunkMutex);
		chunks[std::this_thread::get_id()] = std::move(chunk);
		return len;
	}
	catch (...)
	{
		sdkRaiseError(ERROR_SAVE_FAILED);
		return 0;
	}
}

void MotionRecognizer::onGetStateChunk(void* chunk, LongBool Preset)
{
	const std::lock_guard<std::mutex> lock(chunkMutex);
	const auto                        it = chunks.find(std::this_thread::get_id());
	if (it == chunks.end())
		return;
	std::memcpy(chunk, it->second.data(), it->second.size() * sizeof(TPrecision));
	chunks.erase(it);
}

void MotionRecognizer::onSetStateChunk(const void* chunk, int sizeInBytes, LongBool Preset)
{
	try
	{
		{
			// Usine asks the length of the chunk before restoring it: that chunk is never written
			const std::lock_guard<std::mutex> lock(chunkMutex);
			chunks.erase(std::this_thread::get_id());
		}

		const TPrecision* castedChunk = static_cast<const TPrecision*>(chunk);
		// in floats: every read is checked against it
		const size_t      chunkSize   = static_cast<size_t>(std::max(sizeInBytes, 0)) / sizeof(TPrecision);

		if (chunkSize < META_DATA_SIZE)
		{
			sdkRaiseError(ERROR_CORRUPTED_CHUNK);
			return;
		}

		int        version   = 0;
		const bool isVersion = toInt(castedChunk[0], version);
		if (!isVersion || (version != CHUNK_VERSION))
		{
			sdkRaiseError(ERROR_UNKNOWN_CHUNK_VERSION, isVersion ? std::to_string(version) : std::to_string(castedChunk[0]));
			return;
		}

		int savedNbMovements = 0;
		int savedNbSamples   = 0;
		int savedNumberOfIns = 0;
		if (!toInt(castedChunk[1], savedNbMovements) || !toInt(castedChunk[2], savedNbSamples) || !toInt(castedChunk[3], savedNumberOfIns))
		{
			sdkRaiseError(ERROR_CORRUPTED_CHUNK);
			return;
		}

		const bool tooManyMovements = (savedNbMovements > N_MVT_MAX);
		if (tooManyMovements)
		{
			sdkRaiseError(WARNING_TOO_MANY_MOVEMENTS, std::to_string(savedNbMovements) + " movements, " + std::to_string(N_MVT_MAX) + " max");
			savedNbMovements = N_MVT_MAX;
		}

		if (savedNumberOfIns != numOfInputs)
		{
			sdkRaiseError(ERROR_WRONG_NUMBER_OF_INPUTS, "expected " + std::to_string(numOfInputs) + ", received " + std::to_string(savedNumberOfIns));
			return;
		}

		std::vector<std::pair<int, process::Recording>> samples;
		bool                                            corrupted     = false;
		bool                                            invalidValues = false;
		int                                             ignored       = 0;
		size_t                                          readHead      = META_DATA_SIZE;
		for (int sampleNumber = 0; sampleNumber < savedNbSamples; ++sampleNumber)
		{
			int sampleSize = 0;
			if ((readHead + META_DATA_SIZE > chunkSize) || !toInt(castedChunk[readHead + 1], sampleSize) || (sampleSize < 0))
			{
				corrupted = true;
				break;
			}
			const TPrecision label = castedChunk[readHead];
			readHead += META_DATA_SIZE;

			if (readHead + static_cast<size_t>(sampleSize) * numOfInputs > chunkSize)
			{
				corrupted = true;
				break;
			}
			const size_t sampleEnd = readHead + static_cast<size_t>(sampleSize) * numOfInputs;

			// skipped like before: an empty recording (saved by the previous versions after a too short recording) would be
			// the nearest neighbor of every movement
			if (sampleSize == 0)
				continue;

			if (!((label >= 0.0f) && (label < static_cast<TPrecision>(savedNbMovements))))
			{
				// the movements beyond N_MVT_MAX are reported by WARNING_TOO_MANY_MOVEMENTS
				if (!(tooManyMovements && (label >= static_cast<TPrecision>(N_MVT_MAX))))
					ignored++;
				readHead = sampleEnd;
				continue;
			}

			process::Recording sample;
			sample.reserve(static_cast<size_t>(sampleSize));
			for (; readHead < sampleEnd; readHead += numOfInputs)
			{
				std::vector<TPrecision> point(castedChunk + readHead, castedChunk + readHead + numOfInputs);
				// they would make the distances NaN, then the sort of the neighbors undefined
				for (TPrecision& value : point)
				{
					if (!std::isfinite(value))
					{
						value         = 0.0f;
						invalidValues = true;
					}
				}
				sample.push_back(std::move(point));
			}
			samples.emplace_back(static_cast<int>(label), std::move(sample));
		}

		// the restored recordings replace the current ones, under one lock
		handler->replaceSamples(samples);

		if (corrupted)
			sdkRaiseError(ERROR_CORRUPTED_CHUNK);
		if (invalidValues)
			sdkRaiseError(WARNING_INVALID_SAVED_VALUES);
		if (ignored > 0)
			sdkRaiseError(WARNING_IGNORED_RECORDINGS, std::to_string(ignored));
	}
	catch (...)
	{
		sdkRaiseError(ERROR_RESTORE_FAILED);
	}

	// [sample out] is drawn in the main thread: Usine may restore the chunk in another one
	sdkNotifyUsine(NOTIFY_TARGET_USER_MODULE, NOTIFY_MSG_USINE_CALLBACK, UPDATE_RECORDINGS_CBID, MSG_CHANGE, TNotifyUsineMode::numMainThread);
}

// sends the last recording of the selected movement to the sample outputs, one array per input; main thread only
void MotionRecognizer::drawSample()
{
	const process::Recording sample = handler->getLastSample(selectedMovement());

	for (int axis = 0; axis < numOfInputs; ++axis)
	{
		sampleArrOut[axis].setSize(0);
		for (const auto& point : sample)
			sampleArrOut[axis].pushArrayData(point[axis]);
	}
}

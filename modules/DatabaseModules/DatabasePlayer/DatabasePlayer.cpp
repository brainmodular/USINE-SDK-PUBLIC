//-----------------------------------------------------------------------------
// @file    DatabasePlayer.cpp
// @author  BrainModular team
// @brief   Plays a sound database: 2D map of the samples, resynthesis of the samples under the cursor.
//
// This file is part of the Usine Hollyhock CPP SDK.
// Copyright (C) 2013-2026 BrainModular, BeSpline, Adamson
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <numbers>
#if defined(__APPLE__)
#include <sys/stat.h>
#else
#include <filesystem>
#include <memory>
#endif
#include "DatabasePlayer.h"
#include "ModuleDefinition.h"
#if defined(__APPLE__)
// after the SDK headers: it defines TRUE and FALSE as macros
#include <dispatch/dispatch.h>
#endif

static_assert(FIRST_DIMENSION_COLUMNS.size() == SECOND_DIMENSION_COLUMNS.size());
static_assert(POLYPHONY_MAX * 2 == SAMPLE_QUEUE_MAX_POLY);

namespace
{
	// std::filesystem needs macOS 10.15 at run time (libc++ availability), the modules target 10.13
	bool fileExists(const std::string& path)
	{
#if defined(__APPLE__)
		struct stat st;
		return ::stat(path.c_str(), &st) == 0;
#else
		return std::filesystem::exists(path);
#endif
	}

	void hsv_to_rgb(float h, float s, float v, uint8_t& r, uint8_t& g, uint8_t& b)
	{
		float c = v * s;
		float x = c * (1 - std::fabs(fmod(h / 60.0f, 2.0f) - 1));
		float m = v - c;
		float r1, g1, b1;

		if (h < 60)
		{
			r1 = c;
			g1 = x;
			b1 = 0;
		}
		else if (h < 120)
		{
			r1 = x;
			g1 = c;
			b1 = 0;
		}
		else if (h < 180)
		{
			r1 = 0;
			g1 = c;
			b1 = x;
		}
		else if (h < 240)
		{
			r1 = 0;
			g1 = x;
			b1 = c;
		}
		else if (h < 300)
		{
			r1 = x;
			g1 = 0;
			b1 = c;
		}
		else
		{
			r1 = c;
			g1 = 0;
			b1 = x;
		}

		r = static_cast<uint8_t>((r1 + m) * 255);
		g = static_cast<uint8_t>((g1 + m) * 255);
		b = static_cast<uint8_t>((b1 + m) * 255);
	}

	// color of a point of the panel: its angle around the center gives the hue, its distance the saturation
	TUsineColor xy_to_rgb_colorwheel_radial(float x, float y)
	{
		constexpr float DEG_PI = 180.0f / std::numbers::pi_v<float>;

		x = std::clamp(x, 0.0f, 1.0f);
		y = std::clamp(y, 0.0f, 1.0f);

		float dx  = x - 0.5f;
		float dy  = y - 0.5f;
		float hue = std::atan2(dy, dx) * DEG_PI;
		if (hue < 0.0f)
			hue += 360.0f;

		float dist = std::sqrt(dx * dx + dy * dy) * 1.4142f;
		float sat  = std::clamp(dist * 3.0f, 0.5f, 1.0f);

		float val = 1.0f;

		uint8_t r, g, b;
		hsv_to_rgb(hue, sat, val, r, g, b);
		return (255 << 24) | (r << 16) | (g << 8) | b; // ARGB
	}

	// a prepared statement, finalized on every path: a statement left unfinalized keeps the database from closing
	class Statement
	{
	public:
		Statement(sqlite3* db, const std::string& query)
		{
			result = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
		}
		~Statement()
		{
			sqlite3_finalize(stmt);
		}
		Statement(const Statement&)            = delete;
		Statement& operator=(const Statement&) = delete;

		[[nodiscard]] bool isPrepared() const
		{
			return result == SQLITE_OK && stmt != nullptr;
		}
		[[nodiscard]] sqlite3_stmt* get() const
		{
			return stmt;
		}

	private:
		sqlite3_stmt* stmt   = nullptr;
		int           result = SQLITE_ERROR;
	};

	// a closed connection is freed by sqlite: forget it, so that it is never closed twice
	int closeDatabase(DatabasePlayer* module)
	{
		const int result = sqlite3_close(module->db);
		if (result == SQLITE_OK)
			module->db = nullptr;
		return result;
	}

	// item of a dimension list box, as an index of FIRST_DIMENSION_COLUMNS and SECOND_DIMENSION_COLUMNS
	int dimensionItem(const UsineEventClass& dimensionLbox)
	{
		const float value = dimensionLbox.getData();
		if (!(value >= 0.0f))
			return 0;
		return std::clamp(static_cast<int>(std::min(value, 1000.0f)), 0, static_cast<int>(FIRST_DIMENSION_COLUMNS.size()) - 1);
	}

	// the radius of the cursor circle stays in the range of [cursor radius], in relative coordinates
	float clampRadius(float radius)
	{
		if (!(radius >= CURSOR_RADIUS_MIN * 0.01f))
			return CURSOR_RADIUS_MIN * 0.01f;
		return std::min(radius, CURSOR_RADIUS_MAX * 0.01f);
	}

	// a statement of sqlite is interrupted when the module is deleted
	int progressHandler(void* pModule)
	{
		return static_cast<DatabasePlayer*>(pModule)->actionQueue.isStopping() ? 1 : 0;
	}

	void workThreadFunc(void* pModule, TThreadPtr thread)
	{
		auto* module = static_cast<DatabasePlayer*>(pModule);
		try
		{
			module->actionQueue.run();
		}
		catch (const std::exception& e)
		{
			module->sdkReport(ERROR_WORK_THREAD, e.what());
		}
	}

	// called every 20 ms: asks the work thread for a search while a database is open
	void initSearchThreadFunc(void* pModule, TThreadPtr thread)
	{
		auto* module = static_cast<DatabasePlayer*>(pModule);
		try
		{
			if (module->isDBOpen.load() && module->actionQueue.m_IsWorking.load(std::memory_order_relaxed))
				module->pushSearch();
		}
		catch (const std::exception& e)
		{
			module->sdkReport(ERROR_SEARCH_THREAD, e.what());
		}
	}
}

#if defined(__APPLE__)
WakeUpSignal::WakeUpSignal() : semaphore(dispatch_semaphore_create(0))
{
}

WakeUpSignal::~WakeUpSignal()
{
	dispatch_release(static_cast<dispatch_semaphore_t>(semaphore));
}

void WakeUpSignal::signal()
{
	dispatch_semaphore_signal(static_cast<dispatch_semaphore_t>(semaphore));
}

void WakeUpSignal::wait(int timeoutMs)
{
	dispatch_semaphore_wait(static_cast<dispatch_semaphore_t>(semaphore), dispatch_time(DISPATCH_TIME_NOW, static_cast<int64_t>(timeoutMs) * static_cast<int64_t>(NSEC_PER_MSEC)));
}
#else
WakeUpSignal::WakeUpSignal()  = default;
WakeUpSignal::~WakeUpSignal() = default;

void WakeUpSignal::signal()
{
	semaphore.release();
}

void WakeUpSignal::wait(int timeoutMs)
{
	(void)semaphore.try_acquire_for(std::chrono::milliseconds(timeoutMs));
}
#endif

void CreateModule(void*& pModule, AnsiCharPtr optionalString, LongBool Flag, TMasterInfo* pMasterInfo, AnsiCharPtr optionalContent)
{
	pModule = new DatabasePlayer();
}

void DestroyModule(void* pModule)
{
	// cast is important to call the good destructor
	delete static_cast<DatabasePlayer*>(pModule);
}

DatabasePlayer::DatabasePlayer()
{
	generateLUT();
	InitializeHanningWindow(hanning, DEFAULT_SPECTRUM);

	// each module gets its own sequence of random phases
	static std::atomic<unsigned> instances {0};
	phaseRandom.seed(1u + instances.fetch_add(1u));
}

DatabasePlayer::~DatabasePlayer()
{
	// the critical section, the threads and the FFT are created by onInitModule, which may not have been called.
	// csPaint is not held meanwhile: the work thread needs it to finish a loading or a request
	try
	{
		if (initSearchThread != nullptr)
			sdkThreadDestroy(initSearchThread);
		if (workThread != nullptr)
		{
			// the work thread leaves ActionQueue::run once its queue is stopped, a long task gives up
			actionQueue.clear();
			actionQueue.stop();
			sdkThreadDestroy(workThread);
		}

		if (ifftPtr != nullptr)
			sdkFFTDestroy(ifftPtr);

		if (closeDatabase(this) != SQLITE_OK)
			sdkReport(ERROR_CLOSE_DATABASE, sqlite3_errmsg(db));
	}
	catch (...)
	{
		sdkReport(ERROR_DELETE);
	}

	if (csPaint != nullptr)
		sdkCriticalSectionDestroy(csPaint);
}

void DatabasePlayer::onGetModuleInfo(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo)
{
	pModuleInfo->ModuleType      = mtControl;
	pModuleInfo->BackColor       = sdkGetUsineColor(clDataModuleColor);
	pModuleInfo->DefaultWidth    = DEFAULT_PANEL_WIDTH;
	pModuleInfo->DefaultHeight   = DEFAULT_PANEL_HEIGHT;
	pModuleInfo->CanBeRandomized = FALSE;
}

void DatabasePlayer::onInitModule(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo)
{
	csPaint = sdkCriticalSectionCreate("paint critical section", false);
	ifftPtr = sdkFFTCreate(SPECTRUM_SIZE);

	// the parameters exist: the state starts from their values, no callback comes for a default value
	blocSize = sdkGetBlocSize();
	sampleRate.store(sdkGetSampleRate(), std::memory_order_relaxed);
	readParameters(false);

	actionQueue.setFrameTask([this]() { produceFrame(); });
	workThread       = sdkThreadCreate("working Thread", workThreadFunc, tpMedium, 0);
	initSearchThread = sdkThreadCreate("init search Thread", initSearchThreadFunc, tpLow, 20);
}

void DatabasePlayer::onCreateParameters(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo)
{
	sdkCreateParam(databaseFileInParamInfo, databaseFileIn);
	sdkCreateParam(reloadDatabaseBtnInParamInfo, reloadDatabaseBtnIn);
	sdkCreateParam(firstDimensionLboxInParamInfo, firstDimensionLboxIn);
	sdkCreateParam(secondDimensionLboxInParamInfo, secondDimensionLboxIn);
	sdkCreateParam(polyphonyFdrInParamInfo, polyphonyFdrIn);
	sdkCreateParam(samplesListTxtOutParamInfo, samplesListTxtOut);
	sdkCreateParam(samplesCoeffsArrOutParamInfo, samplesCoeffsArrOut);
	sdkCreateParam(attackTimeFdrInParamInfo, attackTimeFdrIn);
	sdkCreateParam(releaseTimeFdrInParamInfo, releaseTimeFdrIn);
	sdkCreateParam(onResynthesisSwInParamInfo, onResynthesisSwIn);
	sdkCreateParam(audioOut1ParamInfo, audioOutputs[0]);
	sdkCreateParam(audioOut2ParamInfo, audioOutputs[1]);
	sdkCreateParam(cursorDownSwInParamInfo, cursorDownSwIn);
	sdkCreateParam(cursorXFdrInParamInfo, cursorXFdrIn);
	sdkCreateParam(cursorYFdrInParamInfo, cursorYFdrIn);
	sdkCreateParam(cursorRadiusFdrInParamInfo, cursorRadiusFdrIn);
}

void DatabasePlayer::onCreateSettings()
{
	sdkAddSettingLineCaption(PROPERTIES_TAB_NAME, "graphical settings");

	sdkAddSettingLineSingle(PROPERTIES_TAB_NAME, &canvasPointRadius, "points radius", 2.0f, 10.0f, scLinear, "pix", DEFAULT_FORMAT_INTEGER, 2.0f);
	sdkSetLastSettingCallbackID(POINTS_RADIUS_SETTING_CBID);

	sdkAddSettingLineSingle(PROPERTIES_TAB_NAME, &spreadFactorX, "spread factor X", 0.1f, 1.0f, scLinear, "", "%.2f", 0.25f);
	sdkSetLastSettingCallbackID(SPREAD_FACTOR_X_SETTING_CBID);

	sdkAddSettingLineSingle(PROPERTIES_TAB_NAME, &spreadFactorY, "spread factor Y", 0.1f, 1.0f, scLinear, "", "%.2f", 0.25f);
	sdkSetLastSettingCallbackID(SPREAD_FACTOR_Y_SETTING_CBID);

	sdkAddSettingLineCaption(PROPERTIES_TAB_NAME, "output settings");
	sdkAddSettingLineBoolean(PROPERTIES_TAB_NAME, &normalizeCoeffsSetting, "normalize coeffs");
	sdkSetLastSettingCallbackID(NORMALIZE_COEFFS_SETTING_CBID);
}

// the values loaded with the patch give no callback when they equal the current ones: the state is read again
void DatabasePlayer::onAfterLoading()
{
	readParameters(true);
}

void DatabasePlayer::onBlocSizeChange(int BlocSize)
{
	blocSize = BlocSize;
	updateFadeDurations();
}

void DatabasePlayer::onSampleRateChange(double SampleRate)
{
	sampleRate.store(SampleRate, std::memory_order_relaxed);
	updateFadeDurations();
}

void DatabasePlayer::onCallBack(TUsineMessage* Message)
{
	if ((Message->message != NOTIFY_MSG_USINE_CALLBACK) || (Message->lParam != MSG_CHANGE))
		return;

	try
	{
		switch (Message->wParam)
		{
			case DATABASE_FILE_CBID:
				dbName = std::string(databaseFileIn.getPChar());
				actionQueue.push([this, fileName = dbName]() { openDatabaseTask(fileName); });
				break;
			case RELOAD_DATABASE_BTN_CBID:
				if (!isDBOpen.load())
				{
					sdkReport(ERROR_NO_DATABASE_OPEN);
					break;
				}
				dbName = std::string(databaseFileIn.getPChar());
				if (!isLoading.load(std::memory_order_acquire))
					actionQueue.push([this, fileName = dbName]() { openDatabaseTask(fileName); });
				break;
			case FIRST_DIMENSION_LBOX_CBID:
				firstDimensionItem.store(dimensionItem(firstDimensionLboxIn));
				pushRequest();
				break;
			case SECOND_DIMENSION_LBOX_CBID:
				secondDimensionItem.store(dimensionItem(secondDimensionLboxIn));
				pushRequest();
				break;
			case POLYPHONY_FDR_CBID:
				setPolyphony(polyphonyFdrIn.getData());
				break;
			case ATTACK_TIME_FDR_CBID:
			case RELEASE_TIME_FDR_CBID:
				updateFadeDurations();
				break;
			case ON_RESYNTHESIS_SW_CBID:
				// the audio thread silences its outputs itself
				activeResynth.store(onResynthesisSwIn.getData() != 0.0f, std::memory_order_relaxed);
				break;
			case CURSOR_DOWN_SW_CBID:
				isCursorOn.store(cursorDownSwIn.getData() != 0.0f);
				pushSearch();
				if (!mouseInfo.leftBtnDown.load() && !isCursorOn.load())
				{
					QueueWriteGuard guard(queueLock);
					listOfOutputSamples.softClear();
				}
				sdkRepaintPanel();
				break;
			case CURSOR_X_FDR_CBID:
				if (!mouseInfo.leftBtnDown.load())
					circleCursor.x.store(cursorXFdrIn.getData());
				sdkRepaintPanel();
				break;
			case CURSOR_Y_FDR_CBID:
				if (!mouseInfo.leftBtnDown.load())
					circleCursor.y.store(cursorYFdrIn.getData());
				sdkRepaintPanel();
				break;
			case CURSOR_RADIUS_FDR_CBID:
				circleCursor.setRadius(clampRadius(cursorRadiusFdrIn.getData() * 0.01f));
				sdkRepaintPanel();
				break;
			case POINTS_RADIUS_SETTING_CBID:
			case SPREAD_FACTOR_X_SETTING_CBID:
			case SPREAD_FACTOR_Y_SETTING_CBID:
				refreshSettings();
				pushRequest();
				break;
			case NORMALIZE_COEFFS_SETTING_CBID:
				refreshSettings();
				break;
			default:
				break;
		}
	}
	catch (...)
	{
		sdkReport(ERROR_CALLBACK, std::to_string(Message->wParam));
	}
}

// audio thread: no lock waited for, no allocation. The gains are smoothed here, the samples come from the
// work thread through outputFifo
void DatabasePlayer::onProcess()
{
	const int bloc = sdkGetBlocSize();
	for (const UsineEventClass& audioOut : audioOutputs)
		audioOut.setSize(bloc);

	samplesCoeffsArrOut.setSize(SAMPLE_QUEUE_MAX_POLY);

	// the gains move one bloc towards their target; while a writer holds the queue, the step is made at the next bloc
	pendingCoeffUpdates++;
	if (queueLock.tryLockAudio())
	{
		for (; pendingCoeffUpdates > 0; pendingCoeffUpdates--)
			listOfOutputSamples.updateCoeffs();
		for (int i = 0; i < SAMPLE_QUEUE_MAX_POLY; i++)
			audioCoeffs[i] = listOfOutputSamples.queue[i].coeff.getCoeff();
		audioSumCoeffs = listOfOutputSamples.sumCoeffs;
		if (listOfOutputSamples.textChanged)
		{
			audioSamplesText.swap(listOfOutputSamples.pendingText);
			listOfOutputSamples.textChanged = false;
		}
		queueLock.unlockAudio();
	}

	for (int i = 0; i < SAMPLE_QUEUE_MAX_POLY; i++)
		samplesCoeffsArrOut.setArrayData(i, audioCoeffs[i]);

	samplesListTxtOut.setPChar(audioSamplesText.c_str());

	float* out1 = audioOutputs[0].getDataAddr();
	float* out2 = audioOutputs[1].getDataAddr();
	if (out1 == nullptr || out2 == nullptr || bloc <= 0)
		return;

	if (isLoading.load(std::memory_order_acquire) || !activeResynth.load(std::memory_order_relaxed))
	{
		// silent, and what the work thread computes meanwhile is dropped
		std::fill(out1, out1 + bloc, 0.0f);
		std::fill(out2, out2 + bloc, 0.0f);
		outputFifo.flush();
		streamRunning   = false;
		tailSamples     = 0;
		streamedSamples = 0;
		return;
	}

	if (!streamRunning)
	{
		// the stream starts from silence: the work thread drops the tails of its previous frames
		streamResets.fetch_add(1u, std::memory_order_release);
		streamRunning = true;
	}
	// until the work thread has dropped them, its samples belong to the previous stream
	if (streamResetsDone.load(std::memory_order_acquire) != streamResets.load(std::memory_order_relaxed))
		outputFifo.flush();

	const int read = outputFifo.read(out1, out2, bloc);
	std::fill(out1 + read, out1 + bloc, 0.0f);
	std::fill(out2 + read, out2 + bloc, 0.0f);

	// frames are computed while a sample sounds, then until the tails of the last frames are out. The work thread
	// keeps a bloc and 10 ms of samples ahead, more after an underrun of a running stream (the work thread was late,
	// or the host calls the process in bursts), see LEAD_MARGIN_SECONDS
	const int    hop  = std::max(1, hopSize.load(std::memory_order_relaxed));
	const double rate = sampleRate.load(std::memory_order_relaxed);
	if (bloc != leadBloc)
	{
		leadBloc  = bloc;
		extraLead = 0;
	}
	const int baseLead = bloc + static_cast<int>(rate * LEAD_MARGIN_SECONDS);
	if (read < bloc && tailSamples > 0 && streamedSamples >= baseLead + extraLead + 2 * hop)
		extraLead = std::min(extraLead + std::max(bloc, static_cast<int>(rate * LEAD_STEP_SECONDS)), std::max(bloc, static_cast<int>(rate * LEAD_EXTRA_MAX_SECONDS)));
	const int lead = baseLead + extraLead;

	if (audioSumCoeffs > 1e-5f)
		tailSamples = 3 * hop + lead + bloc;
	else if (tailSamples > 0)
		tailSamples -= bloc;

	if (tailSamples > 0)
	{
		streamedSamples = std::min(streamedSamples + bloc, OUTPUT_FIFO_SIZE);
		const int64_t ahead = static_cast<int64_t>(outputFifo.available()) + static_cast<int64_t>(actionQueue.pendingFrames()) * hop;
		if (ahead < lead)
			actionQueue.requestFrames(static_cast<int>((lead - ahead + hop - 1) / hop));
	}
	else
	{
		streamedSamples = 0;
	}
}

// work thread: a frame of the resynthesis, the spectrum of magnitudes with random phases to the time domain, windowed
void DatabasePlayer::computeIFFTMag(const float* magPtr)
{
	assert(magPtr != nullptr);

	std::fill(rReal.begin(), rReal.end(), 0.0f);
	std::fill(rIm.begin(), rIm.end(), 0.0f);
	std::fill(lReal.begin(), lReal.end(), 0.0f);
	std::fill(lIm.begin(), lIm.end(), 0.0f);

	int idx  = 0;
	int lIdx = 0;

	for (int i = 0; i < SPECTRUM_SIZE_2; i++)
	{
		idx  = static_cast<int>(phaseRandom() % LUT_SIZE);
		lIdx = static_cast<int>(phaseRandom() % LUT_SIZE);

		rReal[i] = magPtr[i] * trigoLUT[idx].cos;
		rIm[i]   = magPtr[i] * trigoLUT[idx].sin;
		lReal[i] = magPtr[i] * trigoLUT[lIdx].cos;
		lIm[i]   = magPtr[i] * trigoLUT[lIdx].sin;
	}

	rIm[0]                   = 0.0f;
	rIm[SPECTRUM_SIZE_2 - 1] = 0.0f;
	lIm[0]                   = 0.0f;
	lIm[SPECTRUM_SIZE_2 - 1] = 0.0f;

	sdkFFTInverse(ifftPtr, rTempBuffer.data(), rReal.data(), rIm.data());
	sdkFFTInverse(ifftPtr, lTempBuffer.data(), lReal.data(), lIm.data());

	for (int i = 0; i < SPECTRUM_SIZE; i++)
	{
		rTempBuffer[i] *= hanning[i];
		lTempBuffer[i] *= hanning[i];
	}
}

// work thread: sums the magnitudes of the samples of the queue weighted by their gain, adds the frame to the
// overlap-add and gives its first hop, complete, to the audio thread
void DatabasePlayer::produceFrame()
{
	try
	{
		const uint32_t resets = streamResets.load(std::memory_order_acquire);
		if (resets != seenStreamResets)
		{
			for (auto& channel : overlapAdd)
				std::fill(channel.begin(), channel.end(), 0.0f);
			seenStreamResets = resets;
			streamResetsDone.store(resets, std::memory_order_release);
		}

		std::array<const float*, SAMPLE_QUEUE_MAX_POLY> mags {};
		std::array<float, SAMPLE_QUEUE_MAX_POLY>        gains {};
		int                                             voices = 0;
		{
			QueueWriteGuard guard(queueLock);
			listOfOutputSamples.clearSamplesToRelease();
			voices = listOfOutputSamples.collectVoices(mags, gains);
			listOfOutputSamples.publishText();
		}

		const int size = SPECTRUM_SIZE;
		const int hop  = SPECTRUM_SIZE_4;
		if (voices > 0 && ifftPtr != nullptr)
		{
			std::fill(spectrumMags.begin(), spectrumMags.end(), 0.0f);
			for (int v = 0; v < voices; v++)
			{
				for (int i = 0; i < SPECTRUM_SIZE_2; i++)
					spectrumMags[i] += mags[v][i] * gains[v];
			}
			computeIFFTMag(spectrumMags.data());

			for (int i = 0; i < size; i++)
			{
				overlapAdd[0][i] += rTempBuffer[i];
				overlapAdd[1][i] += lTempBuffer[i];
			}
		}

		if (outputFifo.freeSpace() >= hop)
			outputFifo.write(overlapAdd[0].data(), overlapAdd[1].data(), hop);
		for (auto& channel : overlapAdd)
		{
			std::copy(channel.begin() + hop, channel.end(), channel.begin());
			std::fill(channel.end() - hop, channel.end(), 0.0f);
		}
	}
	catch (const std::exception& e)
	{
		sdkReport(ERROR_RESYNTHESIS, e.what());
	}
	actionQueue.frameDone();
}

void DatabasePlayer::onPaint()
{
	try
	{
		sdkCriticalSectionLock(csPaint);
		if (numberOfPointsToDraw > 0 && numberOfPointsToDraw == static_cast<int>(pointsToDraw.size()) && numberOfPointsToDraw == static_cast<int>(colorsToDraw.size()))
			sdkDrawMultiPoints(pointsToDraw.data(), colorsToDraw.data(), canvasPointRadius, FALSE, numberOfPointsToDraw);
		sdkCriticalSectionUnLock(csPaint);

		// the diameter of the circle of the search, in pixels of the panel width
		if (mouseInfo.leftBtnDown.load() || isCursorOn.load())
		{
			sdkDrawPoint(TPointF {.x = circleCursor.x.load(), .y = circleCursor.y.load()},
				sdkAlphaColor(0.5f, 0.5f, 0.5f, 0.2f),
				2.0f * sdkWidthPercentToPixel(circleCursor.radius.load()),
				TRUE);
		}
	}
	catch (const std::exception& e)
	{
		sdkReport(ERROR_PAINT, e.what());
	}
}

void DatabasePlayer::onMouseMove(TShiftState Shift, float X, float Y)
{
	if (mouseInfo.leftBtnDown.load())
	{
		circleCursor.x.store(X);
		circleCursor.y.store(Y);
	}
}

void DatabasePlayer::onMouseDown(TMouseButton MouseButton, TShiftState Shift, float X, float Y)
{
	if (MouseButton == mbLeft)
	{
		circleCursor.x.store(X);
		circleCursor.y.store(Y);
		mouseInfo.leftBtnDown.store(true);

		pushSearch();
	}
}

void DatabasePlayer::onMouseUp(TMouseButton MouseButton, TShiftState Shift, float X, float Y)
{
	if (MouseButton == mbLeft)
	{
		mouseInfo.leftBtnDown.store(false);
		if (!isCursorOn.load())
		{
			{
				QueueWriteGuard guard(queueLock);
				listOfOutputSamples.softClear();
			}
			pushSearch();
		}
	}
}

void DatabasePlayer::onMouseWheel(TShiftState Shift, int WheelDelta)
{
	if (isCursorOn.load() || mouseInfo.leftBtnDown.load())
		circleCursor.setRadius(clampRadius(circleCursor.radius.load() + static_cast<float>(WheelDelta) * 0.005f));
}

void DatabasePlayer::pushSearch()
{
	if (!searchPending.exchange(true))
	{
		if (!actionQueue.push([this]() { searchTask(); }))
			searchPending.store(false);
	}
}

void DatabasePlayer::pushRequest()
{
	if (!requestPending.exchange(true))
	{
		if (!actionQueue.push([this]() { requestTask(); }))
			requestPending.store(false);
	}
}

// the tasks below are run by the work thread, see ActionQueue

// opens a database in place of the current one, then places its samples
void DatabasePlayer::openDatabaseTask(const std::string& fileName)
{
	isLoading.store(true, std::memory_order_release);
	try
	{
		if (isDBOpen.load())
		{
			if (closeDatabase(this) != SQLITE_OK)
				sdkReport(ERROR_CLOSE_DATABASE, sqlite3_errmsg(db));
			else
				isDBOpen.store(false);
		}
		if (!isDBOpen.load())
		{
			if (fileExists(fileName))
			{
				if (sqlite3_open(fileName.c_str(), &db) != SQLITE_OK)
				{
					sdkReport(ERROR_OPEN_DATABASE, sqlite3_errmsg(db));
					// the connection of a failed open must be freed too
					closeDatabase(this);
				}
				else
				{
					sqlite3_progress_handler(db, 10000, progressHandler, this);
					const std::string openingDbString = std::string("Opening database at : \n") + fileName;
					sdkProgressBar(openingDbString.c_str(), sdkAlphaColor(1.0f, 1.0f, 1.0f, 1.0f));
					if (openDB())
					{
						isDBOpen.store(true);
						pushRequest();
					}
					else
					{
						if (!actionQueue.isStopping())
							sdkReport(ERROR_LOAD_FAILED);
						closeDatabase(this);
					}
				}
			}
			else
			{
				sdkReport(ERROR_DATABASE_NOT_FOUND, fileName);
			}
		}
	}
	catch (const std::exception& e)
	{
		sdkReport(ERROR_LOADING, e.what());
	}
	isLoading.store(false, std::memory_order_release);
}

// places the samples on the panel according to the dimensions
void DatabasePlayer::requestTask()
{
	// a dimension changed from now on is requested again
	requestPending.store(false);
	try
	{
		if (isDBOpen.load())
		{
			const std::string firstDimension  = FIRST_DIMENSION_COLUMNS[static_cast<size_t>(firstDimensionItem.load())];
			const std::string secondDimension = SECOND_DIMENSION_COLUMNS[static_cast<size_t>(secondDimensionItem.load())];
			if (sendRequestToDB(firstDimension, secondDimension))
			{
				sdkRepaintPanel();
			}
			else if (!actionQueue.isStopping())
			{
				sdkReport(ERROR_REQUEST_FAILED);
				closeDatabase(this);
				isDBOpen.store(false);
			}
		}
	}
	catch (const std::exception& e)
	{
		sdkReport(ERROR_REQUEST, e.what());
	}
}

// selects the samples inside the cursor circle, or releases every sample when the cursor is up
void DatabasePlayer::searchTask()
{
	// a move from now on is searched again
	searchPending.store(false);
	try
	{
		if (isCursorOn.load() || mouseInfo.leftBtnDown.load())
		{
			findEveryNearestPoint(circleCursor, listOfSamplesInCircle);
		}
		else
		{
			QueueWriteGuard guard(queueLock);
			listOfOutputSamples.clearSamplesToRelease();
			if (listOfOutputSamples.activeSamples > 0)
				listOfOutputSamples.softClear();
			listOfOutputSamples.publishText();
		}
	}
	catch (const std::exception& e)
	{
		sdkReport(ERROR_SEARCH, e.what());
	}
}

bool DatabasePlayer::sendRequestToDB(const std::string firstDimension, const std::string secondDimension)
{
	try
	{
		std::string query = queryBuilder(firstDimension, secondDimension);

		const float          factorX            = spreadX.load(std::memory_order_relaxed);
		const float          factorY            = spreadY.load(std::memory_order_relaxed);
		std::vector<Point2D> pendingRequestData = requestData;

		std::vector<TPointF>     pendingPointsToDraw = pointsToDraw;
		std::vector<TUsineColor> pendingColorsToDraw = colorsToDraw;

		{
			Statement stmt(db, query);
			if (!stmt.isPrepared())
			{
				sdkReport(ERROR_PREPARE_REQUEST, std::string(sqlite3_errmsg(db)) + ", query: " + query);
				return false;
			}

			int stepResult = SQLITE_ROW;
			while ((stepResult = sqlite3_step(stmt.get())) == SQLITE_ROW)
			{
				float normX = static_cast<float>(sqlite3_column_double(stmt.get(), 0));
				float normY = static_cast<float>(sqlite3_column_double(stmt.get(), 1));
				int   id    = sqlite3_column_int(stmt.get(), 2);

				// the samples are sorted by id, which can have gaps (rows deleted from the database)
				const auto sample = std::lower_bound(pendingRequestData.begin(), pendingRequestData.end(), id, [](const Point2D& point, int value) {
					return point.uID < value;
				});
				const auto index = static_cast<size_t>(sample - pendingRequestData.begin());
				if (sample != pendingRequestData.end() && sample->uID == id && index < pendingPointsToDraw.size() && index < pendingColorsToDraw.size())
				{
					float coordX                    = powf(normX, factorX);
					float coordY                    = powf(normY, factorY);
					pendingRequestData[index].dim1  = coordX;
					pendingRequestData[index].dim2  = 1.0f - coordY;
					pendingRequestData[index].color = xy_to_rgb_colorwheel_radial(coordX, coordY);

					pendingPointsToDraw[index] = TPointF {.x = coordX, .y = 1.0f - coordY};
					pendingColorsToDraw[index] = pendingRequestData[index].color;
				}
				else
				{
					sdkReport(ERROR_INVALID_SAMPLE_ID, std::to_string(id));
					return false;
				}
			}
			if (stepResult != SQLITE_DONE)
			{
				if (!actionQueue.isStopping())
					sdkReport(ERROR_REQUEST, sqlite3_errmsg(db));
				return false;
			}
		}

		std::vector<Point2D*> ptrVector;
		ptrVector.reserve(pendingRequestData.size());
		for (auto& point : pendingRequestData)
			ptrVector.push_back(&point);

		KDTree pendingTree(2);
		pendingTree.root = pendingTree.buildTree(ptrVector, 0);
		if (!pendingTree.root)
			return false;

		// only moves under the lock, they do not throw
		sdkCriticalSectionLock(csPaint);

		// the moved vector keeps its buffer: the nodes of the tree still point to the samples
		requestData  = std::move(pendingRequestData);
		kdTree.root  = std::move(pendingTree.root);
		pointsToDraw = std::move(pendingPointsToDraw);
		colorsToDraw = std::move(pendingColorsToDraw);

		sdkCriticalSectionUnLock(csPaint);
	}
	catch (const std::exception& e)
	{
		sdkReport(ERROR_REQUEST, e.what());
		return false;
	}

	return true;
}

// each coordinate is the value of its column normalized between its minimum and maximum in the FileResults table:
// (x - min) / (max - min), or a random value between 0 and 1 for random
std::string DatabasePlayer::queryBuilder(const std::string& firstDimension, const std::string& secondDimension) const
{
	std::string queryFunction = "SELECT\n";
	std::string normX, normY;
	std::string subSelectFunc, subSelectNorm, subSelectEnd;
	bool        needsMinMax = (firstDimension != "random" || secondDimension != "random");

	if (firstDimension == "random")
	{
		normX = "(abs(random()) / 9223372036854775807.0) AS norm_x,\n";
	}
	else
	{
		normX = "(" + firstDimension + " - min_max.min_" + firstDimension + ") / " + "(min_max.max_" + firstDimension + " - min_max.min_" + firstDimension + ") AS norm_x,\n";

		subSelectNorm += "MIN(" + firstDimension + ") AS min_" + firstDimension + ", MAX(" + firstDimension + ") AS max_" + firstDimension;
	}

	if (secondDimension == "random")
	{
		normY = "(abs(random()) / 9223372036854775807.0) AS norm_y,\n";
	}
	else
	{
		normY = "(" + secondDimension + " - min_max.min_" + secondDimension + ") / " + "(min_max.max_" + secondDimension + " - min_max.min_" + secondDimension + ") AS norm_y,\n";

		if (!subSelectNorm.empty())
			subSelectNorm += ",\n";
		subSelectNorm += "MIN(" + secondDimension + ") AS min_" + secondDimension + ", MAX(" + secondDimension + ") AS max_" + secondDimension;
	}

	if (needsMinMax)
	{
		subSelectFunc = "(SELECT\n";
		subSelectEnd  = "\nFROM FileResults) AS min_max";
	}

	std::string finalQuery;
	finalQuery += queryFunction;
	finalQuery += normX;
	finalQuery += normY;
	finalQuery += "id\nFROM FileResults";
	if (needsMinMax)
	{
		finalQuery += ",\n";
		finalQuery += subSelectFunc;
		finalQuery += subSelectNorm;
		finalQuery += subSelectEnd;
	}
	finalQuery += ";\n";

	return finalQuery;
}

void DatabasePlayer::findEveryNearestPoint(const CircleCursor& circleCursor, std::vector<PointDistance2D>& listOfSamplesInCircle)
{
	// the tree is empty from the loading of a database to its request
	listOfSamplesInCircle.clear();
	kdTree.searchInRange(circleCursor.x.load(), circleCursor.y.load(), circleCursor.radius.load(), listOfSamplesInCircle);
	// the closest samples first
	std::sort(listOfSamplesInCircle.begin(), listOfSamplesInCircle.end(), std::greater<>());

	QueueWriteGuard guard(queueLock);
	listOfOutputSamples.isCoeffNormalized = normalizeCoeffs.load(std::memory_order_relaxed);
	listOfOutputSamples.clearSamplesToRelease();
	listOfOutputSamples.compareListsAndUpdate(listOfSamplesInCircle);

	// when the circle holds more samples than the polyphony, they are taken at regular steps
	int step = static_cast<int>(listOfSamplesInCircle.size()) / listOfOutputSamples.NbPoints;
	if (step == 0)
		step = 1;
	for (size_t i = 0; i < listOfSamplesInCircle.size(); i = i + step)
	{
		const Point2D* point = listOfSamplesInCircle[i].point;
		// the samples whose magnitudes do not match the FFT size are not played
		if (point == nullptr || point->mag == nullptr || point->mag->vec.size() < static_cast<size_t>(SPECTRUM_SIZE_2))
			continue;

		if (listOfOutputSamples.isInQueue(point->uID) < 0)
		{
			// with no empty slot, the sample is not played
			int idx = listOfOutputSamples.findEmptySpot();
			if (idx >= 0)
			{
				// without normalization, the gain is the squared closeness to the center of the cursor
				listOfOutputSamples.putInQueue(point->samplePath, point->uID, point->mag->vec.data(), listOfSamplesInCircle[i].distance * listOfSamplesInCircle[i].distance, idx);
			}
		}
	}
	listOfOutputSamples.publishText();
}

bool DatabasePlayer::openDB()
{
	try
	{
		// FFT size of the analysis made by Database Creation
		int spectrumSize = 0;
		{
			Statement stmtFFT(db, "SELECT fftSizeValue FROM FileFFTSize ");
			if (!stmtFFT.isPrepared())
			{
				sdkReport(ERROR_READ_FFT_SIZE, sqlite3_errmsg(db));
				return false;
			}
			if (sqlite3_step(stmtFFT.get()) != SQLITE_ROW)
			{
				sdkReport(ERROR_NO_FFT_SIZE);
				return false;
			}
			spectrumSize = sqlite3_column_int(stmtFFT.get(), 0);
		}
		// the FFT of Usine computes powers of two
		if (spectrumSize < MIN_SPECTRUM || spectrumSize > MAX_SPECTRUM || (spectrumSize & (spectrumSize - 1)) != 0)
		{
			sdkReport(ERROR_INVALID_FFT_SIZE, std::to_string(spectrumSize));
			return false;
		}
		const int bins = spectrumSize / 2;

		// the samples, with the mean of the magnitudes of their spectrum
		int pointCount = 0;
		{
			Statement stmtCount(db, "SELECT COUNT(*) FROM FileResults");
			if (stmtCount.isPrepared() && sqlite3_step(stmtCount.get()) == SQLITE_ROW)
				pointCount = sqlite3_column_int(stmtCount.get(), 0);
		}

		std::vector<Point2D> pendingRequestPoints;
		pendingRequestPoints.reserve(static_cast<size_t>(std::max(pointCount, 0)));
		int invalidMagnitudes = 0;
		int nanMagnitudes     = 0;
		{
			Statement stmtPoints(db, "SELECT id, filePath, mag_mean, tsne_x, tsne_y FROM FileResults ORDER BY id ASC");
			if (!stmtPoints.isPrepared())
			{
				sdkReport(ERROR_READ_SAMPLES, sqlite3_errmsg(db));
				return false;
			}

			int stepResult = SQLITE_ROW;
			while ((stepResult = sqlite3_step(stmtPoints.get())) == SQLITE_ROW)
			{
				int id = sqlite3_column_int(stmtPoints.get(), 0);

				const unsigned char* filepath_text = sqlite3_column_text(stmtPoints.get(), 1);
				std::string          filePath      = filepath_text ? reinterpret_cast<const char*>(filepath_text) : "";

				const void* magBlob     = sqlite3_column_blob(stmtPoints.get(), 2);
				int         magBlobSize = sqlite3_column_bytes(stmtPoints.get(), 2);

				float normX = static_cast<float>(sqlite3_column_double(stmtPoints.get(), 3));
				float normY = static_cast<float>(sqlite3_column_double(stmtPoints.get(), 4));

				Point2D point;
				point.samplePath = filePath;
				point.uID        = id;
				point.dim1       = normX;
				point.dim2       = normY;

				// mag_mean holds size/2 floats (Database Creation before 2026-04-30) or size/2+1, with the Nyquist
				// bin; the resynthesis uses the size/2 first ones. Another size comes from another FFT size: not played
				const int floats = magBlobSize / static_cast<int>(sizeof(float));
				if (magBlob != nullptr && magBlobSize % static_cast<int>(sizeof(float)) == 0 && (floats == bins || floats == bins + 1))
				{
					std::vector<float> magData(static_cast<size_t>(bins));
					std::memcpy(magData.data(), magBlob, magData.size() * sizeof(float));

					// the magnitudes of a silent file are not numbers: 0 instead
					bool notNumbers = false;
					for (float& magnitude : magData)
					{
						if (!std::isfinite(magnitude))
						{
							magnitude  = 0.0f;
							notNumbers = true;
						}
					}
					if (notNumbers)
						nanMagnitudes++;

					point.mag = std::make_shared<PointMag>(PointMag {std::move(magData), id});
				}
				else
				{
					invalidMagnitudes++;
				}
				pendingRequestPoints.push_back(std::move(point));
			}
			if (stepResult != SQLITE_DONE)
			{
				if (!actionQueue.isStopping())
					sdkReport(ERROR_READ_SAMPLES, sqlite3_errmsg(db));
				return false;
			}
		}

		const int count = static_cast<int>(pendingRequestPoints.size());
		if (count == 0)
		{
			sdkReport(ERROR_NO_SAMPLES);
			return false;
		}
		if (invalidMagnitudes == count)
		{
			sdkReport(ERROR_NO_MAGNITUDES);
			return false;
		}
		if (invalidMagnitudes > 0)
			sdkReport(WARNING_INVALID_MAGNITUDES, std::to_string(invalidMagnitudes));
		if (nanMagnitudes > 0)
			sdkReport(WARNING_NAN_MAGNITUDES, std::to_string(nanMagnitudes));

		// the FFT of the new size, before the previous database is replaced
		TUsineFFTPtr newFFT = sdkFFTCreate(static_cast<UINT32>(spectrumSize));
		if (newFFT == nullptr)
		{
			sdkReport(ERROR_CREATE_FFT, std::to_string(spectrumSize));
			return false;
		}
		if (ifftPtr != nullptr)
			sdkFFTDestroy(ifftPtr);
		ifftPtr = newFFT;

		std::vector<TPointF>     pendingPointsToDraw(static_cast<size_t>(count));
		std::vector<TUsineColor> pendingColorsToDraw(static_cast<size_t>(count));

		// the new database replaces the previous one: nothing plays the previous samples any more, and the tree is
		// empty until the request places the new ones
		{
			QueueWriteGuard guard(queueLock);
			listOfOutputSamples.hardClear();
			listOfOutputSamples.publishText();
		}

		// only moves under the lock, they do not throw
		sdkCriticalSectionLock(csPaint);
		kdTree.root.reset();
		requestData          = std::move(pendingRequestPoints);
		pointsToDraw         = std::move(pendingPointsToDraw);
		colorsToDraw         = std::move(pendingColorsToDraw);
		numberOfPointsToDraw = count;
		sdkCriticalSectionUnLock(csPaint);

		// the resynthesis at the FFT size of the database
		SPECTRUM_SIZE   = spectrumSize;
		SPECTRUM_SIZE_2 = SPECTRUM_SIZE / 2;
		SPECTRUM_SIZE_4 = SPECTRUM_SIZE / 4;
		// the inverse FFT reads size/2+1 real values, the last one (Nyquist) stays at 0
		rReal.assign(static_cast<size_t>(SPECTRUM_SIZE_2 + 1), 0.0f);
		rIm.assign(static_cast<size_t>(SPECTRUM_SIZE_2), 0.0f);
		lReal.assign(static_cast<size_t>(SPECTRUM_SIZE_2 + 1), 0.0f);
		lIm.assign(static_cast<size_t>(SPECTRUM_SIZE_2), 0.0f);
		rTempBuffer.assign(static_cast<size_t>(SPECTRUM_SIZE), 0.0f);
		lTempBuffer.assign(static_cast<size_t>(SPECTRUM_SIZE), 0.0f);
		spectrumMags.assign(static_cast<size_t>(SPECTRUM_SIZE_2), 0.0f);
		for (auto& channel : overlapAdd)
			channel.assign(static_cast<size_t>(SPECTRUM_SIZE), 0.0f);
		InitializeHanningWindow(hanning, SPECTRUM_SIZE);
		hopSize.store(SPECTRUM_SIZE_4, std::memory_order_relaxed);
	}
	catch (const std::exception& e)
	{
		sdkReport(ERROR_LOADING, e.what());
		return false;
	}

	return true;
}

// the state from the current values of the parameters and of the settings, in onInitModule and onAfterLoading:
// harmless when a callback did the same before
void DatabasePlayer::readParameters(bool fromLoading)
{
	const bool spreadChanged = refreshSettings();

	const int  firstItem         = dimensionItem(firstDimensionLboxIn);
	const int  secondItem        = dimensionItem(secondDimensionLboxIn);
	const bool firstChanged      = firstDimensionItem.exchange(firstItem) != firstItem;
	const bool secondChanged     = secondDimensionItem.exchange(secondItem) != secondItem;
	const bool dimensionsChanged = firstChanged || secondChanged;

	setPolyphony(polyphonyFdrIn.getData());
	updateFadeDurations();
	activeResynth.store(onResynthesisSwIn.getData() != 0.0f, std::memory_order_relaxed);
	isCursorOn.store(cursorDownSwIn.getData() != 0.0f);
	if (!mouseInfo.leftBtnDown.load())
	{
		circleCursor.x.store(cursorXFdrIn.getData());
		circleCursor.y.store(cursorYFdrIn.getData());
	}
	circleCursor.setRadius(clampRadius(cursorRadiusFdrIn.getData() * 0.01f));

	if (fromLoading)
	{
		if (dimensionsChanged || spreadChanged)
			pushRequest();
		pushSearch();
		sdkRepaintPanel();
	}
}

// the work thread reads copies of the settings, the host writes the bound values in the main thread;
// returns true when a spread factor changed
bool DatabasePlayer::refreshSettings()
{
	const float factorX = std::isfinite(spreadFactorX) ? std::clamp(spreadFactorX, 0.1f, 1.0f) : 0.25f;
	const float factorY = std::isfinite(spreadFactorY) ? std::clamp(spreadFactorY, 0.1f, 1.0f) : 0.25f;
	const bool  changedX = spreadX.exchange(factorX) != factorX;
	const bool  changedY = spreadY.exchange(factorY) != factorY;
	normalizeCoeffs.store(normalizeCoeffsSetting != FALSE, std::memory_order_relaxed);
	return changedX || changedY;
}

void DatabasePlayer::setPolyphony(float value)
{
	const int polyphony = std::isfinite(value) ? static_cast<int>(std::clamp(value, static_cast<float>(POLYPHONY_MIN), static_cast<float>(POLYPHONY_MAX))) : DEFAULT_POLYPHONY;

	QueueWriteGuard guard(queueLock);
	listOfOutputSamples.setNbPoints(polyphony);
	listOfOutputSamples.publishText();
}

// the fades are counted in blocs: their length follows the bloc duration
void DatabasePlayer::updateFadeDurations()
{
	const double rate         = sampleRate.load(std::memory_order_relaxed);
	const float  blocDuration = (rate > 0.0 && blocSize > 0) ? (static_cast<float>(blocSize) / static_cast<float>(rate)) * 1000.0f : 0.0f;
	const float  attack       = std::max(0.0f, attackTimeFdrIn.getData());
	const float  release      = std::max(0.0f, releaseTimeFdrIn.getData());

	QueueWriteGuard guard(queueLock);
	listOfOutputSamples.setDurations(attack, release, blocDuration);
}

void DatabasePlayer::generateLUT()
{
	int N = LUT_SIZE;
	trigoLUT.resize(N);

	for (int i = 0; i < N; i++)
	{
		float theta     = 2.0f * std::numbers::pi_v<float> * static_cast<float>(i) / static_cast<float>(N);
		trigoLUT[i].cos = cosf(theta);
		trigoLUT[i].sin = sinf(theta);
	}
}

void DatabasePlayer::InitializeHanningWindow(std::vector<float>& hanningWindow, int size)
{
	hanningWindow.resize(size);
	for (int i = 0; i < size; ++i)
	{
		hanningWindow[i] = 0.5f * (1.0f - cosf(2.0f * std::numbers::pi_v<float> * i / (size - 1)));
		hanningWindow[i] /= size;
	}
}

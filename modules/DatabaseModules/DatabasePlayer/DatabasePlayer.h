//-----------------------------------------------------------------------------
// @file    DatabasePlayer.h
// @author  BrainModular team
// @brief   Plays a sound database: 2D map of the samples, resynthesis of the samples under the cursor.
//
// This file is part of the Usine Hollyhock CPP SDK.
// Copyright (C) 2013-2026 BrainModular, BeSpline, Adamson
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
#pragma once

#include <algorithm>
#include <array>
#include <atomic>
#include <cassert>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <mutex>
#include <queue>
#include <random>
#include <string>
#include <thread>
#include <vector>
#if !defined(__APPLE__)
#include <semaphore>
#endif
#include "../../../sdk/UserDefinitions.h"
#include "../libs/sqlite3lib/sqlite3.h"
#include "../libs/utils/KDTree.h"

constexpr int DEFAULT_PANEL_HEIGHT = 500;
constexpr int DEFAULT_PANEL_WIDTH  = 500;

// size of the table of the random phases of the resynthesis
constexpr int LUT_SIZE = 10000;
// a released sample leaves the queue once its gain is below this threshold
constexpr float COEFF_THRESHOLD = 0.01f;
// slots of the samples queue: twice the maximum of [polyphony]
constexpr int SAMPLE_QUEUE_MAX_POLY = 40;
// range and default of [polyphony], see polyphonyFdrInParamInfo
constexpr int POLYPHONY_MIN     = 1;
constexpr int POLYPHONY_MAX     = SAMPLE_QUEUE_MAX_POLY / 2;
constexpr int DEFAULT_POLYPHONY = 10;
// FFT size of the resynthesis until a database gives its own
constexpr int DEFAULT_SPECTRUM = 16384;
// FFT sizes accepted from a database: the powers of two the Usine FFT computes
constexpr int MIN_SPECTRUM = 32;
constexpr int MAX_SPECTRUM = 65536;
// samples per channel between the work thread and the audio thread (a power of two): the lead of the
// resynthesis (a bloc and 10 ms) plus a hop, at the largest FFT size and 192 kHz
constexpr int OUTPUT_FIFO_SIZE = 65536;
// the work thread sleeps until it is woken up, at most this time
constexpr int WORK_THREAD_IDLE_MS = 100;
// the work thread computes the frames a bloc and LEAD_MARGIN_SECONDS ahead of the audio thread; each underrun of a
// running stream adds max(bloc, LEAD_STEP_SECONDS), up to LEAD_EXTRA_MAX_SECONDS
constexpr double LEAD_MARGIN_SECONDS    = 0.010;
constexpr double LEAD_STEP_SECONDS      = 0.005;
constexpr double LEAD_EXTRA_MAX_SECONDS = 0.040;

// the cursor circle, in coordinates relative to the panel; the main thread moves it, the work thread reads it
struct CircleCursor
{
	std::atomic<float> x {0.0f};
	std::atomic<float> y {0.0f};
	std::atomic<float> radius {50.0f / (2.0f * static_cast<float>(DEFAULT_PANEL_WIDTH))};

	void setRadius(float newRadius)
	{
		this->radius.store(newRadius, std::memory_order_relaxed);
	}
};

// while the left button is down on the panel, the mouse moves the cursor
struct MouseInfo
{
	std::atomic<bool> leftBtnDown {false};
};

// linear smoothing of the gain of a sample of the queue, updated once per bloc
struct SmoothCoeff
{
	float current        = 0.0f;
	float target         = 0.0f;
	float step           = 0.0f;
	int   remainingBlocs = 0;

	void setTarget(float newTarget, float duration, float blocDuration)
	{
		target = newTarget;
		if (duration <= 0.0f || blocDuration <= 0.0f)
		{
			current        = target;
			remainingBlocs = 0;
			step           = 0.0f;
			return;
		}

		remainingBlocs = static_cast<int>(duration / blocDuration);
		if (remainingBlocs <= 0)
		{
			current        = target;
			remainingBlocs = 0;
			step           = 0.0f;
			return;
		}

		step = (target - current) / static_cast<float>(remainingBlocs);
	}

	// the bloc duration changed: the ramp in progress ends at the time it would have ended
	void rescale(float oldBlocDuration, float newBlocDuration)
	{
		if (remainingBlocs <= 0 || oldBlocDuration <= 0.0f || newBlocDuration <= 0.0f)
			return;
		setTarget(target, static_cast<float>(remainingBlocs) * oldBlocDuration, newBlocDuration);
	}

	void update()
	{
		if (remainingBlocs > 0)
		{
			current += step;
			remainingBlocs--;
			if (remainingBlocs == 0)
				current = target;
		}
	}

	[[nodiscard]] float getCoeff() const
	{
		return current;
	}
};

// the samples queue is written by the work thread and the main thread, which wait for each other, and read
// and smoothed by the audio thread, which never waits: when a writer holds the queue, it skips its update
class AudioTryLock
{
public:
	void lockWriter()
	{
		writers.lock();
		// the audio thread holds the queue for a few microseconds at most
		while (busy.exchange(true, std::memory_order_acquire))
			std::this_thread::yield();
	}

	void unlockWriter()
	{
		busy.store(false, std::memory_order_release);
		writers.unlock();
	}

	// audio thread: true when the queue is taken
	[[nodiscard]] bool tryLockAudio()
	{
		return !busy.exchange(true, std::memory_order_acquire);
	}

	void unlockAudio()
	{
		busy.store(false, std::memory_order_release);
	}

private:
	std::mutex        writers;
	std::atomic<bool> busy {false};
};

class QueueWriteGuard
{
public:
	explicit QueueWriteGuard(AudioTryLock& lock) : lock(lock)
	{
		lock.lockWriter();
	}
	~QueueWriteGuard()
	{
		lock.unlockWriter();
	}
	QueueWriteGuard(const QueueWriteGuard&)            = delete;
	QueueWriteGuard& operator=(const QueueWriteGuard&) = delete;

private:
	AudioTryLock& lock;
};

// resynthesized samples of the 2 audio outputs, from the work thread (the only writer) to the audio thread
// (the only reader), without lock: the counters published with release/acquire order the samples
class OutputFifo
{
public:
	// work thread
	[[nodiscard]] int freeSpace() const
	{
		return OUTPUT_FIFO_SIZE - static_cast<int>(writeCount.load(std::memory_order_relaxed) - readCount.load(std::memory_order_acquire));
	}

	void write(const float* out1, const float* out2, int count)
	{
		const size_t start = writeCount.load(std::memory_order_relaxed);
		for (int i = 0; i < count; i++)
		{
			data[0][(start + static_cast<size_t>(i)) & MASK] = out1[i];
			data[1][(start + static_cast<size_t>(i)) & MASK] = out2[i];
		}
		writeCount.store(start + static_cast<size_t>(count), std::memory_order_release);
	}

	// audio thread
	[[nodiscard]] int available() const
	{
		return static_cast<int>(writeCount.load(std::memory_order_acquire) - readCount.load(std::memory_order_relaxed));
	}

	// returns the number of samples read, at most count
	int read(float* out1, float* out2, int count)
	{
		const size_t start = readCount.load(std::memory_order_relaxed);
		const int    n     = std::min(count, static_cast<int>(writeCount.load(std::memory_order_acquire) - start));
		for (int i = 0; i < n; i++)
		{
			out1[i] = data[0][(start + static_cast<size_t>(i)) & MASK];
			out2[i] = data[1][(start + static_cast<size_t>(i)) & MASK];
		}
		readCount.store(start + static_cast<size_t>(n), std::memory_order_release);
		return n;
	}

	// drops every sample written
	void flush()
	{
		readCount.store(writeCount.load(std::memory_order_acquire), std::memory_order_release);
	}

private:
	static constexpr size_t MASK = OUTPUT_FIFO_SIZE - 1;
	static_assert((OUTPUT_FIFO_SIZE & (OUTPUT_FIFO_SIZE - 1)) == 0);

	std::array<std::vector<float>, 2> data {std::vector<float>(OUTPUT_FIFO_SIZE, 0.0f), std::vector<float>(OUTPUT_FIFO_SIZE, 0.0f)};
	// samples written and read since the start, they wrap around the power of two size
	std::atomic<size_t> writeCount {0};
	std::atomic<size_t> readCount {0};
};

// wakes the work thread up. signal() takes no lock and allocates nothing: the audio thread calls it. A libdispatch
// semaphore on macOS (std::counting_semaphore needs macOS 11, the modules target 10.13), std::counting_semaphore elsewhere
class WakeUpSignal
{
public:
	WakeUpSignal();
	~WakeUpSignal();
	WakeUpSignal(const WakeUpSignal&)            = delete;
	WakeUpSignal& operator=(const WakeUpSignal&) = delete;

	void signal();
	// until signaled, or the timeout
	void wait(int timeoutMs);

private:
#if defined(__APPLE__)
	// a dispatch_semaphore_t, see DatabasePlayer.cpp: <dispatch/dispatch.h> defines TRUE and FALSE, declared by the SDK too
	void* semaphore = nullptr;
#else
	std::counting_semaphore<> semaphore {0};
#endif
};

// FIFO queue of the actions run one after the other by the work thread, at most 100 pending actions, protected
// by a mutex. The frames of the resynthesis requested by the audio thread are computed before the other actions:
// the audio thread only increments a counter and signals the work thread, without lock nor allocation.
class ActionQueue
{
public:
	// before run: the task computing one frame
	void setFrameTask(std::function<void()> task)
	{
		frameTask = std::move(task);
	}

	// false when the queue is full
	bool push(std::function<void()> task)
	{
		{
			std::lock_guard<std::mutex> lock(m_Mutex);
			if (m_TaskQueue.size() >= m_MaxQueueSize)
				return false;
			m_TaskQueue.push(std::move(task));
		}
		m_WakeUp.signal();
		return true;
	}

	// audio thread
	void requestFrames(int count)
	{
		m_FramesRequested.fetch_add(count, std::memory_order_release);
		m_WakeUp.signal();
	}

	// frames requested and not computed yet, the one being computed included
	[[nodiscard]] int pendingFrames() const
	{
		return m_FramesRequested.load(std::memory_order_acquire);
	}

	// work thread, after a frame: its samples are in the FIFO
	void frameDone()
	{
		m_FramesRequested.fetch_sub(1, std::memory_order_release);
	}

	// runs the actions until stop is called and the queue is empty
	void run()
	{
		m_IsWorking.store(true, std::memory_order_relaxed);

		while (true)
		{
			std::function<void()> task;
			bool                  frame = false;
			{
				std::lock_guard<std::mutex> lock(m_Mutex);
				if (m_StopFlag.load() && m_TaskQueue.empty())
					break;

				if (m_FramesRequested.load(std::memory_order_acquire) > 0 && frameTask)
					frame = true;
				else if (!m_TaskQueue.empty())
				{
					task = std::move(m_TaskQueue.front());
					m_TaskQueue.pop();
				}
			}

			if (frame)
				frameTask();
			else if (task)
				task();
			else
				m_WakeUp.wait(WORK_THREAD_IDLE_MS);
		}
	}

	void clear()
	{
		std::lock_guard<std::mutex> lock(m_Mutex);
		for (; !m_TaskQueue.empty(); m_TaskQueue.pop())
		{
		}
	}

	void stop()
	{
		{
			std::lock_guard<std::mutex> lock(m_Mutex);
			m_StopFlag = true;
		}
		m_WakeUp.signal();
	}

	// the long tasks give up when the module is deleted
	[[nodiscard]] bool isStopping() const
	{
		return m_StopFlag.load(std::memory_order_relaxed);
	}

	std::atomic<bool> m_IsWorking {false};

private:
	std::queue<std::function<void()>> m_TaskQueue;
	std::mutex                        m_Mutex;
	WakeUpSignal                      m_WakeUp;
	std::atomic<bool>                 m_StopFlag {false};
	std::atomic<int>                  m_FramesRequested {0};
	size_t                            m_MaxQueueSize = 100;
	std::function<void()>             frameTask;
};

class DatabasePlayer : public UserModuleBase
{
public:
	// a slot of the samples queue: a sample with its gain and its magnitudes
	struct SampleString
	{
		std::string  sample = "null";
		// id of the sample in the database, compared instead of the path
		int          uID = -1;
		SmoothCoeff  coeff;
		const float* magData     = nullptr;
		bool         isToRelease = false;

		[[nodiscard]] bool isEmpty() const
		{
			return (sample == "null" && magData == nullptr);
		}

		[[nodiscard]] bool isToClear(float threshold) const
		{
			return (this->coeff.getCoeff() < threshold);
		}

		void clear()
		{
			coeff.setTarget(0.0f, 0.0f, 0.0f);
			sample      = "null";
			uID         = -1;
			magData     = nullptr;
			isToRelease = false;
		}
	};

	// polyphonic queue of the samples played: NbPoints active samples at most, and as many released samples
	// fading out, in the maxPoly = 2 x NbPoints first slots; the slots above always stay empty.
	// Protected by queueLock, see AudioTryLock.
	struct SampleQueue
	{
		std::vector<SampleString> queue          = std::vector<SampleString>(SAMPLE_QUEUE_MAX_POLY);
		int                       maxPoly        = 2 * DEFAULT_POLYPHONY;
		int                       NbPoints       = DEFAULT_POLYPHONY;
		float                     smoothDuration = 0.0f;
		float                     attackDuration = 0.0f;
		float                     blocDuration   = 0.0f;
		int                       availSpace     = DEFAULT_POLYPHONY;
		// the normalize coeffs setting, see DatabasePlayer::normalizeCoeffs
		bool  isCoeffNormalized = true;
		float normalizedCoeff   = 1.0f;
		int   activeSamples     = 0;
		float sumCoeffs         = 0.0f;
		// comma text of the slots, rebuilt by the writers once a slot changes, taken by the audio thread
		std::string pendingText;
		bool        textChanged  = false;
		bool        slotsChanged = true;

		// a lower polyphony empties the slots above 2 x [polyphony]: their samples move to free slots below,
		// with their gain, or stop when there is none
		void setNbPoints(const int newNbPoints)
		{
			NbPoints = std::clamp(newNbPoints, POLYPHONY_MIN, POLYPHONY_MAX);
			maxPoly  = 2 * NbPoints;

			int freeSlot = 0;
			for (int i = maxPoly; i < static_cast<int>(queue.size()); i++)
			{
				if (queue[i].isEmpty())
					continue;
				while (freeSlot < maxPoly && !queue[freeSlot].isEmpty())
					freeSlot++;
				if (freeSlot < maxPoly)
					std::swap(queue[freeSlot], queue[i]);
				queue[i].clear();
			}
			slotsChanged = true;
			updateAvailSpace();
		}

		// compares the queue with the samples found in the cursor circle: a sample of the queue no longer
		// found is released, the target gain of the others is updated. A released sample found again plays
		// again only when the polyphony allows it, else it keeps fading out.
		void compareListsAndUpdate(const std::vector<PointDistance2D>& closestPoints)
		{
			for (int i = 0; i < maxPoly; i++)
			{
				if (queue[i].isEmpty())
					continue;

				bool toClear = true;
				for (size_t j = 0; j < closestPoints.size(); j++)
				{
					if (queue[i].uID == closestPoints[j].point->uID)
					{
						if (queue[i].isToRelease && availSpace > 0)
						{
							queue[i].isToRelease = false;
							updateAvailSpace();
						}
						toClear = false;
						if (queue[i].isToRelease)
							continue;
						if (isCoeffNormalized)
							updateTargetCoeff(normalizedCoeff, i);
						else
							updateTargetCoeff(closestPoints[j].distance * closestPoints[j].distance, i);
					}
				}

				if (toClear && !queue[i].isToRelease)
				{
					queue[i].isToRelease = true;
					queue[i].coeff.setTarget(0.0f, smoothDuration, blocDuration);
					updateAvailSpace();
				}
			}
		}

		// index of the sample in the queue, -1 if not found
		[[nodiscard]] int isInQueue(const int uID) const
		{
			for (int i = 0; i < maxPoly; i++)
			{
				if (queue[i].uID == uID && !queue[i].isEmpty())
					return i;
			}
			return -1;
		}

		// index of the first empty slot of the queue, -1 if there is none
		[[nodiscard]] int findEmptySpot() const
		{
			if (availSpace <= 0)
				return -1;

			for (int i = 0; i < maxPoly; i++)
			{
				if (queue[i].isEmpty())
					return i;
			}
			return -1;
		}

		// puts a sample in a valid empty slot of the queue, the sample must not be in the queue yet
		void putInQueue(const std::string& string, const int uID, const float* magPtr, const float coeff, int index)
		{
			assert(availSpace >= 0);
			assert(magPtr);
			assert(queue[index].magData == nullptr);

			queue[index].sample  = string;
			queue[index].uID     = uID;
			queue[index].magData = magPtr;
			slotsChanged         = true;

			updateAvailSpace();

			// the sample fades in over the attack time
			if (isCoeffNormalized)
				queue[index].coeff.setTarget(normalizedCoeff, attackDuration, blocDuration);
			else
				queue[index].coeff.setTarget(coeff, attackDuration, blocDuration);
		}

		// a higher target is reached in the attack time, a lower one in the release time. A ramp towards the same
		// target goes on: restarted by every search (20 ms), it would last longer than the attack or release time
		void updateTargetCoeff(const float coeff, const int index)
		{
			if (coeff == queue[index].coeff.target)
				return;
			if (coeff > queue[index].coeff.getCoeff())
				queue[index].coeff.setTarget(coeff, attackDuration, blocDuration);
			else
				queue[index].coeff.setTarget(coeff, smoothDuration, blocDuration);
		}

		// audio thread: moves the gain of every sample of the queue one bloc towards its target
		void updateCoeffs()
		{
			float sum = 0.0f;
			for (int i = 0; i < maxPoly; i++)
			{
				if (!queue[i].isEmpty())
				{
					queue[i].coeff.update();
					sum += queue[i].coeff.getCoeff();
				}
			}
			sumCoeffs = sum;
		}

		// empties the slots of the released samples whose gain went below the threshold
		void clearSamplesToRelease()
		{
			for (int i = 0; i < maxPoly; i++)
			{
				if (!queue[i].isEmpty() && queue[i].isToClear(COEFF_THRESHOLD) && queue[i].isToRelease)
				{
					queue[i].clear();
					slotsChanged = true;
					updateAvailSpace();
				}
			}
		}

		// the magnitudes and the gains of the samples to resynthesize, in the order of the slots
		int collectVoices(std::array<const float*, SAMPLE_QUEUE_MAX_POLY>& mags, std::array<float, SAMPLE_QUEUE_MAX_POLY>& gains) const
		{
			int voices = 0;
			for (int i = 0; i < maxPoly; i++)
			{
				const float gain = queue[i].coeff.getCoeff();
				if (queue[i].isEmpty() || queue[i].magData == nullptr || gain == 0.0f)
					continue;
				mags[voices]  = queue[i].magData;
				gains[voices] = gain;
				voices++;
			}
			return voices;
		}

		// counts the active samples (neither empty nor released), and updates the free space and the normalized gain
		void updateAvailSpace()
		{
			int counter = 0;
			for (int i = 0; i < maxPoly; i++)
			{
				if (!queue[i].isEmpty() && !queue[i].isToRelease)
					counter++;
			}
			availSpace    = NbPoints - counter;
			activeSamples = NbPoints - availSpace;
			if (activeSamples > 0)
				normalizedCoeff = 1.0f / static_cast<float>(activeSamples);
			else
				normalizedCoeff = 0.0f;
		}

		// empties the queue at once, every slot
		void hardClear()
		{
			for (auto& sample : queue)
			{
				if (!sample.isEmpty())
				{
					sample.clear();
					slotsChanged = true;
				}
			}
			updateAvailSpace();
		}

		// releases every sample of the queue: their gain falls to 0 in the release time
		void softClear()
		{
			for (int i = 0; i < maxPoly; i++)
			{
				if (!queue[i].isEmpty() && !queue[i].isToRelease)
				{
					queue[i].isToRelease = true;
					queue[i].coeff.setTarget(0.0f, smoothDuration, blocDuration);
				}
			}
			updateAvailSpace();
		}

		// rebuilds the comma text of the samples after a change of the slots: quoted items, an empty item for
		// each empty slot
		void publishText()
		{
			if (!slotsChanged)
				return;
			slotsChanged = false;

			pendingText.clear();
			for (int i = 0; i < maxPoly; ++i)
			{
				if (i > 0)
					pendingText += ",";

				if (!queue[i].isEmpty() && !queue[i].sample.empty() && queue[i].sample != "null")
					pendingText += "\"" + queue[i].sample + "\"";
				else
					pendingText += "\"\"";
			}
			textChanged = true;
		}

		// durations in ms; a new bloc duration rescales the ramps in progress
		void setDurations(float newAttackDuration, float newReleaseDuration, float newBlocDuration)
		{
			if (newBlocDuration != blocDuration)
			{
				for (auto& sample : queue)
					sample.coeff.rescale(blocDuration, newBlocDuration);
			}
			attackDuration = newAttackDuration;
			smoothDuration = newReleaseDuration;
			blocDuration   = newBlocDuration;
		}
	};

	DatabasePlayer();
	~DatabasePlayer() override;

	void onGetModuleInfo(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo) override;
	void onInitModule(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo) override;
	void onCreateParameters(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo) override;
	void onCreateSettings() override;
	void onAfterLoading() override;
	void onBlocSizeChange(int BlocSize) override;
	void onSampleRateChange(double SampleRate) override;
	void onCallBack(TUsineMessage* Message) override;
	void onProcess() override;
	void onPaint() override;
	void onMouseMove(TShiftState Shift, float X, float Y) override;
	void onMouseDown(TMouseButton MouseButton, TShiftState Shift, float X, float Y) override;
	void onMouseUp(TMouseButton MouseButton, TShiftState Shift, float X, float Y) override;
	void onMouseWheel(TShiftState Shift, int WheelDelta) override;

	// tasks of the work thread, see ActionQueue
	void openDatabaseTask(const std::string& fileName);
	void requestTask();
	void searchTask();
	void produceFrame();
	void computeIFFTMag(const float* magPtr);
	bool sendRequestToDB(const std::string firstDimension, const std::string secondDimension);
	bool openDB();
	void findEveryNearestPoint(const CircleCursor& circleCursor, std::vector<PointDistance2D>& resultQueue);
	// coalesced: a search or a request already waiting in the queue is not pushed twice
	void pushSearch();
	void pushRequest();

	ActionQueue actionQueue;

	sqlite3*          db = nullptr;
	std::atomic<bool> isDBOpen {false};
	std::atomic<bool> isLoading {false};
	std::atomic<bool> searchPending {false};
	std::atomic<bool> requestPending {false};

	// FFT size of the resynthesis, given by the database (work thread)
	int SPECTRUM_SIZE   = DEFAULT_SPECTRUM;
	int SPECTRUM_SIZE_2 = SPECTRUM_SIZE / 2;
	int SPECTRUM_SIZE_4 = SPECTRUM_SIZE / 4;

	CircleCursor      circleCursor;
	MouseInfo         mouseInfo;
	std::atomic<bool> isCursorOn {false};
	// every sample inside the cursor circle, found by the last search
	std::vector<PointDistance2D> listOfSamplesInCircle;

	// items of [first dimension] and [second dimension]: columns FIRST_DIMENSION_COLUMNS and SECOND_DIMENSION_COLUMNS
	std::atomic<int> firstDimensionItem {0};
	std::atomic<int> secondDimensionItem {0};
	// copies of the settings, made by the main thread which the host writes them in, for the work thread
	std::atomic<float> spreadX {0.25f};
	std::atomic<float> spreadY {0.25f};
	std::atomic<bool>  normalizeCoeffs {true};

	AudioTryLock queueLock;
	SampleQueue  listOfOutputSamples;

private:
	struct CosSin
	{
		float cos = 0.0f;
		float sin = 0.0f;
	};

	UsineEventClass databaseFileIn;
	UsineEventClass reloadDatabaseBtnIn;
	UsineEventClass firstDimensionLboxIn;
	UsineEventClass secondDimensionLboxIn;
	UsineEventClass polyphonyFdrIn;
	UsineEventClass samplesListTxtOut;
	UsineEventClass samplesCoeffsArrOut;
	UsineEventClass attackTimeFdrIn;
	UsineEventClass releaseTimeFdrIn;
	UsineEventClass onResynthesisSwIn;
	// audio out 1 plays the first channel of the resynthesis, audio out 2 the second one
	std::array<UsineEventClass, 2> audioOutputs;
	UsineEventClass cursorDownSwIn;
	UsineEventClass cursorXFdrIn;
	UsineEventClass cursorYFdrIn;
	UsineEventClass cursorRadiusFdrIn;

	// created by onInitModule
	TCriticalSectionPtr csPaint          = nullptr;
	TThreadPtr          workThread       = nullptr;
	TThreadPtr          initSearchThread = nullptr;
	TUsineFFTPtr        ifftPtr          = nullptr;

	// main thread: the database file, the audio settings of the fades
	std::string         dbName;
	int                 blocSize = 0;
	std::atomic<double> sampleRate {0.0};

	// audio thread <-> work thread
	std::atomic<bool>     activeResynth {true};
	OutputFifo            outputFifo;
	std::atomic<int>      hopSize {DEFAULT_SPECTRUM / 4};
	// the audio thread restarts the stream: the work thread drops the tails of the previous frames
	std::atomic<uint32_t> streamResets {0};
	std::atomic<uint32_t> streamResetsDone {0};

	// audio thread only
	bool                                     streamRunning       = false;
	int                                      tailSamples         = 0;
	int                                      streamedSamples     = 0;
	int                                      extraLead           = 0;
	int                                      leadBloc            = 0;
	int                                      pendingCoeffUpdates = 0;
	float                                    audioSumCoeffs      = 0.0f;
	std::array<float, SAMPLE_QUEUE_MAX_POLY> audioCoeffs {};
	std::string                              audioSamplesText;

	// work thread only: a frame of the resynthesis is the inverse FFT of the spectrum with random phases,
	// times the Hann window; the frames overlap by 75% in overlapAdd
	uint32_t                          seenStreamResets = 0;
	std::minstd_rand                  phaseRandom;
	std::vector<float>                spectrumMags = std::vector<float>(DEFAULT_SPECTRUM / 2, 0.0f);
	std::array<std::vector<float>, 2> overlapAdd {std::vector<float>(DEFAULT_SPECTRUM, 0.0f), std::vector<float>(DEFAULT_SPECTRUM, 0.0f)};
	std::vector<float>                rTempBuffer = std::vector<float>(DEFAULT_SPECTRUM, 0.0f);
	std::vector<float>                lTempBuffer = std::vector<float>(DEFAULT_SPECTRUM, 0.0f);
	std::vector<float>                rReal       = std::vector<float>(DEFAULT_SPECTRUM / 2 + 1, 0.0f);
	std::vector<float>                rIm         = std::vector<float>(DEFAULT_SPECTRUM / 2 + 1, 0.0f);
	std::vector<float>                lReal       = std::vector<float>(DEFAULT_SPECTRUM / 2 + 1, 0.0f);
	std::vector<float>                lIm         = std::vector<float>(DEFAULT_SPECTRUM / 2 + 1, 0.0f);
	std::vector<float>                hanning;
	std::vector<CosSin>               trigoLUT;

	// the samples of the database, placed by the last request (work thread)
	std::vector<Point2D> requestData;
	KDTree               kdTree {2};

	// bound to the settings, written by the host in the main thread
	float    canvasPointRadius      = 2.0f;
	float    spreadFactorX          = 0.25f;
	float    spreadFactorY          = 0.25f;
	LongBool normalizeCoeffsSetting = TRUE;

	// drawn by onPaint, protected by csPaint
	int                      numberOfPointsToDraw = 0;
	std::vector<TPointF>     pointsToDraw;
	std::vector<TUsineColor> colorsToDraw;

	[[nodiscard]] std::string queryBuilder(const std::string& firstDimension, const std::string& secondDimension) const;
	void                      InitializeHanningWindow(std::vector<float>& hanningWindow, int size);
	void                      generateLUT();
	void                      readParameters(bool fromLoading);
	bool                      refreshSettings();
	void                      setPolyphony(float value);
	void                      updateFadeDurations();
};

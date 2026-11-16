#pragma once

#include "UserDefinitions.h"

#include <array>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <memory>
#include <mutex>
#include <thread>
#include <utility>
#include <vector>

#include "DataHelper.h"

namespace process {

	using Recording = std::vector<std::vector<TPrecision>>;

	// output of the recognition, stable between two recognitions
	struct Result {
		// number of the movement recognized, from 1 to N_MVT_MAX, 0 when the last points match no movement
		TPrecision detected = 0.0f;
		// duration of the last recognition, in ms
		float latencyMs = 0.0f;
	};

	// what happened to the recordings stored since the previous call of storeRecordings
	struct RecordingEvents {
		int empty = 0;      // recordings without any point, ignored
		int truncated = 0;  // recordings that reached the maximum number of points
		int lateStart = 0;  // recordings whose start was lost: no free slot when they started
		int lost = 0;       // recordings lost: no free slot until they ended
	};

	// Recordings and recognition, shared by three threads:
	// - the audio thread records the points and starts the recognitions, it never waits for a lock;
	// - a worker thread runs the recognitions, on its own copy of the recordings;
	// - the other threads (callbacks, chunk) change or read the recordings under a lock held for a copy at most.
	class Handler {
	public:
		
		Handler(int batchSize, int inputDim, int maxRecordedPoints);
		
		~Handler();

		Handler(const Handler&) = delete;
		Handler& operator=(const Handler&) = delete;

	public:
		
		// audio thread, real-time safe

		// pushes the point into the window of the last batchSize points, starts a recognition of the window
		// every PROCESS_NB_POINTS + 1 points unless the previous one is still running, and returns the result
		// of the last finished one
		Result process(const std::vector<TPrecision>& newPoint, TPrecision treshold);

		void startRecording();
		void recordPoint(const std::vector<TPrecision>& point);
		// returns true when the recording waits to be stored by storeRecordings
		bool finishRecording(int movement);

		// other threads

		// stores the finished recordings in their movement, in the order they ended
		RecordingEvents storeRecordings();

		bool undo();

		void clearSamples();

		// replaces all the recordings, they can't be undone
		void replaceSamples(const std::vector<std::pair<int, Recording>>& samples);

		// last recording of a movement, empty if it has none
		Recording getLastSample(int index);

		// the recordings of every movement, taken under one lock
		std::array<std::vector<Recording>, N_MVT_MAX> getMovements();

		// false when the worker thread could not be created: no recognition
		bool canRecognize() const { return worker_.joinable(); }

	private:
		struct RecordingSlot {
			std::vector<TPrecision> points;
			int count = 0;
			int movement = 0;
			bool truncated = false;
			bool lateStart = false;
			// number of the recording, in the order they end
			std::uint64_t order = 0;
			// true from the end of the recording until it is stored, the audio thread only writes it while false
			std::atomic<bool> pending { false };
		};
		static constexpr int RECORDING_SLOTS = 3;

		int findFreeSlot() const;
		void storeRecordingsLocked(std::uint64_t maxOrder);
		void wakeWorker();
		void workerLoop();
		void recognize();
		TPrecision detect(DataHelper& data, TPrecision threshold) const;

		TPrecision recalculateKDist(DataHelper& data, const int i, const int j, const int k) const;

		void asyncRecalculateLRD(DataHelper& data) const;

	private:
		const int batchSize_;
		const int inputDim_;
		const int maxRecordedPoints_;

		// audio thread only {
		std::vector<std::vector<TPrecision>> window_;
		int windowHead_ = 0;
		int pCount_ = 0;
		bool inFlight_ = false;
		Result output_;
		int recordingSlot_ = -1;
		bool recording_ = false;
		// points of the current recording lost while every slot was waiting to be stored
		bool pointsLost_ = false;
		std::uint64_t finishedRecordings_ = 0;
		// }

		std::array<RecordingSlot, RECORDING_SLOTS> slots_;
		std::atomic<int> lostRecordings_ { 0 };

		// request, written by the audio thread while no recognition is in flight, then read by the worker {
		std::vector<std::vector<TPrecision>> requestWindow_;
		TPrecision requestThreshold_ = 0.0f;
		std::uint64_t requestRecordings_ = 0;
		std::atomic<bool> requestPending_ { false };
		// }

		// result, written by the worker, then read by the audio thread once finished_ is set {
		bool resultValid_ = false;
		TPrecision resultDetected_ = 0.0f;
		float resultLatencyMs_ = 0.0f;
		std::atomic<bool> finished_ { false };
		// }

		std::mutex wakeMutex_;
		std::condition_variable wakeCv_;
		std::atomic<bool> stopRequested_ { false };

		// recordings, under dataMutex_ {
		std::mutex dataMutex_;
		DataHelper data_;
		std::uint64_t dataVersion_ = 0;
		RecordingEvents events_;
		// }

		// worker only: copy of the recordings, plus the window during a recognition {
		DataHelper workerData_;
		bool workerDataValid_ = false;
		std::uint64_t workerVersion_ = 0;
		// }

		// last member: the thread starts once the others are built, and stops first
		std::thread worker_;
	};
	

} // namespace process

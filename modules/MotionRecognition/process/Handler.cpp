#include "Handler.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <system_error>

#include "Utils.h"

constexpr int PROCESS_NB_POINTS = 5;

// the worker checks for a request at least this often, in case the audio thread could not wake it up
constexpr std::chrono::milliseconds WAKE_UP_TIMEOUT { 20 };

// order of the last recording to store, see Handler::storeRecordingsLocked
constexpr std::uint64_t ALL_RECORDINGS = std::numeric_limits<std::uint64_t>::max();

namespace process {

	namespace {
		// thrown by the recognition when the handler is destroyed
		struct RecognitionAborted {};
	}
	
	Handler::Handler(int batchSize, int inputDim, int maxRecordedPoints)
		: batchSize_(batchSize)
		, inputDim_(inputDim)
		, maxRecordedPoints_(maxRecordedPoints)
	{
		window_.assign(batchSize_, std::vector<TPrecision>(inputDim_, 0.0f));
		requestWindow_ = window_;
		for (auto& slot : slots_)
			slot.points.assign(static_cast<size_t>(maxRecordedPoints_) * static_cast<size_t>(inputDim_), 0.0f);

		try {
			worker_ = std::thread(&Handler::workerLoop, this);
		}
		catch (const std::system_error&) {
			// no recognition, see canRecognize
		}
	}

	Handler::~Handler() {
		{
			std::lock_guard<std::mutex> lock(wakeMutex_);
			stopRequested_.store(true);
		}
		wakeCv_.notify_one();
		if (worker_.joinable())
			worker_.join();
	}

	Result Handler::process(const std::vector<TPrecision>& newPoint, TPrecision treshold) {

		// the result of the last recognition is taken at the first point after its end
		if (inFlight_ && finished_.load(std::memory_order_acquire)) {
			finished_.store(false, std::memory_order_relaxed);
			inFlight_ = false;
			if (resultValid_) {
				output_.detected = resultDetected_;
				output_.latencyMs = resultLatencyMs_;
			}
		}

		// the window takes every point: a recognition works on its own copy of it
		std::copy_n(newPoint.begin(), std::min(static_cast<int>(newPoint.size()), inputDim_), window_[windowHead_].begin());
		windowHead_ = (windowHead_ + 1) % batchSize_;

		if (pCount_ < PROCESS_NB_POINTS) {
			pCount_++;
			return output_;
		}

		pCount_ = 0;

		// skipped when the previous recognition is still running
		if (!inFlight_ && canRecognize()) {
			for (int i = 0; i < batchSize_; ++i)
				std::copy(window_[(windowHead_ + i) % batchSize_].begin(), window_[(windowHead_ + i) % batchSize_].end(), requestWindow_[i].begin());
			requestThreshold_ = treshold;
			requestRecordings_ = finishedRecordings_;
			inFlight_ = true;
			wakeWorker();
		}
		return output_;
	}

	void Handler::wakeWorker() {
		requestPending_.store(true, std::memory_order_release);
		// taking the mutex orders the request with the check of the worker, so that the notification can't be lost
		// while the worker is about to wait; try_lock never blocks: if it fails, the worker sees the request at its timeout
		if (wakeMutex_.try_lock())
			wakeMutex_.unlock();
		wakeCv_.notify_one();
	}

	int Handler::findFreeSlot() const {
		for (int i = 0; i < RECORDING_SLOTS; ++i)
			if (!slots_[i].pending.load(std::memory_order_acquire))
				return i;
		return -1;
	}

	void Handler::startRecording() {
		recording_ = true;
		pointsLost_ = false;
		recordingSlot_ = findFreeSlot();
		if (recordingSlot_ >= 0) {
			RecordingSlot& slot = slots_[recordingSlot_];
			slot.count = 0;
			slot.truncated = false;
			slot.lateStart = false;
		}
	}

	void Handler::recordPoint(const std::vector<TPrecision>& point) {
		if (!recording_)
			return;

		// every slot was waiting to be stored when the recording started: it starts when one is free
		if (recordingSlot_ < 0) {
			recordingSlot_ = findFreeSlot();
			if (recordingSlot_ < 0) {
				pointsLost_ = true;
				return;
			}
			RecordingSlot& slot = slots_[recordingSlot_];
			slot.count = 0;
			slot.truncated = false;
			slot.lateStart = pointsLost_;
		}

		RecordingSlot& slot = slots_[recordingSlot_];
		if (slot.count >= maxRecordedPoints_) {
			slot.truncated = true;
			return;
		}
		std::copy_n(point.begin(), std::min(static_cast<int>(point.size()), inputDim_),
			slot.points.begin() + static_cast<std::ptrdiff_t>(slot.count) * inputDim_);
		slot.count++;
	}

	bool Handler::finishRecording(int movement) {
		if (!recording_)
			return false;
		recording_ = false;

		if (recordingSlot_ < 0) {
			if (!pointsLost_)
				return false;
			lostRecordings_.fetch_add(1, std::memory_order_relaxed);
			return true;
		}

		RecordingSlot& slot = slots_[recordingSlot_];
		slot.movement = movement;
		slot.order = ++finishedRecordings_;
		slot.pending.store(true, std::memory_order_release);
		recordingSlot_ = -1;
		return true;
	}

	// stores the pending recordings that ended before maxOrder, in their order
	void Handler::storeRecordingsLocked(std::uint64_t maxOrder) {
		while (true) {
			RecordingSlot* next = nullptr;
			for (auto& slot : slots_) {
				if (slot.pending.load(std::memory_order_acquire) && (slot.order <= maxOrder)
					&& ((next == nullptr) || (slot.order < next->order)))
					next = &slot;
			}
			if (next == nullptr)
				return;

			if (next->count == 0) {
				events_.empty++;
			}
			else {
				Recording sample(next->count);
				for (int p = 0; p < next->count; ++p) {
					const auto first = next->points.begin() + static_cast<std::ptrdiff_t>(p) * inputDim_;
					sample[p].assign(first, first + inputDim_);
				}
				data_.setSample(sample, next->movement, true);
				++dataVersion_;
				if (next->truncated)
					events_.truncated++;
				if (next->lateStart)
					events_.lateStart++;
			}
			next->pending.store(false, std::memory_order_release);
		}
	}

	RecordingEvents Handler::storeRecordings() {
		std::lock_guard<std::mutex> lock(dataMutex_);
		storeRecordingsLocked(ALL_RECORDINGS);
		RecordingEvents r = events_;
		events_ = {};
		r.lost = lostRecordings_.exchange(0, std::memory_order_relaxed);
		return r;
	}

	bool Handler::undo() {
		std::lock_guard<std::mutex> lock(dataMutex_);
		// the last recording may still wait to be stored
		storeRecordingsLocked(ALL_RECORDINGS);
		const bool r = data_.undo();
		if (r)
			++dataVersion_;
		return r;
	}

	void Handler::clearSamples() {
		std::lock_guard<std::mutex> lock(dataMutex_);
		storeRecordingsLocked(ALL_RECORDINGS);
		data_.clearSamples();
		++dataVersion_;
	}

	void Handler::replaceSamples(const std::vector<std::pair<int, Recording>>& samples) {
		std::lock_guard<std::mutex> lock(dataMutex_);
		storeRecordingsLocked(ALL_RECORDINGS);
		data_.clearSamples();
		++dataVersion_;
		for (const auto& sample : samples)
			if (!sample.second.empty())
				data_.setSample(sample.second, sample.first, false);
	}

	Recording Handler::getLastSample(int index) {
		std::lock_guard<std::mutex> lock(dataMutex_);
		storeRecordingsLocked(ALL_RECORDINGS);
		if (data_.getSampleNumber(index) == 0)
			return {};
		return data_.getSample(index).back();
	}

	std::array<std::vector<Recording>, N_MVT_MAX> Handler::getMovements() {
		std::lock_guard<std::mutex> lock(dataMutex_);
		storeRecordingsLocked(ALL_RECORDINGS);
		std::array<std::vector<Recording>, N_MVT_MAX> r;
		for (int index = 0; index < N_MVT_MAX; ++index)
			r[index] = data_.getSample(index);
		return r;
	}

	void Handler::workerLoop() {
		std::unique_lock<std::mutex> lock(wakeMutex_);
		while (!stopRequested_.load()) {
			if (!requestPending_.exchange(false, std::memory_order_acq_rel)) {
				wakeCv_.wait_for(lock, WAKE_UP_TIMEOUT);
				continue;
			}
			lock.unlock();
			recognize();
			finished_.store(true, std::memory_order_release);
			lock.lock();
		}
	}

	void Handler::recognize() {
		const auto start = std::chrono::steady_clock::now();
		resultValid_ = false;
		try {
			{
				std::lock_guard<std::mutex> lock(dataMutex_);
				// the recordings that ended before the request, unless the other threads already stored them
				storeRecordingsLocked(requestRecordings_);
				if (!workerDataValid_ || (workerVersion_ != dataVersion_)) {
					workerDataValid_ = false;
					workerData_.copySamplesFrom(data_);
					workerVersion_ = dataVersion_;
					workerDataValid_ = true;
				}
			}
			workerData_.removeSamples(N_MVT_MAX);
			workerData_.setSample(requestWindow_, N_MVT_MAX, false);
			resultDetected_ = detect(workerData_, requestThreshold_);
			resultValid_ = true;
		}
		catch (...) {
			// destroyed or out of memory: no result
		}
		workerData_.removeSamples(N_MVT_MAX);
		resultLatencyMs_ = std::chrono::duration<float, std::milli>(std::chrono::steady_clock::now() - start).count();
	}

	TPrecision Handler::detect(DataHelper& data, const TPrecision threshold) const {
		asyncRecalculateLRD(data);

		// the common value for K is n^1/2
		int k = static_cast<int>(sqrtf(static_cast<float>(data.getSampleNumber())));

		auto kNNs = data.getKNNs(N_MVT_MAX, 0);

		TPrecision LOF = 0;
		const TPrecision lrd = data.getLRD(N_MVT_MAX, 0);

		for (auto& kNN : kNNs) {
			const TPrecision neighborLrd = data.getLRD(std::get<0>(kNN), std::get<1>(kNN));
			// identical recordings have an infinite density: among them the ratio is 1, not inf / inf = NaN
			if (std::isinf(neighborLrd) && std::isinf(lrd))
				LOF += 1.0f / k;
			else
				LOF += neighborLrd / lrd / k;
		}

		if (LOF < threshold && !kNNs.empty())
			return static_cast<TPrecision>(std::get<0>(kNNs.front())) + 1.0f;
		return 0;
	}

	TPrecision Handler::recalculateKDist(DataHelper& data, const int i, const int j, int k) const
	{
		std::vector<std::tuple<TPrecision, int, int>> distanceResults;

		for (int movementIndex = 0; movementIndex < N_MVT_MAX + 1; ++movementIndex) {

			for (int sampleIndex = 0; sampleIndex < data.getSampleNumber(movementIndex); ++sampleIndex) {

				// We verify that we are not parsing our vector
				if ((i != movementIndex) || (j != sampleIndex)) {
					if (stopRequested_.load(std::memory_order_relaxed))
						throw RecognitionAborted();

					std::vector<TPrecision> distanceVector(inputDim_, 0);

					for (int axisIndex = 0; axisIndex < inputDim_; ++axisIndex) {

						distanceVector.at(axisIndex) = DTW_i(
							data.getAxisOfSample(i, j, axisIndex),
							data.getAxisOfSample(movementIndex, sampleIndex, axisIndex));

					}

					distanceResults.push_back({ mean(distanceVector), movementIndex, sampleIndex });
				}
			}

		}
		const auto kNearestNeighbors = getKNNs(distanceResults, k);

		if (kNearestNeighbors.empty())
			return 0;

		std::vector<std::tuple<int, int>> kNNsOnly;
		for (const auto& nn : kNearestNeighbors) {
			// Extract the second and third elements (int values) from the original tuple.
			int secondValue = std::get<1>(nn);
			int thirdValue = std::get<2>(nn);

			// Create a new tuple with the extracted integer values.
			kNNsOnly.push_back(std::make_tuple(secondValue, thirdValue));
		}

		data.setKNNs(kNNsOnly, i, j);

		return std::get<0>(kNearestNeighbors.back());
	}

	void Handler::asyncRecalculateLRD(DataHelper& data) const
	{

		int k = static_cast<int>(sqrtf(static_cast<float>(data.getSampleNumber())));
		if (!k)
			return;
		for (int movementIndex = 0; movementIndex < N_MVT_MAX + 1; ++movementIndex) {
			for (int sampleIndex = 0; sampleIndex < data.getSampleNumber(movementIndex); ++sampleIndex) {
				const TPrecision kDist = recalculateKDist(data, movementIndex, sampleIndex, k);
				data.setKDist(kDist, movementIndex, sampleIndex);
			}
		}

		for (int movementIndex = 0; movementIndex < N_MVT_MAX + 1; ++movementIndex) {

			for (int sampleIndex = 0; sampleIndex < data.getSampleNumber(movementIndex); ++sampleIndex) {
				TPrecision sumReachabilityDistance = 0;
				auto kNNs = data.getKNNs(movementIndex, sampleIndex);

				for (auto& kNN : kNNs) {
					if (stopRequested_.load(std::memory_order_relaxed))
						throw RecognitionAborted();

					std::vector<TPrecision> distances;

					distances.reserve(inputDim_);
					for (int axisIndex = 0; axisIndex < inputDim_; ++axisIndex) {
						distances.push_back(DTW_i(
							data.getAxisOfSample(movementIndex, sampleIndex, axisIndex),
							data.getAxisOfSample(std::get<0>(kNN), std::get<1>(kNN), axisIndex)
						));
					}

					sumReachabilityDistance += std::max(
						data.getkDist(std::get<0>(kNN), std::get<1>(kNN)),
						mean(distances));


				}

				data.setLRD(1 / (sumReachabilityDistance / static_cast<TPrecision>(kNNs.size())), movementIndex, sampleIndex);
			}

		}
	}

} // namespace process

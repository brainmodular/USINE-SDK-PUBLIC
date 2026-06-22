#include "Handler.h"

#include <functional>
#include <future>

#include "Utils.h"

constexpr int PROCESS_NB_POINTS = 5;

namespace process {
	
	Handler::Handler(int batchSize, int inputDim, UserModuleBase* module)
		: inputDim_(inputDim)
		, pCount_(0)
		, processOutCount_(0)
		, status_(0)
		, module_(module)
	{
		criticalSection_ = module_->sdkCriticalSectionCreate("data_lock");
		data_ = std::make_unique<DataHelper>(batchSize, inputDim);
	}

	Handler::~Handler() {
		if (workerThread_.joinable())
			workerThread_.join();

		data_.reset();
		module_->sdkCriticalSectionDestroy(criticalSection_);
	}

	Result Handler::process(const std::vector<TPrecision>& newPoint, TPrecision treshold) {

		if (module_->sdkCriticalSectionTryLock(criticalSection_)) {
			data_->shiftAndPushNewPoint(newPoint);
			module_->sdkCriticalSectionUnLock(criticalSection_);
		}

		if (pCount_ < PROCESS_NB_POINTS) {
			pCount_++;
			Result r;
			if (module_->sdkCriticalSectionTryLock(criticalSection_)) {
				std::copy(processOut_, processOut_ + processOutCount_, r.detected_);
				r.detectedCount_ = processOutCount_;
				module_->sdkCriticalSectionUnLock(criticalSection_);
			} else {
				r.detectedCount_ = 0;
			}
			r.error_ = 0;
			r.latency_ = latency_;
			return r;
		}

		pCount_ = 0;

		unsigned char expected = 0;
		if (status_.compare_exchange_strong(expected, 1)) {
			if (workerThread_.joinable())
				workerThread_.join();
			workerThread_ = std::thread(std::bind(&process::Handler::asyncProcess, this, treshold));
		}

		Result r;
		if (module_->sdkCriticalSectionTryLock(criticalSection_)) {
			std::copy(processOut_, processOut_ + processOutCount_, r.detected_);
			r.detectedCount_ = processOutCount_;
			module_->sdkCriticalSectionUnLock(criticalSection_);
		} else {
			r.detectedCount_ = 0;
		}
		r.error_ = 0;
		r.latency_ = latency_;
		return r;
	}

	void Handler::asyncProcess(const TPrecision threshold) {
        const auto start = std::chrono::system_clock::now();
		module_->sdkCriticalSectionLock(criticalSection_);
		try {
			data_->setSample(data_->getProcess(), N_MVT_MAX, true);

			asyncRecalculateLRD();

			// the common value for K is n^1/2
			int k = static_cast<int>(std::sqrtf(static_cast<float>(data_->getSampleNumber())));

			auto kNNs = data_->getKNNs(N_MVT_MAX, 0);

			TPrecision LOF = 0;

			for (auto& kNN : kNNs)
				LOF += data_->getLRD(std::get<0>(kNN), std::get<1>(kNN)) / data_->getLRD(N_MVT_MAX, 0) / k;

			if (LOF < threshold && !kNNs.empty()) {
				processOut_[0] = static_cast<TPrecision>(std::get<0>(kNNs.front())) + 1.0f;
			} else {
				processOut_[0] = 0;
			}
			processOutCount_ = 1;

			data_->undo();
		}
		catch (...) { }
		module_->sdkCriticalSectionUnLock(criticalSection_);
        const auto end = std::chrono::system_clock::now();
        latency_ = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
		status_.store(0, std::memory_order_release);
	}


	
	void Handler::addSample(const std::vector<std::vector<TPrecision>>& sample, int index, bool putOnStack) const
	{
		module_->sdkCriticalSectionLock(criticalSection_);
		data_->setSample(sample, index, putOnStack);
		module_->sdkCriticalSectionUnLock(criticalSection_);
	}

	bool Handler::undo() const
	{
		module_->sdkCriticalSectionLock(criticalSection_);
		bool r = data_->undo();
		module_->sdkCriticalSectionUnLock(criticalSection_);
		return r;
	}

	void Handler::clearSamples() const
	{
		module_->sdkCriticalSectionLock(criticalSection_);
		data_->clearSamples();
		module_->sdkCriticalSectionUnLock(criticalSection_);
	}

	std::vector<std::vector<std::vector<TPrecision>>> Handler::getMovement(const int index) const
	{
		module_->sdkCriticalSectionLock(criticalSection_);
		auto r = data_->getSample(index);
		module_->sdkCriticalSectionUnLock(criticalSection_);
		return r;
	}

	int Handler::getByteSize() const
	{
		return data_->getByteSize();
	}

	int Handler::getSampleNumber() const
	{
		return data_->getSampleNumber();
	}

	TPrecision Handler::recalculateKDist(const int i, const int j, int k) const
	{
		std::vector<std::tuple<TPrecision, int, int>> distanceResults;

		for (int movementIndex = 0; movementIndex < N_MVT_MAX + 1; ++movementIndex) {

			for (int sampleIndex = 0; sampleIndex < data_->getSampleNumber(movementIndex); ++sampleIndex) {

				// We verify that we are not parsing our vector
				if ((i != movementIndex) || (j != sampleIndex)) {
					std::vector<TPrecision> distanceVector(inputDim_, 0);

					for (int axisIndex = 0; axisIndex < inputDim_; ++axisIndex) {

						distanceVector.at(axisIndex) = DTW_i(
							data_->getAxisOfSample(i, j, axisIndex),
							data_->getAxisOfSample(movementIndex, sampleIndex, axisIndex));

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

		data_->setKNNs(kNNsOnly, i, j);

		return std::get<0>(kNearestNeighbors.back());
	}

	void Handler::asyncRecalculateLRD() const
	{

		int k = static_cast<int>(std::sqrtf(static_cast<float>(data_->getSampleNumber())));
		if (!k)
			return;
		for (int movementIndex = 0; movementIndex < N_MVT_MAX + 1; ++movementIndex) {
			for (int sampleIndex = 0; sampleIndex < data_->getSampleNumber(movementIndex); ++sampleIndex) {
				const TPrecision kDist = recalculateKDist(movementIndex, sampleIndex, k);
				data_->setKDist(kDist, movementIndex, sampleIndex);
			}
		}

		for (int movementIndex = 0; movementIndex < N_MVT_MAX + 1; ++movementIndex) {

			for (int sampleIndex = 0; sampleIndex < data_->getSampleNumber(movementIndex); ++sampleIndex) {
				TPrecision sumReachabilityDistance = 0;
				auto kNNs = data_->getKNNs(movementIndex, sampleIndex);

				for (auto& kNN : kNNs) {
					std::vector<TPrecision> distances;

					distances.reserve(inputDim_);
					for (int axisIndex = 0; axisIndex < inputDim_; ++axisIndex) {
						distances.push_back(DTW_i(
							data_->getAxisOfSample(movementIndex, sampleIndex, axisIndex),
							data_->getAxisOfSample(std::get<0>(kNN), std::get<1>(kNN), axisIndex)
						));
					}

					sumReachabilityDistance += std::max(
						data_->getkDist(std::get<0>(kNN), std::get<1>(kNN)),
						mean(distances));


				}

				data_->setLRD(1 / (sumReachabilityDistance / static_cast<TPrecision>(kNNs.size())), movementIndex, sampleIndex);
			}

		}
	}

} // namespace process

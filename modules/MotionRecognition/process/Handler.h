#pragma once

#include "UserDefinitions.h"
#include <UserModule.h>

#include <atomic>
#include <chrono>
#include <thread>

#include "DataHelper.h"

namespace process {

    struct Result {
        TPrecision detected_[N_MVT_MAX];
        int detectedCount_;
        unsigned char error_;
        std::chrono::milliseconds latency_;
    };

	class Handler {
	public:
		
		Handler(int batchSize, int inputDim, UserModuleBase* module);
		
		~Handler();

	public:
		
		Result process(const std::vector<TPrecision>& newPoint, TPrecision treshold);

		void addSample(const std::vector<std::vector<TPrecision>>& sample, int index, bool putOnStack = true) const;

		bool undo() const;

		void clearSamples() const;

		std::vector<std::vector<std::vector<TPrecision>>> getMovement(const int index) const;

		int getByteSize() const;
		int getSampleNumber() const;

	private:
		void asyncProcess(TPrecision threshold);

		TPrecision recalculateKDist(const int i, const int j, const int k) const;

		void asyncRecalculateLRD() const;

	private:
		int inputDim_;
		
		int pCount_;

		TPrecision processOut_[N_MVT_MAX];
		int processOutCount_;
        std::chrono::milliseconds latency_;
		std::atomic<unsigned char> status_;
		std::thread workerThread_;

		std::unique_ptr<DataHelper> data_;

		UserModuleBase* module_;
		TCriticalSectionPtr criticalSection_;

	};
	

} // namespace process

#pragma once
#include <UserDefinitions.h>

#include <array>
#include <stack>
#include <tuple>
#include <vector>

constexpr int N_MVT_MAX = 8;

constexpr int META_DATA_SIZE = 8;

namespace process {

	using TPrecision2DArray_t = std::vector<std::vector<TPrecision>*>;
	using TPrecision2DArrayPair_t = std::pair<TPrecision2DArray_t, TPrecision2DArray_t>;
	using batch_t = std::vector<std::pair<std::vector<TPrecision>*, std::vector<TPrecision>*>>;

	// Deallocators for prettier code.
	inline void externalFree(TPrecision2DArray_t& vec_vec) {
		for (auto v : vec_vec)
			delete v;
		vec_vec.clear();
	}

	inline void externalFree(TPrecision2DArrayPair_t& p) {
		externalFree(p.first);
		externalFree(p.second);
	};

	// Helper class for data handling.
	class DataHelper {

		// Movement representation class
	private: 

		struct Sample {
			TPrecision2DArray_t* sample_;
			std::vector<std::tuple<int, int>> kNNs_;
			TPrecision kDist_;
			TPrecision lrd_;

			Sample(TPrecision2DArray_t* sample) {
				sample_ = sample;
				kNNs_ = {};
				kDist_ = 0;
				lrd_ = 0;
			}

			TPrecision2DArray_t* operator->() {
				return sample_;
			}

			TPrecision2DArray_t& operator*() {
				return *sample_;
			}
		};

		//constructor and destructors
	public:
		
		DataHelper()
		{
			// fill the samples with empty vectors
			for (int i = 0; i < N_MVT_MAX + 1; ++i)
				trainingMovements_.at(i) = new std::vector<Sample>();
		}

		// the samples are owned through raw pointers: a copy would free them twice
		DataHelper(const DataHelper&) = delete;
		DataHelper& operator=(const DataHelper&) = delete;

		~DataHelper() {
			for (auto movement : trainingMovements_) {
				for (auto& sample : *movement) {
					externalFree(*sample);
					delete sample.sample_;
				}
				delete movement;
			}
		}

	public:

		const std::vector<std::vector<std::vector<TPrecision>>> getSample(const int index) const {
			std::vector<Sample>* movement = trainingMovements_.at(index);
			std::vector<std::vector<std::vector<TPrecision>>> r;
			
			for (auto sample : *movement) {
				r.push_back(std::vector<std::vector<TPrecision>>());
				for (auto& point : *sample)
					r.back().push_back(std::vector<TPrecision>(point->begin(), point->end()));
			}
			return r;
		}

		const std::vector<TPrecision> getAxisOfSample(const int movementIndex, const int sampleIndex,const int axis) const {
			std::vector<Sample>* movement = trainingMovements_.at(movementIndex);
			std::vector<TPrecision> r;
			
			if (movement->size() <= static_cast<size_t>(sampleIndex))
				return {};
			
			for (auto& point : *movement->at(sampleIndex))
				r.push_back(point->at(axis));
		
			return r;
		}

		TPrecision getkDist(const int movementIndex, const int sampleIndex) const {
			std::vector<Sample>* movement = trainingMovements_.at(movementIndex);
			if (!movement->size())
				return 0;

			return movement->at(sampleIndex).kDist_;
		}

		std::vector<std::tuple<int, int>> getKNNs(const int movementIndex, const int sampleIndex) const {
			std::vector<Sample>* movement = trainingMovements_.at(movementIndex);
			if (!movement->size())
				return {};
			
			return movement->at(sampleIndex).kNNs_;
		}

		TPrecision getLRD(const int movementIndex, const int sampleIndex) {
			std::vector<Sample>* movement = trainingMovements_.at(movementIndex);
			if (!movement->size())
				return {};

			return movement->at(sampleIndex).lrd_;
		}

		void setSample(const std::vector<std::vector<TPrecision>> n_arr, const int index, bool putOnStack) {
			std::vector<Sample>* movement = trainingMovements_.at(index);
			movement->push_back(Sample(new TPrecision2DArray_t));
			for (auto& point : n_arr)
				movement->back()->push_back(new std::vector<TPrecision>(point));
			
			if(putOnStack)
				sampleStack_.push(index);
		}

		void setKDist(TPrecision kDist, const int movementIndex, const int sampleIndex) {
			std::vector<Sample>* movement = trainingMovements_.at(movementIndex);
			if (!movement->size())
				return;

			movement->at(sampleIndex).kDist_ = kDist;
		}

		void setKNNs(std::vector<std::tuple<int, int>> kNNs, const int movementIndex, const int sampleIndex) {
			 trainingMovements_.at(movementIndex)->at(sampleIndex).kNNs_ = kNNs;
		}

		void setLRD(TPrecision lrd, const int movementIndex, const int sampleIndex) {
			trainingMovements_.at(movementIndex)->at(sampleIndex).lrd_ = lrd;
		}

		bool undo() {
			if (sampleStack_.empty())
				return false;

			externalFree(*trainingMovements_.at(sampleStack_.top())->back());
			delete trainingMovements_.at(sampleStack_.top())->back().sample_;
			trainingMovements_.at(sampleStack_.top())->pop_back();
			sampleStack_.pop();

			return true;
		}

		void clearSamples() {
			for (auto movement : trainingMovements_) {
				for (auto& sample : *movement) {
					externalFree(*sample);
					delete sample.sample_;
				}
				movement->clear();
			}

			// clearing a stack is cumbersome, we just replace it
			sampleStack_ = std::stack<int>();
		}

		// removes the samples of a movement without touching the undo stack: the recognition adds its window
		// as the only sample of the movement N_MVT_MAX, and removes it even when it fails half way
		void removeSamples(const int index) {
			std::vector<Sample>* movement = trainingMovements_.at(index);
			for (auto& sample : *movement) {
				externalFree(*sample);
				delete sample.sample_;
			}
			movement->clear();
		}

		// replaces the samples by a copy of those of the movements of other, without the undo stack
		void copySamplesFrom(const DataHelper& other) {
			clearSamples();
			for (int index = 0; index < N_MVT_MAX; ++index)
				for (const auto& sample : other.getSample(index))
					setSample(sample, index, false);
		}

		// computes the number of samples from all movements
		int getSampleNumber() const {
			int size = 0;
			for (int movementIndex = 0; movementIndex < N_MVT_MAX; ++movementIndex)
					size += (int)trainingMovements_.at(movementIndex)->size();
			return size;
		}

		// compute only the number of samples from the specified movement
		int getSampleNumber(int movementIndex) const {
			return (int)trainingMovements_.at(movementIndex)->size();
		}

		// Private Members.
	private:

		// training points { 
		// Vector of data points.
		std::array<std::vector<Sample>*, N_MVT_MAX + 1> trainingMovements_;
		
		std::stack<int> sampleStack_;
		//std::array<TPrecision, N_MVT_MAX> trainingLabels_;
		// }

	};
} // namespace process


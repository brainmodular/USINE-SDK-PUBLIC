#pragma once

#include <algorithm>
#include <array>
#include <iostream>
#include <map>
#include <math.h>
#include <numeric>
#include <vector>




namespace process {
	inline float mean(std::vector<float> const& v) {
		float r = 0;
		for (const float f : v)
			r += f;
		return r / static_cast<TPrecision>(v.size());
	}

	inline std::vector<std::tuple<float, int, int>> getKNNs(std::vector<std::tuple<float, int, int>> v_t, size_t k) {
		std::sort(v_t.begin(), v_t.end(), [](const auto& a, const auto& b) {
			return std::get<0>(a) < std::get<0>(b);
		});

		return k <= v_t.size() ?
			std::vector<std::tuple<float, int, int>>(v_t.begin(), v_t.begin() + static_cast<long long>(k)) :
			v_t;
	}

	inline std::vector<int> getLowestK(std::vector<std::pair<int, float>> v_p, size_t k) {
		
		std::sort(v_p.begin(), v_p.end(), [](const auto& a, const auto& b) {
			return a.second < b.second;
			});

		std::vector<int> r;
		
		for (size_t i = 0; i < k && i < v_p.size(); i++) {
			r.push_back(v_p[i].first);
		}
		
		return r;
	}

	inline int findMostFrequentElement(const std::vector<std::tuple<float, int, int>>& vec) {
		std::map<int, int> elementCount;

		// Count the occurrences of each element in the vector
		for (auto& element : vec) {
			elementCount[std::get<1>(element)]++;
		}

		int mostFrequentElement = -1; // Initialize to an invalid value
		int maxCount = 0;

		// Find the element with the highest count
		for (const auto& pair : elementCount) {
			if (pair.second > maxCount) {
				mostFrequentElement = pair.first;
				maxCount = pair.second;
			}
		}

		return mostFrequentElement;
	}

	inline float d(float a, float b) {
		return std::abs(a - b);
	}

	inline float d(const std::vector<float>& a, const std::vector<float>& b, const float p) {
		float d = 0;
		for (size_t i = 0; i < a.size(); i++) {
			d += std::pow(std::abs(a[i] - b[i]), p);
		}
		return std::powf(d, 1.0f/p);
	}

	inline std::vector<std::vector<float>> buildMatrix(size_t n, size_t m) {
		std::vector<std::vector<float>> r(n);
		for (size_t i = 0; i < n; ++i)
			r.at(i) = std::vector<float>(m, 0);
		return r;
	}

	inline float DTW_i(const std::vector<float>& x, const std::vector<float>& x_prime, const int q = 2) {
		size_t n = x.size();
		size_t m = x_prime.size();

		if (!n || !m)
			return -1;

		auto r = buildMatrix(n, m);


		for (size_t i = 0; i < n; ++i) {
			for (size_t j = 0; j < m; ++j) {
				r[i][j] = std::powf(d(x[i], x_prime[j]), (float)q);
				if (i || j) {
					r[i][j] += std::min({
						i ? r[i - 1][j] : INFINITY,
						j ? r[i][j - 1] : INFINITY,
						i && j ? r[i - 1][j - 1] : INFINITY
						});
				}
			}
		}

		return std::pow(r.back().back(), 1.0f / static_cast<float>(q));
	}

	inline float DTW_d(const std::vector<std::vector<float>>& x, const std::vector<std::vector<float>>& x_prime, const int q = 2) {
		size_t n = x.size();
		size_t m = x_prime.size();

		if (!n || !m)
			return -1;
		auto r = buildMatrix(n, m);
		for (size_t i = 0; i < n; ++i) {
			for (size_t j = 0; j < m; ++j) {
				r[i][j] = d(x[i], x_prime[j], static_cast<float>(q));
				if (i || j) {
					r[i][j] += std::min({
						i ? r[i - 1][j] : INFINITY,
						j ? r[i][j - 1] : INFINITY,
						i && j ? r[i - 1][j - 1] : INFINITY
						});
				}
			}
		}

		return std::pow(r.back().back(), 1.0f / static_cast<float>(q));

	}

} // namespace process

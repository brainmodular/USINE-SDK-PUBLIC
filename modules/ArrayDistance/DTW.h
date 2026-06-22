#pragma once

#include <cmath>
#include <numeric>
#include <vector>

template <typename T>
inline T d(T a, T b) {
	return std::abs(a - b);
}

template <typename T>
inline std::vector<std::vector<T>> buildMatrix(size_t n, size_t m) {
	std::vector<std::vector<T>> r;
	r.reserve(n);
	for (size_t i = 0; i < n; ++i)
		r.push_back(std::vector<T>(m, 0));
	return r;
}

// powf is used to guaranty that a float calculation is done and not double for 
// performance reasons
template <typename T>
T maxDTW(
	const std::vector<T>& a, 
	const std::vector<T>& b,
	const T min,
	const T max,
	int q = 2) 
{
	if (!a.size() || !b.size())
		return 1;

	if (max == min)
		return 1;

	return
		std::powf(std::max({static_cast<T>(a.size()), static_cast<T>(b.size()) }), T(1) / q) *
		(max - min);
}

// powf is used to guaranty that a float calculation is done and not double for 
// performance reasons
template <typename T>
T DTW(
	const std::vector<T>& a, 
	const std::vector<T>& b, 
	int q = 2) 
{
	size_t n = a.size();
	size_t m = b.size();

	if (!n || !m)
		return -1;

	auto r = buildMatrix<T>(n, m);

	for (size_t i = 0; i < n; ++i) {
		for (size_t j = 0; j < m; ++j) {
			r[i][j] = std::powf(d(a[i], b[j]), float(q));
			if (i || j) {
				r[i][j] += std::min({
					i ? r[i - 1][j] : T(INFINITY),
					j ? r[i][j - 1] : T(INFINITY),
					i && j ? r[i - 1][j - 1] : T(INFINITY)
					});
			}
		}
	}

	return std::pow(r.back().back(), T(1) / q);
}

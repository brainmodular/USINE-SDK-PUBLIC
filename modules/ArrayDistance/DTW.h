#pragma once

#include <algorithm>
#include <cmath>
#include <numeric>
#include <vector>

template <typename T>
inline T d(T a, T b) {
	return std::abs(a - b);
}

// powf is used to guaranty that a float calculation is done and not double for 
// performance reasons
// the range is |max - min|: swapped bounds give the same factor as ordered ones
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
		powf(std::max({static_cast<T>(a.size()), static_cast<T>(b.size()) }), T(1) / q) *
		std::abs(max - min);
}

// powf is used to guaranty that a float calculation is done and not double for 
// performance reasons
// the n x m cost matrix r[i][j] is calculated row by row and only rows i - 1 and i are kept:
// the memory grows with m only, each cell is calculated from the same values in the same
// order as with the full matrix, so the result is the same bit for bit
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

	std::vector<T> previous(m, 0); // row i - 1
	std::vector<T> current(m, 0);  // row i

	for (size_t i = 0; i < n; ++i) {
		for (size_t j = 0; j < m; ++j) {
			current[j] = powf(d(a[i], b[j]), float(q));
			if (i || j) {
				current[j] += std::min({
					i ? previous[j] : T(INFINITY),
					j ? current[j - 1] : T(INFINITY),
					i && j ? previous[j - 1] : T(INFINITY)
					});
			}
		}
		previous.swap(current);
	}

	// after the last swap, previous is row n - 1
	return std::pow(previous.back(), T(1) / q);
}

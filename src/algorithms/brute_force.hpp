// BRUTE-FORCE with Gray code (report, Section 2.2): a balanced partition or NO.
// Only the test reference; SOLVE never calls it.
#pragma once

#include <cstddef>
#include <optional>
#include <span>
#include <vector>

#include "partition.hpp"

// The n − 1 free bits have to fit in a 64-bit mask.
inline constexpr std::size_t kMaxBruteForceElements = 64;

inline constexpr std::size_t kMaxFindSubsetElements = 63;

// FIND-SUBSET: sorted indices of a subset of b with sum x, or nothing. Gray-code
// enumeration (Section 2.1), O(2^m) time, O(1) extra memory. Needs b.size() ≤ 63
// (64-bit mask), throws std::invalid_argument otherwise. Used by brute force and by
// the recovery step of meet in the middle (Section 3).
template <class V>
std::optional<std::vector<std::size_t>> find_subset(std::span<const V> b, const V& x);

// Needs n ≥ 2. Throws std::invalid_argument if n > kMaxBruteForceElements.
template <class V>
ExactAnswer<V> brute_force(std::span<const V> a);

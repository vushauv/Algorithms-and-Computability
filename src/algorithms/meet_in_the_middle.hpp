// MEET-IN-THE-MIDDLE of Horowitz and Sahni (report, Section 3.2): a balanced partition
// or NO. Time O(n · 2^(n/2)), memory 2^(n/2) sums per half.
#pragma once

#include <cstddef>
#include <span>
#include <vector>

#include "partition.hpp"

// The masks of a half have to fit in 64 bits. The memory limit is much lower and is the
// caller's job (N_MITM).
inline constexpr std::size_t kMaxMeetInTheMiddleElements = 2 * 62;

// SUBSET-SUMS: sums[mask] is the sum of b[k] over the set bits k of mask. b.size() ≤ 62.
template <class V>
std::vector<V> subset_sums(std::span<const V> b);

// Needs n ≥ 2. Throws std::invalid_argument if n > kMaxMeetInTheMiddleElements.
// The sorted lists keep only sums, so the subsets for the matching pair are found again
// with FIND-SUBSET: O(2^(n/2)) more time, no more memory.
template <class V>
ExactAnswer<V> meet_in_the_middle(std::span<const V> a);

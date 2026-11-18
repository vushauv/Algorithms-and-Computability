// MEET-IN-THE-MIDDLE of Horowitz and Sahni (report, Section 3.2): a balanced
// partition, or NO. Time O(n · 2^(n/2)), memory 2^(n/2) sums per half.
#pragma once

#include <cstddef>
#include <span>
#include <vector>

#include "partition.hpp"

// The masks of a half fit in a 64-bit word. The memory bound is much lower
// and is the caller's business (the threshold N_MITM).
inline constexpr std::size_t kMaxMeetInTheMiddleElements = 2 * 62;

// SUBSET-SUMS: sums[mask] is the sum of the b[k] with bit k of mask set.
// Requires b.size() ≤ 62.
std::vector<Value> subset_sums(std::span<const Value> b);

// Requires n ≥ 2. Throws std::invalid_argument if n > kMaxMeetInTheMiddleElements.
// The sorted lists keep only the sums, so the subsets with the two sums that
// meet are found again with FIND-SUBSET: O(2^(n/2)) more time, no more memory.
ExactAnswer meet_in_the_middle(std::span<const Value> a);

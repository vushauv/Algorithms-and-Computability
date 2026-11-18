// BRUTE-FORCE with Gray code (report, Section 2.2): a balanced partition, or
// NO. The reference for testing the other algorithms; SOLVE never calls it.
#pragma once

#include <cstddef>
#include <optional>
#include <span>
#include <vector>

#include "partition.hpp"

// The mask of the n − 1 free bits fits in a 64-bit word.
inline constexpr std::size_t kMaxBruteForceElements = 64;

inline constexpr std::size_t kMaxFindSubsetElements = 63;

// FIND-SUBSET: the indices (sorted) of a subset of b with the sum x, found by
// the Gray-code enumeration of Section 2.1, or nothing if no subset has that
// sum. Time O(2^m), memory O(1) beyond the result. Requires b.size() ≤ 63 (the
// mask is a 64-bit word); throws std::invalid_argument otherwise.
// Brute force and the recovery of meet in the middle (Section 3) use it.
std::optional<std::vector<std::size_t>> find_subset(std::span<const Value> b, Value x);

// Requires n ≥ 2. Throws std::invalid_argument if n > kMaxBruteForceElements.
ExactAnswer brute_force(std::span<const Value> a);

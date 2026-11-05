// BRUTE-FORCE with Gray code (report, Section 2.2). Decides only.
// The reference for testing the other algorithms; SOLVE never calls it.
#pragma once

#include <cstddef>
#include <span>

#include "partition.hpp"

// The mask of the n − 1 free bits fits in a 64-bit word.
inline constexpr std::size_t kMaxBruteForceElements = 64;

// Requires n ≥ 2. Throws std::invalid_argument if n > kMaxBruteForceElements.
Verdict brute_force(std::span<const Value> a);

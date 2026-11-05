// DP-PARTITION over the sums 0..T on a bitset (report, Sections 4.2 and 4.3).
// Decides only. Time O(n · T/64), memory T + 1 bits.
#pragma once

#include <cstddef>
#include <limits>
#include <span>

#include "partition.hpp"

// T must index the bitset, and top + a[i] ≤ 2T must not overflow. The memory
// bound is much lower and is the caller's business (the threshold T_DP).
inline constexpr Value kMaxDpTarget = std::numeric_limits<std::size_t>::max() / 2;

// Requires n ≥ 2. Throws std::invalid_argument if T > kMaxDpTarget; does not
// throw when the answer follows from the checks of Sections 1.2 and 4.1.
Verdict dp_partition(std::span<const Value> a);

// DP-PARTITION over the sums 0..T on a bitset (report, Sections 4.2 and 4.3):
// a balanced partition, or NO. Next to the bitset, first[s] records the step
// that made the sum s reachable, and the subset is read back from T.
// Time O(n · T/64 + T), memory T + 1 bits and T + 1 step numbers of 32 bits.
#pragma once

#include <cstddef>
#include <cstdint>
#include <limits>
#include <span>

#include "partition.hpp"

// T must index the bitset, and top + a[i] ≤ 2T must not overflow. The memory
// bound is much lower and is the caller's business (the threshold T_DP).
inline constexpr std::uint64_t kMaxDpTarget = std::numeric_limits<std::size_t>::max() / 2;

// The steps 1..n are stored in 32 bits; the input has at most 10^6 elements.
inline constexpr std::size_t kMaxDpElements = std::numeric_limits<std::uint32_t>::max() - 1;

// Requires n ≥ 2. Throws std::invalid_argument if T > kMaxDpTarget or
// n > kMaxDpElements; does not throw when the answer follows from the checks
// of Sections 1.2 and 4.1.
template <class V>
ExactAnswer<V> dp_partition(std::span<const V> a);

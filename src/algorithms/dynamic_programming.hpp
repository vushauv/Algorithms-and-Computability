// DP-PARTITION over the sums 0..T on a bitset (report, Sections 4.2 and 4.3): a balanced
// partition or NO. Next to the bitset, first[s] holds the step that made s reachable,
// and the subset is read back from T. Time O(n · T/64 + T), memory T + 1 bits and
// T + 1 step numbers of 32 bits.
#pragma once

#include <cstddef>
#include <cstdint>
#include <limits>
#include <span>

#include "partition.hpp"

// T has to index the bitset and top + a[i] ≤ 2T must not overflow. The memory limit is
// much lower and is the caller's job (T_DP).
inline constexpr Value kMaxDpTarget = std::numeric_limits<std::size_t>::max() / 2;

// The steps 1..n are stored in 32 bits.
inline constexpr std::size_t kMaxDpElements = std::numeric_limits<std::uint32_t>::max() - 1;

// Needs n ≥ 2. Throws std::invalid_argument if T > kMaxDpTarget or n > kMaxDpElements.
// Doesn't throw when the checks of Sections 1.2 and 4.1 already decide.
ExactAnswer dp_partition(std::span<const Value> a);

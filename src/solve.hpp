// SOLVE, the final algorithm: Karmarkar-Karp always, plus an exact
// algorithm when one is in range. Brute force is never used.
#pragma once

#include <cstddef>
#include <cstdint>
#include <span>

#include "partition.hpp"

// Limits of the exact algorithms
struct Ranges {
    std::size_t n_mitm;
    std::uint64_t t_dp;
    std::uint64_t w_dp;
};

enum class DpLimit { InRange, Target, Work };

template <class V>
DpLimit dp_limit(std::size_t n, const V& T, const Ranges& ranges);

// What decided SOLVE's answer.
enum class Method {
    KarmarkarKarp,       // d = 0, the partition is balanced
    OddSum,              // S is odd
    DynamicProgramming,
    MeetInTheMiddle,
    None                 // no exact algorithm in range, the answer is UNKNOWN
};

template <class V>
struct Solution {
    Verdict answer;
    Method decided_by;
    Partition<V> partition;   // of Karmarkar-Karp, with its discrepancy d
    ExactAnswer<V> balanced;  // present iff the answer is YES: the partition above
                              // when d = 0, otherwise that of the exact algorithm
    V S;
    V T;  // S / 2
};

// Asked only for a target in the range of DP, so 64 bits.
bool dp_is_cheaper(std::size_t n, std::uint64_t T);

// The exact algorithm SOLVE runs: DP, MeetInTheMiddle, or None if neither is in range.
template <class V>
Method choose_exact(std::size_t n, const V& T, const Ranges& ranges);

// Needs n >= 2 and, for Native, S < 2^128 (read_instances checks both).
template <class V>
Solution<V> solve(std::span<const V> a, const Ranges& ranges);

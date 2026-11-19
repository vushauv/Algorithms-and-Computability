// SOLVE, the final algorithm (report, Section 6.2): Karmarkar-Karp always,
// plus an exact algorithm when one is in range. Brute force is never used.
#pragma once

#include <cstddef>
#include <cstdint>
#include <span>

#include "partition.hpp"

// The ranges of the exact algorithms (report, Section 1.4): dynamic
// programming if T ≤ t_dp and n · T ≤ w_dp, meet in the middle if n ≤ n_mitm.
struct Ranges {
    std::size_t n_mitm;
    std::uint64_t t_dp;
    std::uint64_t w_dp;
};

// Why dynamic programming is out of range for a set of n numbers with
// target T: Target if T > t_dp (or T > kMaxDpTarget), Work if n · T > w_dp.
enum class DpLimit { InRange, Target, Work };

template <class V>
DpLimit dp_limit(std::size_t n, const V& T, const Ranges& ranges);

// What decided the answer of SOLVE.
enum class Method {
    KarmarkarKarp,       // d = 0, the partition is balanced (step 1)
    OddSum,              // S is odd (step 2)
    DynamicProgramming,  // step 3
    MeetInTheMiddle,     // step 3
    None                 // no exact algorithm in range, the answer is UNKNOWN (step 4)
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

// The rule when both exact algorithms are in range (provisional, task.md
// section 6): dynamic programming if T/32 < 2^(n/2), that is Proposition 4.4
// (T/64 < 2^(n/2)) with the cost of dynamic programming doubled for its table.
// Asked only for a target in the range of dynamic programming, so 64 bits.
bool dp_is_cheaper(std::size_t n, std::uint64_t T);

// The exact algorithm that SOLVE runs for a set of n numbers with target T:
// DynamicProgramming, MeetInTheMiddle, or None if neither is in range.
template <class V>
Method choose_exact(std::size_t n, const V& T, const Ranges& ranges);

// Requires n ≥ 2 and, for Native, S < 2^128 (both checked by read_instances).
template <class V>
Solution<V> solve(std::span<const V> a, const Ranges& ranges);

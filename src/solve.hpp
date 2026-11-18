// SOLVE, the final algorithm (report, Section 6.2): Karmarkar-Karp always,
// plus an exact algorithm when one is in range. Brute force is never used.
#pragma once

#include <cstddef>
#include <span>

#include "partition.hpp"

// The ranges of the exact algorithms (report, Section 1.4): dynamic
// programming if T ≤ t_dp and n · T ≤ w_dp, meet in the middle if n ≤ n_mitm.
struct Ranges {
    std::size_t n_mitm;
    Value t_dp;
    Value w_dp;
};

// Why dynamic programming is out of range for a set of n numbers with
// target T: Target if T > t_dp (or T > kMaxDpTarget), Work if n · T > w_dp.
enum class DpLimit { InRange, Target, Work };

DpLimit dp_limit(std::size_t n, Value T, const Ranges& ranges);

// What decided the answer of SOLVE.
enum class Method {
    KarmarkarKarp,       // d = 0, the partition is balanced (step 1)
    OddSum,              // S is odd (step 2)
    DynamicProgramming,  // step 3
    MeetInTheMiddle,     // step 3
    None                 // no exact algorithm in range, the answer is UNKNOWN (step 4)
};

struct Solution {
    Verdict answer;
    Method decided_by;
    Partition partition;   // of Karmarkar-Karp, with its discrepancy d
    ExactAnswer balanced;  // present iff the answer is YES: the partition above
                           // when d = 0, otherwise that of the exact algorithm
    Value S;
    Value T;  // S / 2
};

// The rule when both exact algorithms are in range (provisional, task.md
// section 6): dynamic programming if T/32 < 2^(n/2), that is Proposition 4.4
// (T/64 < 2^(n/2)) with the cost of dynamic programming doubled for its table.
bool dp_is_cheaper(std::size_t n, Value T);

// The exact algorithm that SOLVE runs for a set of n numbers with target T:
// DynamicProgramming, MeetInTheMiddle, or None if neither is in range.
Method choose_exact(std::size_t n, Value T, const Ranges& ranges);

// Requires n ≥ 2 and S < 2^128 (both checked by read_instances).
Solution solve(std::span<const Value> a, const Ranges& ranges);

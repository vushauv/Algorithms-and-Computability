// SOLVE, the final algorithm (report, Section 6.2): Karmarkar-Karp always, plus an exact
// algorithm when one is in range. Brute force is never used.
#pragma once

#include <cstddef>
#include <span>

#include "partition.hpp"

// Limits of the exact algorithms (report, Section 1.4): DP if T ≤ t_dp and n · T ≤ w_dp,
// meet in the middle if n ≤ n_mitm.
struct Ranges {
    std::size_t n_mitm;
    Value t_dp;
    Value w_dp;
};

// Why DP is out of range: Target if T > t_dp (or kMaxDpTarget), Work if n · T > w_dp.
enum class DpLimit { InRange, Target, Work };

DpLimit dp_limit(std::size_t n, Value T, const Ranges& ranges);

// What decided SOLVE's answer.
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
    Partition partition;   // Karmarkar-Karp, with its d
    ExactAnswer balanced;  // set iff the answer is YES: the partition above when d = 0,
                           // otherwise the exact algorithm's
    Value S;
    Value T;
};

// Which exact algorithm when both are in range (provisional, task.md section 6): DP if
// T/32 < 2^(n/2). That is Proposition 4.4 (T/64) with the cost doubled for the step table.
bool dp_is_cheaper(std::size_t n, Value T);

// The exact algorithm SOLVE runs: DP, MeetInTheMiddle, or None if neither is in range.
Method choose_exact(std::size_t n, Value T, const Ranges& ranges);

// Needs n ≥ 2 and S < 2^128 (read_instances checks both).
Solution solve(std::span<const Value> a, const Ranges& ranges);

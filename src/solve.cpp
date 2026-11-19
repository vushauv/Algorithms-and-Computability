#include "solve.hpp"

#include <cmath>
#include <utility>

#include "algorithms/dynamic_programming.hpp"
#include "algorithms/karmarkar_karp.hpp"
#include "algorithms/meet_in_the_middle.hpp"

bool dp_is_cheaper(std::size_t n, Value T) {
    // In floating point: 2^(n/2) is irrational for odd n, and T² may overflow.
    const long double t = static_cast<long double>(T);
    // t/64 words per pass (Proposition 4.4), doubled for the step table.
    return t / 32 < std::exp2(static_cast<long double>(n) / 2);
}

DpLimit dp_limit(std::size_t n, Value T, const Ranges& ranges) {
    // Above kMaxDpTarget dp_partition can't index its bitset, whatever t_dp says.
    if (T > ranges.t_dp || T > kMaxDpTarget) return DpLimit::Target;
    // n · T ≤ w_dp, written so that the product cannot overflow; n ≥ 2.
    if (T > ranges.w_dp / n) return DpLimit::Work;
    return DpLimit::InRange;
}

Method choose_exact(std::size_t n, Value T, const Ranges& ranges) {
    const bool dp_in_range = dp_limit(n, T, ranges) == DpLimit::InRange;
    const bool mitm_in_range = n <= ranges.n_mitm && n <= kMaxMeetInTheMiddleElements;
    if (dp_in_range && mitm_in_range) {
        return dp_is_cheaper(n, T) ? Method::DynamicProgramming : Method::MeetInTheMiddle;
    }
    if (dp_in_range) return Method::DynamicProgramming;
    if (mitm_in_range) return Method::MeetInTheMiddle;
    return Method::None;
}

Solution solve(std::span<const Value> a, const Ranges& ranges) {
    const Value S = total_sum(a);
    Partition partition = karmarkar_karp(a);  // O(n log n), always
    const Value T = S / 2;
    if (partition.d == 0) {
        ExactAnswer balanced = partition;
        return {Verdict::Yes, Method::KarmarkarKarp, std::move(partition), std::move(balanced), S, T};
    }
    if (S % 2 == 1) return {Verdict::No, Method::OddSum, std::move(partition), std::nullopt, S, T};

    const Method exact = choose_exact(a.size(), T, ranges);
    if (exact == Method::None) {
        return {Verdict::Unknown, exact, std::move(partition), std::nullopt, S, T};
    }
    ExactAnswer balanced =
        exact == Method::DynamicProgramming ? dp_partition(a) : meet_in_the_middle(a);
    const Verdict answer = verdict_of(balanced);
    return {answer, exact, std::move(partition), std::move(balanced), S, T};
}

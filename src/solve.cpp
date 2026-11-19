#include "solve.hpp"

#include <cmath>
#include <utility>

#include "algorithms/dynamic_programming.hpp"
#include "algorithms/karmarkar_karp.hpp"
#include "algorithms/meet_in_the_middle.hpp"

bool dp_is_cheaper(std::size_t n, std::uint64_t T) {
    // In floating point: 2^(n/2) is irrational for odd n, and T² may overflow.
    const long double t = static_cast<long double>(T);
    // t / 64 words per pass (Proposition 4.4) times 2 for the first-reach table.
    return t / 32 < std::exp2(static_cast<long double>(n) / 2);
}

template <class V>
DpLimit dp_limit(std::size_t n, const V& T, const Ranges& ranges) {
    // Above kMaxDpTarget dp_partition cannot index its bitset, whatever t_dp says.
    const std::optional<std::uint64_t> target = to_u64(T);
    if (!target || *target > ranges.t_dp || *target > kMaxDpTarget) return DpLimit::Target;
    // n · T ≤ w_dp, written so that the product cannot overflow; n ≥ 2.
    if (*target > ranges.w_dp / n) return DpLimit::Work;
    return DpLimit::InRange;
}

template <class V>
Method choose_exact(std::size_t n, const V& T, const Ranges& ranges) {
    const bool dp_in_range = dp_limit(n, T, ranges) == DpLimit::InRange;
    const bool mitm_in_range = n <= ranges.n_mitm && n <= kMaxMeetInTheMiddleElements;
    if (dp_in_range && mitm_in_range) {
        return dp_is_cheaper(n, *to_u64(T)) ? Method::DynamicProgramming : Method::MeetInTheMiddle;
    }
    if (dp_in_range) return Method::DynamicProgramming;
    if (mitm_in_range) return Method::MeetInTheMiddle;
    return Method::None;
}

template <class V>
Solution<V> solve(std::span<const V> a, const Ranges& ranges) {
    const V S = total_sum(a);
    Partition<V> partition = karmarkar_karp(a);  // O(n log n), always
    const V T = half(S);
    if (partition.d == 0) {
        ExactAnswer<V> balanced = partition;
        return {Verdict::Yes, Method::KarmarkarKarp, std::move(partition), std::move(balanced), S, T};
    }
    if (is_odd(S)) return {Verdict::No, Method::OddSum, std::move(partition), std::nullopt, S, T};

    const Method exact = choose_exact(a.size(), T, ranges);
    if (exact == Method::None) {
        return {Verdict::Unknown, exact, std::move(partition), std::nullopt, S, T};
    }
    ExactAnswer<V> balanced =
        exact == Method::DynamicProgramming ? dp_partition(a) : meet_in_the_middle(a);
    const Verdict answer = verdict_of(balanced);
    return {answer, exact, std::move(partition), std::move(balanced), S, T};
}

#define INSTANTIATE(V)                                                 \
    template DpLimit dp_limit(std::size_t, const V&, const Ranges&);   \
    template Method choose_exact(std::size_t, const V&, const Ranges&); \
    template Solution<V> solve(std::span<const V>, const Ranges&);
FOR_EACH_NUMBER_TYPE(INSTANTIATE)
#undef INSTANTIATE

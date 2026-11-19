#include "partition.hpp"

#include <algorithm>
#include <stdexcept>
#include <utility>

template <class V>
V total_sum(std::span<const V> a) {
    V S = 0;
    for (const V& x : a) {
        auto sum = checked_add(S, x);
        if (!sum) throw std::overflow_error("the sum of the set is 2^128 or more");
        S = std::move(*sum);
    }
    return S;
}

template <class V>
Preliminary<V> preliminary_checks(std::span<const V> a) {
    const V S = total_sum(a);
    if (is_odd(S)) return {Verdict::No, S, 0};
    if (S == 0) return {Verdict::Yes, S, 0};  // {a[0]} against the rest, n ≥ 2
    return {std::nullopt, S, half(S)};
}

template <class V>
ExactAnswer<V> trivial_answer(const Preliminary<V>& pre, std::size_t n) {
    if (pre.verdict == Verdict::No) return std::nullopt;
    return balanced_partition<V>(n, {0});
}

template <class V>
Partition<V> balanced_partition(std::size_t n, std::vector<std::size_t> A) {
    std::sort(A.begin(), A.end());
    std::vector<std::size_t> B;
    B.reserve(n - A.size());
    std::size_t next = 0;  // the next index of A
    for (std::size_t i = 0; i < n; ++i) {
        if (next < A.size() && A[next] == i) {
            ++next;
        } else {
            B.push_back(i);
        }
    }
    return {std::move(A), std::move(B), 0};
}

#define INSTANTIATE(V)                                                        \
    template V total_sum(std::span<const V>);                                 \
    template Preliminary<V> preliminary_checks(std::span<const V>);           \
    template ExactAnswer<V> trivial_answer(const Preliminary<V>&, std::size_t); \
    template Partition<V> balanced_partition(std::size_t, std::vector<std::size_t>);
FOR_EACH_NUMBER_TYPE(INSTANTIATE)
#undef INSTANTIATE

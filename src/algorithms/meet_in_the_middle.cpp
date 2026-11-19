#include "algorithms/meet_in_the_middle.hpp"

#include <algorithm>
#include <bit>
#include <cstdint>
#include <functional>
#include <stdexcept>
#include <utility>

#include "algorithms/brute_force.hpp"
#include "memory.hpp"

namespace {

// The balanced partition whose part A has the sum x in the first h elements
// and the sum y in the others. x is an entry of LS and y one of RS, so a
// subset with each sum exists and find_subset finds it.
template <class V>
Partition<V> recover(std::span<const V> a, std::size_t h, const V& x, const V& y) {
    std::vector<std::size_t> A = *find_subset(a.first(h), x);
    const std::vector<std::size_t> right = *find_subset(a.subspan(h), y);
    for (std::size_t i : right) A.push_back(h + i);
    return balanced_partition<V>(a.size(), std::move(A));
}

}  // namespace

template <class V>
std::vector<V> subset_sums(std::span<const V> b) {
    const std::size_t m = b.size();
    const std::uint64_t count = std::uint64_t{1} << m;
    std::vector<V> sums(count);
    sums[0] = 0;
    for (std::uint64_t mask = 1; mask < count; ++mask) {
        const int k = std::countr_zero(mask);  // lowest set bit, Section 2.1
        sums[mask] = sums[mask ^ (std::uint64_t{1} << k)] + b[k];
    }
    return sums;
}

template <class V>
ExactAnswer<V> meet_in_the_middle(std::span<const V> a) {
    const std::size_t n = a.size();
    if (n > kMaxMeetInTheMiddleElements) {
        throw std::invalid_argument("meet in the middle needs at most 124 elements");
    }

    const Preliminary<V> pre = preliminary_checks(a);
    if (pre.verdict) return trivial_answer(pre, n);
    const V& T = pre.T;

    const std::size_t h = n / 2;
    // No sum is larger than S, which bounds the bytes of a stored one.
    require_memory("meet in the middle",
                   ((Native{1} << h) + (Native{1} << (n - h))) * stored_bytes(pre.S));
    std::vector<V> LS = subset_sums(a.first(h));
    std::vector<V> RS = subset_sums(a.subspan(h));
    std::sort(LS.begin(), LS.end());
    std::sort(RS.begin(), RS.end(), std::greater<V>{});

    // Every LS[i] + RS[j] is the sum of a subset of a, so at most S.
    std::size_t i = 0;
    std::size_t j = 0;
    while (i < LS.size() && j < RS.size()) {
        const V s = LS[i] + RS[j];
        if (s == T) return recover(a, h, LS[i], RS[j]);
        if (s < T) {
            ++i;
        } else {
            ++j;
        }
    }
    return std::nullopt;
}

#define INSTANTIATE(V)                                     \
    template std::vector<V> subset_sums(std::span<const V>); \
    template ExactAnswer<V> meet_in_the_middle(std::span<const V>);
FOR_EACH_NUMBER_TYPE(INSTANTIATE)
#undef INSTANTIATE

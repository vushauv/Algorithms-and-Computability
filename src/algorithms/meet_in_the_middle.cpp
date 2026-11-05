#include "algorithms/meet_in_the_middle.hpp"

#include <algorithm>
#include <bit>
#include <cstdint>
#include <functional>
#include <stdexcept>

std::vector<Value> subset_sums(std::span<const Value> b) {
    const std::size_t m = b.size();
    const std::uint64_t count = std::uint64_t{1} << m;
    std::vector<Value> sums(count);
    sums[0] = 0;
    for (std::uint64_t mask = 1; mask < count; ++mask) {
        const int k = std::countr_zero(mask);  // lowest set bit, Section 2.1
        sums[mask] = sums[mask ^ (std::uint64_t{1} << k)] + b[k];
    }
    return sums;
}

Verdict meet_in_the_middle(std::span<const Value> a) {
    const std::size_t n = a.size();
    if (n > kMaxMeetInTheMiddleElements) {
        throw std::invalid_argument("meet in the middle needs at most 124 elements");
    }

    const Preliminary pre = preliminary_checks(a);
    if (pre.verdict) return *pre.verdict;
    const Value T = pre.T;

    const std::size_t h = n / 2;
    std::vector<Value> LS = subset_sums(a.first(h));
    std::vector<Value> RS = subset_sums(a.subspan(h));
    std::sort(LS.begin(), LS.end());
    std::sort(RS.begin(), RS.end(), std::greater<Value>{});

    // Every LS[i] + RS[j] is the sum of a subset of a, so at most S < 2^128.
    std::size_t i = 0;
    std::size_t j = 0;
    while (i < LS.size() && j < RS.size()) {
        const Value s = LS[i] + RS[j];
        if (s == T) return Verdict::Yes;
        if (s < T) {
            ++i;
        } else {
            ++j;
        }
    }
    return Verdict::No;
}

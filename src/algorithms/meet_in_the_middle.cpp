#include "algorithms/meet_in_the_middle.hpp"

#include <algorithm>
#include <bit>
#include <cstdint>
#include <functional>
#include <stdexcept>
#include <string>
#include <utility>

#include "algorithms/brute_force.hpp"
#include "memory.hpp"

namespace {

// The balanced partition whose part A has the sum x in the first h elements
// and the sum y in the others. x is an entry of LS and y one of RS, so a
// subset with each sum exists and find_subset finds it.
Partition recover(std::span<const Value> a, std::size_t h, Value x, Value y) {
    std::vector<std::size_t> A = *find_subset(a.first(h), x);
    const std::vector<std::size_t> right = *find_subset(a.subspan(h), y);
    for (std::size_t i : right) A.push_back(h + i);
    return balanced_partition(a.size(), std::move(A));
}

}  // namespace

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

ExactAnswer meet_in_the_middle(std::span<const Value> a) {
    const std::size_t n = a.size();
    if (n > kMaxMeetInTheMiddleElements) {
        throw std::invalid_argument("meet in the middle needs at most " +
                                    std::to_string(kMaxMeetInTheMiddleElements) + " elements");
    }

    const Preliminary pre = preliminary_checks(a);
    if (pre.verdict) return trivial_answer(pre, n);
    const Value T = pre.T;

    const std::size_t h = n / 2;
    require_memory("meet in the middle",
                   ((Value{1} << h) + (Value{1} << (n - h))) * sizeof(Value));
    std::vector<Value> LS = subset_sums(a.first(h));
    std::vector<Value> RS = subset_sums(a.subspan(h));
    std::sort(LS.begin(), LS.end());
    std::sort(RS.begin(), RS.end(), std::greater<Value>{});

    // Every LS[i] + RS[j] is the sum of a subset of a, so at most S < 2^128.
    std::size_t i = 0;
    std::size_t j = 0;
    while (i < LS.size() && j < RS.size()) {
        const Value s = LS[i] + RS[j];
        if (s == T) return recover(a, h, LS[i], RS[j]);
        if (s < T) {
            ++i;
        } else {
            ++j;
        }
    }
    return std::nullopt;
}

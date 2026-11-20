// Checks of the partitions that the algorithms return, shared by the tests.
#pragma once

#include <algorithm>
#include <cstddef>
#include <vector>

#include "partition.hpp"
#include "value.hpp"

namespace partition_check {

inline Value sum_of(const std::vector<Value>& a, const std::vector<std::size_t>& indices) {
    return total_sum<Value>(values_at<Value>(a, indices));
}

// A and B are non-empty, disjoint and cover 0..n-1, and d is the difference
// of their sums.
inline bool is_partition(const std::vector<Value>& a, const Partition<Value>& p) {
    if (p.A.empty() || p.B.empty()) return false;
    std::vector<std::size_t> all = p.A;
    all.insert(all.end(), p.B.begin(), p.B.end());
    std::sort(all.begin(), all.end());
    if (all.size() != a.size()) return false;
    for (std::size_t i = 0; i < a.size(); ++i) {
        if (all[i] != i) return false;
    }
    const Value sumA = sum_of(a, p.A);
    const Value sumB = sum_of(a, p.B);
    return p.d == (sumA >= sumB ? sumA - sumB : sumB - sumA);
}

// The answer of an exact algorithm is the expected verdict, and a YES comes
// with a balanced partition: a valid one with d = 0.
inline bool is_answer(const std::vector<Value>& a, const ExactAnswer<Value>& answer, Verdict expected) {
    if (verdict_of(answer) != expected) return false;
    return !answer || (answer->d == 0 && is_partition(a, *answer));
}

// The same for the answer of the algorithm on a.
template <class Algorithm>
bool answers(Algorithm algorithm, const std::vector<Value>& a, Verdict expected) {
    return is_answer(a, algorithm(a), expected);
}

}  // namespace partition_check

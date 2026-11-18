#include "partition.hpp"

#include <algorithm>
#include <stdexcept>
#include <utility>

Value total_sum(std::span<const Value> a) {
    Value S = 0;
    for (Value x : a) {
        auto sum = checked_add(S, x);
        if (!sum) throw std::overflow_error("the sum of the set is 2^128 or more");
        S = *sum;
    }
    return S;
}

Preliminary preliminary_checks(std::span<const Value> a) {
    const Value S = total_sum(a);
    if (S % 2 == 1) return {Verdict::No, S, 0};
    if (S == 0) return {Verdict::Yes, S, 0};  // {a[0]} against the rest, n ≥ 2
    return {std::nullopt, S, S / 2};
}

ExactAnswer trivial_answer(const Preliminary& pre, std::size_t n) {
    if (pre.verdict == Verdict::No) return std::nullopt;
    return balanced_partition(n, {0});
}

Partition balanced_partition(std::size_t n, std::vector<std::size_t> A) {
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

Verdict verdict_of(const ExactAnswer& answer) {
    return answer ? Verdict::Yes : Verdict::No;
}

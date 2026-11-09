#include "partition.hpp"

#include <stdexcept>

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

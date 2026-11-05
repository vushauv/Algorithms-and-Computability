#include "algorithms/brute_force.hpp"

#include <bit>
#include <cstdint>
#include <stdexcept>

Verdict brute_force(std::span<const Value> a) {
    const std::size_t n = a.size();
    if (n > kMaxBruteForceElements) {
        throw std::invalid_argument("brute force needs at most 64 elements");
    }

    const Preliminary pre = preliminary_checks(a);
    if (pre.verdict) return *pre.verdict;
    const Value T = pre.T;

    // Bit i of the mask g puts a[i] in A; a[n − 1] stays in B.
    const std::size_t m = n - 1;
    const std::uint64_t end = std::uint64_t{1} << m;  // m ≤ 63
    std::uint64_t g = 0;
    Value sumA = 0;
    for (std::uint64_t k = 1; k < end; ++k) {
        const int j = std::countr_zero(k);  // the bit that changes
        g ^= std::uint64_t{1} << j;
        if ((g >> j) & 1) {
            sumA += a[j];  // a[j] enters A
        } else {
            sumA -= a[j];  // a[j] leaves A; it was added before, no underflow
        }
        if (sumA == T) return Verdict::Yes;
    }
    return Verdict::No;
}

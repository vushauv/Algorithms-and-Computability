#include "algorithms/brute_force.hpp"

#include <bit>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <utility>

namespace {

// The indices of the bits of g that are set, ascending.
std::vector<std::size_t> indices_of(std::uint64_t g) {
    std::vector<std::size_t> indices;
    for (; g != 0; g &= g - 1) indices.push_back(static_cast<std::size_t>(std::countr_zero(g)));
    return indices;
}

}  // namespace

std::optional<std::vector<std::size_t>> find_subset(std::span<const Value> b, Value x) {
    if (b.size() > kMaxFindSubsetElements) {
        throw std::invalid_argument("find_subset needs at most " +
                                    std::to_string(kMaxFindSubsetElements) + " elements");
    }
    // Bit i of the mask g puts b[i] in the subset; g = 0 is the empty subset.
    std::uint64_t g = 0;
    Value sum = 0;
    if (sum == x) return std::vector<std::size_t>{};
    const std::uint64_t end = std::uint64_t{1} << b.size();  // b.size() ≤ 63
    for (std::uint64_t k = 1; k < end; ++k) {
        const int j = std::countr_zero(k);
        g ^= std::uint64_t{1} << j;
        if ((g >> j) & 1) {
            sum += b[j];
        } else {
            sum -= b[j];  // b[j] was added before, no underflow
        }
        if (sum == x) return indices_of(g);
    }
    return std::nullopt;
}

ExactAnswer brute_force(std::span<const Value> a) {
    const std::size_t n = a.size();
    if (n > kMaxBruteForceElements) {
        throw std::invalid_argument("brute force needs at most " +
                                    std::to_string(kMaxBruteForceElements) + " elements");
    }

    const Preliminary pre = preliminary_checks(a);
    if (pre.verdict) return trivial_answer(pre, n);

    // a[n − 1] stays in B (Lemma 2.2), so A is a subset of the first n − 1.
    std::optional<std::vector<std::size_t>> A = find_subset(a.first(n - 1), pre.T);
    if (!A) return std::nullopt;
    return balanced_partition(n, std::move(*A));
}

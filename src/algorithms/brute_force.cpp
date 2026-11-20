#include "algorithms/brute_force.hpp"

#include <bit>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <utility>

namespace {

std::vector<std::size_t> indices_of(std::uint64_t g) {
    std::vector<std::size_t> indices;
    for (; g != 0; g &= g - 1) indices.push_back(static_cast<std::size_t>(std::countr_zero(g)));
    return indices;
}

}  // namespace

template <class V>
std::optional<std::vector<std::size_t>> find_subset(std::span<const V> b, const V& x) {
    if (b.size() > kMaxFindSubsetElements) {
        throw std::invalid_argument("find_subset needs at most " +
                                    std::to_string(kMaxFindSubsetElements) + " elements");
    }
    // Bit i of the mask g puts b[i] in the subset; g = 0 is the empty subset.
    std::uint64_t g = 0;
    V sum = 0;
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

template <class V>
ExactAnswer<V> brute_force(std::span<const V> a) {
    const std::size_t n = a.size();
    if (n > kMaxBruteForceElements) {
        throw std::invalid_argument("brute force needs at most " +
                                    std::to_string(kMaxBruteForceElements) + " elements");
    }

    const Preliminary<V> pre = preliminary_checks(a);
    if (pre.verdict) return trivial_answer(pre, n);

    // a[n − 1] stays in B (Lemma 2.2), so A is a subset of the first n − 1.
    std::optional<std::vector<std::size_t>> A = find_subset(a.first(n - 1), pre.T);
    if (!A) return std::nullopt;
    return balanced_partition<V>(n, std::move(*A));
}

#define INSTANTIATE(V)                                                                       \
    template std::optional<std::vector<std::size_t>> find_subset(std::span<const V>, const V&); \
    template ExactAnswer<V> brute_force(std::span<const V>);
FOR_EACH_NUMBER_TYPE(INSTANTIATE)
#undef INSTANTIATE

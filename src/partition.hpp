// Types shared by the algorithms, and the checks every exact algorithm does first.
// Indices are 0-based.
// V is the type of the numbers (number.hpp).
#pragma once

#include <cstddef>
#include <optional>
#include <span>
#include <vector>

#include "number.hpp"

// Answer to the decision problem. Unknown only comes from SOLVE, when no exact
// algorithm is in range.
enum class Verdict { No, Yes, Unknown };

// A partition into the index sets A and B with discrepancy d = |sum(A) - sum(B)|.
template <class V>
struct Partition {
    std::vector<std::size_t> A;
    std::vector<std::size_t> B;
    V d;
};

// Result of an exact algorithm: a balanced partition (d = 0), or nothing for NO.
template <class V>
using ExactAnswer = std::optional<Partition<V>>;

// Result of the trivial cases: a verdict if S is odd (NO) or 0 (YES),
// otherwise no verdict and T = S / 2.
template <class V>
struct Preliminary {
    std::optional<Verdict> verdict;
    V S;
    V T;
};

// S = sum of a[i]. Throws std::overflow_error if S >= 2^128 (Native only). read_instances
// already rejects such sets, so only data made in code can hit this.
template <class V>
V total_sum(std::span<const V> a);

template <class V>
Preliminary<V> preliminary_checks(std::span<const V> a);

// The elements of a at the given indices, in the order of the indices.
template <class V>
std::vector<V> values_at(std::span<const V> a, const std::vector<std::size_t>& indices);

// Answer for a trivial case: NO for an odd S; for S = 0, {a[0]} against the rest.
template <class V>
ExactAnswer<V> trivial_answer(const Preliminary<V>& pre, std::size_t n);

// A and its complement B, both sorted, d = 0. The caller has checked that sum(A) = S/2.
template <class V>
Partition<V> balanced_partition(std::size_t n, std::vector<std::size_t> A);

template <class V>
Verdict verdict_of(const ExactAnswer<V>& answer) {
    return answer ? Verdict::Yes : Verdict::No;
}

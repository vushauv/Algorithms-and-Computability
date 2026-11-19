// Shared types of the algorithms and the preliminary checks of the exact
// algorithms (report, Section 1.2). Indices are 0-based in the code, so the
// element a[i] of the report is a[i - 1] here. V is the type of the numbers
// (number.hpp).
#pragma once

#include <cstddef>
#include <optional>
#include <span>
#include <vector>

#include "number.hpp"

// Answer to the decision problem. Unknown only comes from SOLVE, when no exact
// algorithm is in range (report, Section 6.2).
enum class Verdict { No, Yes, Unknown };

// A partition into the index sets A and B with discrepancy d = |Σ(A) − Σ(B)|.
template <class V>
struct Partition {
    std::vector<std::size_t> A;
    std::vector<std::size_t> B;
    V d;
};

// The answer of an exact algorithm: a balanced partition (d = 0) when one
// exists, nothing (NO) when none exists.
template <class V>
using ExactAnswer = std::optional<Partition<V>>;

// The result of the two trivial cases of Section 1.2: a verdict when S is odd
// (NO) or S = 0 (YES), otherwise no verdict and the target T = S / 2 > 0.
template <class V>
struct Preliminary {
    std::optional<Verdict> verdict;
    V S;
    V T;
};

// S = Σ a[i]. Throws std::overflow_error if S does not fit (Native, S ≥ 2^128);
// read_instances already rejects such sets, so this happens only for data
// made in code.
template <class V>
V total_sum(std::span<const V> a);

template <class V>
Preliminary<V> preliminary_checks(std::span<const V> a);

// The elements of a at the given indices, in the order of the indices.
template <class V>
std::vector<V> values_at(std::span<const V> a, const std::vector<std::size_t>& indices);

// The answer of a trivial case (pre.verdict is set): NO for an odd S, and for
// S = 0 the balanced partition {a[0]} against the rest, as n ≥ 2.
template <class V>
ExactAnswer<V> trivial_answer(const Preliminary<V>& pre, std::size_t n);

// The partition of 0..n−1 into A and its complement B, both sorted, with
// d = 0. Requires Σ(A) = S/2, which the caller has established.
template <class V>
Partition<V> balanced_partition(std::size_t n, std::vector<std::size_t> A);

template <class V>
Verdict verdict_of(const ExactAnswer<V>& answer) {
    return answer ? Verdict::Yes : Verdict::No;
}

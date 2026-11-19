// Types shared by the algorithms, and the checks every exact algorithm does first
// (report, Section 1.2). Indices are 0-based here, so the report's a[i] is a[i - 1].
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
struct Partition {
    std::vector<std::size_t> A;
    std::vector<std::size_t> B;
    Value d;
};

// Result of an exact algorithm: a balanced partition (d = 0), or nothing for NO.
using ExactAnswer = std::optional<Partition>;

// Result of the trivial cases (Section 1.2): a verdict if S is odd (NO) or 0 (YES),
// otherwise no verdict and T = S / 2.
struct Preliminary {
    std::optional<Verdict> verdict;
    Value S;
    Value T;
};

// S = Σ a[i]. Throws std::overflow_error if S ≥ 2^128. read_instances already rejects
// such sets, so only data made in code can hit this.
Value total_sum(std::span<const Value> a);

Preliminary preliminary_checks(std::span<const Value> a);

std::vector<Value> values_at(std::span<const Value> a, const std::vector<std::size_t>& indices);

// Answer for a trivial case: NO for an odd S; for S = 0, {a[0]} against the rest.
ExactAnswer trivial_answer(const Preliminary& pre, std::size_t n);

// A and its complement B, both sorted, d = 0. The caller has checked that Σ(A) = S/2.
Partition balanced_partition(std::size_t n, std::vector<std::size_t> A);

Verdict verdict_of(const ExactAnswer& answer);

// Shared types of the algorithms and the preliminary checks of the exact
// algorithms (report, Section 1.2). Indices are 0-based in the code, so the
// element a[i] of the report is a[i - 1] here.
#pragma once

#include <cstddef>
#include <optional>
#include <span>
#include <vector>

#include "number.hpp"

// The answer to the decision problem. Unknown only comes from SOLVE, when no
// exact algorithm is in range (report, Section 6.2).
enum class Verdict { No, Yes, Unknown };

// A partition into the index sets A and B with discrepancy d = |Σ(A) − Σ(B)|.
struct Partition {
    std::vector<std::size_t> A;
    std::vector<std::size_t> B;
    Value d;
};

// The result of the two trivial cases of Section 1.2: a verdict when S is odd
// (NO) or S = 0 (YES), otherwise no verdict and the target T = S / 2 > 0.
struct Preliminary {
    std::optional<Verdict> verdict;
    Value S;
    Value T;
};

// S = Σ a[i]. Throws std::overflow_error if S ≥ 2^128; read_instances
// already rejects such sets, so this happens only for data made in code.
Value total_sum(std::span<const Value> a);

Preliminary preliminary_checks(std::span<const Value> a);

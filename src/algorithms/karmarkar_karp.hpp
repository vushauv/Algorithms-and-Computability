// KARMARKAR-KARP differencing (report, Section 5.2): the approximation, which
// returns a partition for every input, balanced or not. O(n log n).
#pragma once

#include <span>

#include "partition.hpp"

// Requires n ≥ 2. Returns two non-empty index sets A and B (each sorted) that
// cover 0..n−1, and d = |Σ(A) − Σ(B)| (Theorem 5.2).
Partition karmarkar_karp(std::span<const Value> a);

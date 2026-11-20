// KARMARKAR-KARP differencing (report, Section 5.2), the approximation. Gives a partition
// for every input, balanced or not. O(n log n).
#pragma once

#include <span>

#include "partition.hpp"

// Needs n ≥ 2. Returns non-empty, sorted index sets A and B that cover 0..n−1, and
// d = |Σ(A) − Σ(B)| (Theorem 5.2).
template <class V>
Partition<V> karmarkar_karp(std::span<const V> a);

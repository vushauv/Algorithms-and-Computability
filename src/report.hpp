// The console report of a run: the set read, the partitions and the answer of
// each algorithm. Text and colours only; the algorithms are run by main.cpp.
// The colours come from the output module, so the text is the same with and
// without them.
#pragma once

#include <cstddef>
#include <span>
#include <string>
#include <string_view>

#include "input.hpp"
#include "partition.hpp"
#include "solve.hpp"

namespace report {

// "Set i (line l): n = ..., S = ..." and the numbers of the set.
void print_instance(std::size_t index, const Instance& instance);

// The partition of Karmarkar-Karp with its discrepancy d.
void print_partition(std::span<const Value> a, const Partition& partition);

// The answer of one exact algorithm run alone, with its partition if YES.
void print_exact(std::string_view name, std::span<const Value> a, const ExactAnswer& answer,
                 std::string_view reason);

// The result of SOLVE: the partition of Karmarkar-Karp, the balanced partition
// of the exact algorithm if there is one, and the answer with its reason.
void print_solution(const Instance& instance, const Ranges& ranges, const Solution& solution);

// Why dynamic programming is out of range for n numbers with target T:
// "T = ... > T_DP = ..." or "n * T = ... > W_DP = ...". Requires that it is.
std::string dp_out_of_range(std::size_t n, Value T, const Ranges& ranges);

}  // namespace report

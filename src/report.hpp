// What the program prints for each set: the set, the partitions, the answer.
// Text only, the colours come from output. The algorithms are run by run.cpp.
#pragma once

#include <cstddef>
#include <span>
#include <string>
#include <string_view>

#include "input.hpp"
#include "partition.hpp"
#include "solve.hpp"

namespace report {

// Header of a set ("Set 1 (line 2): n = ..., S = ...") and its numbers.
void print_instance(std::size_t index, const Instance& instance);

// The Karmarkar-Karp partition and its d.
void print_partition(std::span<const Value> a, const Partition& partition);

// The answer of one exact algorithm run alone, and its partition if YES.
void print_exact(std::string_view name, std::span<const Value> a, const ExactAnswer& answer,
                 std::string_view reason);

// The result of SOLVE: the Karmarkar-Karp partition, the exact algorithm's partition if
// there is one, and the answer with its reason.
void print_solution(const Instance& instance, const Ranges& ranges, const Solution& solution);

// Why DP is out of range: "T = ... > T_DP = ..." or "n * T = ... > W_DP = ...".
// Call it only when DP is out of range.
std::string dp_out_of_range(std::size_t n, Value T, const Ranges& ranges);

}  // namespace report

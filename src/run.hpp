// Runs one set with the algorithm chosen on the command line, prints the
// result (through the report module) and gives back the partition for the
// results file.
#pragma once

#include <cstddef>
#include <optional>
#include <span>
#include <string>

#include "cli.hpp"
#include "input.hpp"
#include "partition.hpp"

namespace run {

// "set 2 (line 3)", for the messages about a set; index counts from 1.
std::string set_name(std::size_t index, const Instance& instance);

// Runs SOLVE, or the one algorithm of options.algorithm (the ranges still
// apply to meet in the middle and dynamic programming, so that a large set is
// skipped with a warning instead of exhausting the memory; raise them to go
// further). Returns the partition for the results file: with SOLVE the
// balanced one when the answer is YES, otherwise that of Karmarkar-Karp; with
// one algorithm alone the balanced one, or nothing if it answered NO or was
// skipped. Throws when the algorithm cannot run on the set (std::bad_alloc,
// std::invalid_argument, std::runtime_error).
std::optional<Partition> run_set(std::size_t index, const Instance& instance,
                                 const Options& options);

// The partition for the results file when run_set found none or failed. Every
// set in the file needs a partition, and Karmarkar-Karp gives one for every
// input; it is printed under "For the results file:".
Partition fallback_partition(std::span<const Value> a);

}  // namespace run

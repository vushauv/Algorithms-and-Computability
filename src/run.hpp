// Runs one set with the chosen algorithm, prints the result (through report) and
// returns the partition for the results file.
#pragma once

#include <cstddef>
#include <optional>
#include <span>
#include <string>

#include "cli.hpp"
#include "input.hpp"
#include "partition.hpp"

namespace run {

// "set 2 (line 3)", for messages. index counts from 1.
std::string set_name(std::size_t index, const Instance& instance);

// Runs SOLVE, or the single algorithm from options. The limits still apply to meet in
// the middle and DP, so a big set is skipped with a warning instead of using up the
// memory; raise them to go further.
// Returns the partition for the results file: with SOLVE the balanced one on YES,
// otherwise Karmarkar-Karp's; with a single algorithm the balanced one, or nothing on
// NO or skip. Throws if the algorithm can't run (bad_alloc, invalid_argument,
// runtime_error).
std::optional<Partition> run_set(std::size_t index, const Instance& instance,
                                 const Options& options);

// Partition for the results file when run_set gave none or threw. Every set needs one,
// and Karmarkar-Karp always has one. Printed under "For the results file:".
Partition fallback_partition(std::span<const Value> a);

}  // namespace run

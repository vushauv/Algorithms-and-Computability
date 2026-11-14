// The results file in Homenda's format (raw_regulations/email_27.10.26.txt,
// corrected on 28.10 and 29.10, example AaClab_output.txt):
//
//   N K                      the number of sets and of subsets
//                            a blank line before every set
//   S a1 a2 ... an           the sum of the set, then the set in input order
//   s x1 x2 ...              K lines, one per subset: its sum, then its numbers
//   ...                      in non-increasing order; the lines in
//                            lexicographically non-increasing order of the numbers
//   D                        the largest difference of the subset sums
//
// Numbers are separated by single spaces, lines by "\n", and the file has no
// newline at the end, as in the example.
#pragma once

#include <cstddef>
#include <span>
#include <string>
#include <string_view>
#include <vector>

#include "number.hpp"
#include "partition.hpp"

namespace result_file {

// The input path with _output.txt in place of .txt ("in.txt" gives
// "in_output.txt"); a path not ending in .txt gets _output.txt appended.
std::string path_for(std::string_view input_path);

// The first line, "N K".
std::string header(std::size_t sets, std::size_t subsets);

// The lines of one set, from "S a1 ... an" to "D", without a newline at the end.
std::string format_set(std::span<const Value> a, std::vector<std::vector<Value>> subsets);

// The same for the two parts of a partition of a; D is then its discrepancy d.
std::string format_set(std::span<const Value> a, const Partition& partition);

}  // namespace result_file

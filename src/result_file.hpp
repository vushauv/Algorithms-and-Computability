// The results file, in the course's format:
//
//   N K                 number of sets and of subsets
//                       a blank line before every set
//   S a1 a2 ... an      sum of the set, then the set as read
//   s x1 x2 ...         one line per subset: its sum, then its numbers (largest first);
//   ...                 the lines in lexicographically non-increasing order
//   D                   difference of the subset sums
//
// Single spaces, "\n" line ends (also on Windows), no newline at the end of the file.
#pragma once

#include <cstddef>
#include <span>
#include <string>
#include <string_view>
#include <vector>

#include "number.hpp"
#include "partition.hpp"

namespace result_file {

// The input path with _output.txt in place of .txt ("in.txt" gives "in_output.txt");
// any other name gets _output.txt appended.
std::string path_for(std::string_view input_path);

// First line, "N K".
std::string header(std::size_t sets, std::size_t subsets);

// One set, from "S a1 ... an" to "D", no newline at the end.
template <class V>
std::string format_set(std::span<const V> a, std::vector<std::vector<V>> subsets);

// The same for the two parts of a partition; D is its d.
template <class V>
std::string format_set(std::span<const V> a, const Partition<V>& partition);

}  // namespace result_file

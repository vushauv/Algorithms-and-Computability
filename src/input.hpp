// Reading of the input file: line 1 is the number of sets N, each of the
// next N lines is one multiset of numbers separated by single spaces. Extra
// spaces and tabs between the numbers and at the ends of a line are accepted.
// The limits are those of task.md section 7 (report, Section 1.1).
#pragma once

#include <cstddef>
#include <istream>
#include <stdexcept>
#include <string>
#include <vector>

#include "number.hpp"

inline constexpr std::size_t kMaxSets = 10'000;
inline constexpr std::size_t kMinElements = 2;
inline constexpr std::size_t kMaxElements = 1'000'000;

struct Instance {
    std::vector<Value> values;
    Value sum;         // below 2^128, checked while reading
    std::size_t line;  // line of the file, for messages
};

// Malformed input. line and column start at 1; column is 0 when the
// error concerns the whole line or the whole file.
class InputError : public std::runtime_error {
public:
    InputError(std::size_t line, std::size_t column, const std::string& message);
    std::size_t line() const { return line_; }
    std::size_t column() const { return column_; }

private:
    std::size_t line_;
    std::size_t column_;
};

std::vector<Instance> read_instances(std::istream& in);

// Opens the file and reads it; InputError also when it cannot be opened or
// is a directory. The messages do not repeat the path; the caller adds it.
std::vector<Instance> read_instances_from_file(const std::string& path);

// Reads the input file: line 1 is the number of sets, then one set per line.
// Extra spaces and tabs are accepted. Limits: task.md section 7.
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
    std::size_t line;
};

// Bad input. line and column count from 1; 0 means the whole line or file.
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

// Reads the whole file. Throws InputError if it can't be opened or is a directory.
// The messages don't include the path, the caller adds it.
std::vector<Instance> read_instances_from_file(const std::string& path);

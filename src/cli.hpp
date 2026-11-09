// Command-line options:
//   partition [--no-color] [--n-mitm N] [--t-dp T] FILE
// (--algorithm comes with the final algorithm, step 9 of the plan.)
#pragma once

#include <cstddef>
#include <stdexcept>
#include <string>

#include "number.hpp"

// Provisional ranges of the exact algorithms (task.md section 6).
inline constexpr std::size_t kDefaultNMitm = 44;
inline constexpr Value kDefaultTDp = 10'000'000'000;

// Meet in the middle keeps 2^(n/2) sums per half; beyond n = 64 the
// index of a subset no longer fits in a 64-bit mask.
inline constexpr std::size_t kMaxNMitm = 64;

struct Options {
    std::string path;
    bool no_color = false;
    bool help = false;
    std::size_t n_mitm = kDefaultNMitm;
    Value t_dp = kDefaultTDp;
};

class UsageError : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

Options parse_options(int argc, char** argv);

std::string usage_text();

// Command-line options:
//   partition [--no-color] [--n-mitm N] [--t-dp T] [--w-dp W] [--algorithm A] FILE
#pragma once

#include <cstddef>
#include <stdexcept>
#include <string>

#include "number.hpp"

// Provisional ranges of the exact algorithms (task.md section 6).
inline constexpr std::size_t kDefaultNMitm = 44;
inline constexpr Value kDefaultTDp = 10'000'000'000;
// Bound on n · T for dynamic programming, whose time is O(n · T / 64): about
// 10 s for the worst case on a 2026 laptop (a NO instance with d > 0).
inline constexpr Value kDefaultWDp = 1'000'000'000'000;

// Meet in the middle keeps 2^(n/2) sums per half; beyond n = 64 the
// index of a subset no longer fits in a 64-bit mask.
inline constexpr std::size_t kMaxNMitm = 64;

// --algorithm: SOLVE (the default) or one algorithm alone, for timings.
enum class Algorithm { Solve, BruteForce, MeetInTheMiddle, DynamicProgramming, KarmarkarKarp };

struct Options {
    std::string path;
    bool no_color = false;
    bool help = false;
    std::size_t n_mitm = kDefaultNMitm;
    Value t_dp = kDefaultTDp;
    Value w_dp = kDefaultWDp;
    Algorithm algorithm = Algorithm::Solve;
};

class UsageError : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

Options parse_options(int argc, char** argv);

std::string usage_text();

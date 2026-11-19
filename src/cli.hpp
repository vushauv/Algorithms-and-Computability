// Command-line options:
//   partition [--no-color] [--n-mitm N] [--t-dp T] [--w-dp W] [--algorithm A] FILE [K]
#pragma once

#include <cstddef>
#include <stdexcept>
#include <string>

#include "number.hpp"
#include "solve.hpp"

// Provisional ranges of the exact algorithms (task.md section 6).
inline constexpr std::size_t kDefaultNMitm = 44;
// Dynamic programming keeps T + 1 bits and T + 1 step numbers of 4 bytes, so
// about 1.2 GB at T = 3 · 10^8.
inline constexpr Value kDefaultTDp = 300'000'000;
// Bound on n · T for dynamic programming, whose time is O(n · T / 64): about
// 10 s for the worst case on a 2026 laptop (a NO instance with d > 0).
inline constexpr Value kDefaultWDp = 1'000'000'000'000;

// Meet in the middle keeps 2^(n/2) sums per half; beyond n = 64 the
// index of a subset no longer fits in a 64-bit mask.
inline constexpr std::size_t kMaxNMitm = 64;

// K, the number of subsets, which Homenda passes to every program; the program
// splits a set into two parts, so K = 2 is the only value accepted.
inline constexpr std::size_t kSubsets = 2;

// --algorithm: SOLVE (the default) or one algorithm alone, for timings.
enum class Algorithm { Solve, BruteForce, MeetInTheMiddle, DynamicProgramming, KarmarkarKarp };

struct Options {
    std::string path;
    std::size_t subsets = kSubsets;
    bool no_color = false;
    bool help = false;
    std::size_t n_mitm = kDefaultNMitm;
    Value t_dp = kDefaultTDp;
    Value w_dp = kDefaultWDp;
    Algorithm algorithm = Algorithm::Solve;

    // The ranges of the exact algorithms that SOLVE and the single runs respect.
    Ranges ranges() const { return Ranges{n_mitm, t_dp, w_dp}; }
};

class UsageError : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

Options parse_options(int argc, char** argv);

std::string usage_text();

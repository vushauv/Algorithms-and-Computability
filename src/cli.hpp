// Command-line options:
//   partition [--no-color] [--quiet] [--n-mitm N] [--t-dp T] [--w-dp W] [--algorithm A]
//             [--arithmetic boost|own|native] [--base B] FILE [K]
#pragma once

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <string>

#include "number.hpp"
#include "solve.hpp"

// Default limits of the exact algorithms. Provisional, see task.md section 6.
inline constexpr std::size_t kDefaultNMitm = 44;
// About 4 bytes per unit of T (the step table), so 1.2 GB at the default.
inline constexpr std::uint64_t kDefaultTDp = 300'000'000;
// Limit on n · T for DP, whose time is about n · T / 64. Roughly 10 s worst case on a laptop.
inline constexpr std::uint64_t kDefaultWDp = 1'000'000'000'000;

// Hard cap for meet in the middle: a subset mask has to fit in 64 bits.
inline constexpr std::size_t kMaxNMitm = 64;

// K is the number of subsets Homenda passes to every program. We only split in two,
// so K has to be 2.
inline constexpr std::size_t kSubsets = 2;

// What --algorithm picks: SOLVE by default, or one algorithm alone (for timings).
enum class Algorithm { Solve, BruteForce, MeetInTheMiddle, DynamicProgramming, KarmarkarKarp };

// --arithmetic: the type of the numbers (number.hpp). Native is limited to 128 bits.
enum class Arithmetic { Boost, Own, Native };

struct Options {
    std::string path;
    std::size_t subsets = kSubsets;
    bool no_color = false;
    bool quiet = false;  // nothing on stdout; errors and warnings still go to stderr
    bool help = false;
    std::size_t n_mitm = kDefaultNMitm;
    std::uint64_t t_dp = kDefaultTDp;
    std::uint64_t w_dp = kDefaultWDp;
    Algorithm algorithm = Algorithm::Solve;
    Arithmetic arithmetic = Arithmetic::Boost;
    std::uint64_t base = BigUint::kMaxBase;  // of the Own arithmetic

    Ranges ranges() const { return Ranges{n_mitm, t_dp, w_dp}; }
};

class UsageError : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

Options parse_options(int argc, char** argv);

std::string usage_text();

// Command-line options:
//   partition [--no-color] [--n-mitm N] [--t-dp T] [--w-dp W] [--algorithm A]
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
// Dynamic programming keeps T + 1 bits and T + 1 step numbers of 4 bytes, so
// about 1.2 GB at T = 3 · 10^8.
inline constexpr std::uint64_t kDefaultTDp = 300'000'000;
// Bound on n · T for dynamic programming, whose time is O(n · T / 64): about
// 10 s for the worst case on a 2026 laptop (a NO instance with d > 0).
inline constexpr std::uint64_t kDefaultWDp = 1'000'000'000'000;

// Hard cap for meet in the middle: a subset mask has to fit in 64 bits.
inline constexpr std::size_t kMaxNMitm = 64;

// K is the number of subsets Homenda passes to every program. We only split in two,
// so K has to be 2.
inline constexpr std::size_t kSubsets = 2;

// What --algorithm picks: SOLVE by default, or one algorithm alone (for timings).
enum class Algorithm { Solve, BruteForce, MeetInTheMiddle, DynamicProgramming, KarmarkarKarp };

// --arithmetic: the type of the numbers (number.hpp). Boost and Own have no
// limit on the size of a number; Native is the 128-bit type of the compiler.
enum class Arithmetic { Boost, Own, Native };

struct Options {
    std::string path;
    std::size_t subsets = kSubsets;
    bool no_color = false;
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

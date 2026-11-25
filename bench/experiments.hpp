#pragma once

#include <cstdint>
#include <cmath>
#include <string>
#include <vector>

#include "cli.hpp"

enum class GeneratorFamily {
    Random,
    Trap,
    Yes,
    Target,
    Small,
};

struct ArithmeticRun {
    Arithmetic arithmetic;
    std::uint64_t base = 0;
};

struct ExperimentFile {
    std::string name;
    GeneratorFamily family;
    std::size_t n = 0;
    std::size_t digits = 0;
    std::uint64_t target = 0;
    std::uint64_t small_max = 0;
    std::size_t sets = 0;
};

struct Experiment {
    std::string name;
    Algorithm algorithm;
    std::vector<ExperimentFile> files;
    std::vector<ArithmeticRun> arithmetics;
    int reps = 0;
    std::vector<Algorithm> algorithms;
};

namespace experiments {

inline const std::vector<ArithmeticRun> kAllArithmetics = {
    {Arithmetic::Native, 0},
    {Arithmetic::Boost, 0},
    {Arithmetic::Own, std::uint64_t{1} << 32},
};

inline const std::vector<ArithmeticRun> kBoostOnly = {
    {Arithmetic::Boost, 0},
};

inline ExperimentFile random_file(const std::string& name, std::size_t n,
                                  std::size_t digits, std::size_t sets) {
    return {name, GeneratorFamily::Random, n, digits, 0, 0, sets};
}

inline ExperimentFile trap_file(const std::string& name, std::size_t n,
                                std::size_t digits, std::size_t sets) {
    return {name, GeneratorFamily::Trap, n, digits, 0, 0, sets};
}

inline ExperimentFile yes_file(const std::string& name, std::size_t n,
                               std::size_t digits, std::size_t sets) {
    return {name, GeneratorFamily::Yes, n, digits, 0, 0, sets};
}

inline ExperimentFile target_file(const std::string& name, std::size_t n,
                                  std::uint64_t target, std::size_t sets) {
    return {name, GeneratorFamily::Target, n, 0, target, 0, sets};
}

inline ExperimentFile small_file(const std::string& name, std::size_t n,
                                 std::uint64_t small_max, std::size_t sets) {
    return {name, GeneratorFamily::Small, n, 0, 0, small_max, sets};
}

inline std::vector<ExperimentFile> size_series(GeneratorFamily family,
                                               const std::vector<std::size_t>& sizes,
                                               std::size_t digits, std::size_t sets) {
    std::vector<ExperimentFile> files;
    for (std::size_t n : sizes) {
        const std::string name =
            (family == GeneratorFamily::Trap ? "trap_n" : "random_n") +
            std::to_string(n) + ".txt";
        files.push_back(family == GeneratorFamily::Trap
                            ? trap_file(name, n, digits, sets)
                            : random_file(name, n, digits, sets));
    }
    return files;
}

inline std::vector<std::size_t> range_sizes(std::size_t first, std::size_t last,
                                             const std::vector<std::size_t>& extra) {
    std::vector<std::size_t> sizes;
    for (std::size_t n = first; n <= last; ++n) sizes.push_back(n);
    sizes.insert(sizes.end(), extra.begin(), extra.end());
    return sizes;
}

inline const std::vector<Experiment> kExperiments = [] {
    std::vector<Experiment> result;

    std::vector<std::size_t> bf_sizes = range_sizes(2, 12, {14, 15, 16, 17, 18, 19, 20,
                                                               21, 22, 23, 24, 25, 26, 27,
                                                               28, 29, 30, 31, 32, 33, 34});
    std::vector<ExperimentFile> e1;
    for (std::size_t n : bf_sizes) {
        e1.push_back(trap_file("trap_n" + std::to_string(n) + ".txt", n, 25, 10));
        e1.push_back(random_file("random_n" + std::to_string(n) + ".txt", n, 25, 10));
    }
    result.push_back({"E1", Algorithm::BruteForce, std::move(e1), kAllArithmetics, 3, {}});

    std::vector<std::size_t> mitm_sizes = range_sizes(2, 12, {});
    for (std::size_t n = 14; n <= 50; ++n) mitm_sizes.push_back(n);
    std::vector<ExperimentFile> e2;
    for (std::size_t n : mitm_sizes) {
        e2.push_back(trap_file("trap_n" + std::to_string(n) + ".txt", n, 25, 10));
        e2.push_back(random_file("random_n" + std::to_string(n) + ".txt", n, 25, 10));
    }
    result.push_back({"E2", Algorithm::MeetInTheMiddle, std::move(e2), kAllArithmetics, 3, {}});

    std::vector<ExperimentFile> e3a;
    for (int exponent = 3; exponent <= 9; ++exponent) {
        for (int half = 0; half <= 1; ++half) {
            if (exponent == 9 && half == 1) break;
            const std::uint64_t target = half == 0
                                             ? static_cast<std::uint64_t>(std::pow(10.0, exponent))
                                             : static_cast<std::uint64_t>(std::pow(10.0, exponent + 0.5));
            const std::string name = "n100_T" + std::to_string(target) + ".txt";
            e3a.push_back(target_file(name, 100, target, 5));
        }
    }
    result.push_back({"E3a", Algorithm::DynamicProgramming, std::move(e3a), kAllArithmetics, 3, {}});

    std::vector<ExperimentFile> e3b;
    for (std::size_t n : {std::size_t{2}, std::size_t{5}, std::size_t{10},
                          std::size_t{20}, std::size_t{50}, std::size_t{100},
                          std::size_t{300}, std::size_t{1000}, std::size_t{3000},
                          std::size_t{10000}, std::size_t{30000}, std::size_t{100000}}) {
        e3b.push_back(target_file("n" + std::to_string(n) + "_T10000000.txt",
                                  n, 10'000'000, 5));
    }
    result.push_back({"E3b", Algorithm::DynamicProgramming, std::move(e3b), kAllArithmetics, 3, {}});

    std::vector<ExperimentFile> e4;
    for (std::size_t n : {std::size_t{2}, std::size_t{3}, std::size_t{4},
                          std::size_t{5}, std::size_t{6}, std::size_t{7},
                          std::size_t{8}, std::size_t{9}, std::size_t{10},
                          std::size_t{15}, std::size_t{20}, std::size_t{30},
                          std::size_t{50}, std::size_t{75}, std::size_t{100},
                          std::size_t{150}, std::size_t{200}, std::size_t{300},
                          std::size_t{500}, std::size_t{1000}, std::size_t{3000},
                          std::size_t{10000}, std::size_t{30000},
                          std::size_t{100000}, std::size_t{300000},
                          std::size_t{1000000}}) {
        e4.push_back(random_file("random_n" + std::to_string(n) + ".txt", n, 25, 20));
    }
    result.push_back({"E4", Algorithm::KarmarkarKarp, std::move(e4), kAllArithmetics, 3, {}});

    std::vector<ExperimentFile> e5;
    for (std::size_t n = 24; n <= 44; n += 4) {
        for (std::uint64_t target : {std::uint64_t{100'000}, std::uint64_t{1'000'000},
                                      std::uint64_t{10'000'000}, std::uint64_t{100'000'000},
                                      std::uint64_t{1'000'000'000}}) {
            e5.push_back(target_file("n" + std::to_string(n) + "_T" + std::to_string(target) + ".txt",
                                     n, target, 5));
        }
    }
    result.push_back({"E5", Algorithm::DynamicProgramming, std::move(e5), kBoostOnly, 3, {}});
    // E5 is measured as DP and MITM; the bench runner adds the second algorithm.
    result.back().algorithms = {Algorithm::DynamicProgramming, Algorithm::MeetInTheMiddle};

    std::vector<ExperimentFile> e6;
    for (std::size_t digits : {std::size_t{3}, std::size_t{10}, std::size_t{25}}) {
        for (std::size_t n : {std::size_t{4}, std::size_t{6}, std::size_t{8},
                              std::size_t{10}, std::size_t{15}, std::size_t{20},
                              std::size_t{30}, std::size_t{50}, std::size_t{100},
                              std::size_t{200}, std::size_t{1000},
                              std::size_t{10000}, std::size_t{100000}}) {
            e6.push_back(random_file("random_d" + std::to_string(digits) +
                                         "_n" + std::to_string(n) + ".txt",
                                     n, digits, 50));
        }
    }
    for (std::size_t n : {std::size_t{4}, std::size_t{6}, std::size_t{8},
                          std::size_t{10}, std::size_t{15}, std::size_t{20},
                          std::size_t{30}, std::size_t{50}, std::size_t{100},
                          std::size_t{200}, std::size_t{1000},
                          std::size_t{10000}, std::size_t{100000}}) {
        e6.push_back(yes_file("yes_d3_n" + std::to_string(n) + ".txt", n, 3, 50));
    }
    result.push_back({"E6", Algorithm::KarmarkarKarp, std::move(e6), kBoostOnly, 1, {}});

    std::vector<ExperimentFile> e7;
    for (std::size_t n : {std::size_t{2}, std::size_t{5}, std::size_t{10},
                          std::size_t{20}, std::size_t{50}, std::size_t{100},
                          std::size_t{200}, std::size_t{500}, std::size_t{1000}}) {
        e7.push_back(random_file("random_n" + std::to_string(n) + ".txt", n, 25, 20));
        e7.push_back(small_file("small_n" + std::to_string(n) + "_M1000.txt", n, 1000, 20));
    }
    result.push_back({"E7", Algorithm::Solve, std::move(e7), kBoostOnly, 3, {}});

    const std::vector<std::size_t> digits = {1, 5, 10, 19, 20, 25, 30, 36, 38,
                                              50, 100, 300, 1000, 3000, 10000};
    std::vector<ExperimentFile> a1;
    for (std::size_t d : digits)
        a1.push_back(random_file("random_d" + std::to_string(d) + ".txt", 10000, d, 5));
    result.push_back({"A1", Algorithm::KarmarkarKarp, std::move(a1), kAllArithmetics, 3, {}});

    std::vector<ExperimentFile> a2;
    for (std::size_t d : digits)
        a2.push_back(random_file("random_d" + std::to_string(d) + ".txt", 30, d, 5));
    result.push_back({"A2", Algorithm::MeetInTheMiddle, std::move(a2), kAllArithmetics, 3, {}});

    std::vector<ExperimentFile> a3;
    for (std::size_t d : digits)
        a3.push_back(random_file("random_d" + std::to_string(d) + ".txt", 20, d, 5));
    result.push_back({"A3", Algorithm::BruteForce, std::move(a3), kAllArithmetics, 3, {}});

    std::vector<ExperimentFile> a4;
    for (std::size_t d : {std::size_t{25}, std::size_t{1000}}) {
        a4.push_back(random_file("random_d" + std::to_string(d) + "_n10000.txt", 10000, d, 5));
        a4.push_back(random_file("random_d" + std::to_string(d) + "_n30.txt", 30, d, 5));
    }
    std::vector<ArithmeticRun> a4_arithmetics = {
        {Arithmetic::Native, 0},
        {Arithmetic::Boost, 0},
    };
    for (std::uint64_t base : {std::uint64_t{2}, std::uint64_t{3}, std::uint64_t{10},
                               std::uint64_t{16}, std::uint64_t{256}, std::uint64_t{10000},
                               std::uint64_t{65536}, std::uint64_t{1000000000},
                               std::uint64_t{1} << 32}) {
        a4_arithmetics.push_back({Arithmetic::Own, base});
    }
    result.push_back({"A4", Algorithm::KarmarkarKarp, std::move(a4), std::move(a4_arithmetics), 3, {}});
    result.back().algorithms = {Algorithm::KarmarkarKarp, Algorithm::MeetInTheMiddle};

    return result;
}();

}  // namespace experiments

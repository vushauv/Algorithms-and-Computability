#include "check.hpp"
#include "cli.hpp"
#include "output.hpp"
#include "tests.hpp"
#include "value.hpp"

#include <string>

#include "algorithms/dynamic_programming.hpp"
#include <vector>

namespace {

Options parse(std::vector<std::string> args) {
    args.insert(args.begin(), "partition");
    std::vector<char*> argv;
    for (std::string& arg : args) argv.push_back(arg.data());
    return parse_options(static_cast<int>(argv.size()), argv.data());
}

}  // namespace

void test_cli() {
    Options defaults = parse({"in.txt"});
    CHECK(defaults.path == "in.txt");
    CHECK(!defaults.no_color);
    CHECK(defaults.n_mitm == kDefaultNMitm);
    CHECK(defaults.t_dp == kDefaultTDp);
    CHECK(defaults.algorithm == Algorithm::Solve);

    Options all = parse({"--no-color", "--n-mitm", "30", "--t-dp", "1000", "in.txt"});
    CHECK(all.no_color);
    CHECK(all.n_mitm == 30);
    CHECK(all.t_dp == 1000);
    CHECK(all.path == "in.txt");

    CHECK(parse({"--algorithm", "bf", "in.txt"}).algorithm == Algorithm::BruteForce);
    CHECK(parse({"--algorithm", "mitm", "in.txt"}).algorithm == Algorithm::MeetInTheMiddle);
    CHECK(parse({"--algorithm", "dp", "in.txt"}).algorithm == Algorithm::DynamicProgramming);
    CHECK(parse({"--algorithm", "kk", "in.txt"}).algorithm == Algorithm::KarmarkarKarp);
    CHECK(parse({"--algorithm", "solve", "in.txt"}).algorithm == Algorithm::Solve);
    CHECK_THROWS(parse({"--algorithm", "BF", "in.txt"}), UsageError);
    CHECK_THROWS(parse({"in.txt", "--algorithm"}), UsageError);

    // K, the number of subsets, after FILE: 2 only.
    CHECK(parse({"in.txt"}).subsets == kSubsets);
    Options with_k = parse({"in.txt", "2", "--no-color"});
    CHECK(with_k.path == "in.txt" && with_k.subsets == 2 && with_k.no_color);
    CHECK_THROWS(parse({"in.txt", "5"}), UsageError);
    CHECK_THROWS(parse({"in.txt", "two"}), UsageError);
    CHECK_THROWS(parse({"in.txt", "2", "2"}), UsageError);

    CHECK(parse({"--help"}).help);
    CHECK_THROWS(parse({}), UsageError);
    CHECK_THROWS(parse({"a.txt", "b.txt"}), UsageError);
    CHECK_THROWS(parse({"--bogus", "in.txt"}), UsageError);
    CHECK_THROWS(parse({"in.txt", "--n-mitm"}), UsageError);
    CHECK_THROWS(parse({"--n-mitm", "65", "in.txt"}), UsageError);
    CHECK_THROWS(parse({"--t-dp", "-1", "in.txt"}), UsageError);

    // --t-dp up to the largest target the bitset can index; --w-dp any 64-bit value.
    CHECK(parse({"--t-dp", to_string(kMaxDpTarget), "in.txt"}).t_dp == kMaxDpTarget);
    CHECK_THROWS(parse({"--t-dp", to_string(kMaxDpTarget + 1), "in.txt"}), UsageError);
    CHECK(parse({"in.txt"}).w_dp == kDefaultWDp);
    CHECK(parse({"--w-dp", "0", "in.txt"}).w_dp == 0);
    CHECK(parse({"--w-dp", "18446744073709551615", "in.txt"}).w_dp == ~std::uint64_t{0});
    CHECK_THROWS(parse({"--w-dp", "18446744073709551616", "in.txt"}), UsageError);  // 2^64
    CHECK_THROWS(parse({"--w-dp", "1e12", "in.txt"}), UsageError);

    // The arithmetic of the numbers; a base only for our own one.
    CHECK(parse({"in.txt"}).arithmetic == Arithmetic::Boost);
    CHECK(parse({"--arithmetic", "native", "in.txt"}).arithmetic == Arithmetic::Native);
    CHECK(parse({"--arithmetic", "boost", "in.txt"}).arithmetic == Arithmetic::Boost);
    CHECK(parse({"--arithmetic", "own", "in.txt"}).arithmetic == Arithmetic::Own);
    CHECK(parse({"--arithmetic", "own", "in.txt"}).base == BigUint::kMaxBase);
    CHECK(parse({"--arithmetic", "own", "--base", "10", "in.txt"}).base == 10);
    CHECK(parse({"--base", "2", "--arithmetic", "own", "in.txt"}).base == 2);
    CHECK(parse({"--base", "4294967296", "--arithmetic", "own", "in.txt"}).base == BigUint::kMaxBase);
    CHECK_THROWS(parse({"--arithmetic", "gmp", "in.txt"}), UsageError);
    CHECK_THROWS(parse({"--arithmetic", "own", "--base", "1", "in.txt"}), UsageError);
    CHECK_THROWS(parse({"--arithmetic", "own", "--base", "4294967297", "in.txt"}), UsageError);
    CHECK_THROWS(parse({"--base", "10", "in.txt"}), UsageError);  // Boost has no base
    CHECK_THROWS(parse({"--arithmetic", "native", "--base", "10", "in.txt"}), UsageError);

    // Shortened display of long sets (no colours are involved).
    std::vector<Value> few{1, 2, 3};
    CHECK(output::format_values<Value>(few) == "{1, 2, 3}");
    std::vector<Value> many;
    for (Value v = 1; v <= 30; ++v) many.push_back(v);
    CHECK(output::format_values<Value>(many) == "{1, 2, 3, 4, 5, 6, 7, 8, ... (20 more) ..., 29, 30}");
    CHECK(output::format_values<Value>(std::vector<Value>{}) == "{}");
}

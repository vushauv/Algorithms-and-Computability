#include "check.hpp"
#include "cli.hpp"
#include "output.hpp"
#include "tests.hpp"

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

    CHECK(parse({"--help"}).help);
    CHECK_THROWS(parse({}), UsageError);
    CHECK_THROWS(parse({"a.txt", "b.txt"}), UsageError);
    CHECK_THROWS(parse({"--bogus", "in.txt"}), UsageError);
    CHECK_THROWS(parse({"in.txt", "--n-mitm"}), UsageError);
    CHECK_THROWS(parse({"--n-mitm", "65", "in.txt"}), UsageError);
    CHECK_THROWS(parse({"--t-dp", "-1", "in.txt"}), UsageError);

    // --t-dp up to the largest target the bitset can index; --w-dp any value.
    CHECK(parse({"--t-dp", to_string(kMaxDpTarget), "in.txt"}).t_dp == kMaxDpTarget);
    CHECK_THROWS(parse({"--t-dp", to_string(kMaxDpTarget + 1), "in.txt"}), UsageError);
    CHECK(parse({"in.txt"}).w_dp == kDefaultWDp);
    CHECK(parse({"--w-dp", "0", "in.txt"}).w_dp == 0);
    CHECK(parse({"--w-dp", "99999999999999999999999999999999999999", "in.txt"}).w_dp ==
          kValueLimit - 1);
    CHECK_THROWS(parse({"--w-dp", "1e12", "in.txt"}), UsageError);

    // Shortened display of long sets (no colours are involved).
    std::vector<Value> few{1, 2, 3};
    CHECK(output::format_values(few) == "{1, 2, 3}");
    std::vector<Value> many;
    for (Value v = 1; v <= 30; ++v) many.push_back(v);
    CHECK(output::format_values(many) == "{1, 2, 3, 4, 5, 6, 7, 8, ... (20 more) ..., 29, 30}");
    CHECK(output::format_values(std::vector<Value>{}) == "{}");
}

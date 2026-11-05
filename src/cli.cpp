#include "cli.hpp"

#include <string_view>
#include <vector>

namespace {

Value parse_number_option(std::string_view name, std::string_view text) {
    ParsedValue parsed = parse_value(text);
    if (parsed.status != ParseStatus::Ok)
        throw UsageError(std::string(name) + " expects a non-negative integer below 10^38, got '" +
                         std::string(text) + "'");
    return parsed.value;
}

Algorithm parse_algorithm(std::string_view text) {
    if (text == "solve") return Algorithm::Solve;
    if (text == "bf") return Algorithm::BruteForce;
    if (text == "mitm") return Algorithm::MeetInTheMiddle;
    if (text == "dp") return Algorithm::DynamicProgramming;
    if (text == "kk") return Algorithm::KarmarkarKarp;
    throw UsageError("--algorithm expects solve, bf, mitm, dp or kk, got '" + std::string(text) +
                     "'");
}

}  // namespace

Options parse_options(int argc, char** argv) {
    Options options;
    std::vector<std::string_view> args(argv + 1, argv + argc);

    for (std::size_t i = 0; i < args.size(); ++i) {
        std::string_view arg = args[i];
        auto value_of_option = [&]() -> std::string_view {
            if (i + 1 >= args.size()) throw UsageError(std::string(arg) + " expects a value");
            return args[++i];
        };

        if (arg == "-h" || arg == "--help") {
            options.help = true;
        } else if (arg == "--no-color") {
            options.no_color = true;
        } else if (arg == "--n-mitm") {
            Value n = parse_number_option(arg, value_of_option());
            if (n > kMaxNMitm)
                throw UsageError("--n-mitm must be at most " + std::to_string(kMaxNMitm));
            options.n_mitm = static_cast<std::size_t>(n);
        } else if (arg == "--t-dp") {
            options.t_dp = parse_number_option(arg, value_of_option());
        } else if (arg == "--algorithm") {
            options.algorithm = parse_algorithm(value_of_option());
        } else if (arg.size() > 1 && arg[0] == '-') {
            throw UsageError("unknown option '" + std::string(arg) + "'");
        } else if (options.path.empty()) {
            options.path = arg;
        } else {
            throw UsageError("only one input file is accepted");
        }
    }

    if (options.path.empty() && !options.help) throw UsageError("no input file given");
    return options;
}

std::string usage_text() {
    return "Usage: partition [options] FILE\n"
           "\n"
           "Decides for every set in FILE whether it can be split into two parts\n"
           "with equal sums.\n"
           "\n"
           "FILE: line 1 is the number of sets, each next line is one set of\n"
           "non-negative integers (below 10^38) separated by single spaces.\n"
           "\n"
           "Options:\n"
           "  --no-color     plain output (also when NO_COLOR is set or the output\n"
           "                 is not a terminal)\n"
           "  --n-mitm N     meet in the middle only for sets of at most N numbers\n"
           "                 (default " + std::to_string(kDefaultNMitm) + ", at most " +
           std::to_string(kMaxNMitm) + ")\n"
           "  --t-dp T       dynamic programming only for half sums of at most T\n"
           "                 (default " + to_string(kDefaultTDp) + ")\n"
           "  --algorithm A  the algorithm to run on every set:\n"
           "                   solve  the final algorithm (default): Karmarkar-Karp,\n"
           "                          plus an exact algorithm when one is in range\n"
           "                   bf     brute force alone (exact, at most 64 numbers)\n"
           "                   mitm   meet in the middle alone (exact, within --n-mitm)\n"
           "                   dp     dynamic programming alone (exact, within --t-dp)\n"
           "                   kk     Karmarkar-Karp alone (approximation)\n"
           "  -h, --help     this text\n"
           "\n"
           "Exit codes: 0 success, 1 invalid input file, 2 invalid command line,\n"
           "3 an algorithm could not be run on some set.\n";
}

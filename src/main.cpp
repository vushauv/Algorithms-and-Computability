// Partition problem: reads the sets of the input file and runs SOLVE (report,
// Section 6.2) on each, or the one algorithm chosen with --algorithm.
//
// Exit codes: 0 success, 1 invalid input file, 2 invalid command line,
// 3 an algorithm could not be run on some set (the other sets are still run).

#include <exception>
#include <iostream>
#include <new>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

#include "algorithms/brute_force.hpp"
#include "algorithms/dynamic_programming.hpp"
#include "algorithms/karmarkar_karp.hpp"
#include "algorithms/meet_in_the_middle.hpp"
#include "cli.hpp"
#include "input.hpp"
#include "output.hpp"
#include "solve.hpp"

using output::paint;
using output::Style;

namespace {

// --no-color must hold also for the message about an invalid command line,
// which is printed before the options are known.
bool no_color_requested(int argc, char** argv) {
    for (int i = 1; i < argc; ++i)
        if (std::string_view(argv[i]) == "--no-color") return true;
    return false;
}

std::string set_name(std::size_t index, const Instance& instance) {
    return "set " + std::to_string(index) + " (line " + std::to_string(instance.line) + ")";
}

void print_instance(std::size_t index, const Instance& instance) {
    std::cout << paint("Set " + std::to_string(index), Style::Heading)
              << paint(" (line " + std::to_string(instance.line) + ")", Style::Dim) << ": n = "
              << instance.values.size() << ", S = " << to_string(instance.sum) << "\n"
              << "  " << output::format_values(instance.values) << "\n";
}

void print_part(std::string_view name, std::span<const Value> a,
                const std::vector<std::size_t>& indices) {
    std::vector<Value> values;
    values.reserve(indices.size());
    Value sum = 0;
    for (std::size_t i : indices) {
        values.push_back(a[i]);
        sum += a[i];  // at most S < 2^128
    }
    std::cout << "    " << name << " = " << output::format_values(values) << ", sum "
              << to_string(sum) << "\n";
}

void print_partition(std::span<const Value> a, const Partition& partition) {
    std::cout << "  Karmarkar-Karp: d = " << to_string(partition.d)
              << (partition.d == 0 ? " (balanced)" : "") << "\n";
    print_part("A", a, partition.A);
    print_part("B", a, partition.B);
}

// The balanced partition found by an exact algorithm.
void print_balanced(std::string_view by, std::span<const Value> a, const Partition& partition) {
    std::cout << "  Balanced partition (" << by << "):\n";
    print_part("A", a, partition.A);
    print_part("B", a, partition.B);
}

void print_answer(Verdict answer, std::string_view reason) {
    std::string word = "UNKNOWN";
    Style style = Style::Warning;
    if (answer == Verdict::Yes) word = "YES", style = Style::Yes;
    if (answer == Verdict::No) word = "NO", style = Style::No;
    std::cout << "  Answer: " << paint(word, style) << " " << paint(reason, Style::Dim) << "\n";
}

// The answer of one exact algorithm run alone, with its partition if YES.
void print_exact(std::string_view name, std::span<const Value> a, const ExactAnswer& answer,
                 std::string_view reason) {
    if (answer) print_balanced(name, a, *answer);
    print_answer(verdict_of(answer), reason);
}

std::string method_name(Method method) {
    switch (method) {
        case Method::KarmarkarKarp:      return "Karmarkar-Karp";
        case Method::OddSum:             return "odd sum";
        case Method::DynamicProgramming: return "dynamic programming";
        case Method::MeetInTheMiddle:    return "meet in the middle";
        case Method::None:               return "none";
    }
    return "";
}

Ranges ranges_of(const Options& options) {
    return Ranges{options.n_mitm, options.t_dp, options.w_dp};
}

// Why dynamic programming is out of range, for the messages.
std::string dp_out_of_range(std::size_t n, Value T, const Options& options) {
    if (dp_limit(n, T, ranges_of(options)) == DpLimit::Target)
        return "T = " + to_string(T) + " > T_DP = " + to_string(options.t_dp);
    return "n * T = " + std::to_string(n) + " * " + to_string(T) + " > W_DP = " +
           to_string(options.w_dp);
}

void run_solve(const Instance& instance, const Options& options) {
    const Solution solution = solve(instance.values, ranges_of(options));
    print_partition(instance.values, solution.partition);

    const std::string d = to_string(solution.partition.d);
    switch (solution.decided_by) {
        case Method::KarmarkarKarp:
            print_answer(solution.answer, "(the partition above is balanced)");
            return;
        case Method::OddSum:
            print_answer(solution.answer, "(the sum S is odd)");
            return;
        case Method::DynamicProgramming:
        case Method::MeetInTheMiddle: {
            const std::string by = "(" + method_name(solution.decided_by) + ", T = " +
                                   to_string(solution.T) + ")";
            if (solution.answer == Verdict::Yes) {
                print_balanced(method_name(solution.decided_by), instance.values, *solution.balanced);
                print_answer(solution.answer, by + "; Karmarkar-Karp missed it by d = " + d);
            } else {
                print_answer(solution.answer, by + "; no balanced partition exists");
            }
            return;
        }
        case Method::None: {
            const std::size_t n = instance.values.size();
            print_answer(solution.answer, "(n = " + std::to_string(n) + " > N_MITM = " +
                                              std::to_string(options.n_mitm) + " and " +
                                              dp_out_of_range(n, solution.T, options) +
                                              "; only the approximation, d = " + d + ")");
            return;
        }
    }
}

// One algorithm alone (--algorithm bf|mitm|dp|kk). The ranges still apply to
// meet in the middle and dynamic programming, so that a large set is skipped
// with a warning instead of exhausting the memory; raise them to go further.
void run_single(std::size_t index, const Instance& instance, const Options& options) {
    const std::span<const Value> a = instance.values;
    const std::size_t n = a.size();
    const Value T = instance.sum / 2;

    switch (options.algorithm) {
        case Algorithm::Solve:
            break;
        case Algorithm::KarmarkarKarp:
            print_partition(a, karmarkar_karp(a));
            return;
        case Algorithm::BruteForce:
            if (n > kMaxBruteForceElements) {
                throw std::invalid_argument("brute force needs at most " +
                                            std::to_string(kMaxBruteForceElements) +
                                            " numbers, the set has " + std::to_string(n));
            }
            print_exact("brute force", a, brute_force(a), "(brute force)");
            return;
        case Algorithm::MeetInTheMiddle:
            if (n > options.n_mitm) {
                output::warning(set_name(index, instance) + ": meet in the middle skipped, n = " +
                                std::to_string(n) + " > N_MITM = " + std::to_string(options.n_mitm));
                return;
            }
            print_exact("meet in the middle", a, meet_in_the_middle(a), "(meet in the middle)");
            return;
        case Algorithm::DynamicProgramming:
            if (dp_limit(n, T, ranges_of(options)) != DpLimit::InRange) {
                output::warning(set_name(index, instance) + ": dynamic programming skipped, " +
                                dp_out_of_range(n, T, options));
                return;
            }
            print_exact("dynamic programming", a, dp_partition(a),
                        "(dynamic programming, T = " + to_string(T) + ")");
            return;
    }
}

}  // namespace

int main(int argc, char** argv) {
    Options options;
    try {
        options = parse_options(argc, argv);
    } catch (const UsageError& error) {
        output::init(no_color_requested(argc, argv));
        output::error(error.what());
        std::cerr << "Try 'partition --help'.\n";
        return 2;
    }
    output::init(options.no_color);

    if (options.help) {
        std::cout << usage_text();
        return 0;
    }

    std::vector<Instance> instances;
    try {
        instances = read_instances_from_file(options.path);
    } catch (const InputError& error) {
        output::error(options.path + ": " + error.what());
        return 1;
    }

    int exit_code = 0;
    for (std::size_t i = 0; i < instances.size(); ++i) {
        if (i > 0) std::cout << "\n";
        print_instance(i + 1, instances[i]);
        try {
            if (options.algorithm == Algorithm::Solve) {
                run_solve(instances[i], options);
            } else {
                run_single(i + 1, instances[i], options);
            }
        } catch (const std::bad_alloc&) {
            output::error(set_name(i + 1, instances[i]) + ": not enough memory");
            exit_code = 3;
        } catch (const std::exception& error) {
            output::error(set_name(i + 1, instances[i]) + ": " + error.what());
            exit_code = 3;
        }
    }
    return exit_code;
}

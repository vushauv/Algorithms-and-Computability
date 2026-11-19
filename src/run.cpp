#include "run.hpp"

#include <iostream>
#include <stdexcept>
#include <string_view>

#include "algorithms/brute_force.hpp"
#include "algorithms/dynamic_programming.hpp"
#include "algorithms/karmarkar_karp.hpp"
#include "algorithms/meet_in_the_middle.hpp"
#include "output.hpp"
#include "report.hpp"
#include "solve.hpp"

namespace run {

std::string set_name(std::size_t index, const Instance& instance) {
    return "set " + std::to_string(index) + " (line " + std::to_string(instance.line) + ")";
}

std::optional<Partition> run_set(std::size_t index, const Instance& instance,
                                 const Options& options) {
    const std::span<const Value> a = instance.values;
    const std::size_t n = a.size();

    switch (options.algorithm) {
        case Algorithm::Solve: {
            const Solution solution = solve(a, options.ranges());
            report::print_solution(instance, options.ranges(), solution);
            return solution.balanced ? *solution.balanced : solution.partition;
        }
        case Algorithm::KarmarkarKarp: {
            Partition partition = karmarkar_karp(a);
            report::print_partition(a, partition);
            return partition;
        }
        case Algorithm::BruteForce: {
            if (n > kMaxBruteForceElements) {
                throw std::invalid_argument("brute force needs at most " +
                                            std::to_string(kMaxBruteForceElements) +
                                            " numbers, the set has " + std::to_string(n));
            }
            ExactAnswer answer = brute_force(a);
            report::print_exact("brute force", a, answer, "(brute force)");
            return answer;
        }
        case Algorithm::MeetInTheMiddle: {
            if (n > options.n_mitm) {
                output::warning(set_name(index, instance) + ": meet in the middle skipped, n = " +
                                std::to_string(n) + " > N_MITM = " + std::to_string(options.n_mitm));
                return std::nullopt;
            }
            ExactAnswer answer = meet_in_the_middle(a);
            report::print_exact("meet in the middle", a, answer, "(meet in the middle)");
            return answer;
        }
        case Algorithm::DynamicProgramming: {
            const Value T = instance.sum / 2;
            if (dp_limit(n, T, options.ranges()) != DpLimit::InRange) {
                output::warning(set_name(index, instance) + ": dynamic programming skipped, " +
                                report::dp_out_of_range(n, T, options.ranges()));
                return std::nullopt;
            }
            ExactAnswer answer = dp_partition(a);
            report::print_exact("dynamic programming", a, answer,
                                "(dynamic programming, T = " + to_string(T) + ")");
            return answer;
        }
    }
    return std::nullopt;
}

Partition fallback_partition(std::span<const Value> a) {
    Partition partition = karmarkar_karp(a);
    std::cout << "  " << output::paint("For the results file:", output::Style::Dim) << "\n";
    report::print_partition(a, partition);
    return partition;
}

}  // namespace run

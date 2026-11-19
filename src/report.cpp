#include "report.hpp"

#include <iostream>
#include <vector>

#include "output.hpp"

namespace report {

using output::paint;
using output::Style;

namespace {

void print_part(std::string_view name, std::span<const Value> a,
                const std::vector<std::size_t>& indices) {
    const std::vector<Value> values = values_at(a, indices);
    const Value sum = total_sum(values);
    std::cout << "    " << name << " = " << output::format_values(values) << ", sum "
              << to_string(sum) << "\n";
}

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

}  // namespace

void print_instance(std::size_t index, const Instance& instance) {
    std::cout << paint("Set " + std::to_string(index), Style::Heading)
              << paint(" (line " + std::to_string(instance.line) + ")", Style::Dim) << ": n = "
              << instance.values.size() << ", S = " << to_string(instance.sum) << "\n"
              << "  " << output::format_values(instance.values) << "\n";
}

void print_partition(std::span<const Value> a, const Partition& partition) {
    std::cout << "  Karmarkar-Karp: d = " << to_string(partition.d)
              << (partition.d == 0 ? " (balanced)" : "") << "\n";
    print_part("A", a, partition.A);
    print_part("B", a, partition.B);
}

void print_exact(std::string_view name, std::span<const Value> a, const ExactAnswer& answer,
                 std::string_view reason) {
    if (answer) print_balanced(name, a, *answer);
    print_answer(verdict_of(answer), reason);
}

std::string dp_out_of_range(std::size_t n, Value T, const Ranges& ranges) {
    if (dp_limit(n, T, ranges) == DpLimit::Target)
        return "T = " + to_string(T) + " > T_DP = " + to_string(ranges.t_dp);
    return "n * T = " + std::to_string(n) + " * " + to_string(T) + " > W_DP = " +
           to_string(ranges.w_dp);
}

void print_solution(const Instance& instance, const Ranges& ranges, const Solution& solution) {
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
                                              std::to_string(ranges.n_mitm) + " and " +
                                              dp_out_of_range(n, solution.T, ranges) +
                                              "; only the approximation, d = " + d + ")");
            return;
        }
    }
}

}  // namespace report

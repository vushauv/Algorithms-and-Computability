#include "report.hpp"

#include <iostream>
#include <vector>

#include "output.hpp"

namespace report {

using output::paint;
using output::Style;

namespace {

template <class V>
void print_part(std::string_view name, std::span<const V> a,
                const std::vector<std::size_t>& indices) {
    const std::vector<V> values = values_at(a, indices);
    const V sum = total_sum<V>(values);
    std::cout << "    " << name << " = " << output::format_values<V>(values) << ", sum "
              << to_string(sum) << "\n";
}

template <class V>
void print_balanced(std::string_view by, std::span<const V> a, const Partition<V>& partition) {
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

template <class V>
void print_instance(std::size_t index, const Instance<V>& instance) {
    std::cout << paint("Set " + std::to_string(index), Style::Heading)
              << paint(" (line " + std::to_string(instance.line) + ")", Style::Dim) << ": n = "
              << instance.values.size() << ", S = " << to_string(instance.sum) << "\n"
              << "  " << output::format_values<V>(instance.values) << "\n";
}

template <class V>
void print_partition(std::span<const V> a, const Partition<V>& partition) {
    std::cout << "  Karmarkar-Karp: d = " << to_string(partition.d)
              << (partition.d == 0 ? " (balanced)" : "") << "\n";
    print_part("A", a, partition.A);
    print_part("B", a, partition.B);
}

template <class V>
void print_exact(std::string_view name, std::span<const V> a, const ExactAnswer<V>& answer,
                 std::string_view reason) {
    if (answer) print_balanced(name, a, *answer);
    print_answer(verdict_of(answer), reason);
}

template <class V>
std::string dp_out_of_range(std::size_t n, const V& T, const Ranges& ranges) {
    if (dp_limit(n, T, ranges) == DpLimit::Target)
        return "T = " + to_string(T) + " > T_DP = " + std::to_string(ranges.t_dp);
    return "n * T = " + std::to_string(n) + " * " + to_string(T) + " > W_DP = " +
           std::to_string(ranges.w_dp);
}

template <class V>
void print_solution(const Instance<V>& instance, const Ranges& ranges, const Solution<V>& solution) {
    const std::span<const V> a = instance.values;
    print_partition(a, solution.partition);

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
                print_balanced(method_name(solution.decided_by), a, *solution.balanced);
                print_answer(solution.answer, by + "; Karmarkar-Karp missed it by d = " + d);
            } else {
                print_answer(solution.answer, by + "; no balanced partition exists");
            }
            return;
        }
        case Method::None: {
            const std::size_t n = a.size();
            print_answer(solution.answer, "(n = " + std::to_string(n) + " > N_MITM = " +
                                              std::to_string(ranges.n_mitm) + " and " +
                                              dp_out_of_range(n, solution.T, ranges) +
                                              "; only the approximation, d = " + d + ")");
            return;
        }
    }
}

#define INSTANTIATE(V)                                                                \
    template void print_instance(std::size_t, const Instance<V>&);                    \
    template void print_partition(std::span<const V>, const Partition<V>&);          \
    template void print_exact(std::string_view, std::span<const V>, const ExactAnswer<V>&, \
                              std::string_view);                                      \
    template void print_solution(const Instance<V>&, const Ranges&, const Solution<V>&); \
    template std::string dp_out_of_range(std::size_t, const V&, const Ranges&);
FOR_EACH_NUMBER_TYPE(INSTANTIATE)
#undef INSTANTIATE

}  // namespace report

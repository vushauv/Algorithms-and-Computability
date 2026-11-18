#include "check.hpp"
#include "result_file.hpp"
#include "tests.hpp"

#include <algorithm>
#include <functional>
#include <ranges>
#include <span>
#include <string>
#include <vector>

#include "algorithms/karmarkar_karp.hpp"
#include "random_sets.hpp"
#include "solve.hpp"

namespace {

constexpr Value kAnyWork = ~Value{0};

Value total(std::span<const Value> values) {
    Value sum = 0;
    for (Value v : values) sum += v;
    return sum;
}

// raw_regulations/AaClab_output.txt, Homenda's example with K = 5. Its line "61 7 8 9 10 11 2 3 4 6 1 " ends with a space, which no
// other line has; it is left out here.
const std::string kHomendaExample =
    "2 5\n"
    "\n"
    "66 2 2 1 7 8 1 3 1 4 4 4 1 5 5 5 1 6 6\n"
    "17 8 7 2\n"
    "16 6 6 3 1\n"
    "12 5 4 2 1\n"
    "11 5 4 1 1\n"
    "10 5 4 1\n"
    "7\n"
    "\n"
    "61 7 8 9 10 11 2 3 4 6 1\n"
    "11 11\n"
    "11 10 1\n"
    "11 9 2\n"
    "11 8 3\n"
    "17 7 6 4\n"
    "6";

// The lines of a text.
std::vector<std::string> lines_of(const std::string& text) {
    std::vector<std::string> lines(1);
    for (char c : text) {
        if (c == '\n') {
            lines.emplace_back();
        } else {
            lines.back() += c;
        }
    }
    return lines;
}

// The numbers of a line.
std::vector<Value> numbers_of(const std::string& line) {
    std::vector<Value> numbers;
    std::size_t start = 0;
    while (start <= line.size()) {
        std::size_t end = line.find(' ', start);
        if (end == std::string::npos) end = line.size();
        numbers.push_back(parse_value(line.substr(start, end - start)).value);
        start = end + 1;
    }
    return numbers;
}

// The block of format_set follows the rules of the format for the set a and
// the partition p: the set in input order, each subset with its sum and its
// numbers non-increasing, the subsets lexicographically non-increasing,
// together the multiset a, and the difference of the two sums d.
bool follows_format(std::span<const Value> a, const Partition& p, const std::string& block) {
    const std::vector<std::string> lines = lines_of(block);
    if (lines.size() != 4) return false;

    std::vector<Value> first = numbers_of(lines[0]);
    if (first.front() != total(a) || !std::ranges::equal(first | std::views::drop(1), a))
        return false;

    std::vector<Value> x = numbers_of(lines[1]);
    std::vector<Value> y = numbers_of(lines[2]);
    std::vector<Value> X(x.begin() + 1, x.end());
    std::vector<Value> Y(y.begin() + 1, y.end());
    if (x.front() != total(X) || y.front() != total(Y)) return false;
    if (!std::ranges::is_sorted(X, std::greater<>()) || !std::ranges::is_sorted(Y, std::greater<>()))
        return false;
    if (std::ranges::lexicographical_compare(X, Y)) return false;

    std::vector<Value> together = X;
    together.insert(together.end(), Y.begin(), Y.end());
    std::vector<Value> all(a.begin(), a.end());
    std::ranges::sort(together);
    std::ranges::sort(all);
    if (together != all) return false;

    const Value d = x.front() > y.front() ? x.front() - y.front() : y.front() - x.front();
    return lines[3] == to_string(d) && d == p.d;
}

}  // namespace

void test_result_file() {
    // The name: _output.txt in place of .txt.
    CHECK(result_file::path_for("AaC_example_1.txt") == "AaC_example_1_output.txt");
    CHECK(result_file::path_for("data/in.txt") == "data/in_output.txt");
    CHECK(result_file::path_for("C:\\lab\\IN.TXT") == "C:\\lab\\IN_output.txt");
    CHECK(result_file::path_for("in") == "in_output.txt");
    CHECK(result_file::path_for("in.txt.bak") == "in.txt.bak_output.txt");
    CHECK(result_file::path_for(".txt") == "_output.txt");

    // Homenda's example, from its subsets given in any order.
    std::vector<Value> first{2, 2, 1, 7, 8, 1, 3, 1, 4, 4, 4, 1, 5, 5, 5, 1, 6, 6};
    std::vector<Value> second{7, 8, 9, 10, 11, 2, 3, 4, 6, 1};
    const std::string text =
        result_file::header(2, 5) + "\n\n" +
        result_file::format_set(first, {{1, 4, 5}, {2, 1, 4, 5}, {1, 4, 1, 5}, {2, 7, 8}, {6, 1, 6, 3}}) +
        "\n\n" + result_file::format_set(second, {{4, 6, 7}, {3, 8}, {11}, {1, 10}, {2, 9}});
    CHECK(text == kHomendaExample);

    // Two subsets: the one with the larger first number comes first, not the
    // one with the larger sum.
    std::vector<Value> a{3, 1, 4, 2};
    CHECK(result_file::format_set(a, Partition{{1, 2}, {0, 3}, 0}) == "10 3 1 4 2\n5 4 1\n5 3 2\n0");
    std::vector<Value> b{2, 3, 7};
    CHECK(result_file::format_set(b, Partition{{0, 1}, {2}, 2}) == "12 2 3 7\n7 7\n5 3 2\n2");
    // Equal subsets, zeros, and a value of 38 digits.
    std::vector<Value> ones{1, 1, 1, 1};
    CHECK(result_file::format_set(ones, Partition{{0, 2}, {1, 3}, 0}) == "4 1 1 1 1\n2 1 1\n2 1 1\n0");
    std::vector<Value> zeros{0, 0};
    CHECK(result_file::format_set(zeros, Partition{{0}, {1}, 0}) == "0 0 0\n0 0\n0 0\n0");
    std::vector<Value> large{kValueLimit - 1, 5};
    const std::string big = to_string(kValueLimit - 1);  // 38 nines
    CHECK(result_file::format_set(large, Partition{{1}, {0}, kValueLimit - 6}) ==
          "1" + std::string(37, '0') + "4 " + big + " 5\n" + big + " " + big + "\n5 5\n" +
              to_string(kValueLimit - 6));
    CHECK(result_file::header(3, 2) == "3 2");

    // The partitions of SOLVE and of Karmarkar-Karp on random sets, with
    // small values (duplicates, zeros) and with values of 25 digits.
    for (int round = 0; round < 400; ++round) {
        const std::size_t n = random_sets::size_between(2, 16);
        const std::vector<Value> set =
            round % 2 == 0 ? random_sets::small_values(n, 20) : random_sets::large_values(n);
        const Solution solution = solve(set, Ranges{44, 1'000'000, kAnyWork});
        const Partition& chosen = solution.balanced ? *solution.balanced : solution.partition;
        CHECK(follows_format(set, chosen, result_file::format_set(set, chosen)));
        const Partition kk = karmarkar_karp(set);
        CHECK(follows_format(set, kk, result_file::format_set(set, kk)));
    }
}

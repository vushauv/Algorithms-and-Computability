// Size: numbers of thousands of digits, sets of 10^5 numbers, files of
// thousands of sets, in every arithmetic. The answers are known by
// construction (a set repeated twice is YES), so no slow reference is needed.
#include "algorithms/karmarkar_karp.hpp"
#include "algorithms/meet_in_the_middle.hpp"
#include "check.hpp"
#include "input.hpp"
#include "random_sets.hpp"
#include "result_file.hpp"
#include "solve.hpp"
#include "tests.hpp"

#include <algorithm>
#include <cstdint>
#include <sstream>
#include <string>
#include <vector>

namespace {

// A decimal number of `digits` digits without leading zero.
std::string random_digits(std::size_t digits) {
    std::string text(digits, '0');
    for (char& c : text) c = static_cast<char>('0' + random_sets::size_between(0, 9));
    text[0] = static_cast<char>('1' + random_sets::size_between(0, 8));
    return text;
}

template <class V>
V zero() {
    return parse_value<V>("0").value;
}

template <class V>
std::vector<V> numbers(std::size_t n, std::size_t digits) {
    std::vector<V> a;
    for (std::size_t i = 0; i < n; ++i) a.push_back(parse_value<V>(random_digits(digits)).value);
    return a;
}

template <class V>
V sum_at(const std::vector<V>& a, const std::vector<std::size_t>& indices) {
    V sum = zero<V>();
    for (std::size_t i : indices) sum = sum + a[i];
    return sum;
}

// A and B are non-empty, disjoint, cover 0..n-1, and d is the difference of the sums.
template <class V>
bool is_partition(const std::vector<V>& a, const Partition<V>& p) {
    if (p.A.empty() || p.B.empty()) return false;
    std::vector<std::size_t> all = p.A;
    all.insert(all.end(), p.B.begin(), p.B.end());
    std::sort(all.begin(), all.end());
    if (all.size() != a.size()) return false;
    for (std::size_t i = 0; i < all.size(); ++i) {
        if (all[i] != i) return false;
    }
    const V sa = sum_at(a, p.A);
    const V sb = sum_at(a, p.B);
    return to_string(p.d) == to_string(sa >= sb ? sa - sb : sb - sa);
}

// The numbers twice: YES by construction.
template <class V>
std::vector<V> twice(std::vector<V> a) {
    const std::vector<V> copy = a;
    a.insert(a.end(), copy.begin(), copy.end());
    std::shuffle(a.begin(), a.end(), random_sets::engine());
    return a;
}

constexpr std::uint64_t kAnyWork = ~std::uint64_t{0};

// Numbers of `digits` digits: reading, the exact answer and Karmarkar-Karp.
template <class V>
void check_huge_numbers(std::size_t digits) {
    const std::string text = random_digits(digits);
    CHECK(to_string(parse_value<V>(text).value) == text);

    const std::vector<V> yes = twice(numbers<V>(10, digits));
    const ExactAnswer<V> found = meet_in_the_middle<V>(yes);
    CHECK(found.has_value() && is_partition(yes, *found) && to_string(found->d) == "0");
    CHECK(is_partition(yes, karmarkar_karp<V>(yes)));

    // One more number: the sum is no longer twice a sum of the others.
    std::vector<V> other = yes;
    other.push_back(parse_value<V>(random_digits(digits)).value);
    const Solution<V> s = solve<V>(other, Ranges{44, 0, kAnyWork});
    CHECK(is_partition(other, s.partition));
    CHECK((s.answer == Verdict::Yes) == s.balanced.has_value());
    if (s.balanced) CHECK(is_partition(other, *s.balanced));
}

// n numbers of `digits` digits: Karmarkar-Karp, and SOLVE on a YES set.
template <class V>
void check_many_numbers(std::size_t n, std::size_t digits) {
    const std::vector<V> a = numbers<V>(n, digits);
    CHECK(is_partition(a, karmarkar_karp<V>(a)));
    const std::vector<V> yes = twice(numbers<V>(n / 2, digits));
    const Solution<V> s = solve<V>(yes, Ranges{44, 300'000'000, 1'000'000'000'000});
    CHECK(is_partition(yes, s.partition));
    CHECK((s.answer == Verdict::Yes) == s.balanced.has_value());
    if (s.balanced) CHECK(is_partition(yes, *s.balanced));
}

// A file of `sets` sets: all of them read, in order, and written to the results file.
template <class V>
void check_many_sets(std::size_t sets) {
    std::ostringstream file;
    file << sets << "\n";
    for (std::size_t k = 0; k < sets; ++k) {
        const std::size_t n = 2 + k % 5;
        for (std::size_t i = 0; i < n; ++i) file << (i ? " " : "") << (k * 7 + i * 13) % 1000;
        file << "\n";
    }
    std::istringstream in(file.str());
    const std::vector<Instance<V>> instances = read_instances<V>(in);
    CHECK(instances.size() == sets);
    std::size_t lines = 0;  // a set: its sum and numbers, two subsets, the difference
    for (const Instance<V>& instance : instances) {
        const std::string block =
            result_file::format_set<V>(instance.values, karmarkar_karp<V>(instance.values));
        lines += 1 + static_cast<std::size_t>(std::count(block.begin(), block.end(), '\n'));
    }
    CHECK(lines == 4 * sets);
}

}  // namespace

void test_scale() {
    for (std::size_t d : {1, 38, 39, 40, 300}) check_huge_numbers<BoostInt>(d);
    check_huge_numbers<BoostInt>(3000);
    check_many_numbers<BoostInt>(100'000, 3);
    check_many_numbers<BoostInt>(2'000, 100);
    check_many_sets<BoostInt>(20'000);
    check_many_sets<Native>(20'000);

    for (std::uint64_t base : {BigUint::kMaxBase, std::uint64_t{10}, std::uint64_t{2}}) {
        BigUint::set_base(base);
        check_huge_numbers<BigUint>(base == 2 ? 600 : 1500);
        check_many_numbers<BigUint>(base == 2 ? 2'000 : 20'000, 25);
        check_many_sets<BigUint>(5'000);
    }
    BigUint::set_base(BigUint::kMaxBase);
}

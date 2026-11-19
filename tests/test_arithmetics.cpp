// The long arithmetics (Boost, and our own in several bases) against the
// native one: the algorithms compare and add the same numbers in the same
// order, so they must return the very same partitions.
#include "algorithms/brute_force.hpp"
#include "algorithms/dynamic_programming.hpp"
#include "algorithms/karmarkar_karp.hpp"
#include "algorithms/meet_in_the_middle.hpp"
#include "check.hpp"
#include "input.hpp"
#include "random_sets.hpp"
#include "result_file.hpp"
#include "solve.hpp"
#include "tests.hpp"

#include <cstdint>
#include <sstream>
#include <string>
#include <vector>

namespace {

constexpr std::uint64_t kAnyWork = ~std::uint64_t{0};

// A factor that takes every non-zero number past 128 bits: 10^40.
const std::string kZeros(40, '0');

// The same numbers in the arithmetic V, each multiplied by 10^(zeros.size()).
template <class V>
std::vector<V> convert(const std::vector<Native>& a, const std::string& zeros = "") {
    std::vector<V> result;
    for (Native x : a) result.push_back(parse_value<V>(to_string(x) + zeros).value);
    return result;
}

// The text of d · 10^(zeros.size()).
std::string scaled(Native d, const std::string& zeros) {
    return d == 0 ? "0" : to_string(d) + zeros;
}

template <class V>
bool same(const Partition<V>& p, const Partition<Native>& q, const std::string& zeros = "") {
    return p.A == q.A && p.B == q.B && to_string(p.d) == scaled(q.d, zeros);
}

template <class V>
bool same(const ExactAnswer<V>& p, const ExactAnswer<Native>& q, const std::string& zeros = "") {
    return p.has_value() == q.has_value() && (!p || same(*p, *q, zeros));
}

template <class V>
bool same(const Solution<V>& s, const Solution<Native>& t) {
    return s.answer == t.answer && s.decided_by == t.decided_by && same(s.partition, t.partition) &&
           same(s.balanced, t.balanced) && to_string(s.S) == to_string(t.S) &&
           to_string(s.T) == to_string(t.T);
}

// Every algorithm on the set a, in V and in Native.
template <class V>
void check_set(const std::vector<Native>& a, bool with_dp) {
    const std::vector<V> v = convert<V>(a);
    CHECK(to_string(total_sum<V>(v)) == to_string(total_sum<Native>(a)));
    CHECK(same(brute_force<V>(v), brute_force<Native>(a)));
    CHECK(same(meet_in_the_middle<V>(v), meet_in_the_middle<Native>(a)));
    CHECK(same(karmarkar_karp<V>(v), karmarkar_karp<Native>(a)));
    if (with_dp) CHECK(same(dp_partition<V>(v), dp_partition<Native>(a)));
    for (const Ranges& ranges : {Ranges{44, 300'000'000, kAnyWork}, Ranges{44, 0, kAnyWork},
                                 Ranges{0, 300'000'000, kAnyWork}, Ranges{0, 0, kAnyWork}}) {
        CHECK(same(solve<V>(v, ranges), solve<Native>(a, ranges)));
    }
    const Partition<V> p = karmarkar_karp<V>(v);
    CHECK(result_file::format_set<V>(v, p) ==
          result_file::format_set<Native>(a, karmarkar_karp<Native>(a)));

    // Past 128 bits: all the numbers times 10^40 compare in the same way, so
    // the partitions are those of the small numbers, and d is scaled too.
    const std::vector<V> big = convert<V>(a, kZeros);
    CHECK(same(brute_force<V>(big), brute_force<Native>(a), kZeros));
    CHECK(same(meet_in_the_middle<V>(big), meet_in_the_middle<Native>(a), kZeros));
    CHECK(same(karmarkar_karp<V>(big), karmarkar_karp<Native>(a), kZeros));
}

template <class V>
void check_arithmetic(int rounds) {
    for (int round = 0; round < rounds; ++round) {
        const std::size_t n = random_sets::size_between(2, 12);
        check_set<V>(random_sets::small_values(n, round % 3 == 0 ? 4 : 500), true);
        check_set<V>(random_sets::large_values(n), false);  // 25 digits, T is not a table size
        check_set<V>(random_sets::large_yes_instance(n), false);
    }
    check_set<V>({0, 0}, true);
    check_set<V>({0, 0, 0, 5, 5}, true);
    check_set<V>({5, 6, 7, 8, 10}, true);  // YES that Karmarkar-Karp misses

    // Reading: no limit on a number or on a sum, the same errors otherwise.
    CHECK(parse_value<V>("").status == ParseStatus::Empty);
    CHECK(parse_value<V>("12a").status == ParseStatus::NotDigit);
    CHECK(parse_value<V>("-1").status == ParseStatus::NotDigit);
    CHECK(parse_value<V>("0017").value == 17);  // not read as octal
    CHECK(parse_value<V>("0").value == 0);
    const std::string huge(200, '9');
    CHECK(parse_value<V>(huge).status == ParseStatus::Ok);
    CHECK(to_string(parse_value<V>(huge).value) == huge);

    std::istringstream file("2\n1" + std::string(38, '0') + " 1" + std::string(38, '0') + "\n" +
                            huge + " " + huge + " " + huge + " 1\n");
    const std::vector<Instance<V>> instances = read_instances<V>(file);
    CHECK(instances.size() == 2);
    CHECK(to_string(instances[0].sum) == "2" + std::string(38, '0'));
    CHECK(instances[1].values.size() == 4);
    CHECK(to_string(instances[1].sum) == "2" + std::string(199, '9') + "8");

    // SOLVE on them: two equal numbers, and an even sum whose largest number
    // is below T, where only meet in the middle can answer NO.
    const Ranges ranges{44, 300'000'000, 1'000'000'000'000};
    const std::span<const V> equal = instances[0].values;
    CHECK(solve(equal, ranges).answer == Verdict::Yes);
    const Solution<V> no = solve(std::span<const V>(instances[1].values), ranges);
    CHECK(no.answer == Verdict::No && no.decided_by == Method::MeetInTheMiddle);
    CHECK(dp_limit(4, no.T, ranges) == DpLimit::Target);
    CHECK_THROWS(dp_partition(equal), std::invalid_argument);  // T = 10^38 is no table size
}

}  // namespace

void test_arithmetics() {
    check_arithmetic<BoostInt>(40);
    for (std::uint64_t base : {BigUint::kMaxBase, std::uint64_t{1'000'000'000}, std::uint64_t{10},
                               std::uint64_t{3}, std::uint64_t{2}}) {
        BigUint::set_base(base);
        check_arithmetic<BigUint>(15);
    }
    BigUint::set_base(BigUint::kMaxBase);
}

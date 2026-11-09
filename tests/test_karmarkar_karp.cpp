#include "algorithms/brute_force.hpp"
#include "algorithms/karmarkar_karp.hpp"
#include "check.hpp"
#include "random_sets.hpp"
#include "tests.hpp"

#include <algorithm>
#include <vector>

namespace {

Value sum_of(const std::vector<Value>& a, const std::vector<std::size_t>& indices) {
    Value sum = 0;
    for (std::size_t i : indices) sum += a[i];
    return sum;
}

// Theorem 5.2: A and B are non-empty, disjoint and cover 0..n−1, and d is the
// difference of their sums. Also d ≤ max a[i] (Proposition 5.3) and d has the
// parity of S.
bool is_valid_result(const std::vector<Value>& a, const Partition& p) {
    if (p.A.empty() || p.B.empty()) return false;
    std::vector<std::size_t> all = p.A;
    all.insert(all.end(), p.B.begin(), p.B.end());
    std::sort(all.begin(), all.end());
    if (all.size() != a.size()) return false;
    for (std::size_t i = 0; i < a.size(); ++i) {
        if (all[i] != i) return false;
    }
    const Value sumA = sum_of(a, p.A);
    const Value sumB = sum_of(a, p.B);
    const Value difference = sumA >= sumB ? sumA - sumB : sumB - sumA;
    const Value S = sumA + sumB;
    return difference == p.d && p.d <= *std::max_element(a.begin(), a.end()) &&
           p.d % 2 == S % 2;
}

}  // namespace

void test_karmarkar_karp() {
    using V = std::vector<Value>;

    // The examples of Section 5.3.
    const V optimum{3, 1, 4, 2};
    const Partition p1 = karmarkar_karp(optimum);
    CHECK(p1.d == 0);
    CHECK(is_valid_result(optimum, p1));

    const V missed{5, 6, 7, 8, 10};
    const Partition p2 = karmarkar_karp(missed);
    CHECK(p2.d == 2);
    CHECK(is_valid_result(missed, p2));
    CHECK(brute_force(missed) == Verdict::Yes);  // d > 0 does not mean NO

    // Hand-made cases: two elements, zeros, duplicates.
    CHECK(karmarkar_karp(V{7, 3}).d == 4);
    CHECK(karmarkar_karp(V{0, 0}).d == 0);
    CHECK(karmarkar_karp(V{0, 0, 0}).d == 0);
    CHECK(is_valid_result(V{0, 0, 0}, karmarkar_karp(V{0, 0, 0})));
    CHECK(karmarkar_karp(V{4, 4, 4, 4}).d == 0);
    CHECK(karmarkar_karp(V{2, 3, 4}).d == 1);   // Section 6.3
    const Value all_ones = ~Value{0};
    CHECK(karmarkar_karp(V{all_ones / 2, all_ones / 2}).d == 0);

    // Random inputs: a valid partition, and d = 0 implies YES.
    for (int round = 0; round < 300; ++round) {
        const std::size_t n = random_sets::size_between(2, 16);
        const V a = random_sets::small_values(n, round % 2 == 0 ? 9 : 1000);
        const Partition p = karmarkar_karp(a);
        CHECK(is_valid_result(a, p));
        if (p.d == 0) CHECK(brute_force(a) == Verdict::Yes);
    }
    for (int round = 0; round < 50; ++round) {
        const V a = random_sets::large_values(random_sets::size_between(2, 200));
        CHECK(is_valid_result(a, karmarkar_karp(a)));
    }
    const V big = random_sets::small_values(100'000, 1'000'000);
    CHECK(is_valid_result(big, karmarkar_karp(big)));
}

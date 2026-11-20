#include "algorithms/brute_force.hpp"
#include "algorithms/dynamic_programming.hpp"
#include "algorithms/meet_in_the_middle.hpp"
#include "check.hpp"
#include "memory.hpp"
#include "partition_check.hpp"
#include "random_sets.hpp"
#include "tests.hpp"
#include "value.hpp"

#include <cstdint>
#include <stdexcept>
#include <vector>

using partition_check::answers;

void test_meet_in_the_middle() {
    using V = std::vector<Value>;

    // SUBSET-SUMS on a small example.
    CHECK(subset_sums<Value>(V{3, 1}) == (V{0, 3, 1, 4}));
    CHECK(subset_sums<Value>(V{4, 2}) == (V{0, 4, 2, 6}));
    CHECK(subset_sums<Value>(V{}) == (V{0}));

    // Small worked examples.
    CHECK(answers(meet_in_the_middle<Value>, V{3, 1, 4, 2}, Verdict::Yes));
    CHECK(answers(meet_in_the_middle<Value>, V{2, 3, 7}, Verdict::No));
    CHECK(answers(meet_in_the_middle<Value>, V{2, 3, 4}, Verdict::No));  // odd sum

    // Hand-made cases: two elements, zeros, the solution inside one half.
    CHECK(answers(meet_in_the_middle<Value>, V{1, 1}, Verdict::Yes));
    CHECK(answers(meet_in_the_middle<Value>, V{1, 3}, Verdict::No));
    CHECK(answers(meet_in_the_middle<Value>, V{0, 0, 0}, Verdict::Yes));
    CHECK(answers(meet_in_the_middle<Value>, V{0, 4, 0, 4}, Verdict::Yes));
    CHECK(answers(meet_in_the_middle<Value>, V{5, 6, 7, 8, 10}, Verdict::Yes));
    CHECK(answers(meet_in_the_middle<Value>, V{3, 3, 1, 1, 1, 1}, Verdict::Yes));  // {3, 3} is the left half
    CHECK(answers(meet_in_the_middle<Value>, V{7, 1, 1, 5}, Verdict::Yes));  // first hit: left sum 0
    CHECK(answers(meet_in_the_middle<Value>, V{10, 1, 1}, Verdict::No));

    // Equal to brute force on random sets with n <= 20, duplicates and zeros.
    for (int round = 0; round < 400; ++round) {
        const std::size_t n = random_sets::size_between(2, 20);
        const std::uint64_t max_value = round % 3 == 0 ? 3 : (round % 3 == 1 ? 50 : 100000);
        const V a = random_sets::small_values(n, max_value);
        CHECK(answers(meet_in_the_middle<Value>, a, verdict_of(brute_force<Value>(a))));
    }
    // 25-digit numbers: random sets (almost always NO) and YES by construction.
    for (int round = 0; round < 50; ++round) {
        const std::size_t n = random_sets::size_between(2, 18);
        const V no = random_sets::large_values(n);
        CHECK(answers(meet_in_the_middle<Value>, no, verdict_of(brute_force<Value>(no))));
        const V yes = random_sets::large_yes_instance(n);
        CHECK(answers(brute_force<Value>, yes, Verdict::Yes));
        CHECK(answers(meet_in_the_middle<Value>, yes, Verdict::Yes));
    }
    // Beyond the reach of brute force in the tests: n = 40.
    CHECK(answers(meet_in_the_middle<Value>, random_sets::large_yes_instance(40), Verdict::Yes));

    // Recovery where x or y is 0 at larger n: all the non-zero values in one
    // half (the other half is zeros), then random sparse sets with zeros and
    // duplicates, checked against dynamic programming (cheap for small values).
    for (std::size_t n : {30, 33, 40}) {
        V right_only(n, 0);
        right_only[n - 1] = 6;
        right_only[n - 2] = 2;
        right_only[n - 3] = 4;  // {2, 4} against {6}
        CHECK(answers(meet_in_the_middle<Value>, right_only, Verdict::Yes));
        V left_only(right_only.rbegin(), right_only.rend());
        CHECK(answers(meet_in_the_middle<Value>, left_only, Verdict::Yes));
        V no(n, 0);
        no[n - 1] = 2;
        no[0] = 4;
        no[1] = 4;  // S = 10, T = 5 is not reachable
        CHECK(answers(meet_in_the_middle<Value>, no, Verdict::No));
    }
    for (int round = 0; round < 40; ++round) {
        const std::size_t n = random_sets::size_between(30, 40);
        V a(n, 0);
        const std::size_t nonzero = random_sets::size_between(2, 6);
        for (std::size_t i = 0; i < nonzero; ++i) a[random_sets::size_between(0, n - 1)] = random_sets::size_between(1, 9);
        CHECK(answers(meet_in_the_middle<Value>, a, verdict_of(dp_partition<Value>(a))));
        V d = random_sets::small_values(n, 2);  // many zeros and duplicates
        CHECK(answers(meet_in_the_middle<Value>, d, verdict_of(dp_partition<Value>(d))));
    }

    CHECK_THROWS(meet_in_the_middle<Value>(V(125, 1)), std::invalid_argument);

    // Where the physical memory is known (Linux), a table larger than it is
    // refused before it is allocated.
    const std::uint64_t physical = physical_memory_bytes();
    if (physical > 0) {
        CHECK_THROWS(require_memory("test", Value{physical} + 1), std::runtime_error);
        require_memory("test", physical);
        CHECK_THROWS(meet_in_the_middle<Value>(V(124, 1)), std::runtime_error);  // 2 * 2^62 sums
        CHECK_THROWS(dp_partition<Value>(V{kMaxDpTarget, kMaxDpTarget}), std::runtime_error);
    }
}

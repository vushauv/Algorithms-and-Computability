#include "algorithms/brute_force.hpp"
#include "algorithms/dynamic_programming.hpp"
#include "algorithms/meet_in_the_middle.hpp"
#include "check.hpp"
#include "memory.hpp"
#include "partition_check.hpp"
#include "random_sets.hpp"
#include "tests.hpp"

#include <cstdint>
#include <stdexcept>
#include <vector>

using partition_check::answers;

void test_meet_in_the_middle() {
    using V = std::vector<Value>;

    // SUBSET-SUMS, the table of Section 3.3.
    CHECK(subset_sums(V{3, 1}) == (V{0, 3, 1, 4}));
    CHECK(subset_sums(V{4, 2}) == (V{0, 4, 2, 6}));
    CHECK(subset_sums(V{}) == (V{0}));

    // The examples of Section 3.3.
    CHECK(answers(meet_in_the_middle, V{3, 1, 4, 2}, Verdict::Yes));
    CHECK(answers(meet_in_the_middle, V{2, 3, 7}, Verdict::No));
    CHECK(answers(meet_in_the_middle, V{2, 3, 4}, Verdict::No));  // odd sum

    // Hand-made cases: two elements, zeros, the solution inside one half.
    CHECK(answers(meet_in_the_middle, V{1, 1}, Verdict::Yes));
    CHECK(answers(meet_in_the_middle, V{1, 3}, Verdict::No));
    CHECK(answers(meet_in_the_middle, V{0, 0, 0}, Verdict::Yes));
    CHECK(answers(meet_in_the_middle, V{0, 4, 0, 4}, Verdict::Yes));
    CHECK(answers(meet_in_the_middle, V{5, 6, 7, 8, 10}, Verdict::Yes));
    CHECK(answers(meet_in_the_middle, V{3, 3, 1, 1, 1, 1}, Verdict::Yes));  // {3, 3} is the left half
    CHECK(answers(meet_in_the_middle, V{7, 1, 1, 5}, Verdict::Yes));  // first hit: left sum 0
    CHECK(answers(meet_in_the_middle, V{10, 1, 1}, Verdict::No));

    // Equal to brute force on random sets with n ≤ 20, duplicates and zeros.
    for (int round = 0; round < 400; ++round) {
        const std::size_t n = random_sets::size_between(2, 20);
        const std::uint64_t max_value = round % 3 == 0 ? 3 : (round % 3 == 1 ? 50 : 100000);
        const V a = random_sets::small_values(n, max_value);
        CHECK(answers(meet_in_the_middle, a, verdict_of(brute_force(a))));
    }
    // 25-digit numbers: random sets (almost always NO) and YES by construction.
    for (int round = 0; round < 50; ++round) {
        const std::size_t n = random_sets::size_between(2, 18);
        const V no = random_sets::large_values(n);
        CHECK(answers(meet_in_the_middle, no, verdict_of(brute_force(no))));
        const V yes = random_sets::large_yes_instance(n);
        CHECK(answers(brute_force, yes, Verdict::Yes));
        CHECK(answers(meet_in_the_middle, yes, Verdict::Yes));
    }
    // Beyond the reach of brute force in the tests: n = 40.
    CHECK(answers(meet_in_the_middle, random_sets::large_yes_instance(40), Verdict::Yes));

    CHECK_THROWS(meet_in_the_middle(V(125, 1)), std::invalid_argument);

    // Where the physical memory is known (Linux), a table larger than it is
    // refused before it is allocated.
    const std::uint64_t physical = physical_memory_bytes();
    if (physical > 0) {
        CHECK_THROWS(require_memory("test", Value{physical} + 1), std::runtime_error);
        require_memory("test", physical);
        CHECK_THROWS(meet_in_the_middle(V(124, 1)), std::runtime_error);  // 2 · 2^62 sums
        CHECK_THROWS(dp_partition(V{kMaxDpTarget, kMaxDpTarget}), std::runtime_error);
    }
}

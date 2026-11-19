#include "algorithms/brute_force.hpp"
#include "check.hpp"
#include "partition_check.hpp"
#include "random_sets.hpp"
#include "tests.hpp"

#include <optional>
#include <stdexcept>
#include <vector>

using partition_check::answers;

void test_brute_force() {
    using V = std::vector<Value>;
    using I = std::vector<std::size_t>;

    // Preliminary checks (Section 1.2).
    CHECK(total_sum(V{3, 1, 4, 2}) == 10);
    CHECK(preliminary_checks(V{2, 3, 4}).verdict == Verdict::No);   // S = 9 is odd
    CHECK(preliminary_checks(V{0, 0}).verdict == Verdict::Yes);     // S = 0
    CHECK(!preliminary_checks(V{3, 1, 4, 2}).verdict.has_value());
    CHECK(preliminary_checks(V{3, 1, 4, 2}).T == 5);
    const Value all_ones = ~Value{0};
    CHECK_THROWS(total_sum(V{all_ones, 1}), std::overflow_error);

    CHECK(values_at(V{5, 6, 7, 8}, I{3, 0}) == (V{8, 5}));
    CHECK(values_at(V{5, 6}, I{}).empty());

    // The partition of 0..n−1 into A and its complement.
    const Partition p = balanced_partition(5, I{3, 0});
    CHECK(p.A == (I{0, 3}) && p.B == (I{1, 2, 4}) && p.d == 0);

    // FIND-SUBSET, the Gray-code search (masks 001, 011, 010, 110, 111, ...).
    CHECK(find_subset(V{3, 1, 4}, 0) == I{});
    CHECK(find_subset(V{3, 1, 4}, 5) == (I{1, 2}));
    CHECK(find_subset(V{3, 1, 4}, 8) == (I{0, 1, 2}));
    CHECK(find_subset(V{3, 1, 4}, 2) == std::nullopt);
    CHECK(find_subset(V{}, 0) == I{});
    CHECK(find_subset(V{}, 1) == std::nullopt);

    // m = 62 and 63: only a hit near the start of the Gray code is affordable.
    for (std::size_t m : {62, 63}) {
        V b(m, 0);
        b[0] = 5;
        CHECK(find_subset(b, 0) == I{});          // the empty subset, at once
        CHECK(find_subset(b, 5) == I{0});         // the first step
        b[20] = 7;                                // reached after about 2^20 steps
        const auto found = find_subset(b, 7);
        CHECK(found.has_value() && partition_check::sum_of(b, *found) == 7);
        const auto both = find_subset(b, 12);
        CHECK(both.has_value() && partition_check::sum_of(b, *both) == 12);
    }
    // An unreachable sum is exhausted only for small m.
    CHECK(find_subset(V(20, 2), 7) == std::nullopt);
    CHECK_THROWS(find_subset(V(64, 0), 0), std::invalid_argument);
    CHECK_THROWS(find_subset(V(64, 0), 1), std::invalid_argument);
    CHECK_THROWS(find_subset(V(1000, 1), 3), std::invalid_argument);

    // The examples of Section 2.3: the mask 110 stands for A = {a[1], a[2]}.
    CHECK(answers(brute_force, V{3, 1, 4, 2}, Verdict::Yes));
    CHECK(brute_force(V{3, 1, 4, 2})->A == (I{1, 2}));
    CHECK(answers(brute_force, V{2, 3, 7}, Verdict::No));

    // Hand-made cases.
    CHECK(answers(brute_force, V{1, 1}, Verdict::Yes));
    CHECK(answers(brute_force, V{1, 2}, Verdict::No));
    CHECK(answers(brute_force, V{0, 0}, Verdict::Yes));
    CHECK(answers(brute_force, V{0, 0, 0, 0, 0}, Verdict::Yes));
    CHECK(brute_force(V{0, 0, 0, 0, 0})->A == I{0});         // S = 0: {a[0]} against the rest
    CHECK(answers(brute_force, V{0, 5}, Verdict::No));
    CHECK(answers(brute_force, V{0, 5, 5}, Verdict::Yes));
    CHECK(answers(brute_force, V{2, 3, 4}, Verdict::No));             // odd sum
    CHECK(answers(brute_force, V{2, 2, 3, 5}, Verdict::No));          // Section 4.4
    CHECK(answers(brute_force, V{5, 6, 7, 8, 10}, Verdict::Yes));     // Section 5.3
    CHECK(answers(brute_force, V{1, 1, 1, 1, 1, 1, 6}, Verdict::Yes));  // the last element alone
    CHECK(answers(brute_force, V{10, 1, 1}, Verdict::No));            // one element larger than T

    // 25-digit numbers: 10^24 + 1, 10^24 + 2, 2 · 10^24 + 3.
    const Value e24 = parse_value("1000000000000000000000000").value;
    CHECK(answers(brute_force, V{e24 + 1, e24 + 2, 2 * e24 + 3}, Verdict::Yes));
    CHECK(answers(brute_force, V{e24 + 1, e24 + 2, 2 * e24 + 5}, Verdict::No));

    // n = 20: the whole enumeration of 2^19 masks for a NO instance with even S.
    V many(20, 2);
    many[0] = 4;  // S = 42, T = 21 is odd and every element is even
    CHECK(answers(brute_force, many, Verdict::No));

    // Random sets: every YES comes with a balanced partition.
    for (int round = 0; round < 200; ++round) {
        const std::size_t n = random_sets::size_between(2, 16);
        const V a = random_sets::small_values(n, round % 2 == 0 ? 5 : 200);
        const ExactAnswer answer = brute_force(a);
        CHECK(partition_check::is_answer(a, answer, verdict_of(answer)));
    }

    CHECK_THROWS(brute_force(V(65, 1)), std::invalid_argument);
}

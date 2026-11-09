#include "algorithms/brute_force.hpp"
#include "check.hpp"
#include "tests.hpp"

#include <stdexcept>
#include <vector>

void test_brute_force() {
    using V = std::vector<Value>;

    // Preliminary checks (Section 1.2).
    CHECK(total_sum(V{3, 1, 4, 2}) == 10);
    CHECK(preliminary_checks(V{2, 3, 4}).verdict == Verdict::No);   // S = 9 is odd
    CHECK(preliminary_checks(V{0, 0}).verdict == Verdict::Yes);     // S = 0
    CHECK(!preliminary_checks(V{3, 1, 4, 2}).verdict.has_value());
    CHECK(preliminary_checks(V{3, 1, 4, 2}).T == 5);
    const Value all_ones = ~Value{0};
    CHECK_THROWS(total_sum(V{all_ones, 1}), std::overflow_error);

    // The examples of Section 2.3.
    CHECK(brute_force(V{3, 1, 4, 2}) == Verdict::Yes);
    CHECK(brute_force(V{2, 3, 7}) == Verdict::No);

    // Hand-made cases.
    CHECK(brute_force(V{1, 1}) == Verdict::Yes);
    CHECK(brute_force(V{1, 2}) == Verdict::No);
    CHECK(brute_force(V{0, 0}) == Verdict::Yes);
    CHECK(brute_force(V{0, 0, 0, 0, 0}) == Verdict::Yes);
    CHECK(brute_force(V{0, 5}) == Verdict::No);
    CHECK(brute_force(V{0, 5, 5}) == Verdict::Yes);
    CHECK(brute_force(V{2, 3, 4}) == Verdict::No);           // odd sum
    CHECK(brute_force(V{2, 2, 3, 5}) == Verdict::No);        // Section 4.4
    CHECK(brute_force(V{5, 6, 7, 8, 10}) == Verdict::Yes);   // Section 5.3
    CHECK(brute_force(V{1, 1, 1, 1, 1, 1, 6}) == Verdict::Yes);  // the last element alone
    CHECK(brute_force(V{10, 1, 1}) == Verdict::No);          // one element larger than T

    // 25-digit numbers: 10^24 + 1, 10^24 + 2, 2 · 10^24 + 3.
    const Value e24 = parse_value("1000000000000000000000000").value;
    CHECK(brute_force(V{e24 + 1, e24 + 2, 2 * e24 + 3}) == Verdict::Yes);
    CHECK(brute_force(V{e24 + 1, e24 + 2, 2 * e24 + 5}) == Verdict::No);

    // n = 20: the whole enumeration of 2^19 masks for a NO instance with even S.
    V many(20, 2);
    many[0] = 4;  // S = 42, T = 21 is odd and every element is even
    CHECK(brute_force(many) == Verdict::No);

    CHECK_THROWS(brute_force(V(65, 1)), std::invalid_argument);
}

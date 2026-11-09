#include "algorithms/brute_force.hpp"
#include "algorithms/dynamic_programming.hpp"
#include "check.hpp"
#include "random_sets.hpp"
#include "tests.hpp"

#include <algorithm>
#include <stdexcept>
#include <vector>

namespace {

// n random non-negative values with the sum 2T, each at most T, so that the
// test reaches the bitset and does not stop at the checks of Section 4.1.
std::vector<Value> random_split(std::size_t T, std::size_t n) {
    std::uniform_int_distribution<std::size_t> cut(0, 2 * T);
    while (true) {
        std::vector<std::size_t> cuts{0, 2 * T};
        for (std::size_t i = 0; i + 1 < n; ++i) cuts.push_back(cut(random_sets::engine()));
        std::sort(cuts.begin(), cuts.end());
        std::vector<Value> a;
        for (std::size_t i = 1; i < cuts.size(); ++i) a.push_back(cuts[i] - cuts[i - 1]);
        if (*std::max_element(a.begin(), a.end()) <= T) return a;
    }
}

}  // namespace

void test_dynamic_programming() {
    using V = std::vector<Value>;

    // The examples of Section 4.4.
    CHECK(dp_partition(V{3, 1, 4, 2}) == Verdict::Yes);
    CHECK(dp_partition(V{2, 2, 3, 5}) == Verdict::No);

    // Hand-made cases: the trivial checks, zeros, duplicates.
    CHECK(dp_partition(V{2, 3, 4}) == Verdict::No);      // odd sum
    CHECK(dp_partition(V{0, 0}) == Verdict::Yes);        // S = 0
    CHECK(dp_partition(V{10, 1, 1}) == Verdict::No);     // max a[i] > T
    CHECK(dp_partition(V{1, 1}) == Verdict::Yes);
    CHECK(dp_partition(V{0, 7, 0, 7, 0}) == Verdict::Yes);
    CHECK(dp_partition(V{5, 6, 7, 8, 10}) == Verdict::Yes);
    CHECK(dp_partition(V(1000, 3)) == Verdict::Yes);     // T = 1500
    CHECK(dp_partition(V(1001, 2)) == Verdict::No);      // T = 1001 is odd, all elements even

    // A large value skips the bitset when the checks decide.
    const Value e24 = parse_value("1000000000000000000000000").value;
    CHECK(dp_partition(V{e24, 1}) == Verdict::No);       // odd sum
    CHECK(dp_partition(V{2 * e24, 2, 2}) == Verdict::No);  // max a[i] > T
    CHECK_THROWS(dp_partition(V{e24, e24}), std::invalid_argument);

    // T at word boundaries: shifts across one and two words.
    for (std::size_t T : {1, 2, 31, 32, 63, 64, 65, 127, 128, 129, 191, 192, 193, 255, 256}) {
        CHECK(dp_partition(V{T, T}) == Verdict::Yes);
        CHECK(dp_partition(V{T, T - 1, 1}) == Verdict::Yes);
        for (int round = 0; round < 20; ++round) {
            const V a = random_split(T, random_sets::size_between(2, 14));
            CHECK(dp_partition(a) == brute_force(a));
        }
    }

    // Equal to brute force on random sets with n ≤ 20, duplicates and zeros.
    for (int round = 0; round < 400; ++round) {
        const std::size_t n = random_sets::size_between(2, 20);
        const std::uint64_t max_value = round % 3 == 0 ? 3 : (round % 3 == 1 ? 50 : 3000);
        const V a = random_sets::small_values(n, max_value);
        CHECK(dp_partition(a) == brute_force(a));
    }
}

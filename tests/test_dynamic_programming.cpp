#include "algorithms/brute_force.hpp"
#include "algorithms/dynamic_programming.hpp"
#include "check.hpp"
#include "partition_check.hpp"
#include "random_sets.hpp"
#include "tests.hpp"
#include "value.hpp"

#include <algorithm>
#include <stdexcept>
#include <vector>

using partition_check::answers;

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
    CHECK(answers(dp_partition<Value>, V{3, 1, 4, 2}, Verdict::Yes));
    CHECK(answers(dp_partition<Value>, V{2, 2, 3, 5}, Verdict::No));

    // Hand-made cases: the trivial checks, zeros, duplicates.
    CHECK(answers(dp_partition<Value>, V{2, 3, 4}, Verdict::No));      // odd sum
    CHECK(answers(dp_partition<Value>, V{0, 0}, Verdict::Yes));        // S = 0
    CHECK(answers(dp_partition<Value>, V{10, 1, 1}, Verdict::No));     // max a[i] > T
    CHECK(answers(dp_partition<Value>, V{1, 1}, Verdict::Yes));
    CHECK(answers(dp_partition<Value>, V{0, 7, 0, 7, 0}, Verdict::Yes));
    CHECK(answers(dp_partition<Value>, V{5, 6, 7, 8, 10}, Verdict::Yes));
    CHECK(answers(dp_partition<Value>, V(1000, 3), Verdict::Yes));     // T = 1500
    CHECK(answers(dp_partition<Value>, V(1001, 2), Verdict::No));      // T = 1001 is odd, all elements even

    // Reading back from T: T is first reached with a[2] = 4 from the sum 1 of
    // a[1], so A = {a[1], a[2]}, the values 1 and 4 (Section 4.4).
    CHECK(dp_partition<Value>(V{3, 1, 4, 2})->A == (std::vector<std::size_t>{1, 2}));

    // A large value skips the bitset when the checks decide.
    const Value e24 = parse_value<Value>("1000000000000000000000000").value;
    CHECK(answers(dp_partition<Value>, V{e24, 1}, Verdict::No));       // odd sum
    CHECK(answers(dp_partition<Value>, V{2 * e24, 2, 2}, Verdict::No));  // max a[i] > T
    CHECK_THROWS(dp_partition<Value>(V{e24, e24}), std::invalid_argument);

    // T at word boundaries: shifts across one and two words.
    for (std::size_t T : {1, 2, 31, 32, 63, 64, 65, 127, 128, 129, 191, 192, 193, 255, 256}) {
        CHECK(answers(dp_partition<Value>, V{T, T}, Verdict::Yes));
        CHECK(answers(dp_partition<Value>, V{T, T - 1, 1}, Verdict::Yes));
        for (int round = 0; round < 20; ++round) {
            const V a = random_split(T, random_sets::size_between(2, 14));
            CHECK(answers(dp_partition<Value>, a, verdict_of(brute_force<Value>(a))));
        }
    }

    // Equal to brute force on random sets with n <= 20, duplicates and zeros.
    for (int round = 0; round < 400; ++round) {
        const std::size_t n = random_sets::size_between(2, 20);
        const std::uint64_t max_value = round % 3 == 0 ? 3 : (round % 3 == 1 ? 50 : 3000);
        const V a = random_sets::small_values(n, max_value);
        CHECK(answers(dp_partition<Value>, a, verdict_of(brute_force<Value>(a))));
    }
    // Many elements, where brute force cannot follow: YES by construction
    // (each of the values 1..500 twice), and a valid partition read back.
    V twice;
    for (Value x = 1; x <= 500; ++x) twice.insert(twice.end(), {x, x});
    CHECK(answers(dp_partition<Value>, twice, Verdict::Yes));
}

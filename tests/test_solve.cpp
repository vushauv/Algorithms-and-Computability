#include "algorithms/brute_force.hpp"
#include "check.hpp"
#include "random_sets.hpp"
#include "solve.hpp"
#include "tests.hpp"

#include <vector>

#include "algorithms/dynamic_programming.hpp"

namespace {

// No bound on the work n · T of dynamic programming.
constexpr Value kAnyWork = ~Value{0};

}  // namespace

void test_solve() {
    using V = std::vector<Value>;
    const Ranges defaults{44, 10'000'000'000, kAnyWork};
    const Ranges no_exact{0, 0, kAnyWork};

    // The examples of Section 6.3.
    const Solution balanced = solve(V{3, 1, 4, 2}, defaults);
    CHECK(balanced.answer == Verdict::Yes);
    CHECK(balanced.decided_by == Method::KarmarkarKarp);
    CHECK(balanced.partition.d == 0);

    const Solution odd = solve(V{2, 3, 4}, defaults);
    CHECK(odd.answer == Verdict::No);
    CHECK(odd.decided_by == Method::OddSum);
    CHECK(odd.partition.d == 1);

    const V missed{5, 6, 7, 8, 10};
    const Solution by_dp = solve(missed, defaults);
    CHECK(by_dp.answer == Verdict::Yes);
    CHECK(by_dp.decided_by == Method::DynamicProgramming);  // T/64 < 2^(5/2)
    CHECK(by_dp.partition.d == 2);
    CHECK(by_dp.S == 36 && by_dp.T == 18);

    const Solution by_mitm = solve(missed, Ranges{44, 17, kAnyWork});
    CHECK(by_mitm.answer == Verdict::Yes);
    CHECK(by_mitm.decided_by == Method::MeetInTheMiddle);

    const Solution unknown = solve(missed, Ranges{4, 17, kAnyWork});
    CHECK(unknown.answer == Verdict::Unknown);
    CHECK(unknown.decided_by == Method::None);
    CHECK(unknown.partition.d == 2);

    // Even sum, d > 0, no balanced partition: the exact algorithm says NO.
    CHECK(solve(V{1, 1, 4}, defaults).answer == Verdict::No);
    CHECK(solve(V{1, 1, 4}, defaults).decided_by == Method::DynamicProgramming);
    CHECK(solve(V{1, 1, 4}, Ranges{44, 0, kAnyWork}).decided_by == Method::MeetInTheMiddle);
    CHECK(solve(V{1, 1, 4}, no_exact).answer == Verdict::Unknown);

    // All zeros: d = 0, YES whatever the ranges (Section 6.1, step 1).
    CHECK(solve(V{0, 0, 0}, no_exact).answer == Verdict::Yes);

    // The rule of Proposition 4.4: DP iff T/64 < 2^(n/2).
    CHECK(dp_is_cheaper(2, 127));     // 127/64 < 2
    CHECK(!dp_is_cheaper(2, 128));    // 128/64 = 2
    CHECK(dp_is_cheaper(20, 65535));  // 1023.98 < 1024
    CHECK(!dp_is_cheaper(20, 65536));
    CHECK(dp_is_cheaper(3, 181));     // 2.83 < 2^1.5 = 2.83, 181/64 = 2.828
    CHECK(!dp_is_cheaper(3, 182));
    CHECK(choose_exact(10, 100, Ranges{9, 99, kAnyWork}) == Method::None);
    CHECK(choose_exact(10, 100, Ranges{10, 99, kAnyWork}) == Method::MeetInTheMiddle);
    CHECK(choose_exact(10, 100, Ranges{9, 100, kAnyWork}) == Method::DynamicProgramming);
    CHECK(choose_exact(40, Value{1} << 100, Ranges{44, ~Value{0}, kAnyWork}) == Method::MeetInTheMiddle);
    CHECK(choose_exact(100, Value{1} << 100, Ranges{44, ~Value{0}, kAnyWork}) == Method::None);  // > kMaxDpTarget

    // The work bound n · T ≤ w_dp, exactly at the limit and one above it.
    CHECK(dp_limit(10, 100, Ranges{0, 100, 1000}) == DpLimit::InRange);
    CHECK(dp_limit(10, 100, Ranges{0, 100, 999}) == DpLimit::Work);
    CHECK(dp_limit(10, 101, Ranges{0, 100, kAnyWork}) == DpLimit::Target);
    CHECK(dp_limit(3, 7, Ranges{0, 100, 21}) == DpLimit::InRange);  // 21 / 3 = 7
    CHECK(dp_limit(3, 7, Ranges{0, 100, 20}) == DpLimit::Work);     // 20 / 3 = 6
    // No overflow of n · T: T near the cap and the largest n of the input.
    CHECK(dp_limit(1'000'000, kMaxDpTarget, Ranges{0, kMaxDpTarget, kAnyWork}) == DpLimit::InRange);
    CHECK(dp_limit(1'000'000, kMaxDpTarget, Ranges{0, kMaxDpTarget, 1'000'000}) == DpLimit::Work);
    CHECK(choose_exact(10, 100, Ranges{10, 100, 999}) == Method::MeetInTheMiddle);
    CHECK(choose_exact(10, 100, Ranges{9, 100, 999}) == Method::None);
    const Solution by_work = solve(missed, Ranges{44, 18, 5 * 18 - 1});  // n · T = 90
    CHECK(by_work.decided_by == Method::MeetInTheMiddle);
    CHECK(by_work.answer == Verdict::Yes);
    CHECK(solve(missed, Ranges{44, 18, 5 * 18}).decided_by == Method::DynamicProgramming);

    // Random small sets: the answer agrees with brute force for every choice of
    // the exact algorithm, and UNKNOWN comes only when none is in range.
    const Ranges choices[] = {defaults, Ranges{0, defaults.t_dp, kAnyWork},
                              Ranges{defaults.n_mitm, 0, kAnyWork},
                              Ranges{defaults.n_mitm, defaults.t_dp, 100}, no_exact};
    for (int round = 0; round < 300; ++round) {
        const std::size_t n = random_sets::size_between(2, 14);
        const V a = random_sets::small_values(n, round % 3 == 0 ? 5 : 1000);
        const Verdict expected = brute_force(a);
        for (const Ranges& ranges : choices) {
            const Solution s = solve(a, ranges);
            if (s.decided_by == Method::None) {
                CHECK(s.answer == Verdict::Unknown);
                CHECK(s.partition.d > 0 && s.S % 2 == 0);
            } else {
                CHECK(s.answer == expected);
            }
        }
    }

    // 25-digit values: meet in the middle decides, DP is out of range.
    for (int round = 0; round < 20; ++round) {
        const V yes = random_sets::large_yes_instance(16);
        const Solution s = solve(yes, defaults);
        CHECK(s.answer == Verdict::Yes);
        CHECK(s.decided_by == Method::KarmarkarKarp || s.decided_by == Method::MeetInTheMiddle);
    }
}

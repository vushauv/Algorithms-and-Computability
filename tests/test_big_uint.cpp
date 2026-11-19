// BigUint, our own long arithmetic, in several bases: against the native
// 128-bit integers where they suffice and against Boost beyond them.
#include "check.hpp"
#include "number.hpp"
#include "random_sets.hpp"
#include "tests.hpp"

#include <cstdint>
#include <stdexcept>
#include <string>

namespace {

// A random number of a random length below 2^127, so that a sum of two fits.
Native random_native() {
    const Native high = random_sets::engine()();
    const Native low = random_sets::engine()();
    return ((high << 64) | low) >> random_sets::size_between(1, 127);
}

// A random decimal number of 1 to max_digits digits, without leading zeros.
std::string random_decimal(std::size_t max_digits) {
    std::string digits(random_sets::size_between(1, max_digits), '0');
    for (char& c : digits) c = static_cast<char>('0' + random_sets::size_between(0, 9));
    if (digits.size() > 1 && digits[0] == '0') digits[0] = '7';
    return digits;
}

BigUint big(const std::string& digits) {
    return parse_value<BigUint>(digits).value;
}

// x and y stand for the same number.
template <class Reference>
bool same(const BigUint& x, const Reference& y) {
    return to_string(x) == to_string(y);
}

void check_against_native() {
    for (int round = 0; round < 300; ++round) {
        const Native a = random_native();
        const Native b = round % 10 == 0 ? a : random_native();
        const BigUint x = big(to_string(a));
        const BigUint y = big(to_string(b));
        CHECK(same(x, a));
        CHECK(same(x + y, a + b));
        CHECK(a >= b ? same(x - y, a - b) : same(y - x, b - a));
        CHECK((x == y) == (a == b));
        CHECK((x < y) == (a < b));
        CHECK((x >= y) == (a >= b));
        CHECK(is_odd(x) == is_odd(a));
        CHECK(same(half(x), half(a)));
        CHECK(to_u64(x) == to_u64(a));
        CHECK(stored_bytes(x) >= sizeof(BigUint));
    }
}

void check_against_boost() {
    for (int round = 0; round < 200; ++round) {
        const std::string s = random_decimal(90);
        const std::string t = random_decimal(round % 2 == 0 ? 90 : 12);
        const BigUint x = big(s);
        const BigUint y = big(t);
        const BoostInt a = parse_value<BoostInt>(s).value;
        const BoostInt b = parse_value<BoostInt>(t).value;
        CHECK(to_string(x) == s);
        CHECK(same(x + y, BoostInt(a + b)));
        CHECK(a >= b ? same(x - y, BoostInt(a - b)) : same(y - x, BoostInt(b - a)));
        CHECK((x < y) == (a < b));
        CHECK(is_odd(x) == is_odd(a));
        CHECK(same(half(x), half(a)));
        CHECK(to_u64(x) == to_u64(a));
    }
}

void check_small_cases() {
    CHECK(BigUint().is_zero() && BigUint(0).is_zero());
    CHECK(BigUint() == 0);
    CHECK(to_string(BigUint()) == "0");
    CHECK(to_string(BigUint(1234567890123456789)) == "1234567890123456789");
    const std::uint64_t all_ones = ~std::uint64_t{0};
    CHECK(to_u64(BigUint(all_ones)) == all_ones);
    CHECK(!to_u64(BigUint(all_ones) + 1).has_value());  // 2^64
    CHECK(to_u64(BigUint(all_ones) + 1 - 1) == all_ones);
    CHECK(BigUint(5) - 5 == 0);
    CHECK((BigUint(5) - 5).cell_count() == 0);  // no zero cells are kept
    CHECK(BigUint(7) > 6 && BigUint(7) <= 7 && BigUint(7) != 8);
    CHECK(!is_odd(BigUint(0)) && is_odd(BigUint(1)) && !is_odd(BigUint(2)) && is_odd(BigUint(9)));
    CHECK(half(BigUint(1)) == 0 && half(BigUint(9)) == 4);
    // Zeros inside a number, and leading zeros of the text.
    CHECK(to_string(big("1000000000000000000000000000000000000000000000")) ==
          "1000000000000000000000000000000000000000000000");
    CHECK(to_string(big("000000000000000000000000000000042")) == "42");
    CHECK(big("0000") == 0);

    BigUint x = 6;
    x.multiply_add(1000, 999);
    CHECK(x == 6999);
    CHECK(x.divide(7) == 6 && x == 999);
    x.multiply_add(0, 0);
    CHECK(x.is_zero());
}

}  // namespace

void test_big_uint() {
    CHECK_THROWS(BigUint::set_base(0), std::invalid_argument);
    CHECK_THROWS(BigUint::set_base(1), std::invalid_argument);
    CHECK_THROWS(BigUint::set_base(BigUint::kMaxBase + 1), std::invalid_argument);
    CHECK(BigUint::base() == BigUint::kMaxBase);  // the default, untouched by the errors

    // A bit, an odd base, a decimal digit, a byte minus one, nine digits, a word.
    for (std::uint64_t base : {std::uint64_t{2}, std::uint64_t{3}, std::uint64_t{10},
                               std::uint64_t{255}, std::uint64_t{1'000'000'000},
                               BigUint::kMaxBase - 1, BigUint::kMaxBase}) {
        BigUint::set_base(base);
        CHECK(BigUint::base() == base);
        check_small_cases();
        check_against_native();
        check_against_boost();
    }
    BigUint::set_base(BigUint::kMaxBase);
}

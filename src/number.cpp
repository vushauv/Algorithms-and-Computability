#include "number.hpp"

#include <algorithm>
#include <limits>

namespace {

// Decimal reading and printing of BigUint go nine digits at a time.
constexpr std::size_t kChunkDigits = 9;
constexpr BigUint::Cell kChunk = 1'000'000'000;  // 10^9 < 2^32

bool all_digits(std::string_view text) {
    return std::all_of(text.begin(), text.end(), [](char c) { return c >= '0' && c <= '9'; });
}

// Empty or NotDigit if the text is not a number, otherwise Ok.
ParseStatus check_digits(std::string_view text) {
    if (text.empty()) return ParseStatus::Empty;
    return all_digits(text) ? ParseStatus::Ok : ParseStatus::NotDigit;
}

BigUint::Cell power_of_ten(std::size_t exponent) {
    BigUint::Cell power = 1;
    for (std::size_t i = 0; i < exponent; ++i) power *= 10;
    return power;
}

}  // namespace

template <>
ParsedValue<Native> parse_value<Native>(std::string_view text) {
    if (text.empty()) return {ParseStatus::Empty, 0};

    Native value = 0;
    for (char c : text) {
        if (c < '0' || c > '9') return {ParseStatus::NotDigit, 0};
        // value < 10^38 before this step, so 10 * value + 9 < 10^39 < 2^128.
        value = value * 10 + static_cast<unsigned>(c - '0');
        if (value >= kNativeLimit) {
            // Keep scanning: a later non-digit is the more useful message.
            return {all_digits(text) ? ParseStatus::TooLarge : ParseStatus::NotDigit, 0};
        }
    }
    return {ParseStatus::Ok, value};
}

template <>
ParsedValue<BoostInt> parse_value<BoostInt>(std::string_view text) {
    const ParseStatus status = check_digits(text);
    if (status != ParseStatus::Ok) return {status, 0};
    // Boost reads a number that starts with 0 as octal, so the zeros go first.
    text.remove_prefix(std::min(text.find_first_not_of('0'), text.size() - 1));
    return {ParseStatus::Ok, BoostInt(std::string(text))};
}

template <>
ParsedValue<BigUint> parse_value<BigUint>(std::string_view text) {
    const ParseStatus status = check_digits(text);
    if (status != ParseStatus::Ok) return {status, 0};

    BigUint value;
    // The first chunk takes the digits that do not fill a whole one.
    std::size_t length = text.size() % kChunkDigits;
    if (length == 0) length = kChunkDigits;
    for (std::size_t start = 0; start < text.size(); start += length, length = kChunkDigits) {
        BigUint::Cell chunk = 0;
        for (char c : text.substr(start, length)) chunk = chunk * 10 + static_cast<unsigned>(c - '0');
        value.multiply_add(power_of_ten(length), chunk);
    }
    return {ParseStatus::Ok, value};
}

std::string to_string(Native value) {
    if (value == 0) return "0";
    std::string digits;
    while (value > 0) {
        digits.push_back(static_cast<char>('0' + static_cast<int>(value % 10)));
        value /= 10;
    }
    std::reverse(digits.begin(), digits.end());
    return digits;
}

std::string to_string(const BoostInt& value) {
    return value.str();
}

std::string to_string(const BigUint& value) {
    if (value.is_zero()) return "0";
    // The chunks from the lowest one; each but the highest has nine digits.
    std::string digits;
    for (BigUint rest = value; !rest.is_zero();) {
        BigUint::Cell chunk = rest.divide(kChunk);
        for (std::size_t i = 0; i < kChunkDigits && (chunk > 0 || !rest.is_zero()); ++i) {
            digits.push_back(static_cast<char>('0' + chunk % 10));
            chunk /= 10;
        }
    }
    std::reverse(digits.begin(), digits.end());
    return digits;
}

std::optional<Native> checked_add(Native a, Native b) {
    Native sum = a + b;  // unsigned arithmetic wraps around on overflow
    if (sum < a) return std::nullopt;
    return sum;
}

std::optional<BoostInt> checked_add(const BoostInt& a, const BoostInt& b) {
    return BoostInt(a + b);
}

std::optional<BigUint> checked_add(const BigUint& a, const BigUint& b) {
    return a + b;
}

bool is_odd(Native value) {
    return value % 2 == 1;
}

bool is_odd(const BoostInt& value) {
    return boost::multiprecision::bit_test(value, 0);
}

bool is_odd(const BigUint& value) {
    return value.is_odd();
}

Native half(Native value) {
    return value / 2;
}

BoostInt half(const BoostInt& value) {
    return BoostInt(value >> 1);
}

BigUint half(const BigUint& value) {
    BigUint result = value;
    result.divide(2);
    return result;
}

std::optional<std::uint64_t> to_u64(Native value) {
    if (value > std::numeric_limits<std::uint64_t>::max()) return std::nullopt;
    return static_cast<std::uint64_t>(value);
}

std::optional<std::uint64_t> to_u64(const BoostInt& value) {
    if (value > std::numeric_limits<std::uint64_t>::max()) return std::nullopt;
    return static_cast<std::uint64_t>(value);
}

std::optional<std::uint64_t> to_u64(const BigUint& value) {
    return value.to_u64();
}

std::size_t stored_bytes(Native) {
    return sizeof(Native);
}

std::size_t stored_bytes(const BoostInt& value) {
    // The words of a number are inside the object while they fit there and
    // in a separate block otherwise.
    const std::size_t words = value.backend().size();
    const bool inside = words <= boost::multiprecision::cpp_int_backend<>::internal_limb_count;
    return sizeof(BoostInt) + (inside ? 0 : words * sizeof(boost::multiprecision::limb_type));
}

std::size_t stored_bytes(const BigUint& value) {
    return sizeof(BigUint) + value.cell_count() * sizeof(BigUint::Cell);
}

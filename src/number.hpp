// The numbers of the input, in three arithmetics chosen with --arithmetic:
//
//   Native    unsigned 128-bit integers of the compiler:
//             every value below 10^38, every sum below 2^128;
//   BoostInt  cpp_int of Boost.Multiprecision, of any size;
//   BigUint   our own long arithmetic of any size, with the base of --base.
//
// The algorithms are templates over the type V of the numbers. They use +, -
// (never below zero), the comparisons, and the functions declared here.
#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>

#include <boost/multiprecision/cpp_int.hpp>

#include "big_uint.hpp"

// __extension__ keeps -Wpedantic quiet about the GCC/Clang extension.
__extension__ using Native = unsigned __int128;

// Without expression templates, so that a + b is a number in generic code.
using BoostInt = boost::multiprecision::number<boost::multiprecision::cpp_int_backend<>,
                                               boost::multiprecision::et_off>;

// Applies the macro X to each type of numbers; the .cpp files use it to
// instantiate their templates.
#define FOR_EACH_NUMBER_TYPE(X) X(Native) X(BoostInt) X(BigUint)

// The smallest value that is too large for a Native element: 10^38.
inline constexpr Native kNativeLimit = [] {
    Native limit = 1;
    for (int i = 0; i < 38; ++i) limit *= 10;
    return limit;
}();

enum class ParseStatus { Ok, Empty, NotDigit, TooLarge };

template <class V>
struct ParsedValue {
    ParseStatus status;
    V value;  // meaningful only when status is Ok
};

// Reads a decimal number made of digits only (leading zeros allowed).
// TooLarge only for Native, when the value is 10^38 or more.
template <class V>
ParsedValue<V> parse_value(std::string_view text);

// Decimal representation, without leading zeros.
std::string to_string(Native value);
std::string to_string(const BoostInt& value);
std::string to_string(const BigUint& value);

// a + b, or nothing when the sum does not fit: only for Native, at 2^128.
std::optional<Native> checked_add(Native a, Native b);
std::optional<BoostInt> checked_add(const BoostInt& a, const BoostInt& b);
std::optional<BigUint> checked_add(const BigUint& a, const BigUint& b);

bool is_odd(Native value);
bool is_odd(const BoostInt& value);
bool is_odd(const BigUint& value);

// value / 2, rounded down.
Native half(Native value);
BoostInt half(const BoostInt& value);
BigUint half(const BigUint& value);

// The value as a 64-bit integer, or nothing if it is 2^64 or more.
std::optional<std::uint64_t> to_u64(Native value);
std::optional<std::uint64_t> to_u64(const BoostInt& value);
std::optional<std::uint64_t> to_u64(const BigUint& value);

// An estimate of the bytes that one stored number as large as value takes,
// for the memory check of meet in the middle.
std::size_t stored_bytes(Native value);
std::size_t stored_bytes(const BoostInt& value);
std::size_t stored_bytes(const BigUint& value);

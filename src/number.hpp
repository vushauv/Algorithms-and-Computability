// Numbers of the input: unsigned 128-bit integers (report, Section 1.4).
// Every value is below 10^38; the sum of a set is checked to stay below 2^128.
#pragma once

#include <optional>
#include <string>
#include <string_view>

// The only place where the 128-bit type is named; __extension__ keeps
// -Wpedantic quiet about the GCC/Clang extension.
__extension__ using Value = unsigned __int128;

// The smallest value that is too large for an element: 10^38.
inline constexpr Value kValueLimit = [] {
    Value limit = 1;
    for (int i = 0; i < 38; ++i) limit *= 10;
    return limit;
}();

enum class ParseStatus { Ok, Empty, NotDigit, TooLarge };

struct ParsedValue {
    ParseStatus status;
    Value value;  // meaningful only when status is Ok
};

// Reads a decimal number made of digits only (leading zeros allowed).
// TooLarge when the value is 10^38 or more.
ParsedValue parse_value(std::string_view text);

// Decimal representation, without leading zeros.
std::string to_string(Value value);

// a + b, or nothing when the sum does not fit in 128 bits.
std::optional<Value> checked_add(Value a, Value b);

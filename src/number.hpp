// Numbers: unsigned 128-bit integers (report, Section 1.4). An element is below 10^38,
// the sum of a set has to stay below 2^128.
#pragma once

#include <optional>
#include <string>
#include <string_view>

// The one place that names the 128-bit type. __extension__ keeps -Wpedantic quiet.
__extension__ using Value = unsigned __int128;

// First value that is too large for an element: 10^38.
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

// Digits only, leading zeros allowed. TooLarge from 10^38 up.
ParsedValue parse_value(std::string_view text);

std::string to_string(Value value);

// a + b, or nothing on overflow.
std::optional<Value> checked_add(Value a, Value b);

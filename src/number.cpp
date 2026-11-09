#include "number.hpp"

#include <algorithm>

ParsedValue parse_value(std::string_view text) {
    if (text.empty()) return {ParseStatus::Empty, 0};

    Value value = 0;
    for (char c : text) {
        if (c < '0' || c > '9') return {ParseStatus::NotDigit, 0};
        // value < 10^38 before this step, so 10 * value + 9 < 10^39 < 2^128.
        value = value * 10 + static_cast<unsigned>(c - '0');
        if (value >= kValueLimit) {
            // Keep scanning: a later non-digit is the more useful message.
            bool all_digits = std::all_of(text.begin(), text.end(),
                                          [](char d) { return d >= '0' && d <= '9'; });
            return {all_digits ? ParseStatus::TooLarge : ParseStatus::NotDigit, 0};
        }
    }
    return {ParseStatus::Ok, value};
}

std::string to_string(Value value) {
    if (value == 0) return "0";
    std::string digits;
    while (value > 0) {
        digits.push_back(static_cast<char>('0' + static_cast<int>(value % 10)));
        value /= 10;
    }
    std::reverse(digits.begin(), digits.end());
    return digits;
}

std::optional<Value> checked_add(Value a, Value b) {
    Value sum = a + b;  // unsigned arithmetic wraps around on overflow
    if (sum < a) return std::nullopt;
    return sum;
}

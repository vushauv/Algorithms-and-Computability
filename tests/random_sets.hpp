// Random multisets for comparing the algorithms with brute force. The seed is
// fixed, so a failure can be reproduced.
#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <random>
#include <vector>

#include "value.hpp"

namespace random_sets {

inline std::mt19937_64& engine() {
    static std::mt19937_64 generator(20261120);
    return generator;
}

inline std::size_t size_between(std::size_t low, std::size_t high) {
    return std::uniform_int_distribution<std::size_t>(low, high)(engine());
}

// n values in 0..max_value; small max_value gives many duplicates and zeros.
inline std::vector<Value> small_values(std::size_t n, std::uint64_t max_value) {
    std::uniform_int_distribution<std::uint64_t> value(0, max_value);
    std::vector<Value> a(n);
    for (Value& x : a) x = value(engine());
    return a;
}

// n values below 10^25, about the 25 digits of AaC_example_2.txt.
inline std::vector<Value> large_values(std::size_t n) {
    const Value limit = parse_value<Value>("10000000000000000000000000").value;  // 10^25
    std::vector<Value> a(n);
    for (Value& x : a) {
        const Value high = engine()();
        const Value low = engine()();
        x = ((high << 64) | low) % limit;
    }
    return a;
}

// Large values whose sum is even and that have a balanced partition: a random
// half is copied, with its sum, into the other half (YES by construction).
inline std::vector<Value> large_yes_instance(std::size_t n) {
    std::vector<Value> a = large_values(n);
    Value left = 0;
    for (std::size_t i = 0; i + 1 < n / 2; ++i) left += a[i];
    Value right = 0;
    for (std::size_t i = n / 2; i < n; ++i) right += a[i];
    // Choose a[n/2 − 1] so that the first half has the sum of the second one.
    if (left <= right) {
        a[n / 2 - 1] = right - left;
    } else {
        a[n - 1] += left - right;
        a[n / 2 - 1] = 0;
    }
    std::shuffle(a.begin(), a.end(), engine());
    return a;
}

}  // namespace random_sets

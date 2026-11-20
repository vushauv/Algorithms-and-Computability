// Our own long arithmetic: a non-negative integer of any size, stored as cells
// in a base that is chosen at run time (--base). With the base 2^32 a cell is
// a machine word, with 10 a decimal digit, with 2 a bit;
// Only what the algorithms need is here: addition,
// subtraction, comparison, parity, halving, and the conversions.
#pragma once

#include <compare>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <vector>

class BigUint {
public:
    using Cell = std::uint32_t;

    static constexpr std::uint64_t kMinBase = 2;
    static constexpr std::uint64_t kMaxBase = std::uint64_t{1} << 32;  // a cell is below the base

    // The base of every BigUint. Set it before the first value is made.
    static void set_base(std::uint64_t base);
    static std::uint64_t base() { return base_; }

    BigUint() = default;          // zero
    BigUint(std::uint64_t value);  // implicit, so that x == 0 and x = 5 work

    BigUint& operator+=(const BigUint& other);
    
    BigUint& operator-=(const BigUint& other);
    friend BigUint operator+(BigUint a, const BigUint& b) { return a += b; }
    friend BigUint operator-(BigUint a, const BigUint& b) { return a -= b; }

    friend bool operator==(const BigUint& a, const BigUint& b) = default;
    friend std::strong_ordering operator<=>(const BigUint& a, const BigUint& b);

    bool is_zero() const { return cells_.empty(); }
    bool is_odd() const;

    // this ← this · factor + addend. Needs addend < factor.
    void multiply_add(Cell factor, Cell addend);
    // this ← ⌊this / divisor⌋, returns the remainder. Needs divisor > 0.
    Cell divide(Cell divisor);

    // The value, or nothing if it is 2^64 or more.
    std::optional<std::uint64_t> to_u64() const;

    std::size_t cell_count() const { return cells_.size(); }

private:
    void trim();  // drops the zero cells at the top

    static inline std::uint64_t base_ = kMaxBase;

    // Least significant cell first, every cell below the base, no zero cell at
    // the top; zero is the empty vector.
    std::vector<Cell> cells_;
};

#include "big_uint.hpp"

#include <limits>
#include <stdexcept>
#include <string>

void BigUint::set_base(std::uint64_t base) {
    if (base < kMinBase || base > kMaxBase) {
        throw std::invalid_argument("the base must be between " + std::to_string(kMinBase) +
                                    " and " + std::to_string(kMaxBase));
    }
    base_ = base;
}

BigUint::BigUint(std::uint64_t value) {
    for (; value > 0; value /= base_) cells_.push_back(static_cast<Cell>(value % base_));
}

void BigUint::trim() {
    while (!cells_.empty() && cells_.back() == 0) cells_.pop_back();
}

BigUint& BigUint::operator+=(const BigUint& other) {
    if (cells_.size() < other.cells_.size()) cells_.resize(other.cells_.size(), 0);
    std::uint64_t carry = 0;
    for (std::size_t i = 0; i < cells_.size(); ++i) {
        if (i >= other.cells_.size() && carry == 0) break;  // the rest is unchanged
        // Two cells and a carry: below 2 * base <= 2^33.
        std::uint64_t sum = cells_[i] + carry + (i < other.cells_.size() ? other.cells_[i] : 0);
        carry = sum >= base_ ? 1 : 0;
        if (carry) sum -= base_;
        cells_[i] = static_cast<Cell>(sum);
    }
    if (carry) cells_.push_back(1);
    return *this;
}

BigUint& BigUint::operator-=(const BigUint& other) {
    std::uint64_t borrow = 0;
    for (std::size_t i = 0; i < cells_.size(); ++i) {
        if (i >= other.cells_.size() && borrow == 0) break;
        const std::uint64_t take = borrow + (i < other.cells_.size() ? other.cells_[i] : 0);
        std::uint64_t cell = cells_[i];
        borrow = cell < take ? 1 : 0;
        if (borrow) cell += base_;
        cells_[i] = static_cast<Cell>(cell - take);
    }
    trim();
    return *this;
}

std::strong_ordering operator<=>(const BigUint& a, const BigUint& b) {
    // No zero cell at the top, so the longer number is the larger one.
    if (a.cells_.size() != b.cells_.size()) return a.cells_.size() <=> b.cells_.size();
    for (std::size_t i = a.cells_.size(); i-- > 0;) {
        if (a.cells_[i] != b.cells_[i]) return a.cells_[i] <=> b.cells_[i];
    }
    return std::strong_ordering::equal;
}

bool BigUint::is_odd() const {
    if (cells_.empty()) return false;
    if (base_ % 2 == 0) return cells_[0] % 2 == 1;
    // An odd base: every power of it is odd, so the parity is that of the
    // sum of the cells.
    Cell parity = 0;
    for (Cell cell : cells_) parity ^= cell & 1;
    return parity == 1;
}

void BigUint::multiply_add(Cell factor, Cell addend) {
    // cell * factor + carry < base * factor <= 2^64, and the new carry stays
    // below the factor.
    std::uint64_t carry = addend;
    for (Cell& cell : cells_) {
        const std::uint64_t total = std::uint64_t{cell} * factor + carry;
        cell = static_cast<Cell>(total % base_);
        carry = total / base_;
    }
    for (; carry > 0; carry /= base_) cells_.push_back(static_cast<Cell>(carry % base_));
    trim();  // a zero factor
}

BigUint::Cell BigUint::divide(Cell divisor) {
    std::uint64_t remainder = 0;
    for (std::size_t i = cells_.size(); i-- > 0;) {
        const std::uint64_t current = remainder * base_ + cells_[i];
        cells_[i] = static_cast<Cell>(current / divisor);
        remainder = current % divisor;
    }
    trim();
    return static_cast<Cell>(remainder);
}

std::optional<std::uint64_t> BigUint::to_u64() const {
    __extension__ using Wide = unsigned __int128;
    Wide value = 0;
    for (std::size_t i = cells_.size(); i-- > 0;) {
        value = value * base_ + cells_[i];  // below 2^64 * 2^32 + 2^32
        if (value > std::numeric_limits<std::uint64_t>::max()) return std::nullopt;
    }
    return static_cast<std::uint64_t>(value);
}

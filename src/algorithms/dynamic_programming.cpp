#include "algorithms/dynamic_programming.hpp"

#include <algorithm>
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace {

using Word = std::uint64_t;
constexpr std::size_t kWordBits = 64;

bool test_bit(const std::vector<Word>& bits, std::size_t s) {
    return (bits[s / kWordBits] >> (s % kWordBits)) & 1;
}

// reach ← reach OR (reach << shift), on the bits 0..top (Section 4.2). The
// words are updated from the highest to the lowest, so every word that is
// read still holds the previous row. Requires 0 < shift ≤ top.
void shift_or(std::vector<Word>& reach, std::size_t shift, std::size_t top) {
    const std::size_t word_shift = shift / kWordBits;
    const std::size_t bit_shift = shift % kWordBits;
    for (std::size_t w = top / kWordBits + 1; w-- > word_shift;) {
        const std::size_t source = w - word_shift;
        Word shifted = reach[source] << bit_shift;
        if (bit_shift != 0 && source > 0) {
            shifted |= reach[source - 1] >> (kWordBits - bit_shift);
        }
        reach[w] |= shifted;
    }
    // Truncate at top; the bits above it are FALSE in both rows (Lemma 4.1).
    const std::size_t last_bit = top % kWordBits;
    if (last_bit != kWordBits - 1) {
        reach[top / kWordBits] &= (Word{1} << (last_bit + 1)) - 1;
    }
}

}  // namespace

Verdict dp_partition(std::span<const Value> a) {
    const Preliminary pre = preliminary_checks(a);
    if (pre.verdict) return *pre.verdict;
    if (*std::max_element(a.begin(), a.end()) > pre.T) return Verdict::No;  // Section 4.1
    if (pre.T > kMaxDpTarget) {
        throw std::invalid_argument("the target is too large for dynamic programming");
    }

    // Every a[i] ≤ T now, so it fits in std::size_t as well.
    const auto T = static_cast<std::size_t>(pre.T);
    std::vector<Word> reach(T / kWordBits + 1, 0);
    reach[0] = 1;  // the empty sum
    std::size_t top = 0;  // upper bound on the obtainable sums
    for (Value value : a) {
        const auto ai = static_cast<std::size_t>(value);
        top = std::min(T, top + ai);  // top + ai ≤ 2T, no overflow
        if (ai > 0) shift_or(reach, ai, top);  // a zero changes nothing
        if (test_bit(reach, T)) return Verdict::Yes;
    }
    return Verdict::No;
}

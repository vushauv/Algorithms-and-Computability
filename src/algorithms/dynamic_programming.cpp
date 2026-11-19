#include "algorithms/dynamic_programming.hpp"

#include <algorithm>
#include <bit>
#include <stdexcept>
#include <utility>
#include <vector>

#include "memory.hpp"

namespace {

using Word = std::uint64_t;
constexpr std::size_t kWordBits = 64;

// first[s] is the step (i + 1) where a[i] made the sum s reachable, 0 if s is not.
using Step = std::uint32_t;

bool test_bit(const std::vector<Word>& bits, std::size_t s) {
    return (bits[s / kWordBits] >> (s % kWordBits)) & 1;
}

// reach |= reach << shift on the bits 0..top (Section 4.2), and first[s] = step for every
// bit that becomes set. Words go from the highest to the lowest, so every word we read
// is still from the previous row. Needs 0 < shift ≤ top.
void shift_or(std::vector<Word>& reach, std::vector<Step>& first, std::size_t shift,
              std::size_t top, Step step) {
    const std::size_t word_shift = shift / kWordBits;
    const std::size_t bit_shift = shift % kWordBits;
    const std::size_t top_word = top / kWordBits;
    const std::size_t last_bit = top % kWordBits;
    const Word top_mask = last_bit == kWordBits - 1 ? ~Word{0} : (Word{1} << (last_bit + 1)) - 1;
    for (std::size_t w = top_word + 1; w-- > word_shift;) {
        const std::size_t source = w - word_shift;
        Word shifted = reach[source] << bit_shift;
        if (bit_shift != 0 && source > 0) {
            shifted |= reach[source - 1] >> (kWordBits - bit_shift);
        }
        // Cut at top; the bits above it are false in both rows (Lemma 4.1).
        if (w == top_word) shifted &= top_mask;
        for (Word fresh = shifted & ~reach[w]; fresh != 0; fresh &= fresh - 1) {
            first[w * kWordBits + static_cast<std::size_t>(std::countr_zero(fresh))] = step;
        }
        reach[w] |= shifted;
    }
}

// The subset with the sum T, read back from first. If first[s] = i + 1, the
// sum s − a[i] was reachable before step i + 1 and a[i] > 0 (a zero sets no
// bit), so first[s − a[i]] < i + 1: the indices strictly decrease and are
// distinct, and the sums strictly decrease to 0.
std::vector<std::size_t> read_back(std::span<const std::size_t> a, const std::vector<Step>& first,
                                   std::size_t T) {
    std::vector<std::size_t> A;
    for (std::size_t s = T; s > 0;) {
        const std::size_t i = first[s] - 1;
        A.push_back(i);
        s -= a[i];
    }
    return A;
}

}  // namespace

template <class V>
ExactAnswer<V> dp_partition(std::span<const V> a) {
    const std::size_t n = a.size();
    if (n > kMaxDpElements) {
        throw std::invalid_argument("dynamic programming needs fewer than 2^32 - 1 elements");
    }
    const Preliminary<V> pre = preliminary_checks(a);
    if (pre.verdict) return trivial_answer(pre, n);
    if (*std::max_element(a.begin(), a.end()) > pre.T) return std::nullopt;  // Section 4.1
    const std::optional<std::uint64_t> target = to_u64(pre.T);
    if (!target || *target > kMaxDpTarget) {
        throw std::invalid_argument("the target is too large for dynamic programming");
    }

    // Every a[i] ≤ T now, so it fits in std::size_t as well; the long numbers
    // are converted once, the table works on machine words.
    const auto T = static_cast<std::size_t>(*target);
    std::vector<std::size_t> values(n);
    for (std::size_t i = 0; i < n; ++i) values[i] = static_cast<std::size_t>(*to_u64(a[i]));
    require_memory("dynamic programming",
                   Native{T / kWordBits + 1} * sizeof(Word) + (Native{T} + 1) * sizeof(Step));
    std::vector<Word> reach(T / kWordBits + 1, 0);
    std::vector<Step> first(T + 1, 0);
    reach[0] = 1;
    std::size_t top = 0;  // no reachable sum is above this
    for (std::size_t i = 0; i < n; ++i) {
        const std::size_t ai = values[i];
        top = std::min(T, top + ai);  // top + ai ≤ 2T, no overflow
        if (ai > 0) shift_or(reach, first, ai, top, static_cast<Step>(i + 1));  // a zero changes nothing
        if (test_bit(reach, T)) return balanced_partition<V>(n, read_back(values, first, T));
    }
    return std::nullopt;
}

#define INSTANTIATE(V) template ExactAnswer<V> dp_partition(std::span<const V>);
FOR_EACH_NUMBER_TYPE(INSTANTIATE)
#undef INSTANTIATE

### 1.4 Computational model and number representation

All numbers are **unsigned 128-bit integers**. This is enough for the numbers of interest: $2^{128} - 1 \approx 3.4 \cdot 10^{38}$ has $39$ digits, and the elements are non-negative, so no sign bit is wasted. Consequences used throughout the analysis:

- Comparison, addition and subtraction cost $O(1)$.
- Input constraints (Section 1.1): each $0 \le a_i < 10^{38}$, and the total $\Sigma(M) < 2^{128}$. Both are checked on input. Every intermediate value in every algorithm below is a subset sum or a difference of two subset sums, hence bounded by $\Sigma(M)$, so no other overflow can occur.
- Some algorithms need less than this. Karmarkar-Karp (Section 5) handles only differences, each at most $\max_i a_i$, and dynamic programming (Section 4) handles sums up to $\Sigma(M)/2 \le T_{DP}$, so neither can overflow.
- The range of $n$ in which each exact algorithm is run is given in the summary table (Section 7).
- Therefore every complexity bound below is stated in the **unit-cost RAM model**: an arithmetic operation on a number costs $O(1)$, and the bounds are expressed in $n$ and, for dynamic programming, in $\Sigma(M)$ only.

Memory is counted in machine words of 128 bits.

---


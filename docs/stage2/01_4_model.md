### 1.4 Computational model and number representation

**Representation.** All numbers, that is the elements, their sums and their differences, are unsigned 128-bit integers. The largest such integer is $2^{128} - 1 \approx 3.4 \cdot 10^{38}$, which has $39$ decimal digits. The elements are non-negative, so no sign bit is needed.

**No overflow.** Every value computed by an algorithm below is a subset sum or the non-negative difference of two subset sums, hence it lies between $0$ and $\Sigma(M)$. Together with the assumption $\Sigma(M) < 2^{128}$ of Section 1.1 this means that no overflow occurs. Two algorithms need even less: Karmarkar-Karp (Section 5) computes only differences, each at most $\max_i a_i$, and dynamic programming (Section 4) computes only sums up to $\Sigma(M)/2 \le T_{DP}$, where $T_{DP}$ is defined below.

**Cost model.** We use the unit-cost RAM model: an arithmetic operation or a comparison on a 128-bit number costs $O(1)$, and so does an operation on a machine word of $w = 64$ bits, which dynamic programming uses for its bitset (Section 4). Time bounds count such operations and are expressed in $n$ and, for dynamic programming, in $\Sigma(M)$. Memory is counted in 128-bit numbers, except for the bitset of Section 4, which is counted in bits.

**Ranges of the algorithms.** Each algorithm is applied only within the following range. Outside it the algorithm may not finish or may not fit in memory (Section 1.1).

| Algorithm | Range | Reason |
| --- | --- | --- |
| Brute force (Section 2) | $n \le 30$ | time $O(2^n)$ |
| Meet in the middle (Section 3) | $n \le N_{MITM} = 44$ | memory $O(2^{n/2})$, about $100$ MB per half at the limit |
| Dynamic programming (Section 4) | $\Sigma(M)/2 \le T_{DP} = 10^{10}$ | memory $O(\Sigma(M))$ bits, $1.25$ GB at the limit |
| Karmarkar-Karp (Section 5) | $n \le 10^6$ | limit on the size of the input |

The parameters $N_{MITM}$ and $T_{DP}$ are used by the final algorithm (Section 6). Their values can be changed.

---

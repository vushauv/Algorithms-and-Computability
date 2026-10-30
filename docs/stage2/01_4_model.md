### 1.4 Computational model and number representation

**Representation.** All numbers, that is the elements, their sums and their differences, are unsigned 128-bit integers. The largest such integer is $2^{128} - 1 \approx 3.4 \cdot 10^{38}$, which has $39$ decimal digits. The elements are non-negative, so no sign bit is needed.

**No overflow.** Every value computed by an algorithm below is a subset sum or the non-negative difference of two subset sums, hence it lies between $0$ and $\Sigma(M)$. Together with the assumption $\Sigma(M) < 2^{128}$ of Section 1.1 this means that no overflow occurs. Two algorithms need even less: Karmarkar-Karp (Section 5) computes only differences, each at most $\max_i a_i$, and dynamic programming (Section 4) computes only sums up to $\Sigma(M)$, since its target $\Sigma(M)/2$ is at most $T_{DP}$, where $T_{DP}$ is defined below.

**Cost model.** We use the unit-cost RAM model: an arithmetic operation or a comparison on a 128-bit number costs $O(1)$, and so does an operation on a machine word of $w = 64$ bits, which dynamic programming uses for its bitset (Section 4). Time bounds count such operations and are expressed in $n$ and, for dynamic programming, in $\Sigma(M)$. Memory is counted in 128-bit numbers, except for the bitset of Section 4, which is counted in bits.

**Ranges of the algorithms.** Each exponential algorithm is applied only within a range of inputs, because outside it the algorithm may not finish or may not fit in memory (Section 1.1). The ranges are parameters:

| Algorithm | Range | Limiting resource |
| --- | --- | --- |
| Meet in the middle (Section 3) | $n \le N_{MITM}$ | memory $O(2^{n/2})$ |
| Dynamic programming (Section 4) | $\Sigma(M)/2 \le T_{DP}$ | memory $O(\Sigma(M))$ bits and time $O(n \cdot \Sigma(M))$ |

Brute force (Section 2) is the baseline for the other two and is not used by the final algorithm, so it has no range. The values of $N_{MITM}$ and $T_{DP}$ are not fixed in this document. They depend on the implementation and on the machine, so they are determined experimentally after the algorithms have been implemented. The analysis of each algorithm holds for any values of the parameters. The final algorithm (Section 6) takes them as input.

---

## 6. The final algorithm

None of the algorithms of Sections 2 to 5 is satisfactory on its own. Dynamic programming is cheap when the numbers are small, however many there are, and meet in the middle is cheap when there are few numbers, however large. When both fail, only the approximation of Section 5 remains, and it cannot decide PARTITION. This section combines the algorithms into one that adapts to the input: it always computes the Karmarkar-Karp partition, which costs only $O(n \log n)$, and it adds an exact algorithm whenever one is in range for the input. The exact algorithms decide the problem and do not construct a partition (Section 1.1), so the result consists of a decision and the partition of Section 5, and the decision also tells how good that partition is.

### 6.1 Idea

#### The result

The result of the final algorithm is a triple (answer, $(A, B)$, $d$), where

- $(A, B)$ is the partition produced by Karmarkar-Karp differencing and $d$ is its discrepancy (Theorem 5.2), and
- the answer is YES or NO, if the decision problem has been solved, and UNKNOWN otherwise.

#### The steps

1. If $d = 0$, the partition $(A, B)$ is balanced, and the answer is YES. This also covers $\Sigma(M) = 0$, because $d \le \max_i a_i = 0$ by Proposition 5.3.
2. If $\Sigma(M)$ is odd, the answer is NO (Section 1.2).
3. Otherwise $d > 0$, $\Sigma(M)$ is even and positive, and the question is open. An exact algorithm is applied if one of them is in range, that is, dynamic programming if $T \le T_{DP}$ and meet in the middle if $n \le N_{MITM}$ (Section 1.4). If both are in range, the choice depends on the input: by Section 4.7, dynamic programming is the cheaper one for many small numbers and meet in the middle for few large ones. The rule that decides between them is fixed together with the parameters $N_{MITM}$ and $T_{DP}$, after the algorithms have been implemented and measured. Both algorithms give the same answer (Theorems 3.4 and 4.2), so the choice affects the cost and not the result, which becomes the answer of the final algorithm.
4. If neither exact algorithm is in range, the answer is UNKNOWN.

#### Comparison of the approximation with the exact answer

When an exact algorithm has run, its answer measures the approximation on the same input. If the answer is YES and $d = 0$, the differencing method has found a balanced partition. If the answer is YES and $d > 0$, a balanced partition exists but the method has missed it, as in the second example of Section 5.3. If the answer is NO, no balanced partition exists, and $d$ is the discrepancy of a partition that cannot be improved to $0$; whether it is the smallest possible discrepancy is not decided. The final algorithm thus delivers, for every input within the ranges, the exact decision together with the quality of the approximation.

#### Brute force

Brute force is not used. Its time is $\Theta(2^n)$, whereas the time of meet in the middle is $O(n \cdot 2^{n/2})$, which is smaller already for $n \ge 5$. The only advantage of brute force is its constant memory, and this does not matter for the few tens of elements it can handle, where the lists of meet in the middle are small (Section 3.6). It remains the reference against which the other algorithms are tested.

### 6.2 Algorithm

The input is a multiset $a_1, \dots, a_n$ with $n \ge 2$ (Section 1.1). The parameters $N_{MITM}$ and $T_{DP}$ are those of Section 1.4.

```
SOLVE(a[1..n]; N_MITM, T_DP)
    S ← Σ a[i]
    (A, B, d) ← KARMARKAR-KARP(a)                   // O(n log n), always
    if d = 0 then return (YES, A, B, d)             // balanced; includes S = 0
    if S is odd then return (NO, A, B, d)           // Section 1.2
    T ← S / 2
    if T ≤ T_DP and n ≤ N_MITM then
        EXACT ← DP-PARTITION or MEET-IN-THE-MIDDLE, whichever is cheaper for (n, T)   // Section 4.7
    else if T ≤ T_DP then EXACT ← DP-PARTITION
    else if n ≤ N_MITM then EXACT ← MEET-IN-THE-MIDDLE
    else return (UNKNOWN, A, B, d)
    return (EXACT(a), A, B, d)
```

When only one exact algorithm is in range, it is run. When both are in range, the one with the smaller cost for the given $n$ and $T$ is run, and the two cost estimates of Section 4.7 are the starting point for this rule.

### 6.3 Examples

| input | Karmarkar-Karp | decision | result |
| --- | --- | --- | --- |
| $(3, 1, 4, 2)$ | $d = 0$ | step 1 | YES with the balanced partition $\lbrace 3, 2 \rbrace$, $\lbrace 1, 4 \rbrace$ |
| $(2, 3, 4)$ | $d = 1$ | $S = 9$ is odd | NO, with the partition of discrepancy $1$ |
| $(5, 6, 7, 8, 10)$ | $d = 2$ | $S = 36$, $T = 18$, both exact algorithms are in range and answer YES | YES, but the partition returned has $d = 2$ |

The third row shows what the final algorithm does and does not give. A balanced partition exists, namely $\lbrace 8, 10 \rbrace$ and $\lbrace 5, 6, 7 \rbrace$, and the exact algorithm detects it, but it does not construct it, so the partition returned is the one of Karmarkar-Karp, and the comparison of Section 6.1 records that the approximation has missed the optimum. The answer UNKNOWN occurs only for inputs with an even sum, $d > 0$, $T > T_{DP}$ and $n > N_{MITM}$, for example for $1000$ numbers of $25$ digits.

### 6.4 Correctness

**Theorem 6.1.** Let (answer, $(A, B)$, $d$) be the result of `SOLVE`.

1. $(A, B)$ is a partition of $\lbrace 1, \dots, n \rbrace$ into two non-empty parts, and $d = |\Sigma(A) - \Sigma(B)|$.
2. If the answer is YES, a balanced partition exists. If the answer is NO, none exists.
3. If $d = 0$, the answer is YES.
4. The answer is UNKNOWN only if $\Sigma(M)$ is even, $d > 0$, $T > T_{DP}$ and $n > N_{MITM}$.

*Proof.* Part 1 is Theorem 5.2, and part 3 is the first test of the algorithm. For parts 2 and 4 consider the possible returns.

- *Return in the first test.* Since $d = 0$, $(A, B)$ is a balanced partition, so YES is correct.
- *Return in the second test.* If $S$ is odd, a balanced partition would give $S = 2\Sigma(A)$, which is impossible, so NO is correct (Section 1.2).
- *The remaining case.* Now $S$ is even and $d > 0$. Then $S > 0$, since $S = 0$ gives $d = 0$ as above, so $T > 0$. If `DP-PARTITION` is run, then $T \le T_{DP}$ and its answer is correct by Theorem 4.2. If `MEET-IN-THE-MIDDLE` is run, then $n \le N_{MITM}$ and its answer is correct by Theorem 3.4. Otherwise $T > T_{DP}$ and $n > N_{MITM}$, and the answer is UNKNOWN, as stated in part 4. $\square$

If the answer is YES and $d > 0$, the decision is correct but the returned partition is not balanced. By Proposition 5.3, its discrepancy is at most $\max_i a_i$ in any case. If the answer is NO or UNKNOWN, the discrepancy $d$ is the quality of an approximation, and we do not claim that it is minimal.

The cost of `SOLVE` is the cost of Karmarkar-Karp (Theorem 5.4) plus the cost of the exact algorithm that is run, if any (Theorems 3.5 and 4.3). The algorithm does not contradict the NP-completeness of PARTITION (Section 1.3): it solves only the inputs within the ranges of the exact algorithms and returns UNKNOWN otherwise.

---

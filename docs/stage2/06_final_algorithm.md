## 6. The final algorithm

None of the algorithms of Sections 2 to 5 is satisfactory on its own. Dynamic programming is cheap when the numbers are small, however many there are, and meet in the middle is cheap when there are few numbers, however large. When both fail, only the approximation of Section 5 remains, and it cannot decide PARTITION. This section combines the algorithms into one that adapts to the input: it always computes the Karmarkar-Karp partition, which costs only $O(n \log n)$, and it adds the cheapest exact algorithm that is within its range. The exact algorithms decide the problem and do not construct a partition (Section 1.1), so the result consists of a decision and the partition of Section 5.

### 6.1 Idea

The result of the final algorithm is a triple (answer, $(A, B)$, $d$), where

- $(A, B)$ is the partition produced by Karmarkar-Karp differencing and $d$ is its discrepancy (Theorem 5.2), and
- the answer is YES or NO, if the decision problem has been solved, and UNKNOWN otherwise.

The algorithm proceeds as follows.

1. If $d = 0$, the partition $(A, B)$ is balanced, and the answer is YES. This also covers $\Sigma(M) = 0$, because $d \le \max_i a_i = 0$ by Proposition 5.3.
2. If $\Sigma(M)$ is odd, the answer is NO (Section 1.2).
3. Otherwise $d > 0$, $\Sigma(M)$ is even and positive, and the question is open. An exact algorithm is applied if one of them is in range, that is, dynamic programming if $T \le T_{DP}$ and meet in the middle if $n \le N_{MITM}$ (Section 1.4). If both are in range, we take the one with the smaller cost estimate according to Proposition 4.4: dynamic programming if $T/64 < 2^{n/2}$, and meet in the middle otherwise. Their answer is the answer of the final algorithm.
4. If neither exact algorithm is in range, the answer is UNKNOWN.

Brute force is not used. Its time is $\Theta(2^n)$, whereas the time of meet in the middle is $O(n \cdot 2^{n/2})$, which is smaller already for $n \ge 5$. The only advantage of brute force is its constant memory, and this does not matter for $n \le N_{BF}$, where the lists of meet in the middle are small (Section 3.6).

### 6.2 Algorithm

The input is a multiset $a_1, \dots, a_n$ with $n \ge 2$ (Section 1.1). The parameters $N_{MITM}$ and $T_{DP}$ are those of Section 1.4.

```
SOLVE(a[1..n]; N_MITM, T_DP)
    S ← Σ a[i]
    (A, B, d) ← KARMARKAR-KARP(a)                   // O(n log n), always
    if d = 0 then return (YES, A, B, d)             // balanced; includes S = 0
    if S is odd then return (NO, A, B, d)           // Section 1.2
    T ← S / 2
    useDP ← (T ≤ T_DP) and (n > N_MITM or T^2 < 2^(n+12))   // T/64 < 2^(n/2), Proposition 4.4
    if useDP then
        answer ← DP-PARTITION(a)
    else if n ≤ N_MITM then
        answer ← MEET-IN-THE-MIDDLE(a)
    else
        answer ← UNKNOWN
    return (answer, A, B, d)
```

The test $T/64 < 2^{n/2}$ is written as $T^2 < 2^{n+12}$ to stay in integer arithmetic. It is evaluated only when $T \le T_{DP}$ and $n \le N_{MITM}$, so both sides are small numbers and the evaluation does not overflow. When $n > N_{MITM}$ and $T \le T_{DP}$, only dynamic programming is in range.

### 6.3 Examples

| input | Karmarkar-Karp | decision | result |
| --- | --- | --- | --- |
| $(3, 1, 4, 2)$ | $d = 0$ | step 1 | YES with the balanced partition $\lbrace 3, 2 \rbrace$, $\lbrace 1, 4 \rbrace$ |
| $(2, 3, 4)$ | $d = 1$ | $S = 9$ is odd | NO, with the partition of discrepancy $1$ |
| $(5, 6, 7, 8, 10)$ | $d = 2$ | $S = 36$, $T = 18$, $T^2 = 324 < 2^{17}$, so dynamic programming is used and answers YES | YES, but the partition returned has $d = 2$ |

The third row shows what the final algorithm does and does not give. A balanced partition exists, namely $\lbrace 8, 10 \rbrace$ and $\lbrace 5, 6, 7 \rbrace$, and the exact algorithm detects it, but it does not construct it, so the partition returned is the one of Karmarkar-Karp. The answer UNKNOWN occurs only for inputs with an even sum, $d > 0$, $T > T_{DP}$ and $n > N_{MITM}$, for example for $1000$ numbers of $25$ digits.

### 6.4 Correctness

**Theorem 6.1.** Let (answer, $(A, B)$, $d$) be the result of `SOLVE`.

1. $(A, B)$ is a partition of $\lbrace 1, \dots, n \rbrace$ into two non-empty parts, and $d = |\Sigma(A) - \Sigma(B)|$.
2. If the answer is YES, a balanced partition exists. If the answer is NO, none exists.
3. If $d = 0$, the answer is YES.
4. The answer is UNKNOWN only if $\Sigma(M)$ is even, $d > 0$, $T > T_{DP}$ and $n > N_{MITM}$.

*Proof.* Part 1 is Theorem 5.2, and part 3 is the first test of the algorithm. For parts 2 and 4 consider the possible returns.

- *Return in the first test.* Since $d = 0$, $(A, B)$ is a balanced partition, so YES is correct.
- *Return in the second test.* If $S$ is odd, a balanced partition would give $S = 2\Sigma(A)$, which is impossible, so NO is correct (Section 1.2).
- *The remaining case.* Now $S$ is even and $d > 0$. Then $S > 0$, since $S = 0$ gives $d = 0$ as above, so $T > 0$. If `useDP` holds, then $T \le T_{DP}$ and the answer of `DP-PARTITION` is correct by Theorem 4.2. If `useDP` does not hold and $n \le N_{MITM}$, the answer of `MEET-IN-THE-MIDDLE` is correct by Theorem 3.4. Otherwise `useDP` does not hold and $n > N_{MITM}$. Since `useDP` is false and $n > N_{MITM}$, the test gives $T > T_{DP}$. The answer is UNKNOWN, as stated in part 4. $\square$

If the answer is YES and $d > 0$, the decision is correct but the returned partition is not balanced. By Proposition 5.3, its discrepancy is at most $\max_i a_i$ in any case. If the answer is NO or UNKNOWN, the discrepancy $d$ is the quality of an approximation, and we do not claim that it is minimal.

### 6.5 Complexity

**Theorem 6.2.** `SOLVE` takes time $O(n \log n)$ if the answer is found by Karmarkar-Karp, by the parity test, or if neither exact algorithm is in range. Otherwise it takes time $O(n \log n)$ plus the time of the exact algorithm that is run. Hence the time is always

$$
O\left(n \log n + n \cdot \min\left(\frac{T_{DP}}{64}, 2^{N_{MITM}/2}\right)\right),
$$

if both algorithms are in range for the input, and otherwise it is bounded by the corresponding term alone. The memory is $O(n)$ numbers, or $O(T_{DP})$ bits for dynamic programming, or $O(2^{N_{MITM}/2})$ numbers for meet in the middle, according to the branch taken.

*Proof.* The sum, the parity test and the test for `useDP` take $O(n)$, and Karmarkar-Karp takes $O(n \log n)$ and $O(n)$ memory (Theorem 5.4). Dynamic programming takes time $O(n + n \cdot T/w)$ with $w = 64$ and $T + 1$ bits (Theorem 4.3), and with $T \le T_{DP}$ this is at most $O(n \cdot T_{DP}/64)$. Meet in the middle takes time $O(n \cdot 2^{n/2})$ and $O(2^{n/2})$ numbers (Theorem 3.5), at most $O(n \cdot 2^{N_{MITM}/2})$ for $n \le N_{MITM}$. If both are in range, the algorithm takes the one with the smaller estimate $n \cdot T/64$ or $n \cdot 2^{n/2}$ (Proposition 4.4). $\square$

For fixed parameters the bound of the theorem is a constant multiple of $n$, plus $O(n \log n)$. It does not contradict the NP-completeness of PARTITION (Section 1.3), because the algorithm solves only the inputs within the ranges of the exact algorithms and returns UNKNOWN otherwise. For no choice of the parameters is a polynomial bound claimed on the whole input set.

---

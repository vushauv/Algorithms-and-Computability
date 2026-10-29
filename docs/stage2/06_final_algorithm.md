## 6. Algorithm E: Final algorithm (input-dependent selection)

**Role.** This is the algorithm the program actually runs. Algorithms A-D are its building blocks. It looks at the size $n$ and the total sum $S$ of the input, runs the cheapest exact method that is feasible under the limits of Section 1.4, and falls back to the Karmarkar-Karp approximation when no exact method is feasible. It always returns a partition and its discrepancy, and it says whether the answer is exact.

### 6.1 Idea

The exact methods have complementary ranges: dynamic programming is cheap when the numbers are small regardless of $n$, meet in the middle is cheap when $n$ is small regardless of the numbers, and brute force is only a reference. Karmarkar-Karp always runs: it is $O(n \log n)$, it gives the answer immediately when its discrepancy is 0, and it is the result we report when the exact methods are out of range.

### 6.2 Pseudocode

```
SOLVE(a[1..n], limits N_MITM = 44, T_DP = 10^10)          // requires n ≥ 2, checked when the input is parsed
    S ← Σ a[i]                                   // with overflow check
    if S is odd then
        (A, B, d) ← KARMARKAR-KARP(a)
        return (A, B, d, exact = TRUE)           // no balanced partition exists, d > 0 is a certificate-free answer
    if S = 0 then return ({1}, {2, ..., n}, 0, exact = TRUE)     // trivial case, Section 1.2
    T ← S / 2
    (A, B, d) ← KARMARKAR-KARP(a)                // O(n log n), always
    if d = 0 then return (A, B, 0, exact = TRUE)
    if T ≤ T_DP then
        if DP-PARTITION(a) = YES then return (A', B', 0, exact = TRUE)   // A', B' reconstructed from the table
        else return (A, B, d, exact = TRUE)      // DP proved no balanced partition; KK result is reported
    if n ≤ N_MITM then
        if MEET-IN-THE-MIDDLE(a) = YES then return (A', B', 0, exact = TRUE)
        else return (A, B, d, exact = TRUE)
    return (A, B, d, exact = FALSE)              // out of range for exact methods: approximation only
```

The thresholds `N_MITM` and `T_DP` are the limits of Section 1.4 and are adjustable parameters of the algorithm.

### 6.3 Correctness

- When `exact = TRUE` and $d = 0$, the returned partition is balanced (verified by the exact algorithm or by Karmarkar-Karp's own discrepancy, which equals the discrepancy of its partition, Section 5.3).
- When `exact = TRUE` and $d > 0$, either $S$ is odd or an exact algorithm answered NO, so no balanced partition exists and the returned partition is the Karmarkar-Karp one. We report its discrepancy but make no claim that it is minimal; the decision answer is exact, the discrepancy is a heuristic.
- When `exact = FALSE`, the input is beyond both exact methods; the answer is the Karmarkar-Karp partition and the program states that it is an approximation.

Note for the report: the odd-sum case and the "exact algorithm says NO" case both answer the **decision** problem exactly. Minimising the discrepancy exactly when no balanced partition exists is a different, harder task (the optimisation version) and is out of scope; meet in the middle could be extended to it with a closest-pair sweep, worth one sentence.

### 6.4 Complexity

- Odd $S$ or Karmarkar-Karp succeeds: $O(n \log n)$.
- Dynamic programming branch: $O(n \log n + n \cdot T)$ with $T \le T_{DP}$.
- Meet-in-the-middle branch: $O(n \log n + n \cdot 2^{n/2})$ with $n \le N_{MITM}$.
- Out of range: $O(n \log n)$.
- Memory: the maximum of the branches taken, $O(T)$ bits or $O(2^{n/2})$ words, otherwise $O(n)$.

The worst case is still exponential, which is unavoidable unless P = NP (Section 1.3); the selection only guarantees that the exponential branch is entered when it is known to finish within the limits.

---


## 9. Notes for Stages 3 and 4 (not part of the Stage 2 PDF)

- Number type: `using Num = unsigned __int128;` in one header, with `parse(string) -> Num` (reject $>38$ digits, reject non-digits, allow leading zeros) and `to_string(Num)`. Overflow check on summation via `__builtin_add_overflow`. Differences in Karmarkar-Karp are always $x - y$ with $x \ge y$, so unsigned is safe there; the discrepancy $|\Sigma(A)-\Sigma(B)|$ is computed as max minus min.
- Masks in Algorithm B: `uint64_t` suffices since $n \le 44$ for B.
- Testing: random multisets with $n \le 20$, compare A, B, C (when $\Sigma$ small) for the YES/NO answer and verify the returned partitions sum to $T$. Check D's returned $d$ against the partition it returns.
- Experiments: (i) time vs $n$ at fixed 25-digit magnitude for A, B, D; (ii) time vs magnitude at fixed $n = 20$ for B vs C; (iii) discrepancy of D vs greedy vs optimum (from B) for $n \le 40$.

---


# Stage 2: Documentation of Algorithms (basic version)

> **Status:** working scratch, not the submission. Markdown that converts to LaTeX: math in `$...$`, pseudocode in fenced blocks, no HTML.
> **Scope:** the basic version of the assignment, partition a multiset into **two subsets with equal sums**.
> **Due:** Oct 30, 2026 (we aim for Oct 29). Must state clearly that it is **Stage 2**.

Checklist of what the instructors ask for (task.md section 3 and 4a-b):

- [ ] a) Concepts: definitions with justification or source.
- [ ] b) For **every** algorithm: pseudocode, exhaustive description, complexity analysis.
- [ ] Every exponential exact algorithm has a polynomial-time approximation next to it.
- [ ] Duplicates and large numbers are addressed explicitly.
- [ ] References verified.
- [ ] Proofread (10-20% penalty for sloppy editing).

---

## 1. Problem statement

### 1.1 Definitions (report item a)

**Multiset.** A multiset $M$ over $\mathbb{Z}_{\ge 0}$ is a pair $(U, m)$ where $U$ is a finite set and $m: U \to \mathbb{Z}_{>0}$ gives the multiplicity of each element. We write it as a list $M = \{a_1, a_2, \dots, a_n\}$ in which values may repeat, and $n = \sum_{u \in U} m(u)$ is its size.

*Rationale for defining it this way:* the input lines of the assignment may contain repeated values (course email of 16.10.26), so the elements must be distinguished by **position**, not by value. All algorithms below work on indices $1..n$, which handles duplicates with no special cases.

**Sum.** $\Sigma(M) = \sum_{i=1}^{n} a_i$. For an index set $I \subseteq \{1,\dots,n\}$, $\Sigma(I) = \sum_{i \in I} a_i$.

**Partition into two subsets.** A pair $(A, B)$ with $A \cup B = \{1,\dots,n\}$ and $A \cap B = \emptyset$. Either side may be empty.

**Balanced partition.** A partition with $\Sigma(A) = \Sigma(B)$, equivalently $\Sigma(A) = \Sigma(M)/2$.

**Discrepancy.** $d(A,B) = |\Sigma(A) - \Sigma(B)|$. A partition is balanced iff $d = 0$. This is the quality measure for the approximation algorithm (task.md section 6).

**The decision problem PARTITION.** *Instance:* a multiset $M$ of non-negative integers. *Question:* does a balanced partition exist?

**The optimisation problem.** Find a partition minimising $d(A,B)$. Its optimum is $0$ iff the decision instance is a yes-instance, so an exact optimiser also decides PARTITION.

**Target sum.** $T = \Sigma(M)/2$. If $\Sigma(M)$ is odd there is no balanced partition. (Trivial necessary condition, checked first by every algorithm.)

### 1.2 Hardness (report item a, cited)

- PARTITION is **NP-complete**. It is one of Karp's 21 problems [Karp 1972]; the reduction is from SUBSET SUM (or 3-dimensional matching in Karp's original). Membership in NP: a partition is a certificate checkable in $O(n)$ additions.
- PARTITION is **weakly** NP-hard, not strongly: it has a pseudo-polynomial algorithm (Section 4) whose running time is polynomial in $n$ and in the *magnitude* $\Sigma(M)$, but exponential in the *number of digits* of the input [Garey and Johnson 1979, Section 4.2].
- Consequence for us: with 25-digit numbers (as in `AaC_example_2.txt`) the pseudo-polynomial algorithm is infeasible, and the only exact approach that scales is one whose cost depends on $n$ alone (Section 3). This is the central argument of the whole document and should be stated early.

### 1.3 Computational model and number representation

All numbers are **unsigned 128-bit integers** (see CONSTITUTION.md point 3 and task.md section 6). Consequences used throughout the analysis:

- Comparison, addition and subtraction cost $O(1)$.
- Input constraints (task.md section 7): each $0 \le a_i < 10^{38}$, and the total $\Sigma(M) < 2^{128}$. The program checks both on input and aborts with a message if violated. Every intermediate value in every algorithm below is a subset sum or a difference of two subset sums, hence bounded by $\Sigma(M)$, so no other overflow can occur.
- Limits on $n$ per algorithm are also in task.md section 7 and repeated in the summary table (Section 7).
- Therefore every complexity bound below is stated in the **unit-cost RAM model**, in terms of $n$ and (for the DP) $\Sigma(M)$ only. We should write this sentence explicitly in the report: it is a standard assumption and the instructors will look for it.

Memory is counted in machine words of 128 bits.

---

## 2. Algorithm A: Brute force (exhaustive search)

**Role.** Reference solution for correctness testing of everything else (CONSTITUTION point 7). It also serves as the baseline in the Stage 4 time measurements.

### 2.1 Idea

Every partition corresponds to a bit vector $x \in \{0,1\}^n$ with $x_i = 1$ meaning $i \in A$. Enumerate all vectors, compute $\Sigma(A)$, stop when it equals $T$.

Halving trick: element $n$ can be fixed to side $B$ without loss of generality (swapping $A$ and $B$ gives the same partition), so only $2^{n-1}$ vectors are needed.

### 2.2 Pseudocode

```
BRUTE-FORCE(a[1..n])
    S ← Σ a[i]
    if S is odd then return NO
    T ← S / 2
    for mask ← 0 to 2^(n-1) − 1 do        // bit i−1 of mask chooses side of a[i], a[n] always in B
        sumA ← 0
        for i ← 1 to n − 1 do
            if bit (i−1) of mask = 1 then sumA ← sumA + a[i]
        if sumA = T then
            return YES, A = { i : bit (i−1) of mask = 1 }
    return NO
```

Optional refinement for the implementation: Gray-code enumeration updates `sumA` with one addition or subtraction per step, bringing the inner loop to $O(1)$ amortised. Mention it, keep the simple version as the documented one.

### 2.3 Correctness

Every partition with $n \in B$ is visited exactly once; every partition is equal to one of those up to swapping sides. Hence the algorithm returns YES iff a balanced partition exists. Duplicates are irrelevant because the enumeration is over indices.

### 2.4 Complexity

- Time: $2^{n-1}$ masks, $O(n)$ per mask, so $O(n \cdot 2^{n})$. With Gray code: $O(2^n)$.
- Memory: $O(n)$ (the input and the current mask).
- Practical limit: $n \approx 30$ in seconds on a laptop.

---

## 3. Algorithm B: Meet in the middle (main exact algorithm)

**Source.** Horowitz and Sahni 1974, originally for KNAPSACK / SUBSET SUM. Standard exposition in Cormen et al. (CLRS) exercises and in Kleinberg-Tardos.

### 3.1 Idea

Split the indices into halves $L = \{1..\lfloor n/2 \rfloor\}$ and $R$ = the rest. Enumerate all $2^{|L|}$ subset sums of $L$ and all $2^{|R|}$ subset sums of $R$. A balanced partition exists iff there are $s_L$ from the first list and $s_R$ from the second with $s_L + s_R = T$. Finding such a pair in two lists is done by sorting and a two-pointer sweep (or a hash set).

Cost drops from $2^n$ to about $2 \cdot 2^{n/2}$, at the price of storing $2^{n/2}$ sums. Crucially, the running time **does not depend on the magnitude of the numbers**, which is why it is our main exact algorithm for 25-digit inputs.

### 3.2 Pseudocode

```
MEET-IN-THE-MIDDLE(a[1..n])
    S ← Σ a[i]
    if S is odd then return NO
    T ← S / 2
    h ← ⌊n / 2⌋
    LS ← SUBSET-SUMS(a[1..h])             // list of (sum, mask), 2^h entries
    RS ← SUBSET-SUMS(a[h+1..n])           // 2^(n−h) entries
    sort LS by sum ascending
    sort RS by sum descending
    i ← 1; j ← 1
    while i ≤ |LS| and j ≤ |RS| do
        s ← LS[i].sum + RS[j].sum
        if s = T then return YES, A = indices of LS[i].mask ∪ RS[j].mask
        if s < T then i ← i + 1
        else j ← j + 1
    return NO

SUBSET-SUMS(b[1..m])
    sums ← array of size 2^m
    sums[0] ← 0
    for mask ← 1 to 2^m − 1 do
        k ← index of lowest set bit of mask
        sums[mask] ← sums[mask without bit k] + b[k+1]   // one addition per subset
    return list of (sums[mask], mask) for all mask
```

### 3.3 Correctness

Every subset $A$ of $\{1..n\}$ decomposes uniquely as $A \cap L$ plus $A \cap R$, and $\Sigma(A) = \Sigma(A \cap L) + \Sigma(A \cap R)$. So a balanced $A$ exists iff the sum $T$ is attainable as one element of $LS$ plus one of $RS$.

Two-pointer invariant: when the sweep advances $i$ because $LS[i] + RS[j] < T$, no $RS[j'] $ with $j' \ge j$ (all smaller or equal) can complete $LS[i]$ to $T$, and all $RS[j']$ with $j' < j$ were already shown too large for $LS[i'] \le LS[i]$... (write this out carefully in the final text: standard argument for "pair with given sum in two sorted arrays"). Symmetric for advancing $j$.

Duplicates: repeated values produce repeated sums in $LS$ or $RS$; the sweep handles equal keys correctly because it compares sums, not identities. Alternatively, note that the enumeration is over masks, so duplicates never need separate treatment.

### 3.4 Complexity

Let $h = \lfloor n/2 \rfloor$.

- Generating sums: $O(2^{h})$ and $O(2^{n-h})$ with the lowest-set-bit recurrence (one addition each).
- Sorting: $O(2^{h} \cdot h)$ and $O(2^{n-h} \cdot (n-h))$, since $\log 2^{h} = h$.
- Sweep: $O(2^{h} + 2^{n-h})$.
- **Total time: $O(n \cdot 2^{n/2})$. Memory: $O(2^{n/2})$ words.**
- Independent of the magnitude of $a_i$ (under the 128-bit model). Compare with the DP below.
- Practical limit: memory. $n = 40$ gives $2^{20} \approx 10^6$ entries per side, trivial. $n = 60$ gives $2^{30}$ entries of 16 bytes plus masks, about 20 GB per side, infeasible. So $n \le 50$ or so on the lab machines.

Possible improvement to mention (not to implement unless time allows): Schroeppel-Shamir 1981 reduces memory to $O(2^{n/4})$ with the same time.

---

## 4. Algorithm C: Dynamic programming over sums (pseudo-polynomial exact)

**Source.** Bellman 1957 (dynamic programming); the subset-sum table is in Garey-Johnson 1979 Section 4.2 and in CLRS as an exercise. Included because it is the textbook exact algorithm and because contrasting it with Algorithm B is the clearest way to explain weak NP-hardness.

### 4.1 Idea

Let $\mathrm{reach}[s] = $ TRUE iff some subset of the elements seen so far has sum $s$, for $0 \le s \le T$. Process elements one by one; each element $a_i$ makes $s + a_i$ reachable from each reachable $s$. Answer YES iff $\mathrm{reach}[T]$ at the end.

### 4.2 Pseudocode

```
DP-PARTITION(a[1..n])
    S ← Σ a[i]
    if S is odd then return NO
    T ← S / 2
    reach ← boolean array [0..T], all FALSE
    reach[0] ← TRUE
    for i ← 1 to n do
        for s ← T down to a[i] do           // descending so each element is used at most once
            if reach[s − a[i]] then reach[s] ← TRUE
    return reach[T]
```

Reconstruction of the subset: keep `from[s] ← i` when `reach[s]` first becomes TRUE, then walk back from $T$. Alternatively a 2-D table `reach[i][s]`, $O(nT)$ memory. Mention both.

### 4.3 Correctness

Invariant after processing $i$ elements: $\mathrm{reach}[s]$ is TRUE iff $s$ is a subset sum of $a_1..a_i$. The descending inner loop guarantees that $\mathrm{reach}[s - a_i]$ refers to the state before $a_i$ was added, so each element contributes at most once (0/1, not unbounded). Duplicates are just separate iterations of the outer loop, handled naturally.

### 4.4 Complexity

- Time: $O(n \cdot T) = O(n \cdot \Sigma(M))$.
- Memory: $O(T)$ bits with the 1-D array (can be a bitset, giving a $64\times$ constant-factor speedup via word-parallel shifts: `reach |= reach << a[i]`).
- **This is pseudo-polynomial:** polynomial in the numeric value $\Sigma(M)$, but the input length is $\Theta(n \log \Sigma)$ bits, so the running time is exponential in the input length. For `AaC_example_2.txt`, $T \approx 10^{24}$ and the table would need $10^{24}$ bits. Infeasible by many orders of magnitude.
- Where it wins: small magnitudes and large $n$, e.g. $n = 1000$ elements below $10^4$. The Stage 4 experiments should show exactly this crossover against Algorithm B.

---

## 5. Algorithm D: Karmarkar-Karp differencing (polynomial-time approximation)

**Source.** Karmarkar and Karp 1982 (technical report, UC Berkeley, "The differencing method of set partitioning"). Analysis of its quality: Yakir 1996 (expected discrepancy $n^{-\Theta(\log n)}$ for random inputs); overview in Korf 1998 and in Mertens 2006 (survey, "The easiest hard problem"). Required by the rule that every exponential exact algorithm has a polynomial approximation (task.md 4b, CONSTITUTION 6).

### 5.1 Idea

Repeatedly take the two largest remaining numbers $x \ge y$, and decide that they go to **opposite** sides. The combined effect on the discrepancy is $x - y$, so replace them by $x - y$. Repeat until one number remains; it is the final discrepancy. To recover the actual sets, record each decision as an edge "$x$ and $y$ on opposite sides" and 2-colour the resulting tree at the end.

Why it is better than the simple greedy (put the next largest number on the lighter side): greedy fixes sides immediately; differencing postpones the decision and only commits to the *relation* between elements, which lets large numbers cancel each other.

### 5.2 Pseudocode

```
KARMARKAR-KARP(a[1..n])
    H ← max-heap of pairs (value, id) built from (a[i], i)
    G ← graph on vertices 1..n, no edges
    while |H| > 1 do
        (x, u) ← EXTRACT-MAX(H)
        (y, v) ← EXTRACT-MAX(H)
        INSERT(H, (x − y, u))               // x ≥ y, the difference keeps u's identity
        ADD-EDGE(G, u, v)                   // u and v must be on opposite sides
    (d, _) ← the single remaining element    // d = final discrepancy
    colour G with two colours by BFS/DFS (G is a forest, so this always succeeds)
    A ← vertices coloured 0, B ← vertices coloured 1
    return (A, B, d)
```

### 5.3 Correctness and quality

- *It outputs a valid partition:* $G$ has exactly $n - 1$ edges added over $n$ vertices and each edge merges two components (the vertex $v$ leaves the heap for good), so $G$ is a tree, hence bipartite, hence 2-colourable.
- *The returned $d$ equals the discrepancy of the returned partition:* by induction on the number of differencing steps: replacing $x, y$ on opposite sides by $x - y$ on the side of $x$ leaves $\Sigma(A) - \Sigma(B)$ unchanged.
- *It is not exact:* counterexample for the report, e.g. $\{4, 5, 6, 7, 8\}$ with $\Sigma = 30$, $T = 15$. KK: $8-7=1$, $6-5=1$, $4-1=3$, $3-1=2$, discrepancy 2. But $\{7, 8\}$ vs $\{4, 5, 6\}$ is balanced. (Verify this trace before the final version.)
- *Guarantees to cite:* for the decision version there is no approximation ratio in the classic sense (OPT may be 0), so quality is stated either as a bound on $d$ or as a ratio on $\max(\Sigma(A), \Sigma(B))$. Known results: the discrepancy satisfies $d \le \max_i a_i$ (trivial, also holds for greedy); for uniformly random inputs the expected $d$ is $n^{-\Theta(\log n)}$ [Yakir 1996], versus $\Theta(1/n)$ for greedy. For the makespan view, LPT-style greedy is a $7/6$-approximation for two machines [Graham 1969]. We must be honest that KK's worst-case ratio on makespan is also $7/6$ (Fischetti and Martello 1987). Decide how much of this to include; at minimum: the trivial bound, the counterexample, and the random-input result with citation.

### 5.4 Complexity

- Heap build $O(n)$; $n - 1$ rounds, each with two extractions and one insertion, $O(\log n)$ each.
- 2-colouring: $O(n)$ on a tree.
- **Total time: $O(n \log n)$. Memory: $O(n)$.**
- Duplicates: equal values just produce a difference of 0, which is inserted and later absorbed. No special handling.

### 5.5 Optional: simple greedy as a second baseline

```
GREEDY(a[1..n])
    sort a descending
    for each a[i]: add it to the side with the smaller current sum
```

$O(n \log n)$. Worth one paragraph and a comparison in the Stage 4 experiments; it is what everyone thinks of first, and KK beating it by orders of magnitude is a good result to show.

### 5.6 Optional: complete Karmarkar-Karp (CKK)

Korf 1998 turns KK into an anytime exact search: at each step branch on "opposite sides" ($x-y$) vs "same side" ($x+y$), KK first. Exponential worst case, but with pruning it solves many instances fast and returns the KK answer immediately. Only mention if the team wants a third exact method.

---

## 6. Algorithm E: Final algorithm (input-dependent selection)

**Role.** This is the algorithm the program actually runs. Algorithms A-D are its building blocks. It looks at the size $n$ and the total sum $S$ of the input, runs the cheapest exact method that is feasible under the limits of task.md section 7, and falls back to the Karmarkar-Karp approximation when no exact method is feasible. It always returns a partition and its discrepancy, and it says whether the answer is exact.

### 6.1 Idea

The exact methods have complementary ranges: dynamic programming is cheap when the numbers are small regardless of $n$, meet in the middle is cheap when $n$ is small regardless of the numbers, and brute force is only a reference. Karmarkar-Karp always runs: it is $O(n \log n)$, it gives the answer immediately when its discrepancy is 0, and it is the result we report when the exact methods are out of range.

### 6.2 Pseudocode

```
SOLVE(a[1..n], limits N_MITM = 60, T_DP = 10^10)
    S ← Σ a[i]                                   // with overflow check
    if S is odd then
        (A, B, d) ← KARMARKAR-KARP(a)
        return (A, B, d, exact = TRUE)           // no balanced partition exists, d > 0 is a certificate-free answer
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

The thresholds `N_MITM` and `T_DP` are the limits from task.md section 7 and are command-line options, so the Stage 4 experiments can move them.

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

The worst case is still exponential, which is unavoidable unless P = NP (Section 1.2); the selection only guarantees that the exponential branch is entered when it is known to finish within the limits.

---

## 7. Summary table (goes near the end of the report)

| Algorithm | Exact? | Time | Memory | Depends on magnitude? | Practical range |
|---|---|---|---|---|---|
| A. Brute force | yes | $O(n \cdot 2^n)$ | $O(n)$ | no | $n \le 30$ |
| B. Meet in the middle | yes | $O(n \cdot 2^{n/2})$ | $O(2^{n/2})$ | no | $n \le 60$ |
| C. Dynamic programming | yes | $O(n \cdot \Sigma)$ | $O(\Sigma)$ bits | **yes** | $\Sigma/2 \le 10^{10}$ |
| D. Karmarkar-Karp | no | $O(n \log n)$ | $O(n)$ | no | $n \le 10^6$ |
| E. Final (selection) | yes within limits, else approx. | max of the branch taken | max of the branch taken | via C only | any |

---

## 8. Worked example (one for each algorithm)

Use the first set of `AaC_example_1.txt`: $\{11, 22, 33, 44, 55, 66, 77, 1, 2, 3, 4, 5, 6, 7, 8\}$, $n = 15$, $\Sigma = 344$, $T = 172$. TODO: trace B, C (small version) and D by hand or with a throwaway script, and paste the traces. Also show the odd-sum early exit on $\{999, 889, 778, 667, 123, 456, 133\}$ ($\Sigma = 4045$, odd, so NO immediately).

---

## 9. Notes for Stages 3 and 4 (not part of the Stage 2 PDF)

- Number type: `using Num = unsigned __int128;` in one header, with `parse(string) -> Num` (reject $>38$ digits, reject non-digits, allow leading zeros) and `to_string(Num)`. Overflow check on summation via `__builtin_add_overflow`. Differences in Karmarkar-Karp are always $x - y$ with $x \ge y$, so unsigned is safe there; the discrepancy $|\Sigma(A)-\Sigma(B)|$ is computed as max minus min.
- Masks in Algorithm B: `uint64_t` suffices since $n \le 60$ for B (task.md section 7).
- Testing: random multisets with $n \le 20$, compare A, B, C (when $\Sigma$ small) for the YES/NO answer and verify the returned partitions sum to $T$. Check D's returned $d$ against the partition it returns.
- Experiments: (i) time vs $n$ at fixed 25-digit magnitude for A, B, D; (ii) time vs magnitude at fixed $n = 20$ for B vs C; (iii) discrepancy of D vs greedy vs optimum (from B) for $n \le 40$.

---

## 10. References (to verify before submission)

- R. M. Karp, "Reducibility among combinatorial problems", in *Complexity of Computer Computations*, 1972. (NP-completeness of PARTITION.)
- M. R. Garey, D. S. Johnson, *Computers and Intractability*, 1979. (Weak vs strong NP-hardness, pseudo-polynomial DP, Section 4.2.)
- E. Horowitz, S. Sahni, "Computing partitions with applications to the knapsack problem", *J. ACM* 21(2), 1974. (Meet in the middle.)
- R. Schroeppel, A. Shamir, "A $T = O(2^{n/2})$, $S = O(2^{n/4})$ algorithm for certain NP-complete problems", *SIAM J. Comput.* 10(3), 1981. (Memory improvement, optional.)
- N. Karmarkar, R. M. Karp, "The differencing method of set partitioning", Tech. Rep. UCB/CSD 82/113, UC Berkeley, 1982.
- B. Yakir, "The differencing algorithm LDM for partitioning: a proof of a conjecture of Karmarkar and Karp", *Math. Oper. Res.* 21(1), 1996.
- R. E. Korf, "A complete anytime algorithm for number partitioning", *Artificial Intelligence* 106(2), 1998.
- S. Mertens, "The easiest hard problem: number partitioning", in *Computational Complexity and Statistical Physics*, 2006. (Survey; good for the introduction.)
- R. L. Graham, "Bounds on multiprocessing timing anomalies", *SIAM J. Appl. Math.* 17(2), 1969. (LPT bound, if we include the greedy baseline.)
- M. Fischetti, S. Martello, "Worst-case analysis of the differencing method for the partition problem", *Math. Programming* 37, 1987. (7/6 bound for KK, if we include it.)
- T. H. Cormen et al., *Introduction to Algorithms*, 3rd or 4th ed. (General reference for DP, heaps, sorting.)

All of these are real and well known, but page numbers, exact titles and the Fischetti-Martello details should be checked against the originals before they go into the PDF.

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

**Role.** Reference solution for correctness testing of everything else (CONSTITUTION point 7). It also serves as the baseline in the Stage 4 time measurements. It is an exact algorithm with exponential time; the polynomial-time approximation that goes with it (task.md 4b, CONSTITUTION 6) is Algorithm D in Section 5.

### 2.1 Idea

Every partition $(A, B)$ of the indices $\{1, \dots, n\}$ corresponds to a bit vector $x \in \{0,1\}^n$ with $x_i = 1$ if and only if $i \in A$. The algorithm enumerates these vectors one after another, computes $\Sigma(A)$ for each, and stops as soon as $\Sigma(A) = T$. If the enumeration ends without a hit, no balanced partition exists.

**Halving trick.** Swapping $A$ and $B$ gives the same partition and the same discrepancy, so we may assume that index $n$ lies in $B$. Only the first $n-1$ bits are then free, and $2^{n-1}$ vectors suffice instead of $2^n$. The vector is stored as an integer `mask` in $\{0, \dots, 2^{n-1} - 1\}$, where bit $i-1$ of `mask` is $x_i$.

**Why indices and not values.** The input may contain repeated values (course email of 16.10.26). The enumeration runs over index sets, so equal values at different positions are different elements and no special treatment of duplicates is needed. Elements equal to $0$ are handled in the same way.

### 2.2 Pseudocode

```
BRUTE-FORCE(a[1..n])
    S ← Σ a[i]                              // with overflow check, see Section 2.7
    if S is odd then return NO
    T ← S / 2
    for mask ← 0 to 2^(n−1) − 1 do          // bit i−1 of mask puts a[i] in A; a[n] is always in B
        sumA ← 0
        for i ← 1 to n − 1 do
            if bit (i−1) of mask = 1 then sumA ← sumA + a[i]
        if sumA = T then
            return YES, A = { i : bit (i−1) of mask = 1 }, B = the other indices
    return NO
```

For $n = 1$ the loop has the single value `mask` $= 0$ with `sumA` $= 0$, so the answer is YES exactly when $a_1 = 0$, which is correct. For $S = 0$ (all elements zero) the first mask already succeeds with $A = \emptyset$, which is a valid partition because either side may be empty (Section 1.1).

### 2.3 Gray-code variant

In the simple version every mask recomputes its sum from scratch in $n - 1$ steps. If the masks are visited in the order of the **binary reflected Gray code**, two consecutive masks differ in exactly one bit, so the sum can be updated with one addition or one subtraction.

The $k$-th Gray code word is $g_k = k \oplus (k \gg 1)$ ($\oplus$ is bit-wise exclusive or, $\gg$ is a right shift). Two facts are used:

1. $k \mapsto g_k$ is a bijection of $\{0, \dots, 2^{m} - 1\}$ onto itself, so every mask is visited exactly once.
2. $g_k \oplus g_{k-1} = 2^{j}$ where $j$ is the position of the lowest set bit of $k$ (`ctz(k)`, count of trailing zeros). So going from $g_{k-1}$ to $g_k$ flips only bit $j$.

```
BRUTE-FORCE-GRAY(a[1..n])
    S ← Σ a[i]
    if S is odd then return NO
    T ← S / 2
    m ← n − 1
    g ← 0;  sumA ← 0                        // g is the current mask, A = ∅
    if sumA = T then return YES, A = ∅
    for k ← 1 to 2^m − 1 do
        j ← ctz(k)                          // the one bit that changes
        g ← g XOR 2^j
        if bit j of g = 1 then sumA ← sumA + a[j+1]     // a[j+1] entered A
        else                   sumA ← sumA − a[j+1]     // a[j+1] left A
        if sumA = T then
            return YES, A = { i : bit (i−1) of g = 1 }
    return NO
```

The subtraction never underflows: a value is subtracted only if it was added before and not yet removed, so it is part of `sumA`. The invariant is that `sumA` $= \sum_{i:\,\text{bit}(i-1)\text{ of }g = 1} a_i$ after every step. The documented baseline for Stage 4 is the simple version of Section 2.2; the Gray-code version is the one we implement for speed, and the two are compared in the tests (they must give the same YES/NO answer).

### 2.4 Example

**A YES instance.** Let $a = (3, 1, 4, 2)$, the same set as in Section 4.4. Then $S = 10$ is even, $T = 5$, $n = 4$, so $a_4 = 2$ stays in $B$ and the mask has $3$ bits: bit 0 is $a_1 = 3$, bit 1 is $a_2 = 1$, bit 2 is $a_3 = 4$.

| `mask` | bits (2 1 0) | set $A$ | $\Sigma(A)$ | $= T$? |
| ------ | ------------ | ------- | ----------- | ------ |
| 0      | 000          | $\emptyset$ | 0       | no     |
| 1      | 001          | $\{a_1\}$ | 3         | no     |
| 2      | 010          | $\{a_2\}$ | 1         | no     |
| 3      | 011          | $\{a_1, a_2\}$ | 4    | no     |
| 4      | 100          | $\{a_3\}$ | 4         | no     |
| 5      | 101          | $\{a_1, a_3\}$ | 7    | no     |
| 6      | 110          | $\{a_2, a_3\}$ | 5    | **yes** |

At `mask` $= 6$ the algorithm stops after $7$ of the $8$ masks and answers YES with $A = \{a_2, a_3\} = \{1, 4\}$ and $B = \{a_1, a_4\} = \{3, 2\}$, both with sum $5$.

**The Gray-code variant on the same input.** The masks come in the order $000, 001, 011, 010, 110, \dots$

| step $k$ | $\text{ctz}(k)$ | flipped element | $g$ | $\text{sumA}$ | $= T$? |
| -------- | --------------- | --------------- | --- | ------------- | ------ |
| 0        |                 |                 | 000 | 0             | no     |
| 1        | 0               | $a_1 = 3$ enters | 001 | $0 + 3 = 3$  | no     |
| 2        | 1               | $a_2 = 1$ enters | 011 | $3 + 1 = 4$  | no     |
| 3        | 0               | $a_1 = 3$ leaves | 010 | $4 - 3 = 1$  | no     |
| 4        | 2               | $a_3 = 4$ enters | 110 | $1 + 4 = 5$  | **yes** |

It stops after $5$ masks with the same partition $A = \{a_2, a_3\}$. Each step costs one addition or subtraction instead of up to $3$.

**A NO instance.** Let $a = (2, 3, 7)$. Then $S = 12$, $T = 6$, and the mask has $2$ bits for $a_1 = 2$ and $a_2 = 3$. The four masks give $\Sigma(A) \in \{0, 2, 3, 5\}$ and none equals $6$, so all masks are examined and the answer is NO. Indeed, $7$ alone already exceeds $T$.

### 2.5 Correctness

**Lemma 0 (what is enumerated).** The values `mask` $\in \{0, \dots, 2^{n-1} - 1\}$ correspond one to one to the subsets $A \subseteq \{1, \dots, n-1\}$. The same holds for the Gray-code order, because $k \mapsto g_k$ is a bijection of this range onto itself.

_Proof._ The bits of the mask are the indicator vector of $A$. A bijection of the range permutes the masks and so visits each once. $\square$

**Lemma 1 (reduction to $n \in B$).** A balanced partition of $\{1, \dots, n\}$ exists if and only if there is a subset $A \subseteq \{1, \dots, n-1\}$ with $\Sigma(A) = T$.

_Proof._ ($\Leftarrow$) Take $B = \{1, \dots, n\} \setminus A$. Then $\Sigma(B) = S - T = T = \Sigma(A)$. ($\Rightarrow$) Let $(A, B)$ be balanced. If $n \in B$, then $A \subseteq \{1, \dots, n-1\}$ and $\Sigma(A) = T$ because a balanced partition has $\Sigma(A) = S/2$. Otherwise $n \in A$, and then $(B, A)$ is also balanced, has $n$ in its second part, and its first part $B \subseteq \{1, \dots, n-1\}$ has $\Sigma(B) = T$. $\square$

**Theorem.** `BRUTE-FORCE` and `BRUTE-FORCE-GRAY` return YES if and only if a balanced partition exists, and the sets they return form a balanced partition.

_Proof._ If $S$ is odd, no balanced partition exists, because $\Sigma(A) = \Sigma(B)$ would give $S = 2\Sigma(A)$, and NO is correct. Let $S$ be even. _Soundness:_ the algorithm answers YES only when `sumA` $= T$, and `sumA` is the sum of the returned set $A$ (in the simple version by construction of the inner loop, in the Gray version by the invariant of Section 2.3, which follows by induction on $k$ since every step adds or subtracts the one element whose bit was flipped). Then $B$ is the complement and $\Sigma(B) = S - T = T$, so the partition is balanced. _Completeness:_ if a balanced partition exists, Lemma 1 gives a subset $A \subseteq \{1, \dots, n-1\}$ with $\Sigma(A) = T$, and by Lemma 0 its mask is visited unless the algorithm stops earlier. It stops earlier only with a YES, which is correct by soundness. If the loop ends without a hit, no such subset exists, hence by Lemma 1 no balanced partition exists, and NO is correct. The algorithm terminates because the loop runs over a finite range. $\square$

Duplicates and zeros need no case analysis, since the proof works on index sets and never uses that the values are distinct or positive.

### 2.6 Complexity

We count operations on 128-bit numbers in the unit-cost model of Section 1.3 and use $m = n - 1$.

- **Preliminaries.** The sum and the parity test cost $O(n)$.
- **Simple version.** The outer loop has $2^{n-1}$ masks (in the worst case all of them, when the answer is NO or the hit is the last mask) and the inner loop has $n - 1$ iterations with one test and at most one addition. The worst-case time is therefore $\Theta(n \cdot 2^{n-1}) = \Theta(n \cdot 2^n)$, that is $O(n \cdot 2^n)$.
- **Gray-code version.** Each step does one bit-flip, one `ctz`, one addition or subtraction and one comparison, which is $O(1)$ ($\text{ctz}$ is a single instruction on current processors, and even when computed by a loop it takes $O(1)$ amortised, because it inspects $j + 1$ bits and half of all $k$ have $j = 0$, a quarter have $j = 1$, and so on). The worst-case time is $\Theta(2^{n-1}) = O(2^n)$, which is smaller by a factor $n$.
- **Recovering the partition.** On YES, building $A$ and $B$ from the mask costs $O(n)$ once.
- **Memory.** The input of $n$ numbers, `sumA`, `S`, `T` and one mask: $O(n)$ words in total, and $O(1)$ beyond the input. The mask fits in a `uint64_t` while $n \le 64$, which is far above the limit of Section 2.7.
- **Best and worst case.** The best case is a YES instance whose first mask succeeds ($S = 0$), or an odd $S$, which costs only $O(n)$. The worst case is a NO instance with even $S$, which executes all masks. The running time does **not** depend on the magnitude of the numbers, only on $n$.

### 2.7 Constraints and limitations

**Constraints that the program enforces** (task.md section 7):

1. **Preconditions.** Each $0 \le a_i < 10^{38}$ and $S < 2^{128}$ is checked with overflow detection (`__builtin_add_overflow`) while summing. If $S$ is odd, the program answers NO without enumerating.
2. **Bound on $n$.** The exact algorithm is run only for $n \le 30$. For a larger $n$ the program reports SKIPPED together with the violated condition. The threshold can be changed on the command line so that the Stage 4 experiments can go past it on purpose.

**Limitations.** The time doubles with every additional element. Assuming $10^{9}$ Gray-code steps per second, which is optimistic for 128-bit arithmetic and has to be measured in Stage 4:

| $n$ | masks $2^{n-1}$ | time (Gray code, $10^9$ steps/s) |
| --- | --------------- | -------------------------------- |
| 20  | $5.2 \cdot 10^{5}$  | under a millisecond          |
| 30  | $5.4 \cdot 10^{8}$  | about $0.5$ s                |
| 40  | $5.5 \cdot 10^{11}$ | about $9$ min                |
| 50  | $5.6 \cdot 10^{14}$ | about $6.5$ days             |
| 60  | $5.8 \cdot 10^{17}$ | about $18$ years             |

The simple version needs about $n - 1$ times as many steps, so at $n = 30$ it takes tens of seconds rather than seconds. This is why the limit is $n \le 30$, and why sets with $n$ in the hundreds, as in `AaC_example_2.txt`, cannot be solved by this algorithm, whatever the size of the numbers.

### 2.8 When to use it

Brute force is **never** the algorithm of choice for the final program; its role is the reference. Its value lies in being obviously correct: it contains no cleverness that could hide a bug, uses $O(n)$ memory, and does not depend on the magnitude of the numbers. We therefore use it in Stages 3 and 4 as follows:

- **Oracle in the tests.** On random multisets with $n \le 20$ (including duplicates, zeros and odd sums) the answers of B, C (when $S$ is small) and E are compared with A, and every returned partition is checked to have two sums equal to $T$.
- **Baseline in the time experiments.** The time of A against $n$ shows the $2^n$ growth that motivates B (about $2^{n/2}$) and C (about $n \cdot T / 64$), see Sections 3.4 and 4.8.

|                            | A, brute force (Gray) | B, meet in the middle |
| -------------------------- | --------------------- | --------------------- |
| Time, $n = 30$             | $2^{29} \approx 5.4 \cdot 10^8$ steps | $n \cdot 2^{n/2} \approx 10^{6}$ operations |
| Memory                     | $O(n)$                | $16 \cdot 2^{n/2} \approx 0.5$ MB per half   |
| Practical limit            | $n \le 30$            | $n \le 60$            |

The gain of B over A grows exponentially with $n$, and the price is the memory $O(2^{n/2})$, which is the only advantage that remains to A.

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

Every subset $A$ of $\{1..n\}$ decomposes uniquely as $A \cap L$ + $A \cap R$ = $A$, and $\Sigma(A) = \Sigma(A \cap L) + \Sigma(A \cap R)$. So a balanced $A$ exists iff the sum $T$ is attainable as one element of $LS$ plus one of $RS$.

Two-pointer invariant: initially, the smallest sum from $LS$ and largest sum from $RS$ are summed and compared against $T$. If not equal, depending whether the sum is smaller or larger than $T$, $i$ or $j$ is incremented respectively - advancing the sweep is symmetrical. Without loss of generality, consider the case when the sweep advances incrementing $i$ because $LS[i] + RS[j] < T$. Then, no $RS[j']$ with $j' \ge j$ (all smaller or equal) can complete $LS[i]$ to $T$, and all $RS[j']$ with $j' < j$ were already shown too large for $LS[i'] \le LS[i]$ - a standard argument for a pair with given sum in two sorted arrays. Due to symmetry, the same argument applies for advancing $j$.

Duplicates: repeated values produce repeated sums in $LS$ or $RS$; the sweep handles equal keys correctly because it compares sums, not identities. Alternatively, note that the enumeration is over masks, so duplicates never need separate treatment.

### 3.4 Complexity

Let $h = \lfloor n/2 \rfloor$.

- Generating sums: $O(2^{h})$ and $O(2^{n-h})$ with the lowest-set-bit recurrence (one addition each).
- Sorting: $O(2^{h} \cdot h)$ and $O(2^{n-h} \cdot (n-h))$, since $\log 2^{h} = h$.
- Sweep: $O(2^{h} + 2^{n-h})$.
- **Total time: $O(n \cdot 2^{n/2})$. Memory: $O(2^{n/2})$ words.**
- Independent of the magnitude of $a_i$ (under the 128-bit model). Compare with the DP below.
- Practical limit: memory. $n = 40$ gives $2^{20} \approx 10^6$ entries per side, trivial. $n = 60$ gives $2^{30}$ entries of 16 bytes plus masks, about 20 GB per side, infeasible. So $n \le 50$ or so on the lab machines.

### 3.5 Possible further improvement
 Note: A possible further improvement to mention (not to implement unless time allows): Schroeppel-Shamir 1981 reduces memory to $O(2^{n/4})$ with the same time.

---

## 4. Dynamic programming over subset sums

Exact algorithm whose cost depends on the size of the numbers. Choosing for each element whether it joins the subset or not spans a decision tree with $2^n$ leaves, but many branches reach the same partial sum. The algorithm memoizes every partial result in a table the first time it is obtained and reuses it afterwards. This replaces the $2^n$ branches by a cost proportional to $n$ times the target sum, at the price of memory proportional to the target sum; whether this is a gain depends on how large the target is (Sections 4.6 and 4.8).

### 4.1 From the partition problem to subset sum

Let $M = \{a_1, \dots, a_n\}$ be the input multiset and $S = \Sigma(M)$ its total sum (Section 1.1).

**Step 1: parity test.** In a balanced partition both sides have the same sum $\Sigma(A) = \Sigma(B)$, so $S = \Sigma(A) + \Sigma(B) = 2\,\Sigma(A)$ is even. Hence if $S$ is odd, no balanced partition exists and the algorithm can return the answer immediately, at the cost of one pass over the input.

**Step 2: target sum.** If $S$ is even, set $T = S/2$, where $T$ is the target sum: the sum that each side of a balanced partition must have.

**Step 3: reduction to the subset sum problem.** With the target $T$ fixed, the question becomes the following problem.

> **Subset sum problem.** _Given:_ a multiset $M$ of non-negative integers and a target $T$. _Question:_ is there an index set $A \subseteq \{1, \dots, n\}$ with $\Sigma(A) = T$?

**Lemma 1.** Let $S$ be even and $T = S/2$. A balanced partition of $M$ exists if and only if some subset of $M$ has sum exactly $T$.

_Proof._ If $(A, B)$ is balanced then $\Sigma(A) = S/2 = T$, so $A$ is a solution of the subset sum problem. Conversely, if $\Sigma(A) = T$, put $B = \{1, \dots, n\} \setminus A$. Then $\Sigma(B) = S - T = T = \Sigma(A)$, so $(A, B)$ is balanced. $\square$

A solution $A$ of the subset sum problem is therefore at the same time one side of a balanced partition, and the other side is its complement. From now on the task is to decide whether a subset with sum exactly $T$ exists.

**Step 4: oversized elements.** If some element satisfies $a_j > T$, the side containing it has a sum larger than $T = S/2$, so the two sides cannot be equal and the answer is FALSE. This step does not affect correctness: it saves building the table in this case and guarantees that

$$a_j \le T \quad \text{for every } j \in \{1, \dots, n\}$$

holds for all elements processed later.

### 4.2 The idea

**Why not enumerate subsets.** Brute force and meet in the middle (Sections 2 and 3) examine subsets and there are $2^n$ of them. Many different subsets have the same sum, and only the values $0, 1, \dots, T$ matter for our question. All elements are non-negative, so a sum larger than $T$ can never be completed to $T$. The dynamic programming approach exploits this: instead of remembering subsets, it remembers which _sums_ can be obtained, and there are only $T + 1$ candidates for them.

**The table.** For $0 \le i \le n$ and $0 \le s \le T$ define

$$R_i(s) = \text{TRUE} \iff \text{some subset of } \{a_1, \dots, a_i\} \text{ has sum } s.$$

$R_i$ describes which sums are obtainable from the first $i$ elements. We want $R_n(T)$.

**The recurrence.** Whether a sum $s$ is obtainable from the first $i$ elements depends only on the sums obtainable from the first $i-1$ elements. So the values $R_i(s)$ satisfy a one-step recurrence relation, which consists of a base case and a recurrence step.

_Base case._ With no elements the only obtainable sum is the empty sum $0$:

$$R_0(s) = \begin{cases} \text{TRUE} & s = 0, \\ \text{FALSE} & s > 0. \end{cases}$$

_Recurrence step._ For $i \ge 1$, a subset of $\{a_1, \dots, a_i\}$ with sum $s$ either does not contain $a_i$, and then it is a subset of $\{a_1, \dots, a_{i-1}\}$ with sum $s$, or it contains $a_i$, and then removing $a_i$ leaves a subset of $\{a_1, \dots, a_{i-1}\}$ with sum $s - a_i$. Hence

$$R_i(s) = \begin{cases} R_{i-1}(s) & s < a_i, \\ R_{i-1}(s) \lor R_{i-1}(s - a_i) & s \ge a_i. \end{cases}$$

If $s < a_i$ the second option is impossible, because the remaining sum $s - a_i$ would be negative.

Applying the recurrence step for $i = 1, \dots, n$ yields $R_n(T)$, which by Lemma 1 is TRUE if and only if a balanced partition exists. Equal values need no special treatment, because two equal elements are two different indices and hence two different steps of the recurrence.

**Storing a single row.** Row $i$ depends only on row $i-1$, so the algorithm does not store all rows. It keeps one array `reach[0..T]` and overwrites it, so that after step $i$ the array holds $R_i$. The order in which the cells are overwritten matters, because the recurrence step reads the cell `reach[s − a[i]]` and must find there the value from row $i-1$.

The cell `reach[s − a[i]]` has a smaller index than `reach[s]`, so it is still unchanged when `reach[s]` is computed, provided that the cells are updated from the largest index to the smallest. Hence the sums have to be processed in descending order.

**Bitset form.** Each cell of the array `reach` holds only true or false, so one bit is enough for it. Packing the $T+1$ cells as bits into machine words gives a _bitset_ of $T+1$ bits. A boolean array normally uses one byte per cell, so the bitset needs 8 times less memory. It is also faster: the loop over all $s$ is replaced by the operation `reach ← reach OR (reach << a[i])`, which shifts the whole array by $a_i$ positions and combines it with the old one. The machine carries it out word by word, and each word holds 64 cells, so one step needs about $T/64$ word operations instead of $T$ cell updates. This is the most efficient form to implement.

### 4.3 Pseudocode

**Input.** `a[1..n]` is the multiset of $n$ non-negative integers, and `T_DP` is the largest target sum for which the table is built (Section 4.7).

**Output.** TRUE if a balanced partition exists, FALSE if it does not, and SKIPPED if the target sum exceeds `T_DP`.

**Variables.** `S` is the sum of all elements and `T` is the target sum. `reach[0..T]` is the table of the current row, where `reach[s]` is TRUE if the sum `s` is obtainable. `top` is an upper bound on the sums that are obtainable so far, so that cells above it need not be visited. The pseudocode uses a boolean array, and the bitset form of Section 4.2 replaces its inner loop.

```
DP-PARTITION(a[1..n], T_DP)
    S ← a[1] + a[2] + ... + a[n]        // with overflow check
    if S is odd then return FALSE       // Step 1
    T ← S / 2                           // Step 2
    if max a[i] > T then return FALSE   // Step 4
    if T > T_DP then return SKIPPED     // table too large, see Section 4.7
    reach[0..T] ← FALSE
    reach[0] ← TRUE
    top ← 0
    for i ← 1 to n do
        top ← min(T, top + a[i])
        for s ← top down to a[i] do     // descending order, see Section 4.2
            reach[s] ← reach[s] OR reach[s − a[i]]
        if reach[T] then return TRUE    // the target is already obtainable
    return FALSE
```

### 4.4 Example

Let $a = (3, 1, 4, 2)$. Then $S = 10$ is even, $T = 5$, and $\max a_i = 4 \le T$. Obtainable sums after each step:

| step $i$ | element $a_i$ | obtainable sums $\le T$ |
| -------- | ------------- | ----------------------- |
| 0        |               | $\{0\}$                 |
| 1        | 3             | $\{0, 3\}$              |
| 2        | 1             | $\{0, 1, 3, 4\}$        |
| 3        | 4             | $\{0, 1, 3, 4, 5\}$     |

After step 3 the target $T = 5$ is obtainable, so the algorithm stops and answers TRUE. Indeed, the elements $1$ and $4$ form one side with sum $5$, and the elements $3$ and $2$ form the other side with sum $5$.

### 4.5 Correctness

**Lemma 2 (invariant).** After step $i$ of the main loop, `reach[s]` $= R_i(s)$ for all $0 \le s \le T$.

_Proof._ Induction on $i$. Before the loop only `reach[0]` is TRUE, which is $R_0$. Assume `reach` $= R_{i-1}$ before step $i$. Consider the moment when the loop handles a sum $s \ge a_i$. Cells with an index larger than $s$ have already been rewritten, but those with an index at most $s$ have not, and `reach[s − a[i]]` is among the latter because $s - a_i \le s$. So it still holds $R_{i-1}(s - a_i)$, and the cell `reach[s]` receives $R_{i-1}(s) \lor R_{i-1}(s - a_i)$, which is $R_i(s)$ by the recurrence of Section 4.2. For $s < a_i$ the recurrence gives $R_i(s) = R_{i-1}(s)$, and these cells are not modified. The bound `top` skips only cells that are FALSE in both rows, because no subset of $\{a_1, \dots, a_i\}$ has a sum larger than $a_1 + \dots + a_i$. $\square$

**Theorem.** `DP-PARTITION` returns TRUE if and only if a balanced partition of $M$ exists, provided that the table is built, see Section 4.7.

_Proof._ If $S$ is odd, there is no balanced partition (Step 1), and FALSE is correct. If $\max a_i > T$, FALSE is correct (Step 4). Otherwise, by Lemma 2 the final value of `reach[T]` is $R_n(T)$, unless the loop stopped earlier at a step $i < n$ with `reach[T]` TRUE; this is also correct, because a subset of $\{a_1, \dots, a_i\}$ with sum $T$ is also a subset of $\{a_1, \dots, a_n\}$, so $R_i(T)$ implies $R_n(T)$. By the definition of $R_n$ this holds if and only if a subset with sum $T$ exists, and by Lemma 1 this is equivalent to the existence of a balanced partition. $\square$

### 4.6 Complexity

Let $T = S/2$ and $w = 64$ be the word size of the bitset. We count operations on machine words in the unit-cost model of Section 1.3.

- **Preliminaries.** The sum, the parity test and the maximum cost $O(n)$.
- **Boolean array.** Each of the $n$ steps visits at most $T$ cells, hence time $O(n \cdot T)$, with $O(T)$ cells of memory.
- **Bitset version.** One step consists of a shift and an OR over $\lceil (T+1)/w \rceil$ words, hence $O(n \cdot T/w)$ for the main loop.
- **Total time:** $O(n + n \cdot T/w)$. Since $T = S/2$, this is $O(n + n \cdot S/w)$, that is $O(n \cdot S)$ if the factor $w$ is dropped. The bound `top` makes the early steps cheaper but does not change the worst case.
- **Memory:** the table has $T + 1$ cells, which take $T + 1$ bits as a bitset. So the memory is $O(T)$ bits, that is $O(T/w)$ machine words, and in terms of the input sum $O(S)$ bits.
- **Best and worst case.** The early exit shortens the run only if the answer is TRUE. If the answer is FALSE and $S$ is even with $\max a_i \le T$, all $n$ steps are executed.

**Pseudo-polynomial time.** An algorithm is _pseudo-polynomial_ if its running time is polynomial in the numeric values of the input but not in the length of its encoding [Garey and Johnson 1979, Section 4.2]. Here the input length is $\Theta(n \log S)$ bits, while the running time contains the factor $T = S/2$, which grows tenfold with every additional decimal digit. So the algorithm is exponential in the input length, in agreement with the weak NP-hardness of the partition problem.

### 4.7 Constraints and limitations

**Constraints that the program enforces**:

1. **Preconditions of the table.** The table is built only if $S$ is even, $\max a_i \le T$ and $T \le T_{DP}$. If $S$ is odd or $\max a_i > T$, the algorithm returns FALSE. If $T > T_{DP}$, it reports SKIPPED together with the violated condition.
2. **Bound on the target.** The parameter $T_{DP}$ is the largest target sum for which the table is built. Its default value is $10^{10}$ and it can be changed on the command line. For $T = 10^{10}$ the table has $10^{10}$ bits, which is $1.25$ GB, and one step processes about $1.6 \cdot 10^{8}$ words. This value is therefore the limit of both the memory and the time that we accept. A larger target is not processed, and the result is SKIPPED.
3. **Number of elements.** The algorithm adds no limit on $n$, so only the global limit $n \le 10^6$ of the program applies.

**Limitations.**

- The cost grows with the magnitude of the numbers. For 25-digit values such as those in `AaC_example_2.txt` the target is about $10^{24}$ and the table would need $10^{21}$ bytes, so the algorithm cannot be applied to them.
- The cost depends on $T$ and not only on $n$: for a small $n$ and large values, such as $n = 30$ numbers with 25 digits, it is far slower than meet in the middle.
- The memory grows linearly with $T$, as $O(T)$ bits, while meet in the middle needs only $O(2^{n/2})$ words. For a small $n$ and large values dynamic programming therefore needs far more memory, and for $T = 10^{10}$ already $1.25$ GB.

### 4.8 When to use it

**Comparison with meet in the middle (Section 3).** Meet in the middle takes about $n \cdot 2^{n/2}$ operations whatever the values are; dynamic programming takes about $n \cdot T/64$ and depends on the values only through $T$. The factor $n$ is common to both costs and cancels, so dynamic programming is the cheaper one when $T/64 < 2^{n/2}$, that is, when $T < 2^{n/2+6}$ or, equivalently,

$$n > 2 \log_2 T - 12.$$

For $T \le T_{DP} = 10^{10}$ the right-hand side is at most $54.5$, so dynamic programming is the cheaper one for every $n \ge 55$. The memory behaves similarly: dynamic programming needs $T/8$ bytes and meet in the middle at least $16 \cdot 2^{n/2}$ bytes, so dynamic programming uses less memory when $n > 2 \log_2 T - 14$. In addition, meet in the middle is run only for $n \le 60$, because it stores $2^{n/2}$ sums per half, which is already $2^{30}$ for $n = 60$. Dynamic programming has no such bound on $n$.

Two examples show how the choice is made.

|                             | Example 1                                        | Example 2                                   |
| --------------------------- | ------------------------------------------------ | ------------------------------------------- |
| Input                       | $n = 1000$ elements below $10^4$                 | $n = 30$ elements with 25 digits            |
| Target sum                  | $T \le 5 \cdot 10^{6}$                           | $T$ of order $10^{25}$                      |
| Dynamic programming, time   | $n \cdot T/64 \approx 8 \cdot 10^{7}$ operations | not applicable, because $T > T_{DP}$        |
| Dynamic programming, memory | $T/8 \le 0.7$ MB                                 | not applicable, because $T > T_{DP}$        |
| Meet in the middle, time    | not applicable, because $n > 60$                 | $n \cdot 2^{n/2} \approx 10^{6}$ operations |
| Meet in the middle, memory  | not applicable, because $n > 60$                 | $16 \cdot 2^{n/2} \approx 0.5$ MB per half  |
| Choice                      | dynamic programming                              | meet in the middle                          |

In short, dynamic programming is the right choice for **many elements with small values**: $T \le T_{DP}$ together with either $n > 60$ or $n > 2 \log_2 T - 12$. The same holds for memory: dynamic programming needs $O(T)$ bits and meet in the middle $O(2^{n/2})$ words, so dynamic programming is also the smaller one when $n > 2 \log_2 T - 14$. For large values it is replaced by meet in the middle as long as $n$ is small enough, and when neither exact method is in range only an approximation is possible.

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
| A. Brute force | yes | $O(n \cdot 2^n)$, $O(2^n)$ with Gray code | $O(n)$ | no | $n \le 30$ |
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

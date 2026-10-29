# Stage 2: Documentation of Algorithms (basic version)

## 1. Problem statement

### 1.1 Introduction

Let $M = \{a_1, \dots, a_n\}$ be a finite multiset of non-negative integers, that is, a collection in which values may repeat. A *partition* of $M$ is a division of it into two non-empty parts $A$ and $B$. It is *balanced* if $\Sigma(A) = \Sigma(B)$, where $\Sigma$ denotes the sum of the elements of a part.

For example, $\{3, 1, 4, 2\}$ has the balanced partition $A = \{3, 2\}$, $B = \{1, 4\}$, with $\Sigma(A) = \Sigma(B) = 5$. The multiset $\{1, 1, 4\}$ has no balanced partition: it would require $\Sigma(A) = \Sigma(M)/2 = 3$, but no subset of $\{1, 1, 4\}$ has sum $3$.

We consider two problems. Section 1.2 gives their formal definitions.

- **The decision problem.** Given $M$, decide whether a balanced partition exists. The answer is YES or NO. Each exact algorithm below additionally returns a balanced partition when the answer is YES, which certifies the answer in $O(n)$ additions.
- **The optimisation problem.** Given $M$, find a partition minimising the *discrepancy* $d(A, B) = |\Sigma(A) - \Sigma(B)|$. A partition is balanced if and only if $d(A, B) = 0$.

The decision problem is NP-complete [Karp 1972]. Hence no polynomial-time algorithm for it is known, and none exists unless P = NP. It is NP-complete only in the weak sense: it admits an algorithm whose running time is polynomial in $n$ and $\Sigma(M)$, a *pseudo-polynomial* algorithm [Garey and Johnson 1979]. Two parameters therefore govern the cost: the number of elements $n$ and the magnitude of $\Sigma(M)$. The algorithms below differ in which of them their cost depends on.

We present four algorithms.

- **Section 2, brute force.** It enumerates all $2^n$ subsets. Time $O(2^n)$, memory $O(n)$.
- **Section 3, meet in the middle.** It splits $M$ into two halves and combines their subset sums [Horowitz and Sahni 1974]. Time $O(n \cdot 2^{n/2})$, memory $O(2^{n/2})$, independent of the magnitude of the elements.
- **Section 4, dynamic programming.** It computes the set of reachable subset sums. Time $O(n \cdot \Sigma(M))$, which is pseudo-polynomial.
- **Section 5, Karmarkar-Karp differencing.** This is a polynomial-time approximation algorithm, with time $O(n \log n)$ [Karmarkar and Karp 1982]. It always returns a partition, and its discrepancy is bounded (Section 5.4). We also quote its expected behaviour on random inputs [Yakir 1996].

None of the four algorithms is efficient for every input. Section 6 defines a final algorithm that selects, for each input, the cheapest method whose limits are satisfied. It applies dynamic programming if $\Sigma(M)/2 \le T_{DP}$, meet in the middle if $n \le N_{MITM}$, and the approximation otherwise; the limits $T_{DP}$ and $N_{MITM}$ are fixed in Section 6. For each algorithm we give pseudocode, a description, a correctness proof and a complexity analysis.

The remainder of this section is organised as follows: the assumptions on the input (below), the formal definitions (Section 1.2), the hardness results (Section 1.3) and the computational model (Section 1.4).

**Assumptions.** The problem statement fixes only the form of the input, so we state our assumptions explicitly. The definitions, the proofs and the complexity bounds below all rest on them. An implementation rejects an input that violates one of them with an error message; it never truncates or wraps a value around silently. Some assumptions are repeated where they are used.

| Assumption | Reason |
| --- | --- |
| Every element is a non-negative integer. Zero is allowed. | All algorithms rely on integer arithmetic, in particular on the parity test of the total and on the dynamic programming table indexed by sums. Zero fits the definitions and needs no special case: every algorithm treats it like any other value. |
| Values may repeat (the input is a multiset) and are distinguished by their position $1, \dots, n$, never by their value. | Equal elements are different items of the multiset. Each of them must be placed in a group on its own. |
| The multiset has at least two elements, $n \ge 2$. | A partition into two non-empty groups needs at least two elements. A multiset with a single element has no such partition and is reported as an error. |
| Both groups of a partition are non-empty. | We read a partition into *two* groups as one into two non-empty groups. The requirement costs nothing: as shown under *Trivial cases* in Section 1.2, it matters only when all elements are $0$, where the group $\{1\}$ against the rest is used. |
| Every element satisfies $0 \le a_i < 10^{38}$, and the total satisfies $\Sigma(M) < 2^{128}$. | Numbers are unsigned 128-bit integers (Section 1.4). The bound on the total is an assumption and is not repaired by arithmetic modulo $2^{128}$: equal remainders do not imply equal sums, and the target $\Sigma(M)/2$ cannot be recovered from the remainder of $\Sigma(M)$. An input whose total reaches $2^{128}$ is rejected. For numbers of $25$ digits the bound allows more than $10^{13}$ elements. |
| A file may contain several multisets. Each is solved on its own, and the answers are independent. | The multisets do not interact. |
| Each exponential algorithm is applied only within a stated range of $n$ or of the target sum (Section 1.4 and the summary table in Section 7). | Outside its range the algorithm may not finish or may not fit in memory. The final algorithm of Section 6 then falls back to the approximation. |

### 1.2 Definitions (report item a)

**Multiset.** A multiset $M$ over $\mathbb{N} = \{0, 1, 2, \dots\}$ is a pair $(U, m)$ where $U$ is a finite set and $m: U \to \{1, 2, 3, \dots\}$ gives the multiplicity of each element. We write it as a list $M = \{a_1, a_2, \dots, a_n\}$ in which values may repeat, and $n = \sum_{u \in U} m(u)$ is its size.

**Sum.** $\Sigma(M) = \sum_{i=1}^{n} a_i$. For an index set $I \subseteq \{1,\dots,n\}$, $\Sigma(I) = \sum_{i \in I} a_i$.

**Partition into two subsets.** A pair $(A, B)$ of **non-empty** index sets with $A \cup B = \{1,\dots,n\}$ and $A \cap B = \emptyset$. The assignment asks for a partition into *two* subsets, so we read a subset as a non-empty one; the consequences are discussed under *Constraints on the input* below.

**Balanced partition.** A partition with $\Sigma(A) = \Sigma(B)$, equivalently $\Sigma(A) = \Sigma(M)/2$.

**Discrepancy.** $d(A,B) = |\Sigma(A) - \Sigma(B)|$. A partition is balanced iff $d = 0$. This is the quality measure for the approximation algorithm.

**The decision problem PARTITION.** *Instance:* a multiset $M$ of non-negative integers. *Question:* does a balanced partition exist?

**The optimisation problem.** Find a partition minimising $d(A,B)$. Its optimum is $0$ iff the decision instance is a yes-instance, so an exact optimiser also decides PARTITION.

**Target sum.** $T = \Sigma(M)/2$. If $\Sigma(M)$ is odd there is no balanced partition. (Trivial necessary condition, checked first by every algorithm.)

**Trivial cases.** Every algorithm below starts with the same two checks. If $\Sigma(M)$ is odd, the answer is NO. If $\Sigma(M) = 0$, every element is $0$ and the partition $A = \{1\}$, $B = \{2, \dots, n\}$ is balanced, so the answer is YES with this partition; it has two non-empty sides because $n \ge 2$. After these checks the algorithms may assume $\Sigma(M) > 0$, hence $T > 0$. Then any index set $A$ with $\Sigma(A) = T$ is automatically non-empty, and so is its complement, because $\Sigma(B) = \Sigma(M) - T = T > 0$. This is why the requirement of non-empty sides never appears in the correctness proofs: once $\Sigma(M) > 0$, a balanced partition in the loose sense (sides allowed to be empty) is already a balanced partition in our sense.

### 1.3 Hardness (report item a, cited)

- PARTITION is **NP-complete**. It is one of Karp's 21 problems [Karp 1972]; the reduction is from SUBSET SUM (or 3-dimensional matching in Karp's original). Membership in NP: a partition is a certificate checkable in $O(n)$ additions.
- PARTITION is **weakly** NP-hard, not strongly: it has a pseudo-polynomial algorithm (Section 4) whose running time is polynomial in $n$ and in the *magnitude* $\Sigma(M)$, but exponential in the *number of digits* of the input [Garey and Johnson 1979, Section 4.2].
- Consequence for us: with 25-digit numbers (as in `AaC_example_2.txt`) the pseudo-polynomial algorithm is infeasible, and the only exact approach that scales is one whose cost depends on $n$ alone (Section 3). This is the central argument of the whole document and should be stated early.

### 1.4 Computational model and number representation

All numbers are **unsigned 128-bit integers**. This is enough for the numbers of interest: $2^{128} - 1 \approx 3.4 \cdot 10^{38}$ has $39$ digits, and the elements are non-negative, so no sign bit is wasted. Consequences used throughout the analysis:

- Comparison, addition and subtraction cost $O(1)$.
- Input constraints (Section 1.1): each $0 \le a_i < 10^{38}$, and the total $\Sigma(M) < 2^{128}$. Both are checked on input. Every intermediate value in every algorithm below is a subset sum or a difference of two subset sums, hence bounded by $\Sigma(M)$, so no other overflow can occur.
- Some algorithms need less than this. Karmarkar-Karp (Section 5) handles only differences, each at most $\max_i a_i$, and dynamic programming (Section 4) handles sums up to $\Sigma(M)/2 \le T_{DP}$, so neither can overflow.
- The range of $n$ in which each exact algorithm is run is given in the summary table (Section 7).
- Therefore every complexity bound below is stated in the **unit-cost RAM model**: an arithmetic operation on a number costs $O(1)$, and the bounds are expressed in $n$ and, for dynamic programming, in $\Sigma(M)$ only.

Memory is counted in machine words of 128 bits.

---

## 2. Algorithm A: Brute force (exhaustive search)

**Role.** Reference solution for correctness testing of everything else. It also serves as the baseline in the Stage 4 time measurements. It is an exact algorithm with exponential time; the polynomial-time approximation that goes with it (task.md 4b, CONSTITUTION 6) is Algorithm D in Section 5.

### 2.1 Idea

Every partition $(A, B)$ of the indices $\{1, \dots, n\}$ corresponds to a bit vector $x \in \{0,1\}^n$ with $x_i = 1$ if and only if $i \in A$. The algorithm enumerates these vectors one after another, computes $\Sigma(A)$ for each, and stops as soon as $\Sigma(A) = T$. If the enumeration ends without a hit, no balanced partition exists.

**Halving trick.** Swapping $A$ and $B$ gives the same two parts in the opposite order, with the same discrepancy, and a balanced pair stays balanced. So we may assume that index $n$ lies in $B$. Only the first $n-1$ bits are then free, and $2^{n-1}$ vectors suffice instead of $2^n$. The vector is stored as an integer `mask` in $\{0, \dots, 2^{n-1} - 1\}$, where bit $i-1$ of `mask` is $x_i$.

**Why indices and not values.** The input may contain repeated values (course email of 16.10.26). The enumeration runs over index sets, so equal values at different positions are different elements and no special treatment of duplicates is needed. Elements equal to $0$ are handled in the same way.

### 2.2 Pseudocode

```
BRUTE-FORCE(a[1..n])
    S ← Σ a[i]                              // with overflow check, see Section 2.7
    if S is odd then return NO
    if S = 0 then return YES, A = {1}, B = {2, ..., n}      // trivial case, Section 1.2
    T ← S / 2
    for mask ← 0 to 2^(n−1) − 1 do          // bit i−1 of mask puts a[i] in A; a[n] is always in B
        sumA ← 0
        for i ← 1 to n − 1 do
            if bit (i−1) of mask = 1 then sumA ← sumA + a[i]
        if sumA = T then
            return YES, A = { i : bit (i−1) of mask = 1 }, B = the other indices
    return NO
```

The two trivial cases of Section 1.2 are answered before the loop. After them $T > 0$, so `mask` $= 0$ (which is $A = \emptyset$) never succeeds, and every returned $A$ and its complement are non-empty. The input has $n \ge 2$ (Section 1.1), so the loop runs over at least two masks.

### 2.3 Gray-code variant

In the simple version every mask recomputes its sum from scratch in $n - 1$ steps. If the masks are visited in the order of the **binary reflected Gray code**, two consecutive masks differ in exactly one bit, so the sum can be updated with one addition or one subtraction.

Let $m = n - 1$ be the number of free bits. The $k$-th Gray code word is $g_k = k \oplus (k \gg 1)$ ($\oplus$ is bit-wise exclusive or, $\gg$ is a right shift). Two facts are used:

1. $k \mapsto g_k$ is a bijection of $\{0, \dots, 2^{m} - 1\}$ onto itself, so every mask is visited exactly once.
2. $g_k \oplus g_{k-1} = 2^{j}$ where $j$ is the position of the lowest set bit of $k$ (`ctz(k)`, count of trailing zeros). So going from $g_{k-1}$ to $g_k$ flips only bit $j$.

```
BRUTE-FORCE-GRAY(a[1..n])
    S ← Σ a[i]
    if S is odd then return NO
    if S = 0 then return YES, A = {1}, B = {2, ..., n}      // trivial case, Section 1.2
    T ← S / 2
    m ← n − 1
    g ← 0;  sumA ← 0                        // g is the current mask, A = ∅, and sumA = 0 < T
    for k ← 1 to 2^m − 1 do
        j ← ctz(k)                          // the one bit that changes
        g ← g XOR 2^j
        if bit j of g = 1 then sumA ← sumA + a[j+1]     // a[j+1] entered A
        else                   sumA ← sumA − a[j+1]     // a[j+1] left A
        if sumA = T then
            return YES, A = { i : bit (i−1) of g = 1 }
    return NO
```

The subtraction never underflows: a value is subtracted only if it was added before and not yet removed, so it is part of `sumA`. The invariant is that `sumA` $= \sum_{i:\,\text{bit}(i-1)\text{ of }g = 1} a_i$ after every step. The simple version of Section 2.2 is the documented reference and the test oracle, because it is the one that is obviously correct. We propose to implement the Gray-code version as well, for speed in the Stage 4 measurements, and to check it against the simple version in the tests (the two must give the same YES/NO answer). Whether the Gray-code version is implemented is a team decision.

### 2.4 Example

**A YES instance.** Let $a = (3, 1, 4, 2)$, the same set as in Section 4.4. Then $S = 10$ is even, $T = 5$, $n = 4$, so $a_4 = 2$ stays in $B$ and the mask has $3$ bits: bit 0 is $a_1 = 3$, bit 1 is $a_2 = 1$, bit 2 is $a_3 = 4$.

The masks $0, 1, \dots, 6$ give the sums $\Sigma(A) = 0, 3, 1, 4, 4, 7, 5$, and only the last one equals $T$.

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

**Lemma 1 (reduction to $n \in B$).** Let $S$ be even and $T = S/2$. A balanced partition of $\{1, \dots, n\}$ exists if and only if there is a subset $A \subseteq \{1, \dots, n-1\}$ with $\Sigma(A) = T$.

_Proof._ ($\Leftarrow$) Take $B = \{1, \dots, n\} \setminus A$. Then $\Sigma(B) = S - T = T = \Sigma(A)$. ($\Rightarrow$) Let $(A, B)$ be balanced. If $n \in B$, then $A \subseteq \{1, \dots, n-1\}$ and $\Sigma(A) = T$ because a balanced partition has $\Sigma(A) = S/2$. Otherwise $n \in A$, and then $(B, A)$ is also balanced, has $n$ in its second part, and its first part $B \subseteq \{1, \dots, n-1\}$ has $\Sigma(B) = T$. $\square$

**Theorem.** `BRUTE-FORCE` and `BRUTE-FORCE-GRAY` return YES if and only if a balanced partition exists, and the sets they return form a balanced partition.

_Proof._ If $S$ is odd, no balanced partition exists, because $\Sigma(A) = \Sigma(B)$ would give $S = 2\Sigma(A)$, and NO is correct. If $S = 0$, the returned partition $\{1\}$, $\{2, \dots, n\}$ has two zero sums and two non-empty sides since $n \ge 2$, so YES is correct. Let $S$ be even and positive, so $T > 0$. _Soundness:_ the algorithm answers YES only when `sumA` $= T$, and `sumA` is the sum of the returned set $A$ (in the simple version by construction of the inner loop, in the Gray version by the invariant of Section 2.3, which follows by induction on $k$ since every step adds or subtracts the one element whose bit was flipped). Then $B$ is the complement and $\Sigma(B) = S - T = T$, so the partition is balanced, and both sides are non-empty because both sums are $T > 0$. _Completeness:_ if a balanced partition exists, Lemma 1 gives a subset $A \subseteq \{1, \dots, n-1\}$ with $\Sigma(A) = T$, and by Lemma 0 its mask is visited unless the algorithm stops earlier. It stops earlier only with a YES, which is correct by soundness. If the loop ends without a hit, no such subset exists, hence by Lemma 1 no balanced partition exists, and NO is correct. The algorithm terminates because the loop runs over a finite range. $\square$

Duplicates and zeros need no case analysis, since the proof works on index sets and never uses that the values are distinct or positive.

### 2.6 Complexity

We count operations on 128-bit numbers in the unit-cost model of Section 1.4 and use $m = n - 1$.

- **Preliminaries.** The sum and the parity test cost $O(n)$.
- **Simple version.** The outer loop has $2^{n-1}$ masks (in the worst case all of them, when the answer is NO or the hit is the last mask) and the inner loop has $n - 1$ iterations with one test and at most one addition. The worst-case time is therefore $\Theta(n \cdot 2^{n-1}) = \Theta(n \cdot 2^n)$, that is $O(n \cdot 2^n)$.
- **Gray-code version.** Each step does one bit-flip, one `ctz`, one addition or subtraction and one comparison, which is $O(1)$ ($\text{ctz}$ is a single instruction on current processors, and even when computed by a loop it takes $O(1)$ amortised, because it inspects $j + 1$ bits and half of all $k$ have $j = 0$, a quarter have $j = 1$, and so on). The worst-case time is $\Theta(2^{n-1}) = O(2^n)$, which is smaller by a factor $n$.
- **Recovering the partition.** On YES, building $A$ and $B$ from the mask costs $O(n)$ once.
- **Memory.** The input of $n$ numbers, `sumA`, `S`, `T` and one mask: $O(n)$ words in total, and $O(1)$ beyond the input. The mask fits in a `uint64_t` while $n \le 64$, which is far above the limit of Section 2.7.
- **Best and worst case.** The best case is an odd $S$ or $S = 0$, both answered before the loop in $O(n)$. The worst case is a NO instance with even $S$, which executes all masks. The running time does **not** depend on the magnitude of the numbers, only on $n$.

### 2.7 Constraints and limitations

**Constraints that the program enforces**:

1. **Preconditions.** The input has $n \ge 2$ elements (Section 1.1). The bound $0 \le a_i < 10^{38}$ is checked when the input is parsed, and $S < 2^{128}$ is checked while summing. If $S$ is odd or $S = 0$, the program answers without enumerating.
2. **Bound on $n$.** The exact algorithm is run only for $n \le 30$. For a larger $n$ the program reports SKIPPED together with the violated condition. The threshold can be changed on the command line so that the Stage 4 experiments can go past it on purpose.

**Limitations.** The time doubles with every additional element. Assuming $10^{9}$ Gray-code steps per second, which is optimistic for 128-bit arithmetic and has to be measured in Stage 4:

| $n$ | masks $2^{n-1}$ | time (Gray code, $10^9$ steps/s) |
| --- | --------------- | -------------------------------- |
| 20  | $5.2 \cdot 10^{5}$  | under a millisecond          |
| 30  | $5.4 \cdot 10^{8}$  | about $0.5$ s                |
| 40  | $5.5 \cdot 10^{11}$ | about $9$ min                |
| 50  | $5.6 \cdot 10^{14}$ | about $6.5$ days             |
| 60  | $5.8 \cdot 10^{17}$ | about $18$ years             |

The simple version needs about $n - 1$ times as many steps, so at $n = 30$ it takes tens of seconds rather than seconds. This is the reason for the limit $n \le 30$. The size of the numbers plays no role: `AaC_example_2.txt` has 25-digit numbers but only $n = 10$ elements, so it needs just $2^{9} = 512$ masks and is solved instantly, while a set with $n = 100$ small numbers is out of reach for this algorithm.

### 2.8 When to use it

Brute force is **never** the algorithm of choice for the final program; its role is the reference. Its value lies in being obviously correct: it contains no cleverness that could hide a bug, uses $O(n)$ memory, and does not depend on the magnitude of the numbers. We therefore use it in Stages 3 and 4 as follows:

- **Oracle in the tests.** On random multisets with $n \le 20$ (including duplicates, zeros and odd sums) the answers of B, C (when $S$ is small) and E are compared with A, and every returned partition is checked to have two sums equal to $T$.
- **Baseline in the time experiments.** The time of A against $n$ shows the $2^n$ growth that motivates B (about $2^{n/2}$) and C (about $n \cdot T / 64$), see Sections 3.5 and 4.8.

---

## 3. Algorithm B: Meet in the middle (main exact algorithm)

**Source.** Horowitz and Sahni 1974, originally for KNAPSACK / SUBSET SUM. Standard exposition in Cormen et al. (CLRS) exercises and in Kleinberg-Tardos.

### 3.1 Idea

The algorithm reduces the number of combinations that must be considered by splitting the input into two parts. Let

$$
L = \{1,\dots,h\}, \qquad R = \{h+1,\dots,n\}, \qquad h = \lfloor n/2 \rfloor.
$$

Every subset $A$ of the whole input has exactly one decomposition

$$
A = (A \cap L) \cup (A \cap R).
$$

Consequently, its sum is the sum of one subset sum from the left half and one subset sum from the right half. The algorithm therefore works in four stages:

1. Compute the total sum $S$ and reject the instance if $S$ is odd. Otherwise set the target to $T = S/2$.
2. Enumerate every subset of $L$ and store its sum together with its bit mask in a list `LS`.
3. Enumerate every subset of $R$ and store its sum together with its bit mask in a list `RS`.
4. Search the two lists for a pair whose sums add to $T$. When such a pair is found, the two masks identify one side of the partition; the other side is its complement.

There are $2^h$ subsets of $L$ and $2^{n-h}$ subsets of $R$, rather than $2^n$ subsets in one list. The lists are sorted so that a two-pointer sweep can find a pair with sum $T$ without testing every possible pair. The algorithm is exact: it does not discard any subset, and it returns NO only after every possible left/right combination has been ruled out.

The running time depends on $n$, not on the magnitude of the input values. This makes meet in the middle the main exact algorithm for inputs containing 25-digit numbers. Its disadvantage is the memory required for the two lists.

### 3.2 Pseudocode

```
MEET-IN-THE-MIDDLE(a[1..n])
    S ← Σ a[i]
    if S is odd then return NO
    if S = 0 then return YES, A = {1}, B = {2, ..., n}      // trivial case, Section 1.2
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

`SUBSET-SUMS` represents a subset by a bit mask. Bit $k$ is 1 exactly when the corresponding element belongs to the subset. To calculate a new sum, it removes one set bit from `mask`; the remaining mask has already been processed, so the new sum is obtained with one addition. The stored mask is retained because a numerical sum alone is not enough to reconstruct the partition.

After both lists have been generated, `LS` is sorted in ascending order and `RS` in descending order. The pointers initially refer to the smallest left sum and the largest right sum. If their total is less than $T$, the right sum cannot be increased while keeping the current left sum, so the left pointer is advanced. If their total is greater than $T$, the left sum cannot be decreased while keeping the current right sum, so the right pointer is advanced. Each pointer only moves forward, and the sweep stops when it finds the target or one list is exhausted.

### 3.3 Example

Consider the input

$$
a = (3,1,4,2).
$$

The total is $S = 10$, so the target is $T = 5$. With $h = 2$, the left half is $(3,1)$ and the right half is $(4,2)$.

The subset lists, including the empty subset, are:

The rightmost mask bit is bit 0 and refers to the first element in that half. Thus, for the left half, `01` means $\{3\}$ and `10` means $\{1\}$; for the right half, `01` means $\{4\}$ and `10` means $\{2\}$.

| mask | left subset | left sum | mask | right subset | right sum |
| ---- | ----------- | -------- | ---- | ------------ | --------- |
| 00 | $\emptyset$ | $0$ | 00 | $\emptyset$ | $0$ |
| 01 | $\{3\}$ | $3$ | 01 | $\{4\}$ | $4$ |
| 10 | $\{1\}$ | $1$ | 10 | $\{2\}$ | $2$ |
| 11 | $\{3,1\}$ | $4$ | 11 | $\{4,2\}$ | $6$ |

After sorting, the left sums are $(0,1,3,4)$ and the right sums are $(6,4,2,0)$ in the order used by the sweep. The pointer steps are:

| left sum | right sum | total | action |
| -------- | --------- | ----- | ------ |
| $0$ | $6$ | $6$ | too large; advance the right pointer |
| $0$ | $4$ | $4$ | too small; advance the left pointer |
| $1$ | $4$ | $5$ | target found |

The corresponding subsets are $\{1\}$ from the left half and $\{4\}$ from the right half. Thus one side is $\{1,4\}$ and the other is $\{3,2\}$; both sums are $5$. The algorithm stops as soon as this pair is found.

If the total sum is odd, for example $a=(2,3,4)$ with $S=9$, the algorithm stops before generating either list and returns NO. If no pair of list entries adds to $T$, the sweep exhausts one list and returns NO.

### 3.4 Correctness

**Lemma 1 (subset-sum enumeration).** `SUBSET-SUMS(b[1..m])` returns exactly one entry for every subset of the $m$ indexed elements, and the stored sum is the sum of that subset.

_Proof._ We use induction on the mask value. The mask 0 represents the empty subset and has sum 0. For a nonzero mask, the algorithm chooses one set bit $k$. The mask with this bit removed is a smaller mask, so by the induction hypothesis its stored sum is correct. Adding $b[k+1]$ gives precisely the sum of the original mask. Every mask has one unique bit pattern, so every subset is represented once. $\square$

**Lemma 2 (reduction to two lists).** A subset $A \subseteq \{1,\dots,n\}$ with sum $T$ exists if and only if there are entries $s_L$ in `LS` and $s_R$ in `RS` such that $s_L+s_R=T$.

_Proof._ If $A$ exists, take $A_L=A\cap L$ and $A_R=A\cap R$. These are represented in the two lists by Lemma 1, and

$$
\Sigma(A)=\Sigma(A_L)+\Sigma(A_R)=T.
$$

Conversely, if two list entries have sums adding to $T$, take the union of their represented subsets. The halves are disjoint, so the union is a subset of the whole input with sum $T$. $\square$

**Lemma 3 (two-pointer sweep).** The sweep does not skip any pair of entries that could have sum $T$.

_Proof._ Suppose the current entries have sums $x$ from `LS` and $y$ from `RS`.

- If $x+y<T$, `RS` is descending, so every later right entry is at most $y$. Therefore $x+y' \le x+y<T$ for every right entry $y'$ at or after the current right pointer. No such entry can complete the current $x$ to $T$, so advancing the left pointer is safe.
- If $x+y>T$, `LS` is ascending, so every later left entry is at least $x$. Therefore $x'+y \ge x+y>T$ for every left entry $x'$ at or after the current left pointer. No such entry can complete the current $y$ to $T$, so advancing the right pointer is safe.

When $x+y=T$, the required pair has been found. Repeating these safe steps cannot remove a possible pair before it is examined. $\square$

**Theorem.** `MEET-IN-THE-MIDDLE` returns YES if and only if a balanced partition exists, and the returned sets form a balanced partition.

_Proof._ If $S$ is odd, a balanced partition is impossible because its two equal sums would add to the odd number $S$, so returning NO is correct. If $S = 0$, the returned partition $\{1\}$, $\{2, \dots, n\}$ is balanced with two non-empty sides (Section 1.2). Let $S$ be even and positive, so $T > 0$. By Lemma 2, a balanced partition exists exactly when some left and right list entries add to $T=S/2$. By Lemma 3, the sweep examines a pair if it could have the required sum, and therefore returns YES exactly when such a pair exists. The masks identify a subset $A$ with sum $T$; its complement has sum $S-T=T$, so the returned partition is balanced, and both sides are non-empty because $T > 0$ (the empty subsets in the two lists can only combine to the sum $0 \ne T$). If the sweep ends without finding a pair, no subset with sum $T$ exists, and Lemma 2 proves that NO is correct. $\square$

The proof uses indexed elements rather than distinct values. Therefore duplicate values produce separate masks and repeated sums when appropriate; they are neither merged nor lost. Zero-valued elements are also represented by masks and require no special case.

### 3.5 Complexity

Let $h = \lfloor n/2 \rfloor$.

- Generating sums: $O(2^{h})$ and $O(2^{n-h})$ with the lowest-set-bit recurrence (one addition each).
- Sorting: $O(2^{h} \cdot h)$ and $O(2^{n-h} \cdot (n-h))$, since $\log 2^{h} = h$.
- Sweep: $O(2^{h} + 2^{n-h})$.
- **Total time: $O(n \cdot 2^{n/2})$. Memory: $O(2^{n/2})$ words.**
- Independent of the magnitude of $a_i$ (under the 128-bit model). Compare with the DP below.
- Practical limit: memory. $n = 40$ gives $2^{20} \approx 10^6$ entries per side, trivial. $n = 44$ gives $2^{22}$ entries of 16 bytes plus 8-byte masks, about 100 MB per side. $n = 60$ gives $2^{30}$ entries, about 25 GB per side, infeasible. So $n \le 44$ in practice.

### 3.6 Constraints and limitations

**Constraints that the program enforces**:

1. **Preconditions.** Each value satisfies $0 \le a_i < 10^{38}$, and the total sum satisfies $S < 2^{128}$. The parser and summation check these conditions and report an error instead of truncating a value.
2. **Bound on $n$.** The exact algorithm is run only when $n \le 44$, so that the subset lists fit in memory (about 100 MB per half). A larger input is reported as SKIPPED because the subset lists may require too much memory. The threshold is configurable for Stage 4 experiments.

**Limitations.** The algorithm avoids dependence on the numeric magnitude of the values, but its memory and running time still grow exponentially with the number of elements. At $n=60$, each half has about $2^{30}$ subsets, so storing sums and masks requires many gigabytes. Meet in the middle is therefore unsuitable for large $n$, even when the input values are small. For such inputs the program uses dynamic programming when the target is small, or the polynomial-time approximation when neither exact method is feasible.

### 3.7 When to use it

Meet in the middle is the preferred exact algorithm when the input has relatively few elements and the target sum is too large for dynamic programming. It is especially useful for large values such as those in `AaC_example_2.txt`, because its cost depends on $n$ rather than on the number of digits. The final algorithm-selection procedure compares the applicable limits for dynamic programming and meet in the middle before falling back to the approximation algorithm.

### 3.8 Possible further improvement
 Note: A possible further improvement to mention (not to implement unless time allows): Schroeppel-Shamir 1981 reduces memory to $O(2^{n/4})$ with the same time.

---

## 4. Dynamic programming over subset sums

Exact algorithm whose cost depends on the size of the numbers. Choosing for each element whether it joins the subset or not spans a decision tree with $2^n$ leaves, but many branches reach the same partial sum. The algorithm memoizes every partial result in a table the first time it is obtained and reuses it afterwards. This replaces the $2^n$ branches by a cost proportional to $n$ times the target sum, at the price of memory proportional to the target sum; whether this is a gain depends on how large the target is (Sections 4.6 and 4.8).

### 4.1 From the partition problem to subset sum

Let $M = \{a_1, \dots, a_n\}$ be the input multiset and $S = \Sigma(M)$ its total sum (Section 1.2).

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
    if S = 0 then return TRUE           // Step 1', trivial case of Section 1.2: A = {1}, B = {2, ..., n}
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

_Proof._ If $S$ is odd, there is no balanced partition (Step 1), and FALSE is correct. If $S = 0$, the partition $\{1\}$, $\{2, \dots, n\}$ is balanced (Section 1.2), and TRUE is correct (Step 1'). From now on $T > 0$, so a subset with sum $T$ is non-empty and so is its complement. If $\max a_i > T$, FALSE is correct (Step 4). Otherwise, by Lemma 2 the final value of `reach[T]` is $R_n(T)$, unless the loop stopped earlier at a step $i < n$ with `reach[T]` TRUE; this is also correct, because a subset of $\{a_1, \dots, a_i\}$ with sum $T$ is also a subset of $\{a_1, \dots, a_n\}$, so $R_i(T)$ implies $R_n(T)$. By the definition of $R_n$ this holds if and only if a subset with sum $T$ exists, and by Lemma 1 this is equivalent to the existence of a balanced partition. $\square$

### 4.6 Complexity

Let $T = S/2$ and $w = 64$ be the word size of the bitset. We count operations on machine words in the unit-cost model of Section 1.4.

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

For $T \le T_{DP} = 10^{10}$ the right-hand side is at most $54.5$, so dynamic programming is the cheaper one for every $n \ge 55$. The memory behaves similarly: dynamic programming needs $T/8$ bytes and meet in the middle at least $16 \cdot 2^{n/2}$ bytes, so dynamic programming uses less memory when $n > 2 \log_2 T - 14$. In addition, meet in the middle is run only for $n \le 44$, because it stores $2^{n/2}$ sums per half, which is already about 100 MB per half for $n = 44$. Dynamic programming has no such bound on $n$.

Two examples show how the choice is made.

|                             | Example 1                                        | Example 2                                   |
| --------------------------- | ------------------------------------------------ | ------------------------------------------- |
| Input                       | $n = 1000$ elements below $10^4$                 | $n = 30$ elements with 25 digits            |
| Target sum                  | $T \le 5 \cdot 10^{6}$                           | $T$ of order $10^{25}$                      |
| Dynamic programming, time   | $n \cdot T/64 \approx 8 \cdot 10^{7}$ operations | not applicable, because $T > T_{DP}$        |
| Dynamic programming, memory | $T/8 \le 0.7$ MB                                 | not applicable, because $T > T_{DP}$        |
| Meet in the middle, time    | not applicable, because $n > 44$                 | $n \cdot 2^{n/2} \approx 10^{6}$ operations |
| Meet in the middle, memory  | not applicable, because $n > 44$                 | $16 \cdot 2^{n/2} \approx 0.5$ MB per half  |
| Choice                      | dynamic programming                              | meet in the middle                          |

In short, dynamic programming is the right choice for **many elements with small values**: $T \le T_{DP}$ together with either $n > 44$ or $n > 2 \log_2 T - 12$. The same holds for memory: dynamic programming needs $O(T)$ bits and meet in the middle $O(2^{n/2})$ words, so dynamic programming is also the smaller one when $n > 2 \log_2 T - 14$. For large values it is replaced by meet in the middle as long as $n$ is small enough, and when neither exact method is in range only an approximation is possible.

---

## 5. Algorithm D: Karmarkar-Karp differencing (polynomial-time approximation)

**Source.** Karmarkar and Karp 1982 (technical report, UC Berkeley, "The differencing method of set partitioning"). Worst-case quality: Fischetti and Martello 1987 (ratio $7/6$). Average-case quality: Yakir 1996 (expected discrepancy $n^{-\Theta(\log n)}$ for random items); overviews in Korf 1998 and Mertens 2006. The algorithm is the polynomial-time approximation required by the rule that every exponential exact algorithm has one next to it (task.md 4b, CONSTITUTION 6), here for Algorithms A and B.

**Role.** It always runs, in time $O(n \log n)$ and without any dependence on the size of the numbers. It answers YES whenever its discrepancy is $0$, and otherwise it provides the partition that the program reports when the exact methods are out of range (Section 6).

### 5.1 Idea

Repeatedly take the two largest remaining numbers $x \ge y$ and decide that they go to **opposite** sides. Whatever happens to the other numbers, the pair then contributes only $x - y$ to the difference of the two sides, so the pair is replaced by the single number $x - y$. Repeat until one number is left; it is the discrepancy of the final partition. To recover the two sets, remember each decision as an edge "$u$ and $v$ are on opposite sides" and 2-colour the resulting graph at the end.

Why it is better than the simple greedy method (put the next largest number on the lighter side): greedy commits an element to a side at once. Differencing postpones the decision and only fixes the *relation* between two elements, which lets large numbers cancel each other.

### 5.2 Pseudocode

```
KARMARKAR-KARP(a[1..n])
    H ← max-heap of pairs (value, id) built from (a[i], i)      // ties are broken in any way
    E ← empty list of edges                                      // edges of the graph G on the vertices 1..n
    while |H| > 1 do
        (x, u) ← EXTRACT-MAX(H)
        (y, v) ← EXTRACT-MAX(H)                                  // x ≥ y
        INSERT(H, (x − y, u))                                    // the difference keeps the id of the larger element u
        append (u, v) to E                                       // u and v are put on opposite sides
    (d, _) ← the single entry left in H                          // d = discrepancy of the result
    colour the vertices of G = ({1..n}, E) with 0 and 1 so that the ends of every edge differ      // BFS or DFS
    A ← { i : colour(i) = 0 };  B ← { i : colour(i) = 1 }
    return (A, B, d)
```

The input has $n \ge 2$ (Section 1.1), so the loop runs at least once and $E$ is non-empty. The subtraction $x - y$ is always done with $x \ge y$, so unsigned 128-bit numbers never overflow. The pseudocode keeps the id of the **larger** element on purpose: Section 5.3 shows that this is what makes the colouring step correct.

### 5.3 Correctness

Let $G$ be the graph on the vertices $1, \dots, n$ with the edges of $E$ added so far. For a connected component $C$ of $G$, its **colour classes** are the two sides of a 2-colouring of $C$. If $C$ is a tree, the 2-colouring is unique up to swapping the two colours.

**Lemma 5.1 (invariant).** After every round of the loop (and before the first one) the following holds.

1. $G$ is a forest, and every connected component of $G$ has exactly one entry $(z, u)$ in $H$, with $u$ a vertex of that component.
2. For the entry $(z, u)$ of a component $C$, the sums of the two colour classes of $C$ differ by exactly $z$, and $u$ lies in a class with the larger (or equal) sum.

_Proof._ Induction on the number of rounds. Before the first round, every vertex is a component of its own, with the entry $(a_i, i)$. Its colour classes are $\{i\}$ and $\emptyset$, which differ by $a_i$, and $i$ lies in the larger one.

Assume the invariant holds before a round. The round extracts $(x, u)$ and $(y, v)$, which belong to two **different** components $P$ and $Q$, because every component has only one entry. The new edge $\{u, v\}$ joins two different components, so $G$ stays a forest, the merged component $P \cup Q$ is a tree, and it gets the single entry $(x - y, u)$. This proves part 1.

For part 2, let $P$ have classes $H_P$ (the larger sum, containing $u$) and $L_P$, so $\Sigma(H_P) - \Sigma(L_P) = x$. Let $Q$ have the classes $H_Q \ni v$ and $L_Q$ with $\Sigma(H_Q) - \Sigma(L_Q) = y$. The edge $\{u, v\}$ forces $u$ and $v$ to opposite colours, so the colour classes of the merged tree are $H_P \cup L_Q$ and $L_P \cup H_Q$. Their sums differ by
$$(\Sigma(H_P) + \Sigma(L_Q)) - (\Sigma(L_P) + \Sigma(H_Q)) = x - y \ge 0,$$
and the class $H_P \cup L_Q$ is the larger one and contains $u$. $\square$

**Theorem 5.1.** `KARMARKAR-KARP` terminates after $n - 1$ rounds, $G$ is a tree on all $n$ vertices, both $A$ and $B$ are non-empty, and the returned $d$ is the discrepancy of the returned partition: $d = |\Sigma(A) - \Sigma(B)|$.

_Proof._ Each round removes two entries and inserts one, so $|H|$ drops by $1$ and the loop ends after $n - 1$ rounds, with exactly one entry left. By Lemma 5.1 there is one component, so $G$ is a tree with $n - 1$ edges. A tree is bipartite, so the colouring exists, and $(A, B)$ are its two classes. By part 2 of the lemma, the single entry $d$ is the difference of their sums. Since $n \ge 2$, the tree has at least one edge, and the ends of an edge get different colours, so both colour classes $A$ and $B$ are non-empty. $\square$

**What it decides.** If $d = 0$, then $(A, B)$ is a balanced partition, so the answer YES is correct and the sets are a certificate. If $d > 0$, nothing follows: the algorithm is **not exact**, as the example in Section 5.5 shows. Because $d = |S - 2\Sigma(B)|$, the discrepancy always has the same parity as $S$.

**Duplicates and zeros.** Equal values need no special treatment. Replacing the two largest entries by their difference changes the multiset of values in the same way whichever of two equal values is extracted first, so $d$ does not depend on how ties are broken (only the ids, and hence possibly the returned sets, do). An element equal to $0$ is a normal entry.

### 5.4 Quality of the result

The quality measure is the discrepancy $d$. Since the optimum can be $0$, a ratio on $d$ is meaningless, so we state the following.

**Proposition 5.2 (bound on $d$).** Let $M = \max a_i$. Then $d \le M$.

_Proof._ Every entry of $H$ is at most $M$ at all times: this holds at the start, and the new entry $x - y \le x \le M$. The last entry is $d$. $\square$

This is the only guarantee on $d$ itself, and it is weak: the scaled instances $k \cdot (5, 6, 7, 8, 10)$ of Section 5.5 have optimum $0$ and give $d = 2k$, so no bound of the form "optimum plus a constant" exists.

**Ratio on the larger side.** If we look at the larger of the two sums, $\max(\Sigma(A), \Sigma(B)) = (S + d)/2$, there is a proper approximation ratio. The optimum is at least $\max(S/2, M)$, so Proposition 5.2 gives
$$\frac{(S + d)/2}{\mathrm{OPT}} \le \frac{S/2 + M/2}{\mathrm{OPT}} \le \frac{\mathrm{OPT} + \mathrm{OPT}/2}{\mathrm{OPT}} = \frac{3}{2}.$$
Fischetti and Martello [1987] show that the worst-case ratio of the differencing method is exactly $7/6$.

**Random inputs.** For items drawn independently from a uniform distribution, the expected discrepancy of the differencing method is $n^{-\Theta(\log n)}$ [Yakir 1996], which proves a conjecture of Karmarkar and Karp. We quote this result and do not reprove it. It says that on random inputs with many elements the discrepancy becomes extremely small, far below that of simple greedy methods. For our integer inputs this is only an indication, and we measure it in Stage 4.

### 5.5 Example

Let $a = (3, 1, 4, 2)$, the set of Sections 2.4 and 4.4. Ties are broken in favour of the smaller index.

| round | extracted $(x, u)$, $(y, v)$ | inserted $(x - y, u)$ | edge | heap after the round |
| ----- | ---------------------------- | --------------------- | ---- | -------------------- |
| 0     |                              |                       |      | $4_{a_3}, 3_{a_1}, 2_{a_4}, 1_{a_2}$ |
| 1     | $(4, a_3), (3, a_1)$         | $(1, a_3)$            | $a_3 - a_1$ | $2_{a_4}, 1_{a_2}, 1_{a_3}$ |
| 2     | $(2, a_4), (1, a_2)$         | $(1, a_4)$            | $a_4 - a_2$ | $1_{a_3}, 1_{a_4}$          |
| 3     | $(1, a_3), (1, a_4)$         | $(0, a_3)$            | $a_3 - a_4$ | $0_{a_3}$                   |

Here $x_{u}$ means the value $x$ with the id of $u$. The final entry is $d = 0$. The graph has the edges $a_3 a_1$, $a_4 a_2$, $a_3 a_4$, a path $a_1 - a_3 - a_4 - a_2$. Colouring it from $a_1 = 0$ gives $a_1 = 0$, $a_3 = 1$, $a_4 = 0$, $a_2 = 1$. So $A = \{a_1, a_4\} = \{3, 2\}$ and $B = \{a_2, a_3\} = \{1, 4\}$, both with sum $5$, and the answer YES is certified.

**An instance where it fails to find the optimum.** Let $a = (5, 6, 7, 8, 10)$, with $S = 36$ and $T = 18$. The rounds are $10 - 8 = 2$ (edge $a_5 a_4$), $7 - 6 = 1$ (edge $a_3 a_2$), $5 - 2 = 3$ (edge $a_1 a_5$) and $3 - 1 = 2$ (edge $a_1 a_3$), so $d = 2$. The colouring from $a_1 = 0$ gives $A = \{a_1, a_2, a_4\} = \{5, 6, 8\}$ with sum $19$ and $B = \{a_3, a_5\} = \{7, 10\}$ with sum $17$. But $\{8, 10\}$ and $\{5, 6, 7\}$ both have sum $18$, so the optimum is $0$. The same input shows that $d > 0$ does not prove that the answer is NO.

### 5.6 Complexity

We count operations on 128-bit numbers in the unit-cost model of Section 1.4.

- Building the heap with `heapify` costs $O(n)$.
- The loop has $n - 1$ rounds. Each round has two extractions and one insertion, $O(\log n)$ each, so $O(n \log n)$ in total.
- Colouring the tree with BFS or DFS over an adjacency list costs $O(n)$, because $G$ has $n$ vertices and $n - 1$ edges.
- **Total time: $O(n \log n)$. Memory: $O(n)$** for the heap, the edge list and the adjacency list.
- The running time does not depend on the size of the numbers, only on $n$.

### 5.7 Constraints and limitations

**Constraints that the program enforces**:

1. **Preconditions.** Each $0 \le a_i < 10^{38}$. The sum $S$ is not needed by the algorithm, and every difference $x - y$ is at most $\max a_i$, so no overflow can occur.
2. **Bound on $n$.** $1 \le n \le 10^6$. For $n = 10^6$ the heap, the edges and the adjacency list take well under $100$ MB, and about $3 \cdot 10^6$ heap operations of $20$ steps each are expected to run in under a second (to be measured in Stage 4). The algorithm is never skipped.

**Limitations.**

- It is a heuristic. The only guarantees are $d \le \max a_i$ and the ratio $7/6$ on the larger side; there is no bound on how far $d$ is from the optimal discrepancy.
- It cannot answer NO. A result $d > 0$ can occur on a YES instance (Section 5.5), so the program may say "NO" only when an exact algorithm or the parity test has said so (Section 6).

### 5.8 When to use it

Karmarkar-Karp is the only algorithm that works for every input: it needs $O(n \log n)$ time and $O(n)$ memory for any $n \le 10^6$ and any numbers below $10^{38}$. It is therefore the fallback when dynamic programming (Section 4) and meet in the middle (Section 3) are out of range, for example for $n = 1000$ elements with 25 digits. It is also cheap enough to run **first** on every input, because $d = 0$ already proves YES (Section 6).

**Optional: a simpler baseline.** The greedy method sorts the numbers in descending order and puts each into the side with the smaller sum so far. It also takes $O(n \log n)$ time and satisfies $d \le \max a_i$; as a scheduling algorithm (LPT) its ratio on the larger side is $7/6$ for two machines [Graham 1969]. We include it in the Stage 4 experiments to compare discrepancies, since we expect Karmarkar-Karp to be much better.

**Optional: complete Karmarkar-Karp.** Korf 1998 turns the method into an exact search: at each step it branches on "opposite sides" ($x - y$) and "same side" ($x + y$), trying Karmarkar-Karp's choice first. The worst case is exponential, so it would only be a third exact method. We mention it and do not implement it unless there is time.

---

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

## 7. Summary table (goes near the end of the report)

| Algorithm | Exact? | Time | Memory | Depends on magnitude? | Practical range |
|---|---|---|---|---|---|
| A. Brute force | yes | $O(n \cdot 2^n)$, $O(2^n)$ with Gray code | $O(n)$ | no | $n \le 30$ |
| B. Meet in the middle | yes | $O(n \cdot 2^{n/2})$ | $O(2^{n/2})$ | no | $n \le 44$ |
| C. Dynamic programming | yes | $O(n \cdot \Sigma)$ | $O(\Sigma)$ bits | **yes** | $\Sigma/2 \le 10^{10}$ |
| D. Karmarkar-Karp | no | $O(n \log n)$ | $O(n)$ | no | $n \le 10^6$ |
| E. Final (selection) | yes within limits, else approx. | max of the branch taken | max of the branch taken | via C only | any |

---

## 8. Worked example (one for each algorithm)

Use the first set of `AaC_example_1.txt`: $\{11, 22, 33, 44, 55, 66, 77, 1, 2, 3, 4, 5, 6, 7, 8\}$, $n = 15$, $\Sigma = 344$, $T = 172$. TODO: trace B, C (small version) and D by hand or with a throwaway script, and paste the traces. Also show the odd-sum early exit on $\{999, 889, 778, 667, 123, 456, 133\}$ ($\Sigma = 4045$, odd, so NO immediately).

---

## 9. Notes for Stages 3 and 4 (not part of the Stage 2 PDF)

- Number type: `using Num = unsigned __int128;` in one header, with `parse(string) -> Num` (reject $>38$ digits, reject non-digits, allow leading zeros) and `to_string(Num)`. Overflow check on summation via `__builtin_add_overflow`. Differences in Karmarkar-Karp are always $x - y$ with $x \ge y$, so unsigned is safe there; the discrepancy $|\Sigma(A)-\Sigma(B)|$ is computed as max minus min.
- Masks in Algorithm B: `uint64_t` suffices since $n \le 44$ for B.
- Testing: random multisets with $n \le 20$, compare A, B, C (when $\Sigma$ small) for the YES/NO answer and verify the returned partitions sum to $T$. Check D's returned $d$ against the partition it returns.
- Experiments: (i) time vs $n$ at fixed 25-digit magnitude for A, B, D; (ii) time vs magnitude at fixed $n = 20$ for B vs C; (iii) discrepancy of D vs greedy vs optimum (from B) for $n \le 40$.

---

## 10. References (to verify before submission)

- R. M. Karp, "Reducibility among combinatorial problems", in *Complexity of Computer Computations*, 1972. (NP-completeness of PARTITION.)
- M. R. Garey, D. S. Johnson, *Computers and Intractability*, 1979. (Weak vs strong NP-hardness, pseudo-polynomial DP, Section 4.2.)
- E. Horowitz, S. Sahni, "Computing partitions with applications to the knapsack problem", *J. ACM* 21(2), 1974. (Meet in the middle.)
- R. Schroeppel, A. Shamir, "A $T = O(2^{n/2})$, $S = O(2^{n/4})$ algorithm for certain NP-complete problems", *SIAM J. Comput.* 10(3), 1981. (Memory improvement, optional.)
- N. Karmarkar, R. M. Karp, "The differencing method of set partitioning", Tech. Rep. UCB/CSD 82/113, UC Berkeley, 1982. (The Berkeley EECS listing gives the number UCB/CSD-83-113 and the year 1983; check the original report before the final PDF.)
- B. Yakir, "The differencing algorithm LDM for partitioning: a proof of a conjecture of Karmarkar and Karp", *Math. Oper. Res.* 21(1), 85-99, 1996. (Bibliographic data checked Oct 29, 2026. The result n^(-Θ(log n)) for i.i.d. uniform items is quoted from a summary of the paper and not reproved; check the exact statement in the paper.)
- R. E. Korf, "A complete anytime algorithm for number partitioning", *Artificial Intelligence* 106(2), 1998.
- S. Mertens, "The easiest hard problem: number partitioning", in *Computational Complexity and Statistical Physics*, 2006. (Survey; good for the introduction.)
- R. L. Graham, "Bounds on multiprocessing timing anomalies", *SIAM J. Appl. Math.* 17(2), 1969. (LPT bound, if we include the greedy baseline.)
- M. Fischetti, S. Martello, "Worst-case analysis of the differencing method for the partition problem", *Math. Programming* 37, 117-120, 1987. (Worst-case ratio 7/6 of the differencing method; bibliographic data checked Oct 29, 2026.)
- T. H. Cormen et al., *Introduction to Algorithms*, 3rd or 4th ed. (General reference for DP, heaps, sorting.)

All of these are real and well known, but page numbers, exact titles and the Fischetti-Martello details should be checked against the originals before they go into the PDF.

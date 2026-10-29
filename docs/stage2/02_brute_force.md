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
    if S = 0 then return YES                 // trivial case, Section 1.2
    T ← S / 2
    for mask ← 0 to 2^(n−1) − 1 do          // bit i−1 of mask puts a[i] in A; a[n] is always in B
        sumA ← 0
        for i ← 1 to n − 1 do
            if bit (i−1) of mask = 1 then sumA ← sumA + a[i]
        if sumA = T then return YES
    return NO
```

The two trivial cases of Section 1.2 are answered before the loop. After them $T > 0$, so `mask` $= 0$ (which is $A = \emptyset$) never succeeds, and every set $A$ that succeeds and its complement are non-empty. The input has $n \ge 2$ (Section 1.1), so the loop runs over at least two masks.

### 2.3 Gray-code variant

In the simple version every mask recomputes its sum from scratch in $n - 1$ steps. If the masks are visited in the order of the **binary reflected Gray code**, two consecutive masks differ in exactly one bit, so the sum can be updated with one addition or one subtraction.

Let $m = n - 1$ be the number of free bits. The $k$-th Gray code word is $g_k = k \oplus (k \gg 1)$ ($\oplus$ is bit-wise exclusive or, $\gg$ is a right shift). Two facts are used:

1. $k \mapsto g_k$ is a bijection of $\{0, \dots, 2^{m} - 1\}$ onto itself, so every mask is visited exactly once.
2. $g_k \oplus g_{k-1} = 2^{j}$ where $j$ is the position of the lowest set bit of $k$ (`ctz(k)`, count of trailing zeros). So going from $g_{k-1}$ to $g_k$ flips only bit $j$.

```
BRUTE-FORCE-GRAY(a[1..n])
    S ← Σ a[i]
    if S is odd then return NO
    if S = 0 then return YES                 // trivial case, Section 1.2
    T ← S / 2
    m ← n − 1
    g ← 0;  sumA ← 0                        // g is the current mask, A = ∅, and sumA = 0 < T
    for k ← 1 to 2^m − 1 do
        j ← ctz(k)                          // the one bit that changes
        g ← g XOR 2^j
        if bit j of g = 1 then sumA ← sumA + a[j+1]     // a[j+1] entered A
        else                   sumA ← sumA − a[j+1]     // a[j+1] left A
        if sumA = T then return YES
    return NO
```

The subtraction never underflows: a value is subtracted only if it was added before and not yet removed, so it is part of `sumA`. The invariant is that `sumA` $= \sum_{i:\,\text{bit}(i-1)\text{ of }g = 1} a_i$ after every step. The simple version of Section 2.2 is the documented reference and the test oracle, because it is the one that is obviously correct. We propose to implement the Gray-code version as well, for speed in the Stage 4 measurements, and to check it against the simple version in the tests (the two must give the same YES/NO answer). Whether the Gray-code version is implemented is a team decision.

### 2.4 Example

**A YES instance.** Let $a = (3, 1, 4, 2)$, the same set as in Section 4.4. Then $S = 10$ is even, $T = 5$, $n = 4$, so $a_4 = 2$ stays in $B$ and the mask has $3$ bits: bit 0 is $a_1 = 3$, bit 1 is $a_2 = 1$, bit 2 is $a_3 = 4$.

The masks $0, 1, \dots, 6$ give the sums $\Sigma(A) = 0, 3, 1, 4, 4, 7, 5$, and only the last one equals $T$.

At `mask` $= 6$ the algorithm stops after $7$ of the $8$ masks and answers YES. This mask stands for $A = \{a_2, a_3\} = \{1, 4\}$, whose complement $\{a_1, a_4\} = \{3, 2\}$ has the same sum $5$.

**The Gray-code variant on the same input.** The masks come in the order $000, 001, 011, 010, 110, \dots$

| step $k$ | $\text{ctz}(k)$ | flipped element | $g$ | $\text{sumA}$ | $= T$? |
| -------- | --------------- | --------------- | --- | ------------- | ------ |
| 0        |                 |                 | 000 | 0             | no     |
| 1        | 0               | $a_1 = 3$ enters | 001 | $0 + 3 = 3$  | no     |
| 2        | 1               | $a_2 = 1$ enters | 011 | $3 + 1 = 4$  | no     |
| 3        | 0               | $a_1 = 3$ leaves | 010 | $4 - 3 = 1$  | no     |
| 4        | 2               | $a_3 = 4$ enters | 110 | $1 + 4 = 5$  | **yes** |

It stops after $5$ masks at the same set $A = \{a_2, a_3\}$ and answers YES. Each step costs one addition or subtraction instead of up to $3$.

**A NO instance.** Let $a = (2, 3, 7)$. Then $S = 12$, $T = 6$, and the mask has $2$ bits for $a_1 = 2$ and $a_2 = 3$. The four masks give $\Sigma(A) \in \{0, 2, 3, 5\}$ and none equals $6$, so all masks are examined and the answer is NO. Indeed, $7$ alone already exceeds $T$.

### 2.5 Correctness

**Lemma 0 (what is enumerated).** The values `mask` $\in \{0, \dots, 2^{n-1} - 1\}$ correspond one to one to the subsets $A \subseteq \{1, \dots, n-1\}$. The same holds for the Gray-code order, because $k \mapsto g_k$ is a bijection of this range onto itself.

_Proof._ The bits of the mask are the indicator vector of $A$. A bijection of the range permutes the masks and so visits each once. $\square$

**Lemma 1 (reduction to $n \in B$).** Let $S$ be even and $T = S/2$. A balanced partition of $\{1, \dots, n\}$ exists if and only if there is a subset $A \subseteq \{1, \dots, n-1\}$ with $\Sigma(A) = T$.

_Proof._ ($\Leftarrow$) Take $B = \{1, \dots, n\} \setminus A$. Then $\Sigma(B) = S - T = T = \Sigma(A)$. ($\Rightarrow$) Let $(A, B)$ be balanced. If $n \in B$, then $A \subseteq \{1, \dots, n-1\}$ and $\Sigma(A) = T$ because a balanced partition has $\Sigma(A) = S/2$. Otherwise $n \in A$, and then $(B, A)$ is also balanced, has $n$ in its second part, and its first part $B \subseteq \{1, \dots, n-1\}$ has $\Sigma(B) = T$. $\square$

**Theorem.** `BRUTE-FORCE` and `BRUTE-FORCE-GRAY` return YES if and only if a balanced partition exists.

_Proof._ If $S$ is odd, no balanced partition exists, because $\Sigma(A) = \Sigma(B)$ would give $S = 2\Sigma(A)$, and NO is correct. If $S = 0$, the partition $\{1\}$, $\{2, \dots, n\}$ has two zero sums and two non-empty sides since $n \ge 2$, so YES is correct. Let $S$ be even and positive, so $T > 0$. _Soundness:_ the algorithm answers YES only when `sumA` $= T$, and `sumA` is the sum of the set $A$ of the current mask (in the simple version by construction of the inner loop, in the Gray version by the invariant of Section 2.3, which follows by induction on $k$ since every step adds or subtracts the one element whose bit was flipped). Its complement $B$ has $\Sigma(B) = S - T = T$, so $(A, B)$ is a balanced partition, and both sides are non-empty because both sums are $T > 0$. _Completeness:_ if a balanced partition exists, Lemma 1 gives a subset $A \subseteq \{1, \dots, n-1\}$ with $\Sigma(A) = T$, and by Lemma 0 its mask is visited unless the algorithm stops earlier. It stops earlier only with a YES, which is correct by soundness. If the loop ends without a hit, no such subset exists, hence by Lemma 1 no balanced partition exists, and NO is correct. The algorithm terminates because the loop runs over a finite range. $\square$

Duplicates and zeros need no case analysis, since the proof works on index sets and never uses that the values are distinct or positive.

### 2.6 Complexity

We count operations on 128-bit numbers in the unit-cost model of Section 1.4 and use $m = n - 1$.

- **Preliminaries.** The sum and the parity test cost $O(n)$.
- **Simple version.** The outer loop has $2^{n-1}$ masks (in the worst case all of them, when the answer is NO or the hit is the last mask) and the inner loop has $n - 1$ iterations with one test and at most one addition. The worst-case time is therefore $\Theta(n \cdot 2^{n-1}) = \Theta(n \cdot 2^n)$, that is $O(n \cdot 2^n)$.
- **Gray-code version.** Each step does one bit-flip, one `ctz`, one addition or subtraction and one comparison, which is $O(1)$ ($\text{ctz}$ is a single instruction on current processors, and even when computed by a loop it takes $O(1)$ amortised, because it inspects $j + 1$ bits and half of all $k$ have $j = 0$, a quarter have $j = 1$, and so on). The worst-case time is $\Theta(2^{n-1}) = O(2^n)$, which is smaller by a factor $n$.
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

- **Oracle in the tests.** On random multisets with $n \le 20$ (including duplicates, zeros and odd sums) the answers of B, C (when $S$ is small) and E are compared with A.
- **Baseline in the time experiments.** The time of A against $n$ shows the $2^n$ growth that motivates B (about $2^{n/2}$) and C (about $n \cdot T / 64$), see Sections 3.5 and 4.8.

---


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
2. Enumerate every subset of $L$ and store its sum in a list `LS`.
3. Enumerate every subset of $R$ and store its sum in a list `RS`.
4. Search the two lists for a pair whose sums add to $T$. Such a pair exists if and only if a balanced partition exists (Lemma 2 below).

There are $2^h$ subsets of $L$ and $2^{n-h}$ subsets of $R$, rather than $2^n$ subsets in one list. The lists are sorted so that a two-pointer sweep can find a pair with sum $T$ without testing every possible pair. The algorithm is exact: it does not discard any subset, and it returns NO only after every possible left/right combination has been ruled out.

The running time depends on $n$, not on the magnitude of the input values. This makes meet in the middle the main exact algorithm for inputs containing 25-digit numbers. Its disadvantage is the memory required for the two lists.

### 3.2 Pseudocode

```
MEET-IN-THE-MIDDLE(a[1..n])
    S ← Σ a[i]
    if S is odd then return NO
    if S = 0 then return YES                 // trivial case, Section 1.2
    T ← S / 2
    h ← ⌊n / 2⌋
    LS ← SUBSET-SUMS(a[1..h])             // list of sums, 2^h entries
    RS ← SUBSET-SUMS(a[h+1..n])           // 2^(n−h) entries
    sort LS by sum ascending
    sort RS by sum descending
    i ← 1; j ← 1
    while i ≤ |LS| and j ≤ |RS| do
        s ← LS[i].sum + RS[j].sum
        if s = T then return YES
        if s < T then i ← i + 1
        else j ← j + 1
    return NO

SUBSET-SUMS(b[1..m])
    sums ← array of size 2^m
    sums[0] ← 0
    for mask ← 1 to 2^m − 1 do
        k ← index of lowest set bit of mask
        sums[mask] ← sums[mask without bit k] + b[k+1]   // one addition per subset
    return sums
```

`SUBSET-SUMS` represents a subset by a bit mask, which is the index into `sums`. Bit $k$ is 1 exactly when the corresponding element belongs to the subset. To calculate a new sum, it removes one set bit from `mask`; the remaining mask has already been processed, so the new sum is obtained with one addition. Only the sums are kept, because the algorithm decides existence and does not construct the partition.

After both lists have been generated, `LS` is sorted in ascending order and `RS` in descending order. The pointers initially refer to the smallest left sum and the largest right sum. If their total is less than $T$, the right sum cannot be increased while keeping the current left sum, so the left pointer is advanced. If their total is greater than $T$, the left sum cannot be decreased while keeping the current right sum, so the right pointer is advanced. Each pointer only moves forward, and the sweep stops when it finds the target or one list is exhausted.

### 3.3 Example

Consider the input

$$
a = (3,1,4,2).
$$

The total is $S = 10$, so the target is $T = 5$. With $h = 2$, the left half is $(3,1)$ and the right half is $(4,2)$.

The subset lists, including the empty subset, are given below.

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

The sums $1$ and $4$ come from the subsets $\{1\}$ of the left half and $\{4\}$ of the right half, so $\{1,4\}$ and its complement $\{3,2\}$ both have sum $5$. The algorithm stops as soon as this pair is found and answers YES.

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

**Theorem.** `MEET-IN-THE-MIDDLE` returns YES if and only if a balanced partition exists.

_Proof._ If $S$ is odd, a balanced partition is impossible because its two equal sums would add to the odd number $S$, so returning NO is correct. If $S = 0$, the partition $\{1\}$, $\{2, \dots, n\}$ is balanced with two non-empty sides (Section 1.2). Let $S$ be even and positive, so $T > 0$. By Lemma 2, a balanced partition exists exactly when some left and right list entries add to $T=S/2$. By Lemma 3, the sweep examines a pair if it could have the required sum, and therefore returns YES exactly when such a pair exists. A pair with $x+y=T$ is the sum of a subset $A$ with $\Sigma(A)=T$; its complement has sum $S-T=T$, so a balanced partition exists, and both sides are non-empty because $T > 0$. If the sweep ends without finding a pair, no subset with sum $T$ exists, and Lemma 2 proves that NO is correct. $\square$

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


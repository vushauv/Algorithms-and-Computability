## 3. Meet in the middle

Brute force examines $2^n$ subsets because it chooses all $n$ bits at once. The idea of Horowitz and Sahni [1974] is to choose the bits of the first half of the elements and of the second half separately, and to combine the two results by a search instead of an enumeration. The cost then depends on $n$ through $2^{n/2}$ and, as for brute force, not on the magnitude of the numbers. This section gives the idea, the algorithm with an example, its correctness and complexity, and the range in which it can be applied. Its limit is memory instead of time, which is what makes it complementary to Sections 2 and 4.

### 3.1 Idea

Let

$$
L = \lbrace 1, \dots, h \rbrace, \qquad R = \lbrace h+1, \dots, n \rbrace, \qquad h = \lfloor n/2 \rfloor .
$$

Every subset $A \subseteq \lbrace 1, \dots, n \rbrace$ decomposes uniquely as $A = (A \cap L) \cup (A \cap R)$, and $\Sigma(A) = \Sigma(A \cap L) + \Sigma(A \cap R)$. Hence a subset with sum $T$ exists if and only if some subset sum of $L$ and some subset sum of $R$ add up to $T$ (Lemma 3.2 below).

The algorithm computes the lists `LS` and `RS` of all $2^h$ subset sums of $L$ and all $2^{n-h}$ subset sums of $R$, sorts them, and looks for a pair of entries with sum $T$ by a two-pointer sweep. This avoids testing all $2^h \cdot 2^{n-h}$ pairs. The price is memory: the two lists must be stored. The lists hold only sums, so when the sweep finds two entries that add up to $T$, the subsets with these sums are found again by `FIND-SUBSET` of Section 2.2, one in each half.

### 3.2 Algorithm

The input is a multiset $a_1, \dots, a_n$ with $n \ge 2$ (Section 1.1), so $h \ge 1$ and both halves are non-empty.

```
MEET-IN-THE-MIDDLE(a[1..n])
    S ← Σ a[i]
    if S is odd then return NO
    if S = 0 then return ({1}, {2, …, n})          // Section 1.2
    T ← S / 2
    h ← ⌊n / 2⌋
    LS ← SUBSET-SUMS(a[1..h])                      // 2^h sums
    RS ← SUBSET-SUMS(a[h+1..n])                    // 2^(n−h) sums
    sort LS in ascending order
    sort RS in descending order
    i ← 1; j ← 1
    while i ≤ |LS| and j ≤ |RS| do
        s ← LS[i] + RS[j]
        if s = T then
            AL ← FIND-SUBSET(a[1..h], LS[i])       // Section 2.2
            AR ← FIND-SUBSET(a[h+1..n], RS[j])
            A ← AL ∪ { h + k : k ∈ AR }
            return (A, {1, …, n} \ A)
        if s < T then i ← i + 1
        else j ← j + 1
    return NO

SUBSET-SUMS(b[1..m])
    sums[0..2^m − 1]
    sums[0] ← 0
    for mask ← 1 to 2^m − 1 do
        k ← ctz(mask)                              // lowest set bit, Section 2.1
        sums[mask] ← sums[mask XOR 2^k] + b[k+1]
    return sums
```

#### Generating the subset sums

A subset of $b_1, \dots, b_m$ is represented by a mask as in Section 2.1: bit $k$ of the mask is $1$ if and only if $b_{k+1}$ belongs to the subset. The array `sums` is indexed by masks, so a mask must fit in a machine word. This is no restriction, since the array of $2^m$ numbers must fit in memory. The mask `mask XOR 2^k` is the mask without its lowest set bit $k$. It is smaller than `mask`, so its sum has been computed earlier, and one addition gives the sum of `mask`.

#### The sweep

The sweep starts at the smallest left sum and the largest right sum. If their total is smaller than $T$, no entry of `RS` at or after the current position can reach $T$ together with the current left sum, because those entries are not larger than the current one. So the left pointer advances. If the total is larger than $T$, the right pointer advances for the symmetric reason. Each iteration either returns or moves one pointer forward.

#### Recovering the subsets

Sorting loses the masks, so the lists cannot tell which subsets produce the two sums that meet. The sums suffice: `LS[i]` is the sum of some subset of the left half by Lemma 3.1, and `FIND-SUBSET` finds such a subset by the Gray-code enumeration of the $2^h$ masks of the left half, in time $O(2^h)$ and constant memory. The same holds for `RS[j]` and the right half, whose indices $k$ refer to $a_{h+1}, \dots, a_n$ and are therefore shifted by $h$. Storing a mask next to every sum would avoid the two enumerations, but it would enlarge each of the $O(2^{n/2})$ entries of the lists, whose memory limits the algorithm (Section 3.6), whereas the enumerations cost less time than the sorting (Section 3.5) and no memory.

#### No overflow

Every sum $x + y$ with $x \in$ `LS` and $y \in$ `RS` is the sum of a subset of $M$, namely the union of two subsets of the disjoint index sets $L$ and $R$ (Lemma 3.2). Hence it is at most $S < 2^{128}$, and the argument of Section 1.4 applies: the sweep never overflows.

### 3.3 Examples

In the tables, subsets are written by the values of their elements.

#### A YES instance

Let $a = (3, 1, 4, 2)$. Then $S = 10$, $T = 5$, $h = 2$, the left half is $(3, 1)$ and the right half is $(4, 2)$. The subset sums, indexed by masks whose bit $0$ refers to the first element of the half, are

| mask | left subset | left sum | right subset | right sum |
| ---- | ----------- | -------- | ------------ | --------- |
| 00   | $\emptyset$ | $0$      | $\emptyset$  | $0$       |
| 01   | $\lbrace 3 \rbrace$     | $3$      | $\lbrace 4 \rbrace$      | $4$       |
| 10   | $\lbrace 1 \rbrace$     | $1$      | $\lbrace 2 \rbrace$      | $2$       |
| 11   | $\lbrace 3, 1 \rbrace$  | $4$      | $\lbrace 4, 2 \rbrace$   | $6$       |

After sorting, `LS` $= (0, 1, 3, 4)$ and `RS` $= (6, 4, 2, 0)$. The sweep proceeds as follows.

| left sum | right sum | total | action                       |
| -------- | --------- | ----- | ---------------------------- |
| $0$      | $6$       | $6$   | too large, advance right     |
| $0$      | $4$       | $4$   | too small, advance left      |
| $1$      | $4$       | $5$   | equal to $T$, recover the subsets |

The subsets with the sums $1$ and $4$ are found again by `FIND-SUBSET`. In the left half $(3, 1)$ it visits the masks $01$, $11$ and $10$ with the sums $3$, $4$ and $1$, so the left sum $1$ is that of $\lbrace 1 \rbrace$, the element $a_2$. In the right half $(4, 2)$ the first mask $01$ already has the sum $4$, so the right sum $4$ is that of $\lbrace 4 \rbrace$, the element $a_3$. The algorithm returns the balanced partition $A = \lbrace 2, 3 \rbrace$, $B = \lbrace 1, 4 \rbrace$, with the values $\lbrace 1, 4 \rbrace$ and $\lbrace 3, 2 \rbrace$ and the sum $5$ on both sides.

#### A NO instance

Let $a = (2, 3, 7)$. Then $S = 12$, $T = 6$ and $h = 1$, so the left half is $(2)$ and the right half is $(3, 7)$, with a different size because $n$ is odd. Then `LS` $= (0, 2)$ and `RS` $= (10, 7, 3, 0)$.

| left sum | right sum | total | action                       |
| -------- | --------- | ----- | ---------------------------- |
| $0$      | $10$      | $10$  | too large, advance right     |
| $0$      | $7$       | $7$   | too large, advance right     |
| $0$      | $3$       | $3$   | too small, advance left      |
| $2$      | $3$       | $5$   | too small, advance left      |

The left pointer is now past the end of `LS`, the loop ends and the answer is NO.

For $a = (2, 3, 4)$ the sum $S = 9$ is odd and the answer NO is given before the lists are built.

### 3.4 Correctness

**Lemma 3.1** (subset sums). `SUBSET-SUMS(b[1..m])` returns the sum of every subset of $b_1, \dots, b_m$ exactly once, indexed by the mask of the subset.

*Proof.* Induction on the mask. The mask $0$ is the empty subset with sum $0$. For a non-zero mask with lowest set bit $k$, the mask without bit $k$ is smaller, so its sum is correct by the induction hypothesis, and adding $b_{k+1}$ gives the sum of the mask. Different masks are different subsets and every subset has a mask. $\square$

**Lemma 3.2** (two lists). A subset $A \subseteq \lbrace 1, \dots, n \rbrace$ with $\Sigma(A) = T$ exists if and only if `LS` and `RS` contain entries $x$ and $y$ with $x + y = T$. Sorting only permutes the lists, so this holds before and after the sorting steps.

*Proof.* If $A$ exists, then $\Sigma(A \cap L)$ is in `LS` and $\Sigma(A \cap R)$ is in `RS` by Lemma 3.1, and they add up to $\Sigma(A) = T$. Conversely, let $x$ and $y$ be sums of a subset $A_L \subseteq L$ and of a subset $A_R \subseteq R$. As $L$ and $R$ are disjoint, $A_L \cup A_R$ has sum $x + y = T$. $\square$

**Lemma 3.3** (sweep). The loop performs at most $|LS| + |RS| - 1$ iterations. If entries with $x + y = T$ exist, the sweep stops at such a pair of entries and recovers the subsets.

*Proof.* Each iteration either returns or advances one pointer, and the loop ends when a pointer leaves its list, so the number of iterations is bounded as stated. Now suppose that entries with $x + y = T$ exist, and fix such a pair of positions. Let the pointers be at the entries $x'$ and $y'$. We claim that, as long as the loop has not returned, the fixed $x$ is at or after $x'$ and the fixed $y$ is at or after $y'$. This holds at the start. In particular, while the claim holds the pointers are inside the lists, so the loop condition is true. If $x' + y' = T$, the sweep stops at this pair. If $x' + y' < T$, then $y \le y'$, because `RS` is descending, hence $x' + y \le x' + y' < T$. So $x'$ is not the entry $x$, and $x$ lies strictly after $x'$ in `LS`. Advancing the left pointer keeps the claim. If $x' + y' > T$, then $x \ge x'$, because `LS` is ascending, hence $x + y' \ge x' + y' > T$. So $y'$ is not the entry $y$, and $y$ lies strictly after $y'$. Advancing the right pointer keeps the claim. Since the number of iterations is finite and the loop cannot end by exhaustion while the claim holds, it ends with a return, and the only return inside the loop is at an entry pair with $x' + y' = T$. $\square$

**Theorem 3.4.** `MEET-IN-THE-MIDDLE` terminates and returns a balanced partition if one exists, and NO otherwise.

*Proof.* The cases of odd $S$ and of $S = 0$ are as in Theorem 2.4; the latter uses $n \ge 2$. Let $S$ be even and positive, so $T > 0$. The algorithm terminates, since the subset sums are generated by finite loops, the sweep ends by Lemma 3.3 and `FIND-SUBSET` terminates by Theorem 2.4. Suppose the sweep stops at entries $x$ of `LS` and $y$ of `RS` with $x + y = T$. By Lemma 3.1, $x$ is the sum of a subset of $a_1, \dots, a_h$ and $y$ the sum of a subset of $a_{h+1}, \dots, a_n$, so by Theorem 2.4 the two calls of `FIND-SUBSET` return such subsets $A_L \subseteq L$ and, after the shift by $h$, $A_R \subseteq R$. As $L$ and $R$ are disjoint, $A = A_L \cup A_R$ has the sum $x + y = T$. Its complement has the sum $S - T = T$, so $(A, B)$ is a balanced partition, and both parts are non-empty because $T > 0$. Conversely, if a balanced partition exists, there is $A$ with $\Sigma(A) = T$, so by Lemma 3.2 and Lemma 3.3 the sweep finds such entries, and the algorithm returns a balanced partition as just shown. If no balanced partition exists, the sweep cannot find such entries by Lemma 3.2, so the answer is NO. $\square$

The proof works with indices, so equal values give separate masks and are neither merged nor lost, and zeros need no special case.

### 3.5 Complexity

We count operations on 128-bit numbers in the model of Section 1.4.

**Theorem 3.5.** In the worst case `MEET-IN-THE-MIDDLE` takes time $O(n \cdot 2^{n/2})$ and memory $O(2^{n/2})$ numbers beyond the input and the returned partition, and the time does not depend on the magnitude of the numbers. If $S$ is odd or zero, the answer is found in $O(n)$.

*Proof.* The sum and the parity test take $O(n)$. Let $h = \lfloor n/2 \rfloor$. By Lemma 3.1 the generation of the lists takes, per entry, one $\mathrm{ctz}$, one exclusive or and one addition, which is $O(1)$ by Theorem 2.5, so $O(2^h + 2^{n-h})$ in total. Sorting with a comparison sort takes $O(N \log N)$ comparisons for a list of $N$ entries, each $O(1)$, which gives $O(2^h \cdot h + 2^{n-h} \cdot (n-h))$ because $\log_2 2^h = h$. The sweep performs at most $2^h + 2^{n-h}$ iterations by Lemma 3.3, each $O(1)$. No iteration overflows (Section 3.2). The recovery is performed at most once: the two calls of `FIND-SUBSET` take $O(2^h + 2^{n-h})$ time and $O(1)$ memory by Theorem 2.5, and writing the partition takes $O(n)$. Since $2^h \le 2^{n/2}$, $2^{n-h} \le 2 \cdot 2^{n/2}$ and $h, n - h \le n$, the total is $O(n \cdot 2^{n/2})$. The two lists hold $2^h + 2^{n-h} = O(2^{n/2})$ numbers. An in-place sort needs no further memory, and a merge sort needs another $O(2^{n/2})$ numbers. $\square$

The factor $n$ comes only from sorting. Generating the lists already sorted, by merging, would avoid it; we sort for simplicity. The factor does not matter for the range of application, which is limited by memory (Section 3.6).

### 3.6 Range of application

The algorithm stores $2^{n/2}$ sums for each half, each a 128-bit number of $16$ bytes, so its memory doubles with every two additional elements. For a few tens of elements the lists are small, for forty to fifty elements they take from tens of megabytes to about a gigabyte, and for sixty elements they exceed the memory of a computer. The memory is therefore the limiting resource, and the algorithm is applied for $n \le N_{MITM}$ (Section 1.4), where $N_{MITM}$ is of the order of a few tens. This range is larger than that of brute force, which is limited by time, and it does not depend on the size of the numbers. This makes the algorithm suitable for numbers of $25$ digits and more, up to the bound of Section 1.1, where dynamic programming is infeasible. When $n > N_{MITM}$ but the target sum is small, the dynamic programming of Section 4 takes over.

**Remark.** Schroeppel and Shamir [1981] reduce the memory to $O(2^{n/4})$ at essentially the same time. We do not use their algorithm.

---

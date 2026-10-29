## 3. Meet in the middle

Brute force examines $2^n$ subsets because it chooses all $n$ bits at once. The idea of Horowitz and Sahni [1974] is to choose the bits of the first half of the elements and of the second half separately, and to combine the two results by a search instead of an enumeration. The cost then depends on $n$ through $2^{n/2}$ and, as for brute force, not on the magnitude of the numbers.

### 3.1 Idea

Let
$$
L = \{1, \dots, h\}, \qquad R = \{h+1, \dots, n\}, \qquad h = \lfloor n/2 \rfloor .
$$
Every subset $A \subseteq \{1, \dots, n\}$ decomposes uniquely as $A = (A \cap L) \cup (A \cap R)$, and $\Sigma(A) = \Sigma(A \cap L) + \Sigma(A \cap R)$. Hence a subset with sum $T$ exists if and only if some subset sum of $L$ and some subset sum of $R$ add up to $T$ (Lemma 3.2 below).

The algorithm computes the lists `LS` and `RS` of all $2^h$ subset sums of $L$ and all $2^{n-h}$ subset sums of $R$, sorts them, and looks for a pair of entries with sum $T$ by a two-pointer sweep. This avoids testing all $2^h \cdot 2^{n-h}$ pairs. The price is memory: the two lists must be stored.

### 3.2 Algorithm

```
MEET-IN-THE-MIDDLE(a[1..n])
    S ← Σ a[i]
    if S is odd then return NO
    if S = 0 then return YES                       // Section 1.2
    T ← S / 2
    h ← ⌊n / 2⌋
    LS ← SUBSET-SUMS(a[1..h])                      // 2^h sums
    RS ← SUBSET-SUMS(a[h+1..n])                    // 2^(n−h) sums
    sort LS in ascending order
    sort RS in descending order
    i ← 1; j ← 1
    while i ≤ |LS| and j ≤ |RS| do
        s ← LS[i] + RS[j]
        if s = T then return YES
        if s < T then i ← i + 1
        else j ← j + 1
    return NO

SUBSET-SUMS(b[1..m])
    sums[0..2^m − 1]
    sums[0] ← 0
    for mask ← 1 to 2^m − 1 do
        k ← index of the lowest set bit of mask
        sums[mask] ← sums[mask without bit k] + b[k+1]
    return sums
```

A subset of $b_1, \dots, b_m$ is represented by a mask whose bit $k$ is $1$ if and only if $b_{k+1}$ belongs to it. The sum of a mask is obtained from the sum of the mask without its lowest set bit, which has been computed earlier, by one addition.

The sweep starts at the smallest left sum and the largest right sum. If their total is smaller than $T$, no entry of `RS` that is not larger than the current one can reach $T$ together with the current left sum, so the left pointer advances. If the total is larger than $T$, the right pointer advances for the symmetric reason. Each pointer only moves forward.

### 3.3 Example

Let $a = (3, 1, 4, 2)$. Then $S = 10$, $T = 5$, $h = 2$, the left half is $(3, 1)$ and the right half is $(4, 2)$. The subset sums, indexed by masks whose bit $0$ refers to the first element of the half, are

| mask | left subset | left sum | right subset | right sum |
| ---- | ----------- | -------- | ------------ | --------- |
| 00   | $\emptyset$ | $0$      | $\emptyset$  | $0$       |
| 01   | $\{3\}$     | $3$      | $\{4\}$      | $4$       |
| 10   | $\{1\}$     | $1$      | $\{2\}$      | $2$       |
| 11   | $\{3, 1\}$  | $4$      | $\{4, 2\}$   | $6$       |

After sorting, `LS` $= (0, 1, 3, 4)$ and `RS` $= (6, 4, 2, 0)$. The sweep proceeds as follows.

| left sum | right sum | total | action                       |
| -------- | --------- | ----- | ---------------------------- |
| $0$      | $6$       | $6$   | too large, advance right     |
| $0$      | $4$       | $4$   | too small, advance left      |
| $1$      | $4$       | $5$   | equal to $T$, answer YES     |

The sums $1$ and $4$ come from the subsets $\{1\}$ and $\{4\}$, so $\{1, 4\}$ and its complement $\{3, 2\}$ both have sum $5$.

For $a = (2, 3, 4)$ the sum $S = 9$ is odd and the answer NO is given before the lists are built. If $S$ is even but no pair of entries adds up to $T$, the sweep exhausts one of the lists and answers NO.

### 3.4 Correctness

**Lemma 3.1** (subset sums). `SUBSET-SUMS(b[1..m])` returns the sum of every subset of $b_1, \dots, b_m$ exactly once, indexed by the mask of the subset.

*Proof.* Induction on the mask. The mask $0$ is the empty subset with sum $0$. For a non-zero mask with lowest set bit $k$, the mask without bit $k$ is smaller, so its sum is correct by the induction hypothesis, and adding $b_{k+1}$ gives the sum of the mask. Different masks are different subsets and every subset has a mask. $\square$

**Lemma 3.2** (two lists). A subset $A \subseteq \{1, \dots, n\}$ with $\Sigma(A) = T$ exists if and only if `LS` and `RS` contain entries $x$ and $y$ with $x + y = T$.

*Proof.* If $A$ exists, then $\Sigma(A \cap L)$ is in `LS` and $\Sigma(A \cap R)$ is in `RS` by Lemma 3.1, and they add up to $\Sigma(A) = T$. Conversely, let $x$ and $y$ be sums of a subset $A_L \subseteq L$ and of a subset $A_R \subseteq R$. As $L$ and $R$ are disjoint, $A_L \cup A_R$ has sum $x + y = T$. $\square$

**Lemma 3.3** (sweep). If entries with $x + y = T$ exist, the sweep reaches such a pair.

*Proof.* Let the pointers be at the entries $x'$ and $y'$ and suppose that all steps so far have preserved the following property: some pair $(x, y)$ with $x + y = T$ has $x$ at or after $x'$ and $y$ at or after $y'$. This holds at the start. If $x' + y' = T$, the sweep stops with YES. If $x' + y' < T$, then $y \le y'$, because `RS` is descending, hence $x' + y \le x' + y' < T$, so $x' \ne x$, and $x$ lies strictly after $x'$ in the ascending list `LS`. Advancing the left pointer keeps the property. If $x' + y' > T$, then $x \ge x'$ gives $x + y' \ge x' + y' > T$, so $y' \ne y$, and advancing the right pointer keeps the property. As each step moves a pointer, and the lists are finite, the sweep stops, and it can only stop with YES while the property holds. $\square$

**Theorem 3.4.** `MEET-IN-THE-MIDDLE` returns YES if and only if a balanced partition exists.

*Proof.* The cases of odd $S$ and of $S = 0$ are as in Theorem 2.4. Let $S$ be even and positive, so $T > 0$. If the algorithm answers YES, it has found $x \in$ `LS` and $y \in$ `RS` with $x + y = T$, so by Lemma 3.2 there is $A$ with $\Sigma(A) = T$. Its complement has sum $S - T = T$, so $(A, B)$ is a balanced partition, and both parts are non-empty because $T > 0$. Conversely, if a balanced partition exists, there is $A$ with $\Sigma(A) = T$, so by Lemma 3.2 and Lemma 3.3 the sweep answers YES. $\square$

The proof works with indices, so equal values give separate masks and are neither merged nor lost, and zeros need no special case.

### 3.5 Complexity

**Theorem 3.5.** `MEET-IN-THE-MIDDLE` takes time $O(n \cdot 2^{n/2})$ and memory $O(2^{n/2})$, and the time does not depend on the magnitude of the numbers.

*Proof.* Let $h = \lfloor n/2 \rfloor$. By Lemma 3.1 the generation of the lists takes one addition per entry, which is $O(2^h + 2^{n-h})$. Sorting takes $O(2^h \cdot h + 2^{n-h} \cdot (n-h))$, because $\log_2 2^h = h$. The sweep moves each pointer forward only, so it takes $O(2^h + 2^{n-h})$ steps. Since $2^{n-h} \le 2 \cdot 2^{n/2}$ and $h, n - h \le n$, the total is $O(n \cdot 2^{n/2})$. The two lists hold $2^h + 2^{n-h} = O(2^{n/2})$ numbers. $\square$

### 3.6 Range of application

The algorithm stores $2^{n/2}$ sums of $16$ bytes for each half, so its memory doubles with every two additional elements. For a few tens of elements the lists are small, for forty to fifty elements they take hundreds of megabytes to gigabytes, and for sixty elements they exceed the memory of a computer. The memory is therefore the limiting resource, and the algorithm is applied for $n \le N_{MITM}$ (Section 1.4), where $N_{MITM}$ is of the order of a few tens. This is a much larger range than for brute force, but it does not depend on the size of the numbers, which makes the algorithm suitable for numbers of $25$ digits and more, where dynamic programming cannot be used.

**Remark.** Schroeppel and Shamir [1981] reduce the memory to $O(2^{n/4})$ without changing the time. We do not use their algorithm.

---

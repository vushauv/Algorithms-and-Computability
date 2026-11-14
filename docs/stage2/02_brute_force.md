## 2. Brute force

The most direct way to solve PARTITION is to examine every candidate. By Section 1.2 it suffices to find an index set $A$ with $\Sigma(A) = T$, and there are only finitely many index sets. This section analyses the resulting algorithm in the form that visits the index sets in Gray-code order and updates the sum with one operation per set. It runs in time exponential in $n$ and uses almost no memory, so it sets the baseline against which the algorithms of Sections 3 and 4 are measured, and it is the reference against which they are tested.

### 2.1 Idea

#### Subsets as bit vectors

Every subset $A \subseteq \{1, \dots, n\}$ corresponds to a bit vector $x \in \{0,1\}^n$ with $x_i = 1$ if and only if $i \in A$. The algorithm enumerates these vectors, maintains $\Sigma(A)$ along the way, and stops as soon as $\Sigma(A) = T$, returning $A$ and its complement. If the enumeration ends without a hit, no balanced partition exists.

#### Halving

The partitions $(A, B)$ and $(B, A)$ have the same sums, so we may assume that the index $n$ lies in $B$. Only the first $n - 1$ bits are then free, and $2^{n-1}$ vectors suffice instead of $2^n$. A vector is stored as an integer $\mathrm{mask} \in \{0, 1, 2, \dots, 2^{n-1} - 1\}$ whose binary digits are the bits $x_i$: the lowest digit (bit $0$) is $x_1$, the next one (bit $1$) is $x_2$, and in general bit $i - 1$ is $x_i$. For $n = 4$ the masks are $0, 1, \dots, 7$. For example, $\mathrm{mask} = 5 = 101_2$ has bits $0$ and $2$ set, so it stands for $A = \{1, 3\}$, and $\mathrm{mask} = 0$ stands for $A = \emptyset$.

#### Gray-code enumeration

The simplest enumeration visits the masks in the order $0, 1, 2, \dots$ and computes the sum of each from scratch, with $n - 1$ additions per mask and $\Theta(n \cdot 2^n)$ operations in total. We do not pursue this version. If the masks are visited instead in the order of the binary reflected Gray code, two consecutive masks differ in exactly one bit, and the sum is updated with a single addition or subtraction. Let $m = n - 1$. The $k$-th Gray code word is $g_k = k \oplus (k \gg 1)$, where $\oplus$ is the bitwise exclusive or and $\gg$ the right shift. The function $\mathrm{ctz}(k)$, "count trailing zeros", is the number of zeros at the right end of the binary notation of $k$. For example, $12 = 1100_2$ has $\mathrm{ctz} = 2$ and $5 = 101_2$ has $\mathrm{ctz} = 0$. It is computed by a single processor instruction, or by a loop of $O(1)$ amortised time (Theorem 2.5). We use two facts.

1. The map $k \mapsto g_k$ is a bijection of $\{0, \dots, 2^m - 1\}$ onto itself, so every mask is visited exactly once.
2. $g_k \oplus g_{k-1} = 2^{j}$, where $j = \mathrm{ctz}(k)$. Hence going from $g_{k-1}$ to $g_k$ flips only bit $j$.

For $m = 3$ free bits ($n = 4$) the words are

| $k$ | binary | $\mathrm{ctz}(k)$ | $g_k$ | bit flipped |
| --- | ------ | ----------------- | ----- | ----------- |
| 0   | 000    |                   | 000   |             |
| 1   | 001    | 0                 | 001   | 0           |
| 2   | 010    | 1                 | 011   | 1           |
| 3   | 011    | 0                 | 010   | 0           |
| 4   | 100    | 2                 | 110   | 2           |
| 5   | 101    | 0                 | 111   | 0           |
| 6   | 110    | 1                 | 101   | 1           |
| 7   | 111    | 0                 | 100   | 0           |

Consecutive words differ in one bit, namely bit $\mathrm{ctz}(k)$, and every value from $0$ to $7$ occurs once. The algorithm does not compute $g_k$ by the formula: it keeps the current mask and flips bit $\mathrm{ctz}(k)$ of it.

#### Indices instead of values

The enumeration runs over index sets, so equal values at different positions are different elements, and no special treatment of duplicates or of zeros is needed.

### 2.2 Algorithm

The input is a multiset $a_1, \dots, a_n$ with $n \ge 2$ (Section 1.1). The enumeration is written as a procedure `FIND-SUBSET`, which returns a subset of $b_1, \dots, b_m$ with a given sum $x$, or NONE if there is no such subset. Meet in the middle uses the same procedure (Section 3.2).

```
FIND-SUBSET(b[1..m], x)
    if x = 0 then return ∅
    g ← 0;  sum ← 0                                // bit i−1 of the mask g puts b[i] in the subset
    for k ← 1 to 2^m − 1 do
        j ← ctz(k)                                 // the bit that changes
        g ← g XOR 2^j
        if bit j of g = 1 then sum ← sum + b[j+1]       // b[j+1] enters the subset
        else                   sum ← sum − b[j+1]       // b[j+1] leaves the subset
        if sum = x then return { i : bit i−1 of g = 1 }
    return NONE

BRUTE-FORCE(a[1..n])
    S ← Σ a[i]
    if S is odd then return NO
    if S = 0 then return ({1}, {2, …, n})          // Section 1.2
    T ← S / 2
    A ← FIND-SUBSET(a[1..n−1], T)                  // a[n] stays in B
    if A = NONE then return NO
    return (A, {1, …, n} \ A)
```

For `FIND-SUBSET` with $x > 0$ the initial mask $g = 0$, which stands for the empty subset, can never succeed, and it is not tested. After the two trivial cases of Section 1.2 brute force calls it with $x = T > 0$, so every set $A$ that succeeds is non-empty together with its complement. The subtraction never underflows, because a value is subtracted only after it has been added and not yet removed, so it is a part of `sum`.

### 2.3 Examples

#### A YES instance

Let $a = (3, 1, 4, 2)$. Then $S = 10$, $T = 5$ and $n = 4$, so $a_4 = 2$ stays in $B$ and the mask has three bits: bit $0$ is $a_1 = 3$, bit $1$ is $a_2 = 1$ and bit $2$ is $a_3 = 4$. The algorithm visits the masks in the order $000, 001, 011, 010, 110, \dots$

| step $k$ | $\mathrm{ctz}(k)$ | flipped element | $g$ | $\mathrm{sum}$ | $= T$? |
| -------- | ----------------- | --------------- | --- | --------------- | ------ |
| 0        |                   |                 | 000 | $0$             | no     |
| 1        | 0                 | $a_1 = 3$ enters | 001 | $0 + 3 = 3$    | no     |
| 2        | 1                 | $a_2 = 1$ enters | 011 | $3 + 1 = 4$    | no     |
| 3        | 0                 | $a_1 = 3$ leaves | 010 | $4 - 3 = 1$    | no     |
| 4        | 2                 | $a_3 = 4$ enters | 110 | $1 + 4 = 5$    | yes    |

It stops after $5$ of the $8$ masks. The mask $110$ stands for $A = \{2, 3\}$, with the values $1$ and $4$, and the algorithm returns the balanced partition $A = \{2, 3\}$, $B = \{1, 4\}$: the complement $B$ has the values $3$ and $2$ and the same sum $5$.

#### A NO instance

Let $a = (2, 3, 7)$. Then $S = 12$ and $T = 6$, and the mask has two bits, for $a_1 = 2$ and $a_2 = 3$. The three steps visit the masks $01$, $11$ and $10$ with the sums $2$, $5$ and $3$, none of which equals $6$, so all masks are examined, `FIND-SUBSET` returns NONE and the answer is NO.

### 2.4 Correctness

**Lemma 2.1** (enumeration). The masks $\{0, \dots, 2^{m} - 1\}$ correspond one to one to the subsets of $\{1, \dots, m\}$, and the Gray-code order visits every mask exactly once.

*Proof.* The bits of a mask form the indicator vector of the subset, and this correspondence is a bijection. By fact 1 of Section 2.1, the Gray-code order is a permutation of the masks. $\square$

**Lemma 2.2** (reduction to $n \in B$). Let $S$ be even and $T = S/2$. A balanced partition of $\{1, \dots, n\}$ exists if and only if there is a subset $A \subseteq \{1, \dots, n-1\}$ with $\Sigma(A) = T$.

*Proof.* ($\Leftarrow$) Put $B = \{1, \dots, n\} \setminus A$. Then $\Sigma(B) = S - T = T = \Sigma(A)$. ($\Rightarrow$) Let $(A, B)$ be balanced, so $\Sigma(A) = T$. If $n \in B$, then $A \subseteq \{1, \dots, n-1\}$. Otherwise $n \in A$, and then $(B, A)$ is balanced with $B \subseteq \{1, \dots, n-1\}$ and $\Sigma(B) = T$. $\square$

**Lemma 2.3** (invariant). In `FIND-SUBSET`, after step $k$, $\mathrm{sum} = \sum_{i \,:\, \text{bit } i-1 \text{ of } g = 1} b_i$.

*Proof.* Induction on $k$. For $k = 0$ both sides are $0$. By fact 2 of Section 2.1, step $k$ flips bit $j$ of $g$ and adds or subtracts $b_{j+1}$ according to the new value of the bit, which is exactly the change of the right-hand side. $\square$

**Theorem 2.4.** `FIND-SUBSET(b[1..m], x)` returns a subset of $\{1, \dots, m\}$ with the sum $x$ if one exists, and NONE otherwise. Consequently, `BRUTE-FORCE` returns a balanced partition if one exists, and NO otherwise.

*Proof.* Consider `FIND-SUBSET` first. If $x = 0$, the empty subset has the sum $x$ and is returned. Let $x > 0$. *Soundness:* the procedure returns a subset only when $\mathrm{sum} = x$, and by Lemma 2.3 $\mathrm{sum}$ is the sum of the subset of the current mask, which is the subset returned. *Completeness:* if some subset has the sum $x$, it is not empty, because $x > 0$, so its mask is not the initial mask $0$, and by Lemma 2.1 the mask is visited and its sum tested unless the procedure has already returned another subset with the sum $x$. Hence NONE is returned only if no subset has the sum $x$. The loop is finite, so the procedure terminates.

Now consider `BRUTE-FORCE`. If $S$ is odd, a balanced partition would give $S = 2\Sigma(A)$, which is impossible, so NO is correct. If $S = 0$, the partition $\{1\}$, $\{2, \dots, n\}$ is balanced and has two non-empty parts since $n \ge 2$. Let $S$ be even and positive, so $T > 0$. By the first part, `FIND-SUBSET(a[1..n−1], T)` returns a set $A \subseteq \{1, \dots, n-1\}$ with $\Sigma(A) = T$ if one exists. Its complement has the sum $S - T = T$, so $(A, B)$ is a balanced partition, and both parts are non-empty because both sums are positive. If the procedure returns NONE, no such $A$ exists, and by Lemma 2.2 no balanced partition exists, so NO is correct. $\square$

The proof works on index sets and uses neither that the values are distinct nor that they are positive.

### 2.5 Complexity

We count operations on 128-bit numbers in the model of Section 1.4.

**Theorem 2.5.** `FIND-SUBSET(b[1..m], x)` takes time $O(2^m)$ and $O(1)$ memory beyond the input and the returned subset. In the worst case `BRUTE-FORCE` takes time $\Theta(2^n)$. It uses $O(1)$ memory beyond the input and the returned partition, and neither bound depends on the magnitude of the numbers.

*Proof.* Each of the at most $2^m - 1$ steps of `FIND-SUBSET` consists of one bit flip, one $\mathrm{ctz}$, one addition or subtraction and one comparison. This is $O(1)$, since $\mathrm{ctz}$ is a single processor instruction and even a loop for it takes $O(1)$ amortised time: half of all $k$ have $j = 0$, a quarter have $j = 1$, and so on. Reading the subset off the final mask takes $O(m)$, which is $O(2^m)$. In `BRUTE-FORCE` the sum and the parity test take $O(n)$, the call of `FIND-SUBSET` has $m = n - 1$, and writing the partition takes $O(n)$. The worst case is a NO instance with even $S$, which examines all masks, so the time is $\Theta(2^{n-1}) = \Theta(2^n)$. The variables $\mathrm{sum}$, $S$, $T$ and the mask need $O(1)$ numbers, and the mask fits in a 64-bit word as long as $n \le 64$. $\square$

In the best case, $S$ is odd or $S = 0$, and the answer is found in $O(n)$. The direct enumeration of Section 2.1, which recomputes the sum for every mask, takes $\Theta(n \cdot 2^n)$ in the worst case; the Gray code saves the factor $n$.

### 2.6 Range of application

The time doubles with every additional element, so each ten additional elements multiply it by about $10^3$. If the algorithm performs about $10^9$ steps per second, which is optimistic for 128-bit arithmetic, a few tens of elements already take seconds to minutes and about fifty elements take days. The algorithm is therefore usable only for a few tens of elements. The final algorithm (Section 6) does not use it: meet in the middle (Section 3) is faster for every $n \ge 5$ and dynamic programming (Section 4) covers the small numbers, so brute force serves as the baseline against which both are measured and as the reference for testing them.

The size of the numbers plays no role. A set with $n = 10$ elements of $25$ digits needs only $2^{9} = 512$ masks, whereas a set with $n = 100$ elements of one digit is out of reach. Dynamic programming has the opposite behaviour, and Section 3 shows how the dependence on $n$ can be reduced from $2^n$ to about $2^{n/2}$.

---

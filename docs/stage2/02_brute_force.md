## 2. Brute force

The most direct way to decide PARTITION is to examine every candidate. By Section 1.2 it suffices to find an index set $A$ with $\Sigma(A) = T$, and there are only finitely many index sets. This section analyses the resulting algorithm and a variant that is faster by a factor $n$. Both run in time exponential in $n$ and use little memory, so they set the baseline against which the algorithms of Sections 3 and 4 are measured.

### 2.1 Idea

Every subset $A \subseteq \{1, \dots, n\}$ corresponds to a bit vector $x \in \{0,1\}^n$ with $x_i = 1$ if and only if $i \in A$. The algorithm enumerates these vectors, computes $\Sigma(A)$ for each, and stops as soon as $\Sigma(A) = T$. If the enumeration ends without a hit, no balanced partition exists.

**Halving.** The partitions $(A, B)$ and $(B, A)$ have the same sums, so we may assume that the index $n$ lies in $B$. Only the first $n - 1$ bits are then free, and $2^{n-1}$ vectors suffice instead of $2^n$. A vector is stored as an integer $\mathrm{mask} \in \{0, \dots, 2^{n-1} - 1\}$ whose bit $i - 1$ is $x_i$.

**Indices instead of values.** The enumeration runs over index sets, so equal values at different positions are different elements, and no special treatment of duplicates or of zeros is needed.

### 2.2 Algorithm

```
BRUTE-FORCE(a[1..n])
    S ← Σ a[i]
    if S is odd then return NO
    if S = 0 then return YES                       // Section 1.2
    T ← S / 2
    for mask ← 0 to 2^(n−1) − 1 do                 // bit i−1 of mask puts a[i] in A; a[n] is in B
        sumA ← 0
        for i ← 1 to n − 1 do
            if bit (i−1) of mask = 1 then sumA ← sumA + a[i]
        if sumA = T then return YES
    return NO
```

After the two trivial cases of Section 1.2 we have $T > 0$. Hence $\mathrm{mask} = 0$, which stands for $A = \emptyset$, never succeeds, and every set $A$ that succeeds is non-empty together with its complement.

**Gray-code variant.** The inner loop recomputes the sum of $A$ from scratch. If the masks are visited in the order of the binary reflected Gray code, two consecutive masks differ in exactly one bit, and the sum is updated with a single addition or subtraction. Let $m = n - 1$. The $k$-th Gray code word is $g_k = k \oplus (k \gg 1)$, where $\oplus$ is the bitwise exclusive or and $\gg$ the right shift. We use two facts.

1. The map $k \mapsto g_k$ is a bijection of $\{0, \dots, 2^m - 1\}$ onto itself, so every mask is visited exactly once.
2. $g_k \oplus g_{k-1} = 2^{j}$, where $j$ is the number of trailing zeros of $k$, written $\mathrm{ctz}(k)$. Hence going from $g_{k-1}$ to $g_k$ flips only bit $j$.

```
BRUTE-FORCE-GRAY(a[1..n])
    S ← Σ a[i]
    if S is odd then return NO
    if S = 0 then return YES                       // Section 1.2
    T ← S / 2
    m ← n − 1
    g ← 0;  sumA ← 0                               // A = ∅
    for k ← 1 to 2^m − 1 do
        j ← ctz(k)                                 // the bit that changes
        g ← g XOR 2^j
        if bit j of g = 1 then sumA ← sumA + a[j+1]     // a[j+1] enters A
        else                   sumA ← sumA − a[j+1]     // a[j+1] leaves A
        if sumA = T then return YES
    return NO
```

The subtraction never underflows, because a value is subtracted only after it has been added and not yet removed, so it is a part of `sumA`.

### 2.3 Examples

**A YES instance.** Let $a = (3, 1, 4, 2)$. Then $S = 10$, $T = 5$ and $n = 4$, so $a_4 = 2$ stays in $B$ and the mask has three bits: bit $0$ is $a_1 = 3$, bit $1$ is $a_2 = 1$ and bit $2$ is $a_3 = 4$. The masks $0, 1, \dots, 6$ give the sums $\Sigma(A) = 0, 3, 1, 4, 4, 7, 5$, and only the last equals $T$. The algorithm stops at $\mathrm{mask} = 6$, after $7$ of the $8$ masks, and answers YES. This mask stands for $A = \{1, 4\}$, whose complement $\{3, 2\}$ has the same sum $5$.

The Gray-code variant visits the masks in the order $000, 001, 011, 010, 110, \dots$

| step $k$ | $\mathrm{ctz}(k)$ | flipped element | $g$ | $\mathrm{sumA}$ | $= T$? |
| -------- | ----------------- | --------------- | --- | --------------- | ------ |
| 0        |                   |                 | 000 | $0$             | no     |
| 1        | 0                 | $a_1 = 3$ enters | 001 | $0 + 3 = 3$    | no     |
| 2        | 1                 | $a_2 = 1$ enters | 011 | $3 + 1 = 4$    | no     |
| 3        | 0                 | $a_1 = 3$ leaves | 010 | $4 - 3 = 1$    | no     |
| 4        | 2                 | $a_3 = 4$ enters | 110 | $1 + 4 = 5$    | yes    |

It stops after $5$ masks at the same set $A = \{1, 4\}$ and answers YES.

**A NO instance.** Let $a = (2, 3, 7)$. Then $S = 12$ and $T = 6$, and the mask has two bits, for $a_1 = 2$ and $a_2 = 3$. The four masks give $\Sigma(A) \in \{0, 2, 3, 5\}$, none of which equals $6$, so all masks are examined and the answer is NO.

### 2.4 Correctness

**Lemma 2.1** (enumeration). The values $\mathrm{mask} \in \{0, \dots, 2^{n-1} - 1\}$ correspond one to one to the subsets $A \subseteq \{1, \dots, n-1\}$. The same holds for the Gray-code order.

*Proof.* The bits of a mask form the indicator vector of $A$, and this correspondence is a bijection. By fact 1 of Section 2.2, the Gray-code order is a permutation of the masks. $\square$

**Lemma 2.2** (reduction to $n \in B$). Let $S$ be even and $T = S/2$. A balanced partition of $\{1, \dots, n\}$ exists if and only if there is a subset $A \subseteq \{1, \dots, n-1\}$ with $\Sigma(A) = T$.

*Proof.* ($\Leftarrow$) Put $B = \{1, \dots, n\} \setminus A$. Then $\Sigma(B) = S - T = T = \Sigma(A)$. ($\Rightarrow$) Let $(A, B)$ be balanced, so $\Sigma(A) = T$. If $n \in B$, then $A \subseteq \{1, \dots, n-1\}$. Otherwise $n \in A$, and then $(B, A)$ is balanced with $B \subseteq \{1, \dots, n-1\}$ and $\Sigma(B) = T$. $\square$

**Lemma 2.3** (invariant of the Gray-code variant). After step $k$, $\mathrm{sumA} = \sum_{i \,:\, \text{bit } i-1 \text{ of } g = 1} a_i$.

*Proof.* Induction on $k$. For $k = 0$ both sides are $0$. Step $k$ flips bit $j$ of $g$ and adds or subtracts $a_{j+1}$ according to the new value of the bit, which is exactly the change of the right-hand side. $\square$

**Theorem 2.4.** `BRUTE-FORCE` and `BRUTE-FORCE-GRAY` return YES if and only if a balanced partition exists.

*Proof.* If $S$ is odd, a balanced partition would give $S = 2\Sigma(A)$, which is impossible, so NO is correct. If $S = 0$, the partition $\{1\}$, $\{2, \dots, n\}$ is balanced and has two non-empty parts since $n \ge 2$, so YES is correct. Let $S$ be even and positive, so $T > 0$.

*Soundness.* The algorithm answers YES only when $\mathrm{sumA} = T$, where $\mathrm{sumA}$ is the sum of the set $A$ of the current mask, by construction in the first version and by Lemma 2.3 in the second. Its complement has the sum $S - T = T$, so $(A, B)$ is a balanced partition, and both parts are non-empty because both sums are positive.

*Completeness.* If a balanced partition exists, Lemma 2.2 gives $A \subseteq \{1, \dots, n-1\}$ with $\Sigma(A) = T$, and by Lemma 2.1 its mask is visited unless the algorithm has stopped earlier, which is only possible with the answer YES. If the loop ends without a hit, no such $A$ exists, and by Lemma 2.2 no balanced partition exists. The loops are finite, so the algorithm terminates. $\square$

The proof works on index sets and uses neither that the values are distinct nor that they are positive.

### 2.5 Complexity

We count operations on 128-bit numbers in the model of Section 1.4.

**Theorem 2.5.** In the worst case `BRUTE-FORCE` takes time $\Theta(n \cdot 2^n)$ and `BRUTE-FORCE-GRAY` takes time $\Theta(2^n)$. Both use $O(1)$ memory beyond the input, and neither depends on the magnitude of the numbers.

*Proof.* The sum and the parity test take $O(n)$. In the first version the outer loop has $2^{n-1}$ iterations, each with an inner loop of $n - 1$ steps, which gives $\Theta(n \cdot 2^{n-1}) = \Theta(n \cdot 2^n)$ when all masks are examined. In the second version each of the $2^{n-1} - 1$ steps consists of one bit flip, one $\mathrm{ctz}$, one addition or subtraction and one comparison. This is $O(1)$, since $\mathrm{ctz}$ is a single processor instruction and even a loop for it takes $O(1)$ amortised time: half of all $k$ have $j = 0$, a quarter have $j = 1$, and so on. The worst case is a NO instance with even $S$, which examines all masks. The variables $\mathrm{sumA}$, $S$, $T$ and the mask need $O(1)$ numbers, and the mask fits in a 64-bit word as long as $n \le 64$. $\square$

In the best case, $S$ is odd or $S = 0$, and the answer is found in $O(n)$.

### 2.6 Range of application

The time doubles with every additional element, so each ten additional elements multiply it by about $10^3$. If the Gray-code variant performs about $10^9$ steps per second, which is optimistic for 128-bit arithmetic, a few tens of elements already take seconds to minutes and about fifty elements take days. The algorithm is therefore applied only for $n \le N_{BF}$ (Section 1.4), where $N_{BF}$ is of the order of a few tens. Beyond that range it is replaced by meet in the middle (Section 3) or dynamic programming (Section 4).

The size of the numbers plays no role. A set with $n = 10$ elements of $25$ digits needs only $2^{9} = 512$ masks, whereas a set with $n = 100$ elements of one digit is out of reach. Dynamic programming has the opposite behaviour, and Section 3 shows how the dependence on $n$ can be reduced from $2^n$ to about $2^{n/2}$.

---

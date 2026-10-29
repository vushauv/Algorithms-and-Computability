## 4. Dynamic programming

Brute force and meet in the middle look at subsets, and their cost grows with $n$ whatever the numbers are. Dynamic programming looks at subset *sums* instead. Many different subsets have the same sum, and only the sums between $0$ and $T$ matter, because all elements are non-negative. The cost therefore depends on the magnitude of the target $T$ and not only on $n$. This makes the algorithm pseudo-polynomial in the sense of Section 1.3: it is the best choice for many small numbers and the worst for few large ones, which is the opposite of meet in the middle.

### 4.1 Preliminary checks

Let $S = \Sigma(M)$ and $T = S/2$ as in Section 1.2. If $S$ is odd, no balanced partition exists, and if $S = 0$ one exists. Let $S$ be even and positive. By Section 1.2 the question is whether some index set $A$ has $\Sigma(A) = T$. One more case is decided at once: if some element satisfies $a_j > T$, then every set containing $j$ has a sum larger than $T$, and every set not containing $j$ has a complement that does. Hence the sums of the two parts cannot be equal, and the answer is NO. After this check $a_j \le T$ for all $j$.

### 4.2 Idea

For $0 \le i \le n$ and $0 \le s \le T$ let
$$
R_i(s) = \text{TRUE} \iff \text{some subset of } \{a_1, \dots, a_i\} \text{ has sum } s .
$$
We want $R_n(T)$. With no elements the only obtainable sum is the empty sum, so
$$
R_0(s) = \begin{cases} \text{TRUE} & s = 0, \\ \text{FALSE} & s > 0. \end{cases}
$$
A subset of $\{a_1, \dots, a_i\}$ with sum $s$ either does not contain $a_i$, and is then a subset of $\{a_1, \dots, a_{i-1}\}$ with sum $s$, or contains $a_i$, and removing it leaves a subset of $\{a_1, \dots, a_{i-1}\}$ with sum $s - a_i$. Hence for $i \ge 1$
$$
R_i(s) = \begin{cases} R_{i-1}(s) & s < a_i, \\ R_{i-1}(s) \lor R_{i-1}(s - a_i) & s \ge a_i. \end{cases}
$$
Equal values need no special treatment: two equal elements are two indices and give two steps of the recurrence.

**One row.** Row $i$ depends only on row $i - 1$, so a single array `reach[0..T]` suffices, which after step $i$ holds $R_i$. The step reads `reach[s − a[i]]`, which must still hold the value of row $i - 1$. Since $s - a_i < s$ whenever $a_i > 0$, this is guaranteed if the cells are updated from the largest index to the smallest.

**Bitset.** Every cell is a single bit, so the array is stored as a bitset of $T + 1$ bits packed into words of $w = 64$ bits. The update of the whole row is then `reach ← reach OR (reach << a[i])`, truncated at bit $T$. It is carried out word by word, in the order from the highest word to the lowest when it is done in place, and needs about $T/w$ word operations instead of $T$ cell updates.

### 4.3 Algorithm

The input is a multiset $a_1, \dots, a_n$ with $n \ge 2$ (Section 1.1).

```
DP-PARTITION(a[1..n])
    S ← Σ a[i]
    if S is odd then return NO
    if S = 0 then return YES                       // Section 1.2
    T ← S / 2
    if max a[i] > T then return NO                 // Section 4.1
    reach[0..T] ← FALSE
    reach[0] ← TRUE
    top ← 0                                        // upper bound on the obtainable sums
    for i ← 1 to n do
        top ← min(T, top + a[i])
        for s ← top down to a[i] do
            reach[s] ← reach[s] OR reach[s − a[i]]
        if reach[T] then return YES
    return NO
```

The variable `top` bounds the obtainable sums after step $i$ by $a_1 + \dots + a_i$ and by $T$, so cells above it are not visited. The inner loop is replaced by the shift of Section 4.2 in the bitset form.

### 4.4 Example

Let $a = (3, 1, 4, 2)$. Then $S = 10$, $T = 5$ and $\max a_i = 4 \le T$. The obtainable sums not exceeding $T$ are

| step $i$ | element $a_i$ | obtainable sums |
| -------- | ------------- | --------------- |
| 0        |               | $\{0\}$         |
| 1        | $3$           | $\{0, 3\}$      |
| 2        | $1$           | $\{0, 1, 3, 4\}$ |
| 3        | $4$           | $\{0, 1, 3, 4, 5\}$ |

After step $3$ the target $5$ is obtainable and the algorithm answers YES. Indeed, $1 + 4 = 3 + 2 = 5$.

### 4.5 Correctness

**Lemma 4.1** (invariant). After step $i$ of the loop, `reach[s]` $= R_i(s)$ for all $0 \le s \le T$.

*Proof.* Induction on $i$. Before the loop only `reach[0]` is TRUE, which is $R_0$. Assume that `reach` $= R_{i-1}$ before step $i$. Consider the moment at which the loop handles $s \ge a_i$. The cells with an index larger than $s$ have been rewritten, those with an index at most $s$ have not, and `reach[s − a[i]]` is among the latter. So it holds $R_{i-1}(s - a_i)$, and `reach[s]` receives $R_{i-1}(s) \lor R_{i-1}(s - a_i) = R_i(s)$. For $s < a_i$ the cell is not modified, and $R_i(s) = R_{i-1}(s)$. The bound `top` skips only cells that are FALSE in both rows, because no subset of $\{a_1, \dots, a_i\}$ has a sum larger than $a_1 + \dots + a_i$. $\square$

**Theorem 4.2.** `DP-PARTITION` returns YES if and only if a balanced partition exists.

*Proof.* The cases of odd $S$ and of $S = 0$ are as in Theorem 2.4 (the latter uses $n \ge 2$), and the case $\max a_i > T$ is Section 4.1. Otherwise $T > 0$, and by Section 1.2 a balanced partition exists if and only if $R_n(T)$ holds. By Lemma 4.1 the value of `reach[T]` after the last step is $R_n(T)$. If the loop stops earlier, at a step $i < n$ with `reach[T]` $= R_i(T)$ TRUE, then a subset of $\{a_1, \dots, a_i\}$ with sum $T$ is also a subset of $\{a_1, \dots, a_n\}$, so $R_n(T)$ holds as well. $\square$

### 4.6 Complexity

**Theorem 4.3.** `DP-PARTITION` takes time $O(n \cdot T)$ with a boolean array and $O(n + n \cdot T/w)$ with the bitset, where $w = 64$ is the word length. It needs $T + 1$ bits of memory.

*Proof.* The sum, the parity test and the maximum take $O(n)$. Each of the $n$ steps visits at most $T$ cells of the array, or shifts and combines $\lceil (T+1)/w \rceil$ words of the bitset. The table has $T + 1$ cells of one bit. $\square$

Since $T = S/2$, the time is $O(n \cdot \Sigma(M))$, as stated in Section 1.1. The bound `top` makes the early steps cheaper but does not change the worst case, and the early exit helps only for YES instances: for a NO instance with even $S$ and $\max a_i \le T$ all $n$ steps are executed.

**Pseudo-polynomial time.** The length of the input is $\Theta(n \log S)$ bits, whereas the running time contains the factor $T$, which grows tenfold with every additional decimal digit of the numbers. The algorithm is thus exponential in the length of the input, in agreement with the NP-completeness of PARTITION (Section 1.3).

### 4.7 Range of application and comparison with meet in the middle

The table has $T + 1$ bits, so the memory limits the algorithm to $T \le T_{DP}$ (Section 1.4). A target of order $10^{10}$ already needs about a gigabyte, and the numbers of $25$ digits of interest have $T$ of order $10^{24}$, which is far out of range.

Section 3 gives the alternative whose cost depends on $n$ only. Ignoring constant factors, the bitset version of dynamic programming costs $n \cdot T/w$ and meet in the middle costs $n \cdot 2^{n/2}$ operations. The common factor $n$ cancels.

**Proposition 4.4.** Dynamic programming performs fewer operations than meet in the middle if and only if $T/64 < 2^{n/2}$, that is,
$$
n > 2 \log_2 T - 12 .
$$
It needs less memory if and only if $T/8 < 16 \cdot 2^{n/2}$ bytes, that is, $n > 2 \log_2 T - 14$.

*Proof.* Take logarithms to base $2$ in the two inequalities. $\square$

Since $T \le T_{DP}$, the threshold is at most $2 \log_2 T_{DP} - 12$, which is of the order of fifty for $T_{DP}$ of order $10^{10}$. Moreover, meet in the middle is applied only for $n \le N_{MITM}$, whereas dynamic programming has no bound on $n$. Two examples illustrate the choice.

| | many small numbers | few large numbers |
| --- | --- | --- |
| Input | $n = 1000$ elements below $10^4$ | $n = 30$ elements of $25$ digits |
| Target | $T \le 5 \cdot 10^6$ | $T$ of order $10^{25}$ |
| Dynamic programming | $n \cdot T/64 \approx 8 \cdot 10^7$ operations, $T/8 \le 0.7$ MB | not applicable, since $T > T_{DP}$ |
| Meet in the middle | not applicable, since $n > N_{MITM}$ | $n \cdot 2^{n/2} \approx 10^6$ operations, $16 \cdot 2^{n/2} \approx 0.5$ MB per half |

Dynamic programming is thus the method for many elements with small values, and meet in the middle for few elements with arbitrary values. When both are out of range, only the approximation of Section 5 remains.

---

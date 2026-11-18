## 4. Dynamic programming

Brute force and meet in the middle look at subsets, and their cost grows with $n$ whatever the numbers are. Dynamic programming looks at subset *sums* instead. Many different subsets have the same sum, and only the sums between $0$ and $T$ matter, because all elements are non-negative. The cost therefore depends on the magnitude of the target $T$ and not only on $n$. This makes the algorithm pseudo-polynomial in the sense of Section 1.3: it is the best choice for many small numbers and the worst for few large ones, which is the opposite of meet in the middle.

### 4.1 Preliminary checks

Let $S = \Sigma(M)$ and $T = S/2$ as in Section 1.2. If $S$ is odd, no balanced partition exists, and if $S = 0$ one exists. Let $S$ be even and positive. By Section 1.2 the question is whether some index set $A$ has $\Sigma(A) = T$. One more case is decided at once: if some element satisfies $a_j > T$, then every set containing $j$ has a sum larger than $T$, and every set not containing $j$ has a complement that does. Hence the sums of the two parts cannot be equal, and the answer is NO. After this check $a_j \le T$ for all $j$. The check is not needed for correctness, since the update of Section 4.2 does nothing for an element larger than $T$. It keeps the arithmetic within range, though: the algorithm below computes $top + a_i$ with $top \le T$, and $a_i \le T$ gives $top + a_i \le 2T = S < 2^{128}$ (Section 1.4).

### 4.2 Idea

#### The recurrence

For $0 \le i \le n$ and $0 \le s \le T$ let

$$
R_i(s) = \text{TRUE} \iff \text{some subset of } \lbrace a_1, \dots, a_i \rbrace \text{ has sum } s .
$$

We want $R_n(T)$. With no elements the only obtainable sum is the empty sum, so

$$
R_0(s) = \begin{cases} \text{TRUE} & s = 0, \\ \text{FALSE} & s > 0. \end{cases}
$$

A subset of $\lbrace a_1, \dots, a_i \rbrace$ with sum $s$ either does not contain $a_i$, and is then a subset of $\lbrace a_1, \dots, a_{i-1} \rbrace$ with sum $s$, or contains $a_i$, and removing it leaves a subset of $\lbrace a_1, \dots, a_{i-1} \rbrace$ with sum $s - a_i$. Hence for $i \ge 1$

$$
R_i(s) = \begin{cases} R_{i-1}(s) & s < a_i, \\ R_{i-1}(s) \lor R_{i-1}(s - a_i) & s \ge a_i. \end{cases}
$$

Equal values need no special treatment: two equal elements are two indices and give two steps of the recurrence.

#### One row

Row $i$ depends only on row $i - 1$, so a single array `reach[0..T]` suffices, which after step $i$ holds $R_i$. The step reads `reach[s − a[i]]`, which must still hold the value of row $i - 1$. Since $s - a_i < s$ whenever $a_i > 0$, this is guaranteed if the cells are updated from the largest index to the smallest. For $a_i = 0$ the step reads the cell it is about to write, and the update `reach[s] ← reach[s] OR reach[s]` changes nothing, which is also correct.

#### Bitset

Every cell is a single bit, so the array is stored as a bitset of $T + 1$ bits packed into words of $w = 64$ bits. The update of the whole row is then `reach ← reach OR (reach << a[i])`, truncated at bit $T$. It is carried out word by word, in the order from the highest word to the lowest when it is done in place, and needs about $T/w$ word operations instead of $T$ cell updates.

#### Reading the subset back

The array `reach` tells whether the sum $T$ is obtainable, but not from which elements. A second array `first[0..T]` records, for every sum $s$, the step $i$ at which `reach[s]` became TRUE, and $0$ while it is FALSE. If `first[s]` $= i$, then `reach[s − a[i]]` was TRUE before step $i$, so the sum $s - a_i$ is obtained from elements with smaller indices, and $a_i > 0$, since a zero element changes no cell. Starting from $s = T$ and repeatedly putting $i =$ `first[s]` into $A$ and replacing $s$ by $s - a_i$ therefore collects distinct indices, in decreasing order, until $s = 0$. In the bitset form the cells that become TRUE in step $i$ are the set bits of the shifted word that are not set in the old word; they are found word by word, and every cell is recorded once during the whole run.

### 4.3 Algorithm

The input is a multiset $a_1, \dots, a_n$ with $n \ge 2$ (Section 1.1). The algorithm is applied only when $T \le T_{DP}$ (Section 1.4), so $T$ fits in a machine word and can index the array.

```
DP-PARTITION(a[1..n])
    S ← Σ a[i]
    if S is odd then return NO
    if S = 0 then return ({1}, {2, …, n})          // Section 1.2
    T ← S / 2
    if max a[i] > T then return NO                 // Section 4.1
    reach[0..T] ← FALSE;  first[0..T] ← 0
    reach[0] ← TRUE
    top ← 0                                        // upper bound on the obtainable sums
    for i ← 1 to n do
        top ← min(T, top + a[i])
        for s ← top down to a[i] do
            if not reach[s] and reach[s − a[i]] then
                reach[s] ← TRUE;  first[s] ← i     // s is obtained for the first time
        if reach[T] then return READ-BACK(a, first, T)
    return NO

READ-BACK(a[1..n], first[0..T], T)
    A ← ∅;  s ← T
    while s > 0 do
        i ← first[s]
        A ← A ∪ {i};  s ← s − a[i]
    return (A, {1, …, n} \ A)
```

The variable `top` bounds the obtainable sums after step $i$ by $a_1 + \dots + a_i$ and by $T$, so cells above it are not visited. The inner loop sets exactly the cells that `reach[s] ← reach[s] OR reach[s − a[i]]` would change, and records them in `first`. In the bitset form it is replaced by the shift of Section 4.2, together with the recording of the new bits.

### 4.4 Example

#### A YES instance

Let $a = (3, 1, 4, 2)$. Then $S = 10$, $T = 5$ and $\max a_i = 4 \le T$. The obtainable sums not exceeding $T$ are

| step $i$ | element $a_i$ | obtainable sums |
| -------- | ------------- | --------------- |
| 0        |               | $\lbrace 0 \rbrace$         |
| 1        | $3$           | $\lbrace 0, 3 \rbrace$      |
| 2        | $1$           | $\lbrace 0, 1, 3, 4 \rbrace$ |
| 3        | $4$           | $\lbrace 0, 1, 3, 4, 5 \rbrace$ |

After step $3$ the target $5$ is obtainable. The table `first` records the step at which each sum appeared: `first[3]` $= 1$, `first[1]` $=$ `first[4]` $= 2$ and `first[5]` $= 3$. Reading back from $T = 5$ gives $i = 3$ with $a_3 = 4$ and the remaining sum $1$, then $i = 2$ with $a_2 = 1$ and the remaining sum $0$. The algorithm returns $A = \lbrace 2, 3 \rbrace$, $B = \lbrace 1, 4 \rbrace$, and indeed $1 + 4 = 3 + 2 = 5$.

In the bitset form the sums are the bits that are set. Step $2$ computes $\lbrace 0, 3 \rbrace \cup (\lbrace 0, 3 \rbrace + 1) = \lbrace 0, 1, 3, 4 \rbrace$, the shift by $a_2 = 1$ moving every bit one place up, and step $3$ shifts by $4$ and cuts the result at $T = 5$.

#### A NO instance

Let $a = (2, 2, 3, 5)$. Then $S = 12$, $T = 6$ and $\max a_i = 5 \le T$. The obtainable sums not exceeding $T$ are $\lbrace 0 \rbrace$, $\lbrace 0, 2 \rbrace$, $\lbrace 0, 2, 4 \rbrace$, $\lbrace 0, 2, 3, 4, 5 \rbrace$ and, after the last element, again $\lbrace 0, 2, 3, 4, 5 \rbrace$. The target $6$ is never reached, so all $4$ steps are executed and the answer is NO.

### 4.5 Correctness

**Lemma 4.1** (invariant). After step $i$ of the loop, `reach[s]` $= R_i(s)$ for all $0 \le s \le T$. Moreover, for every $s > 0$ with $R_i(s)$, the entry $j =$ `first[s]` satisfies $1 \le j \le i$, $a_j > 0$ and $R_{j-1}(s - a_j)$, and it is not changed by later steps.

*Proof.* Induction on $i$. Before the loop only `reach[0]` is TRUE, which is $R_0$, and the second claim is empty. Assume both claims after step $i - 1$. Consider the moment at which the loop handles $s \ge a_i$. The cells with an index larger than $s$ have been rewritten, those with an index at most $s$ have not, and `reach[s − a[i]]` is among the latter. So it equals $R_{i-1}(s - a_i)$, and after the step `reach[s]` is TRUE exactly when $R_{i-1}(s) \lor R_{i-1}(s - a_i) = R_i(s)$ holds. For $s < a_i$ the cell is not modified, and $R_i(s) = R_{i-1}(s)$. The bound `top` skips only cells that are FALSE in both rows. The cell `reach[s]` above the new bound keeps its value $R_{i-1}(s)$, and both $R_{i-1}(s)$ and $R_i(s)$ are FALSE there: no subset of $\lbrace a_1, \dots, a_i \rbrace$ has a sum larger than $a_1 + \dots + a_i$, and the previous value of `top` is the same bound for $i - 1$.

For the table, a cell that was TRUE before step $i$ is not written again, so its entry keeps the property it had. A cell $s$ that becomes TRUE in step $i$ receives `first[s]` $= i$, and at that moment `reach[s − a[i]]` $= R_{i-1}(s - a_i)$ is TRUE. Moreover $a_i > 0$: for $a_i = 0$ the condition would require `reach[s]` to be FALSE and TRUE at once. $\square$

**Theorem 4.2.** `DP-PARTITION` returns a balanced partition if one exists, and NO otherwise.

*Proof.* The cases of odd $S$ and of $S = 0$ are as in Theorem 2.4 (the latter uses $n \ge 2$), and the case $\max a_i > T$ is Section 4.1. Otherwise $T > 0$, and by Section 1.2 a balanced partition exists if and only if $R_n(T)$ holds. By Lemma 4.1 the value of `reach[T]` after step $i$ is $R_i(T)$, and $R_i(T)$ implies $R_n(T)$, because a subset of $\lbrace a_1, \dots, a_i \rbrace$ is also a subset of $\lbrace a_1, \dots, a_n \rbrace$. Hence, if no balanced partition exists, `READ-BACK` is never called and the answer is NO. If one exists, $R_n(T)$ holds and `READ-BACK` is called at the latest after step $n$.

It remains to show that `READ-BACK`, called after a step $i$ with $R_i(T)$, returns a balanced partition. By Lemma 4.1, for the current sum $s = T > 0$ the index $j =$ `first[s]` satisfies $1 \le j \le i$, $a_j > 0$ and $R_{j-1}(s - a_j)$. So the next sum $s - a_j$ is smaller than $s$ and obtainable from $a_1, \dots, a_{j-1}$. If it is positive, Lemma 4.1 after step $j - 1$ applies to it, and its index is at most $j - 1$, since the entries of `first` are not changed by later steps. Hence the indices put into $A$ strictly decrease and are distinct, the sums $s$ strictly decrease and stay non-negative, and the loop ends with $s = 0$ after at most $n$ iterations. The elements put into $A$ account for the decrease of $s$ from $T$ to $0$, so $\Sigma(A) = T$. Its complement has the sum $S - T = T$, and both parts are non-empty because $T > 0$. $\square$

### 4.6 Complexity

**Theorem 4.3.** `DP-PARTITION` takes time $O(n \cdot T)$ with boolean arrays and $O(n + n \cdot T/w + T)$ with the bitset, where $w = 64$ is the word length. It needs $T + 1$ bits for `reach` and $T + 1$ entries of `first`, each an index of $\lceil \log_2 (n+1) \rceil$ bits, that is $O(T \log n)$ bits.

*Proof.* The sum, the parity test and the maximum take $O(n)$. Each of the $n$ steps visits at most $T$ cells of the arrays, or shifts and combines $\lceil (T+1)/w \rceil$ words of the bitset. In the bitset form the new bits of a word are found one by one, and every cell becomes TRUE at most once, so their recording in `first` takes $O(T)$ during the whole run. The initialisation of both tables takes $O(T)$. `READ-BACK` performs at most $n$ iterations, since its indices are distinct, and writing the partition takes $O(n)$. The table `reach` has $T + 1$ cells of one bit in the bitset, and `first` has $T + 1$ entries with values in $\lbrace 0, \dots, n \rbrace$. $\square$

Since $T = S/2$, the time is $O(n \cdot \Sigma(M))$, as stated in Section 1.1. The bound `top` makes the early steps cheaper but does not change the worst case, and the early exit helps only for YES instances: for a NO instance with even $S$ and $\max a_i \le T$ all $n$ steps are executed.

#### Pseudo-polynomial time

The length of the input is $\sum_i \log_2 (a_i + 1) = O(n \log S)$ bits, whereas the running time contains the factor $T$, which grows tenfold with every additional decimal digit of the numbers. The algorithm is thus exponential in the length of the input, in agreement with the NP-completeness of PARTITION (Section 1.3).

### 4.7 Range of application and comparison with meet in the middle

The tables have $T + 1$ bits and $T + 1$ entries of `first`. Stored as $32$-bit numbers, which suffice for $n < 2^{32}$, the entries take $4(T + 1)$ bytes, thirty-two times the bitset, so the memory limits the algorithm to $T \le T_{DP}$ (Section 1.4). A target of order $2.5 \cdot 10^8$ already needs about a gigabyte, and the numbers of $25$ digits of interest have $T$ of order $10^{24}$ at least, since a single such number is at least $10^{24}$, which is far out of range.

#### Comparison with meet in the middle

Section 3 gives the alternative whose cost depends on $n$ only. We compare two rough estimates with constant factors dropped: the bitset version of dynamic programming costs about $n \cdot T/w$ operations (the additional term $T$ for the table `first` is dropped) and meet in the middle about $n \cdot 2^{n/2}$ (the factor $n$ of meet in the middle comes from sorting). The common factor $n$ cancels. For the memory we take $4T$ bytes for the table `first`, which dominates the $T/8$ bytes of the bitset, and, for meet in the middle, the two lists of $2^{n/2}$ sums of $16$ bytes each, that is $32 \cdot 2^{n/2}$ bytes for even $n$.

**Proposition 4.4.** For these estimates, dynamic programming performs fewer operations than meet in the middle if and only if $T/64 < 2^{n/2}$, that is,

$$
n > 2 \log_2 T - 12 .
$$

It needs less memory if and only if $4T < 32 \cdot 2^{n/2}$, that is, $n > 2 \log_2 T - 6$.

*Proof.* Take logarithms to base $2$ in the two inequalities. $\square$

Since $T \le T_{DP}$, the threshold for the number of operations is at most $2 \log_2 T_{DP} - 12$ and the threshold for the memory at most $2 \log_2 T_{DP} - 6$, which are about forty-one and about forty-seven for $T_{DP} = 10^{8}$, and about forty-four and about fifty for $T_{DP} = 3 \cdot 10^{8}$. Moreover, meet in the middle is applied only for $n \le N_{MITM}$, whereas dynamic programming has no bound on $n$. Two examples illustrate the choice.

| | many small numbers | few large numbers |
| --- | --- | --- |
| Input | $n = 1000$ elements below $10^4$ | $n = 30$ elements of $25$ digits |
| Target | $T \le 5 \cdot 10^6$ | $T$ at least of order $10^{24}$ |
| Dynamic programming | $n \cdot T/64 \approx 8 \cdot 10^7$ operations, $4T \le 20$ MB | not applicable, since $T > T_{DP}$ |
| Meet in the middle | not applicable, since $n > N_{MITM}$ | $n \cdot 2^{n/2} \approx 10^6$ operations, $32 \cdot 2^{n/2} \approx 1$ MB |

Dynamic programming is thus the method for many elements with small values, and meet in the middle for few elements with arbitrary values. When both are out of range, only the approximation of Section 5 remains.

---

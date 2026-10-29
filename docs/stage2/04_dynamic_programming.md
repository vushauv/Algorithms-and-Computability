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


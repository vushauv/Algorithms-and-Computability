# Stage 2: Documentation of Algorithms (basic version)

## Abstract

This report documents the algorithms for the partition problem: given a multiset $M$ of non-negative integers, split it into two non-empty parts with equal sums or establish that no such split exists, and find a partition whose two sums are as close as possible. The decision problem is NP-complete, but only in the weak sense, so its cost is governed by two parameters, the number $n$ of elements and the magnitude of their sum $\Sigma(M)$. We describe four algorithms, each with pseudocode, a correctness proof and a complexity analysis. Three of them are exact and return a balanced partition whenever one exists: brute force with a Gray-code enumeration, in time $\Theta(2^n)$; meet in the middle, in time $O(n \cdot 2^{n/2})$ and memory $O(2^{n/2})$; and dynamic programming over the subset sums, in pseudo-polynomial time $O(n \cdot \Sigma(M))$. The fourth, the Karmarkar-Karp differencing method, is a polynomial-time approximation in time $O(n \log n)$ that returns a partition for every input. The final algorithm always runs the approximation and, whenever the input allows it, one exact algorithm chosen by the size and the magnitude of the input, so that the answer is exact, comes with a balanced partition whenever one exists, and the quality of the approximation can be compared with it.


## 1. Problem statement

### 1.1 Introduction

The terms used in this section are defined formally in Section 1.2.

Let $M = \{a_1, \dots, a_n\}$ be a finite multiset of non-negative integers, that is, a collection in which values may repeat. A *partition* of $M$ is a division of it into two non-empty parts $A$ and $B$. It is *balanced* if $\Sigma(A) = \Sigma(B)$, where $\Sigma$ denotes the sum of the elements of a part.

For example, $\{3, 1, 4, 2\}$ has the balanced partition $A = \{3, 2\}$, $B = \{1, 4\}$, with $\Sigma(A) = \Sigma(B) = 5$. The multiset $\{1, 1, 4\}$ has no balanced partition: it would require $\Sigma(A) = \Sigma(M)/2 = 3$, but no subset of $\{1, 1, 4\}$ has sum $3$.

#### The two problems

We consider two problems. Section 1.2 gives their formal definitions.

- **The decision problem.** Given $M$, decide whether a balanced partition exists. The answer is YES or NO. The exact algorithms below solve it in its search form: they return a balanced partition when one exists, and NO otherwise.
- **The optimisation problem.** Given $M$, find a partition minimising the *discrepancy* $d(A, B) = |\Sigma(A) - \Sigma(B)|$. A partition is balanced if and only if $d(A, B) = 0$.

The decision problem is NP-complete [Karp 1972]. Hence no polynomial-time algorithm for it is known, and none exists unless P = NP. It is NP-complete only in the weak sense: it admits an algorithm whose running time is polynomial in $n$ and $\Sigma(M)$, a *pseudo-polynomial* algorithm [Garey and Johnson 1979]. Two parameters therefore govern the cost: the number of elements $n$ and the magnitude of $\Sigma(M)$. The algorithms below differ in which of them their cost depends on.

#### The algorithms of this report

We present four algorithms.

- **Section 2, brute force.** It enumerates all $2^n$ subsets. Time $\Theta(2^n)$ with a Gray-code enumeration, memory $O(1)$ beyond the input. It serves as the baseline and is not part of the final algorithm.
- **Section 3, meet in the middle.** It splits $M$ into two halves and combines their subset sums [Horowitz and Sahni 1974]. Time $O(n \cdot 2^{n/2})$, memory $O(2^{n/2})$, independent of the magnitude of the elements.
- **Section 4, dynamic programming.** It computes the set of reachable subset sums and reads a subset with the target sum back from a table. Time $O(n \cdot \Sigma(M))$, which is pseudo-polynomial.
- **Section 5, Karmarkar-Karp differencing.** This is a polynomial-time approximation algorithm, with time $O(n \log n)$ [Karmarkar and Karp 1983]. It returns a partition for every input, balanced or not, and its discrepancy is bounded (Section 5.5). We also quote its expected behaviour on random inputs [Yakir 1996].

None of the four algorithms is efficient for every input. Section 6 defines a final algorithm that selects, for each input, the algorithm that best matches it. For each algorithm we give pseudocode, a description, a correctness proof and a complexity analysis.

The remainder of this section is organised as follows: the constraints and assumptions on the input (below), the formal definitions (Section 1.2), the hardness results (Section 1.3) and the computational model (Section 1.4).

#### Constraints on the input

The input is a multiset $M = \{a_1, \dots, a_n\}$ of non-negative integers. Equal values are different elements and are distinguished by their position $1, \dots, n$. Both parts of a partition must be non-empty, because we do not assign a sum to the empty part, in particular we do not set it to $0$. Consequently $n \ge 2$, and a multiset with one element has no partition.

#### Assumptions

The analysis also rests on the following assumptions. An input that violates one of them is rejected with an error message and is never truncated.

| Assumption | Reason |
| --- | --- |
| $0 \le a_i < 10^{38}$ | The numbers of interest have about $25$ digits, which exceed $64$ bits, so we use unsigned 128-bit integers (Section 1.4). A wider integer type would lift the bound without changing the algorithms. We do not need it: dynamic programming is already infeasible for numbers of this size, and the cost of the other algorithms does not depend on the magnitude of the numbers. |
| $\Sigma(M) < 2^{128}$ | The same reason as for the bound above: the total is also an unsigned 128-bit integer. |
| Each exponential algorithm is applied only within a stated range of $n$ or of the target sum. | Outside its range it may not finish or fit in memory; Section 6 then uses the approximation. |


### 1.2 Definitions

**Multiset.** A multiset $M$ over $\mathbb{N} = \{0, 1, 2, \dots\}$ is a pair $(U, m)$ where $U$ is a finite set and $m: U \to \{1, 2, 3, \dots\}$ gives the multiplicity of each element. We write it as a list $M = \{a_1, a_2, \dots, a_n\}$ in which values may repeat, and $n = \sum_{u \in U} m(u)$ is its size.

**Sum.** $\Sigma(M) = \sum_{i=1}^{n} a_i$. For an index set $I \subseteq \{1,\dots,n\}$, $\Sigma(I) = \sum_{i \in I} a_i$.

**Partition into two subsets.** A pair $(A, B)$ of **non-empty** index sets with $A \cup B = \{1,\dots,n\}$ and $A \cap B = \emptyset$. The non-emptiness of both parts is a constraint on the input (Section 1.1) and implies $n \ge 2$.

**Balanced partition.** A partition with $\Sigma(A) = \Sigma(B)$, equivalently $\Sigma(A) = \Sigma(M)/2$.

**Discrepancy.** $d(A,B) = |\Sigma(A) - \Sigma(B)|$. A partition is balanced iff $d = 0$. This is the quality measure for the approximation algorithm.

**The decision problem PARTITION.** *Instance:* a multiset $M$ of non-negative integers. *Question:* does a balanced partition exist?

**The search problem.** *Instance:* as above. *Task:* return a balanced partition if one exists, and NO otherwise. An algorithm for the search problem decides PARTITION, and a returned partition can be checked with $O(n)$ additions (Section 1.3).

**The optimisation problem.** Find a partition minimising $d(A,B)$. Its optimum is $0$ iff the decision instance is a yes-instance, so an exact optimiser also decides PARTITION.

**Target sum.** $T = \Sigma(M)/2$. If $\Sigma(M)$ is odd there is no balanced partition.

#### Trivial cases

Every exact algorithm below (Sections 2 to 4) starts with two checks. The approximation of Section 5 needs neither, and the final algorithm of Section 6 obtains them from its result.

- **Odd total.** If $\Sigma(M)$ is odd, no balanced partition exists, and the answer is NO.
- **Zero total.** If $\Sigma(M) = 0$, every element is $0$, and the partition $A = \{1\}$, $B = \{2, \dots, n\}$ is balanced. Both parts are non-empty because $n \ge 2$, the answer is YES, and this partition is returned.

After these checks every exact algorithm may assume $\Sigma(M) > 0$, hence $T > 0$. Then:

- **Non-empty parts.** Any index set $A$ with $\Sigma(A) = T$ is non-empty, because $T > 0$. Its complement $B$ satisfies $\Sigma(B) = \Sigma(M) - T = T > 0$, so it is non-empty as well.
- **Consequence for the algorithms.** To solve the search problem, an algorithm only has to find an index set $A$ with $\Sigma(A) = T$ and return it together with its complement $B = \{1, \dots, n\} \setminus A$, or to establish that no such $A$ exists. Every such $A$ and its complement $B$ form a partition into two non-empty parts, by the previous item. Hence no correctness proof needs an additional argument that the parts are non-empty.

### 1.3 Hardness

**Theorem 1.1** (Karp [1972]). PARTITION is NP-complete.

PARTITION belongs to NP: a balanced partition is a certificate, and checking it takes $O(n)$ additions. NP-hardness was shown by Karp, who lists PARTITION among his 21 NP-complete problems. We quote this result and do not reprove it.

#### Weak hardness

PARTITION is NP-complete only in the weak sense. It is solved in time $O(n \cdot \Sigma(M))$ by dynamic programming (Section 4). This bound is polynomial in $n$ and in the *magnitude* $\Sigma(M)$, but exponential in the *length* of the input, which is proportional to $n$ times the number of digits of the elements. Such an algorithm is called pseudo-polynomial. Unless P = NP, a problem with a pseudo-polynomial algorithm is not NP-complete in the strong sense [Garey and Johnson 1979, Section 4.2].

#### Consequences for the algorithms

- **Large numbers.** For numbers of about $25$ digits, $\Sigma(M)$ is of order $10^{25}$ and the cost $O(n \cdot \Sigma(M))$ is out of reach. An exact algorithm must then have a cost that depends on $n$ alone. The best ones are exponential in $n$ (Sections 2 and 3).
- **Approximation.** Since PARTITION is NP-complete, no polynomial-time algorithm for it is known, and none exists unless P = NP.

### 1.4 Computational model and number representation

#### Representation

All numbers, that is the elements, their sums and their differences, are unsigned 128-bit integers. The largest such integer is $2^{128} - 1 \approx 3.4 \cdot 10^{38}$, which has $39$ decimal digits. The elements are non-negative, so no sign bit is needed.

#### No overflow

Every value computed by an algorithm below is a subset sum or the non-negative difference of two subset sums, hence it lies between $0$ and $\Sigma(M)$. Together with the assumption $\Sigma(M) < 2^{128}$ of Section 1.1 this means that no overflow occurs. Two algorithms need even less: Karmarkar-Karp (Section 5) computes only differences, each at most $\max_i a_i$, and dynamic programming (Section 4) computes only sums up to $\Sigma(M)$, since its target $\Sigma(M)/2$ is at most $T_{DP}$, where $T_{DP}$ is defined below.

#### Cost model

We use the unit-cost RAM model: an arithmetic operation or a comparison on a 128-bit number costs $O(1)$, and so does an operation on a machine word of $w = 64$ bits, which dynamic programming uses for its bitset (Section 4). Time bounds count such operations and are expressed in $n$ and, for dynamic programming, in $\Sigma(M)$. Memory is counted in 128-bit numbers, except for the tables of Section 4, which are counted in bits.

#### Ranges of the algorithms

Each exponential algorithm is applied only within a range of inputs, because outside it the algorithm may not finish or may not fit in memory (Section 1.1). The ranges are parameters:

| Algorithm | Range | Limiting resource |
| --- | --- | --- |
| Meet in the middle (Section 3) | $n \le N_{MITM}$ | memory $O(2^{n/2})$ |
| Dynamic programming (Section 4) | $\Sigma(M)/2 \le T_{DP}$ | memory $O(\Sigma(M) \log n)$ bits and time $O(n \cdot \Sigma(M))$ |

Brute force (Section 2) is the baseline for the other two and is not used by the final algorithm, so it has no range. The values of $N_{MITM}$ and $T_{DP}$ are not fixed in this document. They depend on the implementation and on the machine, so they are determined experimentally after the algorithms have been implemented. The analysis of each algorithm holds for any values of the parameters. The final algorithm (Section 6) takes them as input.

---

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

**Lemma 3.3** (sweep). The loop performs at most $|LS| + |RS| - 1$ iterations. If entries with $x + y = T$ exist, the sweep stops at such a pair of entries.

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

## 5. Karmarkar-Karp differencing

By Section 1.3 no polynomial-time algorithm for PARTITION is known, and Sections 2 to 4 are exponential in $n$ or in the magnitude of the numbers. This section gives a polynomial-time approximation of the optimisation problem of Section 1.1, which minimises the discrepancy $d$. The differencing method of Karmarkar and Karp [1983] runs in time $O(n \log n)$ whatever the numbers are (in the model of Section 1.4), and it returns a partition for every input, whereas the exact algorithms return one only when it is balanced. It cannot decide PARTITION, since it may return $d > 0$ for an instance that has a balanced partition (Section 5.3), but $d = 0$ proves that the answer is YES.

### 5.1 Idea

Take the two largest remaining numbers $x \ge y$ and decide that they go to *opposite* parts. Whatever happens to the other numbers, the pair then contributes only $x - y$ to the difference of the two parts, so the pair is replaced by the single number $x - y$. Repeating this until one number is left, that number is the discrepancy of the final partition. To recover the parts, every decision is recorded as an edge "$u$ and $v$ are in opposite parts", and the resulting graph is two-coloured at the end.

#### Comparison with the greedy method

The simple greedy method, which puts the next largest number into the lighter part, fixes the part of an element at once. Differencing postpones this decision and fixes only the relation between two elements, which lets large numbers cancel each other.

### 5.2 Algorithm

The input is a multiset $a_1, \dots, a_n$ with $n \ge 2$ (Section 1.1).

```
KARMARKAR-KARP(a[1..n])
    H ← max-heap of the pairs (a[i], i)
    E ← empty list of edges on the vertices 1..n
    while |H| > 1 do
        (x, u) ← EXTRACT-MAX(H)
        (y, v) ← EXTRACT-MAX(H)                    // x ≥ y
        INSERT(H, (x − y, u))                      // the difference keeps the index of the larger element
        append (u, v) to E                         // u and v are in opposite parts
    (d, _) ← the single entry of H
    colour the vertices of G = ({1..n}, E) with 0 and 1 so that the ends of every edge differ   // breadth-first search from vertex 1
    A ← { i : colour(i) = 0 };  B ← { i : colour(i) = 1 }
    return (A, B, d)
```

Ties in the heap are broken in any way. The subtraction $x - y$ is done with $x \ge y$, so it never gives a negative number. The loop runs at least once because $n \ge 2$. The new entry keeps the index of the *larger* element $u$ on purpose: this is what makes the colouring step correct (Lemma 5.1).

### 5.3 Examples

#### The method finds the optimum

Let $a = (3, 1, 4, 2)$, and break ties in favour of the smaller index.

| round | extracted $(x, u)$, $(y, v)$ | inserted | edge | heap after the round |
| ----- | ---------------------------- | -------- | ---- | -------------------- |
| 0     |                              |          |      | $4_{3}, 3_{1}, 2_{4}, 1_{2}$ |
| 1     | $(4, 3), (3, 1)$             | $(1, 3)$ | $\lbrace 3, 1 \rbrace$ | $2_{4}, 1_{2}, 1_{3}$ |
| 2     | $(2, 4), (1, 2)$             | $(1, 4)$ | $\lbrace 4, 2 \rbrace$ | $1_{3}, 1_{4}$        |
| 3     | $(1, 3), (1, 4)$             | $(0, 3)$ | $\lbrace 3, 4 \rbrace$ | $0_{3}$               |

In the heap column a subscript marks the index, so $4_{3}$ is the value $4$ with the index $3$. The final entry is $d = 0$. The edges form the path $1 - 3 - 4 - 2$, which is coloured $1 \mapsto 0$, $3 \mapsto 1$, $4 \mapsto 0$, $2 \mapsto 1$. So $A = \lbrace 1, 4 \rbrace$ with values $3, 2$ and $B = \lbrace 2, 3 \rbrace$ with values $1, 4$ have both sum $5$.

#### The method misses the optimum

Let $a = (5, 6, 7, 8, 10)$, so $S = 36$ and $T = 18$. The rounds replace $10, 8$ by $2$ (edge $\lbrace 5, 4 \rbrace$), $7, 6$ by $1$ (edge $\lbrace 3, 2 \rbrace$), $5, 2$ by $3$ (edge $\lbrace 1, 5 \rbrace$) and $3, 1$ by $2$ (edge $\lbrace 1, 3 \rbrace$). Hence $d = 2$, with the parts $A = \lbrace 1, 2, 4 \rbrace$ of sum $5 + 6 + 8 = 19$ and $B = \lbrace 3, 5 \rbrace$ of sum $7 + 10 = 17$. However, the elements $\lbrace 8, 10 \rbrace$ and $\lbrace 5, 6, 7 \rbrace$, that is, the index sets $\lbrace 4, 5 \rbrace$ and $\lbrace 1, 2, 3 \rbrace$, both have sum $18$, so the optimum is $0$. Thus $d > 0$ does not show that the answer is NO.

### 5.4 Correctness

Let $G$ be the graph on the vertices $1, \dots, n$ whose edges are those of $E$ added so far. The *classes* of a tree are the two sides of its two-colouring, which is unique up to swapping the colours.

**Lemma 5.1** (invariant). Before the first round and after every round:

1. $G$ is a forest, and every connected component $C$ of $G$ has exactly one entry $(z, u)$ in $H$, where $u$ is a vertex of $C$;
2. the sums of the two classes of $C$ differ by exactly $z$, and $u$ lies in a class with the larger or equal sum.

*Proof.* Induction on the number of rounds. At the start every vertex is a component with the entry $(a_i, i)$, whose classes $\lbrace i \rbrace$ and $\emptyset$ differ by $a_i$, and $i$ lies in the larger one.

Assume the claim before a round. The round extracts $(x, u)$ and $(y, v)$, which belong to two different components $P$ and $Q$, since each component has one entry. The new edge $\lbrace u, v \rbrace$ joins two components, so $G$ remains a forest, and the merged tree has the single entry $(x - y, u)$. This proves part 1.

For part 2, let $H_P \ni u$ and $L_P$ be the classes of $P$ with $\Sigma(H_P) - \Sigma(L_P) = x$, and $H_Q \ni v$ and $L_Q$ those of $Q$ with $\Sigma(H_Q) - \Sigma(L_Q) = y$. The edge forces $u$ and $v$ into opposite colours, so the classes of the merged tree are $H_P \cup L_Q$ and $L_P \cup H_Q$. Their sums differ by

$$
(\Sigma(H_P) + \Sigma(L_Q)) - (\Sigma(L_P) + \Sigma(H_Q)) = x - y \ge 0 ,
$$

and the larger class $H_P \cup L_Q$ contains $u$. $\square$

**Theorem 5.2.** `KARMARKAR-KARP` stops after $n - 1$ rounds, $G$ is a tree on all $n$ vertices, both $A$ and $B$ are non-empty, and $d = |\Sigma(A) - \Sigma(B)|$.

*Proof.* Each round removes two entries and inserts one, so $|H|$ decreases by $1$, and the loop stops after $n - 1$ rounds with one entry left. By Lemma 5.1 there is one component, so $G$ is a tree with $n - 1 \ge 1$ edges. A tree is bipartite, so the colouring exists, and $A$ and $B$ are its two classes. By part 2 of the lemma, $d$ is the difference of their sums. The ends of an edge have different colours, so neither class is empty. $\square$

If $d = 0$, the returned $(A, B)$ is a balanced partition and the answer YES is certified. If $d > 0$, nothing follows about the existence of a balanced partition, as the second example shows. As $d = |S - 2\Sigma(B)|$, the discrepancy always has the same parity as $S$.

Equal values and zeros need no special treatment. The multiset of values after a round is the same whichever of two equal values is extracted first, so $d$ does not depend on the tie-breaking. Only the indices, and hence the returned parts, may differ.

### 5.5 Quality of the result

Since the optimum of the discrepancy can be $0$, a ratio of discrepancies is meaningless. We state what can be guaranteed.

#### An absolute bound

**Proposition 5.3.** Let $m = \max_i a_i$. Then $d \le m$.

*Proof.* Every entry of $H$ is at most $m$ at the start, and a new entry $x - y \le x \le m$ is again at most $m$. The last entry is $d$. $\square$

This bound is weak: the instances $k \cdot (5, 6, 7, 8, 10)$ have the optimum $0$ and give $d = 2k$, so no bound of the form "optimum plus a constant" exists.

#### Ratio on the larger part

The larger part has the sum $(S + d)/2$. Let $\mathrm{OPT}$ be the smallest possible sum of the larger part over all partitions. It is at least $\max(S/2, m)$, because the larger part has at least the average sum $S/2$, and the part that contains the largest element has at least $m$. So by Proposition 5.3

$$
\frac{(S + d)/2}{\mathrm{OPT}} \le \frac{S/2 + m/2}{\mathrm{OPT}} \le \frac{\mathrm{OPT} + \mathrm{OPT}/2}{\mathrm{OPT}} = \frac{3}{2} .
$$

The bound $3/2$ is what this elementary argument gives. Fischetti and Martello [1987] show a sharper result, that the worst-case ratio of the differencing method is exactly $7/6$. We quote it without proof.

#### Random inputs

For items drawn independently from a uniform distribution on $[0, 1]$, the expected discrepancy of the differencing method is $n^{-\Theta(\log n)}$ [Yakir 1996], which proves a conjecture of Karmarkar and Karp. We quote this result without proof. It says that for many random items the discrepancy becomes extremely small. For integers it is only an indication.

### 5.6 Complexity

**Theorem 5.4.** `KARMARKAR-KARP` takes time $O(n \log n)$ and memory $O(n)$ numbers and indices, and the time does not depend on the magnitude of the numbers.

*Proof.* Building the heap takes $O(n)$. The loop has $n - 1$ rounds, each with two extractions and one insertion of $O(\log n)$ time, which gives $O(n \log n)$. The graph $G$ has $n$ vertices and $n - 1$ edges, so the colouring by a breadth-first or depth-first search takes $O(n)$. The heap, the edges and the adjacency lists use $O(n)$ numbers and indices. $\square$

### 5.7 Range of application and remarks

The algorithm needs no bound on $n$ beyond $n \ge 2$ and no bound on the numbers beyond the assumptions of Section 1.1; all differences are at most $\max_i a_i$, so the sum $S$ is not needed (Section 1.4). It works for every input, in particular when neither dynamic programming nor meet in the middle is in range, for example for $1000$ elements of $25$ digits. A result with $d = 0$ proves YES, but $d > 0$ gives no answer, so a NO can come only from the parity test or from an exact algorithm.

#### Related methods

The greedy method, which processes the numbers in descending order and puts each into the part with the smaller sum so far, also takes time $O(n \log n)$ and satisfies $d \le \max_i a_i$: when a number $a$ is added to the lighter part, the difference $d$ of the two parts becomes $|d - a| \le \max(d, a)$, and the claim follows by induction. As a scheduling algorithm (longest processing time first) its ratio on the larger part is $7/6$ for two machines [Graham 1969]. Korf [1998] turns the differencing method into an exact search that at each step also tries to put the two largest numbers into the *same* part, replacing them by $x + y$. Its worst case is exponential, and we do not use it.

---

## 6. The final algorithm

None of the algorithms of Sections 2 to 5 is satisfactory on its own. Dynamic programming is cheap when the numbers are small, however many there are, and meet in the middle is cheap when there are few numbers, however large. When both fail, only the approximation of Section 5 remains, and it cannot decide PARTITION. This section combines the algorithms into one that adapts to the input: it always computes the Karmarkar-Karp partition, which costs only $O(n \log n)$, and it adds an exact algorithm whenever one is in range for the input. The result consists of the answer, the partition of Section 5 and, with the answer YES, a balanced partition, so the answer also tells how good the partition of Section 5 is.

### 6.1 Idea

#### The result

The result of the final algorithm is a quadruple (answer, $(A, B)$, $d$, $P$), where

- $(A, B)$ is the partition produced by Karmarkar-Karp differencing and $d$ is its discrepancy (Theorem 5.2),
- the answer is YES or NO, if the decision problem has been solved, and UNKNOWN otherwise, and
- $P$ is a balanced partition if the answer is YES, and is absent otherwise.

#### The steps

1. If $d = 0$, the partition $(A, B)$ is balanced, the answer is YES, and $P = (A, B)$. This also covers $\Sigma(M) = 0$, because $d \le \max_i a_i = 0$ by Proposition 5.3.
2. If $\Sigma(M)$ is odd, the answer is NO (Section 1.2).
3. Otherwise $d > 0$, $\Sigma(M)$ is even and positive, and the question is open. An exact algorithm is applied if one of them is in range, that is, dynamic programming if $T \le T_{DP}$ and meet in the middle if $n \le N_{MITM}$ (Section 1.4). If both are in range, the choice depends on the input: by Section 4.7, dynamic programming is the cheaper one for many small numbers and meet in the middle for few large ones. The rule that decides between them is fixed together with the parameters $N_{MITM}$ and $T_{DP}$, after the algorithms have been implemented and measured. Both algorithms return a balanced partition exactly when one exists (Theorems 3.4 and 4.2), so the choice affects the cost, and possibly which balanced partition is returned, but not the answer. A returned balanced partition becomes $P$ with the answer YES, and NO stays NO.
4. If neither exact algorithm is in range, the answer is UNKNOWN.

#### Comparison of the approximation with the exact answer

When an exact algorithm has run, its answer measures the approximation on the same input. If the answer is YES and $d = 0$, the differencing method has found a balanced partition. If the answer is YES and $d > 0$, a balanced partition exists but the method has missed it, as in the second example of Section 5.3; the result then contains the balanced partition $P$ found by the exact algorithm next to $(A, B)$. If the answer is NO, no balanced partition exists, and $d$ is the discrepancy of a partition that cannot be improved to $0$; whether it is the smallest possible discrepancy is not decided. The final algorithm thus delivers, for every input within the ranges, the exact decision together with the quality of the approximation.

#### Brute force

Brute force is not used. Its time is $\Theta(2^n)$, whereas the time of meet in the middle is $O(n \cdot 2^{n/2})$, which is smaller already for $n \ge 5$. The only advantage of brute force is its constant memory, and this does not matter for the few tens of elements it can handle, where the lists of meet in the middle are small (Section 3.6). It remains the reference against which the other algorithms are tested.

### 6.2 Algorithm

The input is a multiset $a_1, \dots, a_n$ with $n \ge 2$ (Section 1.1). The parameters $N_{MITM}$ and $T_{DP}$ are those of Section 1.4.

```
SOLVE(a[1..n]; N_MITM, T_DP)
    S ← Σ a[i]
    (A, B, d) ← KARMARKAR-KARP(a)                   // O(n log n), always
    if d = 0 then return (YES, A, B, d, (A, B))     // balanced; includes S = 0
    if S is odd then return (NO, A, B, d, —)        // Section 1.2
    T ← S / 2
    if T ≤ T_DP and n ≤ N_MITM then
        EXACT ← DP-PARTITION or MEET-IN-THE-MIDDLE, whichever is cheaper for (n, T)   // Section 4.7
    else if T ≤ T_DP then EXACT ← DP-PARTITION
    else if n ≤ N_MITM then EXACT ← MEET-IN-THE-MIDDLE
    else return (UNKNOWN, A, B, d, —)
    P ← EXACT(a)                                    // a balanced partition, or NO
    if P = NO then return (NO, A, B, d, —)
    return (YES, A, B, d, P)
```

The symbol — stands for the absent balanced partition.

When only one exact algorithm is in range, it is run. When both are in range, the one with the smaller cost for the given $n$ and $T$ is run, and the two cost estimates of Section 4.7 are the starting point for this rule.

### 6.3 Examples

| input | Karmarkar-Karp | decision | result |
| --- | --- | --- | --- |
| $(3, 1, 4, 2)$ | $d = 0$ | step 1 | YES with the balanced partition $\lbrace 3, 2 \rbrace$, $\lbrace 1, 4 \rbrace$ |
| $(2, 3, 4)$ | $d = 1$ | $S = 9$ is odd | NO, with the partition of discrepancy $1$ |
| $(5, 6, 7, 8, 10)$ | $d = 2$ | $S = 36$, $T = 18$, both exact algorithms are in range and answer YES | YES with the balanced partition $\lbrace 5, 6, 7 \rbrace$, $\lbrace 8, 10 \rbrace$ found by the exact algorithm, next to the partition of discrepancy $2$ |

The third row shows what the exact algorithm adds. The approximation misses the balanced partition $\lbrace 5, 6, 7 \rbrace$, $\lbrace 8, 10 \rbrace$, the exact algorithm finds it, and the result contains both, so the comparison of Section 6.1 records that the approximation has missed the optimum. The answer UNKNOWN occurs only for inputs with an even sum, $d > 0$, $T > T_{DP}$ and $n > N_{MITM}$, for example for $1000$ numbers of $25$ digits.

### 6.4 Correctness

**Theorem 6.1.** Let (answer, $(A, B)$, $d$, $P$) be the result of `SOLVE`.

1. $(A, B)$ is a partition of $\lbrace 1, \dots, n \rbrace$ into two non-empty parts, and $d = |\Sigma(A) - \Sigma(B)|$.
2. If the answer is YES, $P$ is a balanced partition. If the answer is NO, no balanced partition exists. If the answer is NO or UNKNOWN, $P$ is absent.
3. If $d = 0$, the answer is YES and $P = (A, B)$.
4. The answer is UNKNOWN only if $\Sigma(M)$ is even, $d > 0$, $T > T_{DP}$ and $n > N_{MITM}$.

*Proof.* Part 1 is Theorem 5.2, and part 3 is the first test of the algorithm. For parts 2 and 4 consider the possible returns.

- *Return in the first test.* Since $d = 0$, $P = (A, B)$ is a balanced partition, so YES is correct.
- *Return in the second test.* If $S$ is odd, a balanced partition would give $S = 2\Sigma(A)$, which is impossible, so NO is correct (Section 1.2), and $P$ is absent.
- *The remaining case.* Now $S$ is even and $d > 0$. Then $S > 0$, since $S = 0$ gives $d = 0$ as above, so $T > 0$. If `DP-PARTITION` is run, then $T \le T_{DP}$, and by Theorem 4.2 it returns a balanced partition, which becomes $P$ with the answer YES, if one exists, and NO otherwise. The same holds for `MEET-IN-THE-MIDDLE` with $n \le N_{MITM}$ by Theorem 3.4. Otherwise $T > T_{DP}$ and $n > N_{MITM}$, the answer is UNKNOWN, as stated in part 4, and $P$ is absent. $\square$

If the answer is YES and $d > 0$, the partition $(A, B)$ of the approximation is not balanced, and $P$ is. By Proposition 5.3, the discrepancy of $(A, B)$ is at most $\max_i a_i$ in any case. If the answer is NO or UNKNOWN, the discrepancy $d$ is the quality of an approximation, and we do not claim that it is minimal.

The cost of `SOLVE` is the cost of Karmarkar-Karp (Theorem 5.4) plus the cost of the exact algorithm that is run, if any (Theorems 3.5 and 4.3). The algorithm does not contradict the NP-completeness of PARTITION (Section 1.3): it solves only the inputs within the ranges of the exact algorithms and returns UNKNOWN otherwise.

---

## 7. References

- M. Fischetti and S. Martello, "Worst-case analysis of the differencing method for the partition problem", *Mathematical Programming* 37(1), 117–120, 1987. doi:10.1007/BF02591687. Cited in Section 5.5 for the worst-case ratio $7/6$ of the differencing method.
- M. R. Garey and D. S. Johnson, *Computers and Intractability: A Guide to the Theory of NP-Completeness*, W. H. Freeman, 1979. ISBN 0-7167-1045-5. Cited in Sections 1.1 and 1.3 for pseudo-polynomial algorithms and strong NP-completeness (Section 4.2 of the book).
- R. L. Graham, "Bounds on multiprocessing timing anomalies", *SIAM Journal on Applied Mathematics* 17(2), 416–429, 1969. doi:10.1137/0117039. Cited in Section 5.7 for the ratio of the greedy method (longest processing time first).
- E. Horowitz and S. Sahni, "Computing partitions with applications to the knapsack problem", *Journal of the ACM* 21(2), 277–292, 1974. doi:10.1145/321812.321823. Cited in Sections 1.1 and 3 as the origin of meet in the middle.
- N. Karmarkar and R. M. Karp, "The differencing method of set partitioning", Technical Report UCB/CSD-83-113, Computer Science Division, University of California, Berkeley, 1983. https://www2.eecs.berkeley.edu/Pubs/TechRpts/1983/6353.html. Cited in Sections 1.1 and 5 as the origin of the differencing method.
- R. M. Karp, "Reducibility among combinatorial problems", in R. E. Miller and J. W. Thatcher (eds.), *Complexity of Computer Computations*, Plenum Press, 85–103, 1972. doi:10.1007/978-1-4684-2001-2_9. Cited in Sections 1.1 and 1.3 for the NP-completeness of PARTITION.
- R. E. Korf, "A complete anytime algorithm for number partitioning", *Artificial Intelligence* 106(2), 181–203, 1998. doi:10.1016/S0004-3702(98)00086-1. Cited in Section 5.7 for the exact search built on the differencing method.
- R. Schroeppel and A. Shamir, "A $T = O(2^{n/2})$, $S = O(2^{n/4})$ algorithm for certain NP-complete problems", *SIAM Journal on Computing* 10(3), 456–464, 1981. doi:10.1137/0210033. Cited in Section 3.6 for the reduction of the memory of meet in the middle.
- B. Yakir, "The differencing algorithm LDM for partitioning: a proof of a conjecture of Karmarkar and Karp", *Mathematics of Operations Research* 21(1), 85–99, 1996. doi:10.1287/moor.21.1.85. Cited in Sections 1.1 and 5.5 for the expected discrepancy $n^{-\Theta(\log n)}$ on i.i.d. uniform items.


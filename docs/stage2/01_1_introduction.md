## 1. Problem statement

### 1.1 Introduction

The terms used in this section are defined formally in Section 1.2.

Let $M = \{a_1, \dots, a_n\}$ be a finite multiset of non-negative integers, that is, a collection in which values may repeat. A *partition* of $M$ is a division of it into two non-empty parts $A$ and $B$. It is *balanced* if $\Sigma(A) = \Sigma(B)$, where $\Sigma$ denotes the sum of the elements of a part.

For example, $\{3, 1, 4, 2\}$ has the balanced partition $A = \{3, 2\}$, $B = \{1, 4\}$, with $\Sigma(A) = \Sigma(B) = 5$. The multiset $\{1, 1, 4\}$ has no balanced partition: it would require $\Sigma(A) = \Sigma(M)/2 = 3$, but no subset of $\{1, 1, 4\}$ has sum $3$.

We consider two problems. Section 1.2 gives their formal definitions.

- **The decision problem.** Given $M$, decide whether a balanced partition exists. The answer is YES or NO. The exact algorithms below solve this problem and return only the answer; they do not construct the partition.
- **The optimisation problem.** Given $M$, find a partition minimising the *discrepancy* $d(A, B) = |\Sigma(A) - \Sigma(B)|$. A partition is balanced if and only if $d(A, B) = 0$.

The decision problem is NP-complete [Karp 1972]. Hence no polynomial-time algorithm for it is known, and none exists unless P = NP. It is NP-complete only in the weak sense: it admits an algorithm whose running time is polynomial in $n$ and $\Sigma(M)$, a *pseudo-polynomial* algorithm [Garey and Johnson 1979]. Two parameters therefore govern the cost: the number of elements $n$ and the magnitude of $\Sigma(M)$. The algorithms below differ in which of them their cost depends on.

We present four algorithms.

- **Section 2, brute force.** It enumerates all $2^n$ subsets. Time $O(2^n)$, memory $O(n)$.
- **Section 3, meet in the middle.** It splits $M$ into two halves and combines their subset sums [Horowitz and Sahni 1974]. Time $O(n \cdot 2^{n/2})$, memory $O(2^{n/2})$, independent of the magnitude of the elements.
- **Section 4, dynamic programming.** It computes the set of reachable subset sums. Time $O(n \cdot \Sigma(M))$, which is pseudo-polynomial.
- **Section 5, Karmarkar-Karp differencing.** This is a polynomial-time approximation algorithm, with time $O(n \log n)$ [Karmarkar and Karp 1982]. It is the only algorithm below that constructs a partition. It always returns one, and its discrepancy is bounded (Section 5.4). We also quote its expected behaviour on random inputs [Yakir 1996].

None of the four algorithms is efficient for every input. Section 6 defines a final algorithm that selects, for each input, the algorithm that best matches it. For each algorithm we give pseudocode, a description, a correctness proof and a complexity analysis.

The remainder of this section is organised as follows: the constraints and assumptions on the input (below), the formal definitions (Section 1.2), the hardness results (Section 1.3) and the computational model (Section 1.4).

**Constraints on the input.** The input is a multiset $M = \{a_1, \dots, a_n\}$ of non-negative integers. Equal values are different elements and are distinguished by their position $1, \dots, n$. Both parts of a partition must be non-empty, because we do not assign a sum to the empty part, in particular we do not set it to $0$. Consequently $n \ge 2$, and a multiset with one element has no partition.

**Assumptions.** The analysis also rests on the following assumptions. An input that violates one of them is rejected with an error message and is never truncated.

| Assumption | Reason |
| --- | --- |
| $0 \le a_i < 10^{38}$ | The numbers of interest have about $25$ digits, which exceed $64$ bits, so we use unsigned 128-bit integers (Section 1.4). A wider integer type would lift the bound without changing the algorithms. We do not need it: dynamic programming is already infeasible for numbers of this size, and the cost of the other algorithms does not depend on the magnitude of the numbers. |
| $\Sigma(M) < 2^{128}$ | The same reason as for the bound above: the total is also an unsigned 128-bit integer. |
| Each exponential algorithm is applied only within a stated range of $n$ or of the target sum. | Outside its range it may not finish or fit in memory; Section 6 then uses the approximation. |


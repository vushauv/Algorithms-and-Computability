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

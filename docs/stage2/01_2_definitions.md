### 1.2 Definitions

**Multiset.** A multiset $M$ over $\mathbb{N} = \{0, 1, 2, \dots\}$ is a pair $(U, m)$ where $U$ is a finite set and $m: U \to \{1, 2, 3, \dots\}$ gives the multiplicity of each element. We write it as a list $M = \{a_1, a_2, \dots, a_n\}$ in which values may repeat, and $n = \sum_{u \in U} m(u)$ is its size.

**Sum.** $\Sigma(M) = \sum_{i=1}^{n} a_i$. For an index set $I \subseteq \{1,\dots,n\}$, $\Sigma(I) = \sum_{i \in I} a_i$.

**Partition into two subsets.** A pair $(A, B)$ of **non-empty** index sets with $A \cup B = \{1,\dots,n\}$ and $A \cap B = \emptyset$. The sum of the empty part is not defined, and we do not set it to $0$ (see *Constraints on the input* in Section 1.1). Consequently $n \ge 2$.

**Balanced partition.** A partition with $\Sigma(A) = \Sigma(B)$, equivalently $\Sigma(A) = \Sigma(M)/2$.

**Discrepancy.** $d(A,B) = |\Sigma(A) - \Sigma(B)|$. A partition is balanced iff $d = 0$. This is the quality measure for the approximation algorithm.

**The decision problem PARTITION.** *Instance:* a multiset $M$ of non-negative integers. *Question:* does a balanced partition exist?

**The optimisation problem.** Find a partition minimising $d(A,B)$. Its optimum is $0$ iff the decision instance is a yes-instance, so an exact optimiser also decides PARTITION.

**Target sum.** $T = \Sigma(M)/2$. If $\Sigma(M)$ is odd there is no balanced partition.

**Trivial cases.** Every algorithm below starts with two checks.

- **Odd total.** If $\Sigma(M)$ is odd, no balanced partition exists, and the answer is NO.
- **Zero total.** If $\Sigma(M) = 0$, every element is $0$, and the partition $A = \{1\}$, $B = \{2, \dots, n\}$ is balanced. Both parts are non-empty because $n \ge 2$, and the answer is YES with this partition.

After these checks every algorithm may assume $\Sigma(M) > 0$, hence $T > 0$. Then:

- **Non-empty parts.** Any index set $A$ with $\Sigma(A) = T$ is non-empty, because $T > 0$. Its complement $B$ satisfies $\Sigma(B) = \Sigma(M) - T = T > 0$, so it is non-empty as well.
- **Consequence for the proofs.** The requirement of non-empty parts never appears in the correctness proofs. If $\Sigma(M) > 0$, a pair of index sets with equal sums is already a balanced partition in the sense above, even if empty parts were allowed with sum $0$.

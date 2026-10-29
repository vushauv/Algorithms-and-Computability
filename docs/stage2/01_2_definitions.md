### 1.2 Definitions (report item a)

**Multiset.** A multiset $M$ over $\mathbb{N} = \{0, 1, 2, \dots\}$ is a pair $(U, m)$ where $U$ is a finite set and $m: U \to \{1, 2, 3, \dots\}$ gives the multiplicity of each element. We write it as a list $M = \{a_1, a_2, \dots, a_n\}$ in which values may repeat, and $n = \sum_{u \in U} m(u)$ is its size.

**Sum.** $\Sigma(M) = \sum_{i=1}^{n} a_i$. For an index set $I \subseteq \{1,\dots,n\}$, $\Sigma(I) = \sum_{i \in I} a_i$.

**Partition into two subsets.** A pair $(A, B)$ of **non-empty** index sets with $A \cup B = \{1,\dots,n\}$ and $A \cap B = \emptyset$. The assignment asks for a partition into *two* subsets, so we read a subset as a non-empty one; the consequences are discussed under *Constraints on the input* below.

**Balanced partition.** A partition with $\Sigma(A) = \Sigma(B)$, equivalently $\Sigma(A) = \Sigma(M)/2$.

**Discrepancy.** $d(A,B) = |\Sigma(A) - \Sigma(B)|$. A partition is balanced iff $d = 0$. This is the quality measure for the approximation algorithm.

**The decision problem PARTITION.** *Instance:* a multiset $M$ of non-negative integers. *Question:* does a balanced partition exist?

**The optimisation problem.** Find a partition minimising $d(A,B)$. Its optimum is $0$ iff the decision instance is a yes-instance, so an exact optimiser also decides PARTITION.

**Target sum.** $T = \Sigma(M)/2$. If $\Sigma(M)$ is odd there is no balanced partition. (Trivial necessary condition, checked first by every algorithm.)

**Trivial cases.** Every algorithm below starts with the same two checks. If $\Sigma(M)$ is odd, the answer is NO. If $\Sigma(M) = 0$, every element is $0$ and the partition $A = \{1\}$, $B = \{2, \dots, n\}$ is balanced, so the answer is YES with this partition; it has two non-empty sides because $n \ge 2$. After these checks the algorithms may assume $\Sigma(M) > 0$, hence $T > 0$. Then any index set $A$ with $\Sigma(A) = T$ is automatically non-empty, and so is its complement, because $\Sigma(B) = \Sigma(M) - T = T > 0$. This is why the requirement of non-empty sides never appears in the correctness proofs: once $\Sigma(M) > 0$, a balanced partition in the loose sense (sides allowed to be empty) is already a balanced partition in our sense.


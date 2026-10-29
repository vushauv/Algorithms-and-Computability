## 5. Algorithm D: Karmarkar-Karp differencing (polynomial-time approximation)

**Source.** Karmarkar and Karp 1982 (technical report, UC Berkeley, "The differencing method of set partitioning"). Worst-case quality: Fischetti and Martello 1987 (ratio $7/6$). Average-case quality: Yakir 1996 (expected discrepancy $n^{-\Theta(\log n)}$ for random items); overviews in Korf 1998 and Mertens 2006. The algorithm is the polynomial-time approximation required by the rule that every exponential exact algorithm has one next to it (task.md 4b, CONSTITUTION 6), here for Algorithms A and B.

**Role.** It always runs, in time $O(n \log n)$ and without any dependence on the size of the numbers. It answers YES whenever its discrepancy is $0$, and otherwise it provides the partition that the program reports when the exact methods are out of range (Section 6).

### 5.1 Idea

Repeatedly take the two largest remaining numbers $x \ge y$ and decide that they go to **opposite** sides. Whatever happens to the other numbers, the pair then contributes only $x - y$ to the difference of the two sides, so the pair is replaced by the single number $x - y$. Repeat until one number is left; it is the discrepancy of the final partition. To recover the two sets, remember each decision as an edge "$u$ and $v$ are on opposite sides" and 2-colour the resulting graph at the end.

Why it is better than the simple greedy method (put the next largest number on the lighter side): greedy commits an element to a side at once. Differencing postpones the decision and only fixes the *relation* between two elements, which lets large numbers cancel each other.

### 5.2 Pseudocode

```
KARMARKAR-KARP(a[1..n])
    H ← max-heap of pairs (value, id) built from (a[i], i)      // ties are broken in any way
    E ← empty list of edges                                      // edges of the graph G on the vertices 1..n
    while |H| > 1 do
        (x, u) ← EXTRACT-MAX(H)
        (y, v) ← EXTRACT-MAX(H)                                  // x ≥ y
        INSERT(H, (x − y, u))                                    // the difference keeps the id of the larger element u
        append (u, v) to E                                       // u and v are put on opposite sides
    (d, _) ← the single entry left in H                          // d = discrepancy of the result
    colour the vertices of G = ({1..n}, E) with 0 and 1 so that the ends of every edge differ      // BFS or DFS
    A ← { i : colour(i) = 0 };  B ← { i : colour(i) = 1 }
    return (A, B, d)
```

The input has $n \ge 2$ (Section 1.1), so the loop runs at least once and $E$ is non-empty. The subtraction $x - y$ is always done with $x \ge y$, so unsigned 128-bit numbers never overflow. The pseudocode keeps the id of the **larger** element on purpose: Section 5.3 shows that this is what makes the colouring step correct.

### 5.3 Correctness

Let $G$ be the graph on the vertices $1, \dots, n$ with the edges of $E$ added so far. For a connected component $C$ of $G$, its **colour classes** are the two sides of a 2-colouring of $C$. If $C$ is a tree, the 2-colouring is unique up to swapping the two colours.

**Lemma 5.1 (invariant).** After every round of the loop (and before the first one) the following holds.

1. $G$ is a forest, and every connected component of $G$ has exactly one entry $(z, u)$ in $H$, with $u$ a vertex of that component.
2. For the entry $(z, u)$ of a component $C$, the sums of the two colour classes of $C$ differ by exactly $z$, and $u$ lies in a class with the larger (or equal) sum.

_Proof._ Induction on the number of rounds. Before the first round, every vertex is a component of its own, with the entry $(a_i, i)$. Its colour classes are $\{i\}$ and $\emptyset$, which differ by $a_i$, and $i$ lies in the larger one.

Assume the invariant holds before a round. The round extracts $(x, u)$ and $(y, v)$, which belong to two **different** components $P$ and $Q$, because every component has only one entry. The new edge $\{u, v\}$ joins two different components, so $G$ stays a forest, the merged component $P \cup Q$ is a tree, and it gets the single entry $(x - y, u)$. This proves part 1.

For part 2, let $P$ have classes $H_P$ (the larger sum, containing $u$) and $L_P$, so $\Sigma(H_P) - \Sigma(L_P) = x$. Let $Q$ have the classes $H_Q \ni v$ and $L_Q$ with $\Sigma(H_Q) - \Sigma(L_Q) = y$. The edge $\{u, v\}$ forces $u$ and $v$ to opposite colours, so the colour classes of the merged tree are $H_P \cup L_Q$ and $L_P \cup H_Q$. Their sums differ by
$$(\Sigma(H_P) + \Sigma(L_Q)) - (\Sigma(L_P) + \Sigma(H_Q)) = x - y \ge 0,$$
and the class $H_P \cup L_Q$ is the larger one and contains $u$. $\square$

**Theorem 5.1.** `KARMARKAR-KARP` terminates after $n - 1$ rounds, $G$ is a tree on all $n$ vertices, both $A$ and $B$ are non-empty, and the returned $d$ is the discrepancy of the returned partition: $d = |\Sigma(A) - \Sigma(B)|$.

_Proof._ Each round removes two entries and inserts one, so $|H|$ drops by $1$ and the loop ends after $n - 1$ rounds, with exactly one entry left. By Lemma 5.1 there is one component, so $G$ is a tree with $n - 1$ edges. A tree is bipartite, so the colouring exists, and $(A, B)$ are its two classes. By part 2 of the lemma, the single entry $d$ is the difference of their sums. Since $n \ge 2$, the tree has at least one edge, and the ends of an edge get different colours, so both colour classes $A$ and $B$ are non-empty. $\square$

**What it decides.** If $d = 0$, then $(A, B)$ is a balanced partition, so the answer YES is correct and the sets are a certificate. If $d > 0$, nothing follows: the algorithm is **not exact**, as the example in Section 5.5 shows. Because $d = |S - 2\Sigma(B)|$, the discrepancy always has the same parity as $S$.

**Duplicates and zeros.** Equal values need no special treatment. Replacing the two largest entries by their difference changes the multiset of values in the same way whichever of two equal values is extracted first, so $d$ does not depend on how ties are broken (only the ids, and hence possibly the returned sets, do). An element equal to $0$ is a normal entry.

### 5.4 Quality of the result

The quality measure is the discrepancy $d$. Since the optimum can be $0$, a ratio on $d$ is meaningless, so we state the following.

**Proposition 5.2 (bound on $d$).** Let $M = \max a_i$. Then $d \le M$.

_Proof._ Every entry of $H$ is at most $M$ at all times: this holds at the start, and the new entry $x - y \le x \le M$. The last entry is $d$. $\square$

This is the only guarantee on $d$ itself, and it is weak: the scaled instances $k \cdot (5, 6, 7, 8, 10)$ of Section 5.5 have optimum $0$ and give $d = 2k$, so no bound of the form "optimum plus a constant" exists.

**Ratio on the larger side.** If we look at the larger of the two sums, $\max(\Sigma(A), \Sigma(B)) = (S + d)/2$, there is a proper approximation ratio. The optimum is at least $\max(S/2, M)$, so Proposition 5.2 gives
$$\frac{(S + d)/2}{\mathrm{OPT}} \le \frac{S/2 + M/2}{\mathrm{OPT}} \le \frac{\mathrm{OPT} + \mathrm{OPT}/2}{\mathrm{OPT}} = \frac{3}{2}.$$
Fischetti and Martello [1987] show that the worst-case ratio of the differencing method is exactly $7/6$.

**Random inputs.** For items drawn independently from a uniform distribution, the expected discrepancy of the differencing method is $n^{-\Theta(\log n)}$ [Yakir 1996], which proves a conjecture of Karmarkar and Karp. We quote this result and do not reprove it. It says that on random inputs with many elements the discrepancy becomes extremely small, far below that of simple greedy methods. For our integer inputs this is only an indication, and we measure it in Stage 4.

### 5.5 Example

Let $a = (3, 1, 4, 2)$, the set of Sections 2.4 and 4.4. Ties are broken in favour of the smaller index.

| round | extracted $(x, u)$, $(y, v)$ | inserted $(x - y, u)$ | edge | heap after the round |
| ----- | ---------------------------- | --------------------- | ---- | -------------------- |
| 0     |                              |                       |      | $4_{a_3}, 3_{a_1}, 2_{a_4}, 1_{a_2}$ |
| 1     | $(4, a_3), (3, a_1)$         | $(1, a_3)$            | $a_3 - a_1$ | $2_{a_4}, 1_{a_2}, 1_{a_3}$ |
| 2     | $(2, a_4), (1, a_2)$         | $(1, a_4)$            | $a_4 - a_2$ | $1_{a_3}, 1_{a_4}$          |
| 3     | $(1, a_3), (1, a_4)$         | $(0, a_3)$            | $a_3 - a_4$ | $0_{a_3}$                   |

Here $x_{u}$ means the value $x$ with the id of $u$. The final entry is $d = 0$. The graph has the edges $a_3 a_1$, $a_4 a_2$, $a_3 a_4$, a path $a_1 - a_3 - a_4 - a_2$. Colouring it from $a_1 = 0$ gives $a_1 = 0$, $a_3 = 1$, $a_4 = 0$, $a_2 = 1$. So $A = \{a_1, a_4\} = \{3, 2\}$ and $B = \{a_2, a_3\} = \{1, 4\}$, both with sum $5$, and the answer YES is certified.

**An instance where it fails to find the optimum.** Let $a = (5, 6, 7, 8, 10)$, with $S = 36$ and $T = 18$. The rounds are $10 - 8 = 2$ (edge $a_5 a_4$), $7 - 6 = 1$ (edge $a_3 a_2$), $5 - 2 = 3$ (edge $a_1 a_5$) and $3 - 1 = 2$ (edge $a_1 a_3$), so $d = 2$. The colouring from $a_1 = 0$ gives $A = \{a_1, a_2, a_4\} = \{5, 6, 8\}$ with sum $19$ and $B = \{a_3, a_5\} = \{7, 10\}$ with sum $17$. But $\{8, 10\}$ and $\{5, 6, 7\}$ both have sum $18$, so the optimum is $0$. The same input shows that $d > 0$ does not prove that the answer is NO.

### 5.6 Complexity

We count operations on 128-bit numbers in the unit-cost model of Section 1.4.

- Building the heap with `heapify` costs $O(n)$.
- The loop has $n - 1$ rounds. Each round has two extractions and one insertion, $O(\log n)$ each, so $O(n \log n)$ in total.
- Colouring the tree with BFS or DFS over an adjacency list costs $O(n)$, because $G$ has $n$ vertices and $n - 1$ edges.
- **Total time: $O(n \log n)$. Memory: $O(n)$** for the heap, the edge list and the adjacency list.
- The running time does not depend on the size of the numbers, only on $n$.

### 5.7 Constraints and limitations

**Constraints that the program enforces**:

1. **Preconditions.** Each $0 \le a_i < 10^{38}$. The sum $S$ is not needed by the algorithm, and every difference $x - y$ is at most $\max a_i$, so no overflow can occur.
2. **Bound on $n$.** $1 \le n \le 10^6$. For $n = 10^6$ the heap, the edges and the adjacency list take well under $100$ MB, and about $3 \cdot 10^6$ heap operations of $20$ steps each are expected to run in under a second (to be measured in Stage 4). The algorithm is never skipped.

**Limitations.**

- It is a heuristic. The only guarantees are $d \le \max a_i$ and the ratio $7/6$ on the larger side; there is no bound on how far $d$ is from the optimal discrepancy.
- It cannot answer NO. A result $d > 0$ can occur on a YES instance (Section 5.5), so the program may say "NO" only when an exact algorithm or the parity test has said so (Section 6).

### 5.8 When to use it

Karmarkar-Karp is the only algorithm that works for every input: it needs $O(n \log n)$ time and $O(n)$ memory for any $n \le 10^6$ and any numbers below $10^{38}$. It is therefore the fallback when dynamic programming (Section 4) and meet in the middle (Section 3) are out of range, for example for $n = 1000$ elements with 25 digits. It is also cheap enough to run **first** on every input, because $d = 0$ already proves YES (Section 6).

**Optional: a simpler baseline.** The greedy method sorts the numbers in descending order and puts each into the side with the smaller sum so far. It also takes $O(n \log n)$ time and satisfies $d \le \max a_i$; as a scheduling algorithm (LPT) its ratio on the larger side is $7/6$ for two machines [Graham 1969]. We include it in the Stage 4 experiments to compare discrepancies, since we expect Karmarkar-Karp to be much better.

**Optional: complete Karmarkar-Karp.** Korf 1998 turns the method into an exact search: at each step it branches on "opposite sides" ($x - y$) and "same side" ($x + y$), trying Karmarkar-Karp's choice first. The worst case is exponential, so it would only be a third exact method. We mention it and do not implement it unless there is time.

---


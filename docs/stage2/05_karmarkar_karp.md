## 5. Karmarkar-Karp differencing

By Section 1.3 no polynomial-time algorithm for PARTITION is known, and Sections 2 to 4 are exponential in $n$ or in the magnitude of the numbers. This section gives a polynomial-time approximation of the optimisation problem of Section 1.1, which minimises the discrepancy $d$. The differencing method of Karmarkar and Karp [1982] runs in time $O(n \log n)$ whatever the numbers are, and it is the only algorithm of this report that constructs a partition. It cannot decide PARTITION, since it may return $d > 0$ for an instance that has a balanced partition (Section 5.3), but $d = 0$ proves that the answer is YES.

### 5.1 Idea

Take the two largest remaining numbers $x \ge y$ and decide that they go to *opposite* parts. Whatever happens to the other numbers, the pair then contributes only $x - y$ to the difference of the two parts, so the pair is replaced by the single number $x - y$. Repeating this until one number is left, that number is the discrepancy of the final partition. To recover the parts, every decision is recorded as an edge "$u$ and $v$ are in opposite parts", and the resulting graph is two-coloured at the end.

The simple greedy method, which puts the next largest number into the lighter part, fixes the part of an element at once. Differencing postpones this decision and fixes only the relation between two elements, which lets large numbers cancel each other.

### 5.2 Algorithm

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
    colour the vertices of G = ({1..n}, E) with 0 and 1 so that the ends of every edge differ
    A ← { i : colour(i) = 0 };  B ← { i : colour(i) = 1 }
    return (A, B, d)
```

Ties in the heap are broken in any way. The subtraction $x - y$ is done with $x \ge y$, so it never gives a negative number. The loop runs at least once because $n \ge 2$. The new entry keeps the index of the *larger* element $u$ on purpose: this is what makes the colouring step correct (Lemma 5.1).

### 5.3 Examples

**The method finds the optimum.** Let $a = (3, 1, 4, 2)$, and break ties in favour of the smaller index.

| round | extracted $(x, u)$, $(y, v)$ | inserted | edge | heap after the round |
| ----- | ---------------------------- | -------- | ---- | -------------------- |
| 0     |                              |          |      | $4_{3}, 3_{1}, 2_{4}, 1_{2}$ |
| 1     | $(4, 3), (3, 1)$             | $(1, 3)$ | $\{3, 1\}$ | $2_{4}, 1_{2}, 1_{3}$ |
| 2     | $(2, 4), (1, 2)$             | $(1, 4)$ | $\{4, 2\}$ | $1_{3}, 1_{4}$        |
| 3     | $(1, 3), (1, 4)$             | $(0, 3)$ | $\{3, 4\}$ | $0_{3}$               |

Here $x_u$ is the value $x$ with the index $u$. The final entry is $d = 0$. The edges form the path $1 - 3 - 4 - 2$, which is coloured $1 \mapsto 0$, $3 \mapsto 1$, $4 \mapsto 0$, $2 \mapsto 1$. So $A = \{1, 4\}$ with values $3, 2$ and $B = \{2, 3\}$ with values $1, 4$ have both sum $5$.

**The method misses the optimum.** Let $a = (5, 6, 7, 8, 10)$, so $S = 36$ and $T = 18$. The rounds replace $10, 8$ by $2$ (edge $\{5, 4\}$), $7, 6$ by $1$ (edge $\{3, 2\}$), $5, 2$ by $3$ (edge $\{1, 5\}$) and $3, 1$ by $2$ (edge $\{1, 3\}$). Hence $d = 2$, with the parts $A = \{1, 2, 4\}$ of sum $5 + 6 + 8 = 19$ and $B = \{3, 5\}$ of sum $7 + 10 = 17$. However, $\{8, 10\}$ and $\{5, 6, 7\}$ both have sum $18$, so the optimum is $0$. Thus $d > 0$ does not show that the answer is NO.

### 5.4 Correctness

Let $G$ be the graph on the vertices $1, \dots, n$ whose edges are those of $E$ added so far. The *classes* of a tree are the two sides of its two-colouring, which is unique up to swapping the colours.

**Lemma 5.1** (invariant). Before the first round and after every round:

1. $G$ is a forest, and every connected component $C$ of $G$ has exactly one entry $(z, u)$ in $H$, where $u$ is a vertex of $C$;
2. the sums of the two classes of $C$ differ by exactly $z$, and $u$ lies in a class with the larger or equal sum.

*Proof.* Induction on the number of rounds. At the start every vertex is a component with the entry $(a_i, i)$, whose classes $\{i\}$ and $\emptyset$ differ by $a_i$, and $i$ lies in the larger one.

Assume the claim before a round. The round extracts $(x, u)$ and $(y, v)$, which belong to two different components $P$ and $Q$, since each component has one entry. The new edge $\{u, v\}$ joins two components, so $G$ remains a forest, and the merged tree has the single entry $(x - y, u)$. This proves part 1.

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

**Proposition 5.3.** Let $m = \max_i a_i$. Then $d \le m$.

*Proof.* Every entry of $H$ is at most $m$ at the start, and a new entry $x - y \le x \le m$ is again at most $m$. The last entry is $d$. $\square$

This bound is weak: the instances $k \cdot (5, 6, 7, 8, 10)$ have the optimum $0$ and give $d = 2k$, so no bound of the form "optimum plus a constant" exists.

**Ratio on the larger part.** The larger part has the sum $(S + d)/2$. Its optimum $\mathrm{OPT}$ is at least $\max(S/2, m)$, so by Proposition 5.3
$$
\frac{(S + d)/2}{\mathrm{OPT}} \le \frac{S/2 + m/2}{\mathrm{OPT}} \le \frac{\mathrm{OPT} + \mathrm{OPT}/2}{\mathrm{OPT}} = \frac{3}{2} .
$$
Fischetti and Martello [1987] show that the worst-case ratio of the differencing method is exactly $7/6$.

**Random inputs.** For items drawn independently from a uniform distribution, the expected discrepancy of the differencing method is $n^{-\Theta(\log n)}$ [Yakir 1996], which proves a conjecture of Karmarkar and Karp. We quote this result without proof. It says that for many random items the discrepancy becomes extremely small, far below that of simple greedy methods. For integers it is only an indication.

### 5.6 Complexity

**Theorem 5.4.** `KARMARKAR-KARP` takes time $O(n \log n)$ and memory $O(n)$, and the time does not depend on the magnitude of the numbers.

*Proof.* Building the heap takes $O(n)$. The loop has $n - 1$ rounds, each with two extractions and one insertion of $O(\log n)$ time, which gives $O(n \log n)$. The graph $G$ has $n$ vertices and $n - 1$ edges, so the colouring by a breadth-first or depth-first search takes $O(n)$. The heap, the edges and the adjacency lists use $O(n)$ memory. $\square$

### 5.7 Range of application and remarks

The algorithm needs no bound on $n$ beyond $n \ge 2$ and no bound on the numbers beyond the assumptions of Section 1.1; all differences are at most $\max_i a_i$, so the sum $S$ is not needed. It works for every input, in particular when neither dynamic programming nor meet in the middle is in range, for example for $1000$ elements of $25$ digits. A result with $d = 0$ proves YES, but $d > 0$ gives no answer, so a NO can come only from the parity test or from an exact algorithm.

**Remarks.** The greedy method, which processes the numbers in descending order and puts each into the part with the smaller sum so far, also takes time $O(n \log n)$ and satisfies $d \le \max_i a_i$. As a scheduling algorithm (longest processing time first) its ratio on the larger part is $7/6$ for two machines [Graham 1969]. Korf [1998] turns the differencing method into an exact search that at each step also tries to put the two largest numbers into the *same* part, replacing them by $x + y$. Its worst case is exponential, and we do not use it.

---

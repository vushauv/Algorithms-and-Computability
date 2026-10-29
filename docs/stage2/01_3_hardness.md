### 1.3 Hardness (report item a, cited)

- PARTITION is **NP-complete**. It is one of Karp's 21 problems [Karp 1972]; the reduction is from SUBSET SUM (or 3-dimensional matching in Karp's original). Membership in NP: a partition is a certificate checkable in $O(n)$ additions.
- PARTITION is **weakly** NP-hard, not strongly: it has a pseudo-polynomial algorithm (Section 4) whose running time is polynomial in $n$ and in the *magnitude* $\Sigma(M)$, but exponential in the *number of digits* of the input [Garey and Johnson 1979, Section 4.2].
- Consequence for us: with 25-digit numbers (as in `AaC_example_2.txt`) the pseudo-polynomial algorithm is infeasible, and the only exact approach that scales is one whose cost depends on $n$ alone (Section 3). This is the central argument of the whole document and should be stated early.


### 1.3 Hardness

**Theorem 1.1** (Karp [1972]). PARTITION is NP-complete.

PARTITION belongs to NP: a balanced partition is a certificate, and checking it takes $O(n)$ additions. NP-hardness was shown by Karp, who lists PARTITION among his 21 NP-complete problems. We quote this result and do not reprove it.

**Weak hardness.** PARTITION is NP-complete only in the weak sense. It is solved in time $O(n \cdot \Sigma(M))$ by dynamic programming (Section 4). This bound is polynomial in $n$ and in the *magnitude* $\Sigma(M)$, but exponential in the *length* of the input, which is proportional to $n$ times the number of digits of the elements. Such an algorithm is called pseudo-polynomial. Unless P = NP, a problem with a pseudo-polynomial algorithm is not NP-complete in the strong sense [Garey and Johnson 1979, Section 4.2].

**Consequences for the algorithms.**

- **Large numbers.** For numbers of about $25$ digits, $\Sigma(M)$ is of order $10^{25}$ and the cost $O(n \cdot \Sigma(M))$ is out of reach. An exact algorithm must then have a cost that depends on $n$ alone. The best ones are exponential in $n$ (Sections 2 and 3).
- **Approximation.** Since PARTITION is NP-complete, no polynomial-time algorithm for it is known, and none exists unless P = NP.

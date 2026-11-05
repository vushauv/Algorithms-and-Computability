# Stage 2: Documentation of Algorithms (basic version)

## Abstract

This report documents the algorithms for the partition problem: given a multiset $M$ of non-negative integers, decide whether it can be split into two non-empty parts with equal sums, and find a partition whose two sums are as close as possible. The decision problem is NP-complete, but only in the weak sense, so its cost is governed by two parameters, the number $n$ of elements and the magnitude of their sum $\Sigma(M)$. We describe four algorithms, each with pseudocode, a correctness proof and a complexity analysis: brute force with a Gray-code enumeration, in time $\Theta(2^n)$; meet in the middle, in time $O(n \cdot 2^{n/2})$ and memory $O(2^{n/2})$; dynamic programming over the subset sums, in pseudo-polynomial time $O(n \cdot \Sigma(M))$; and the Karmarkar-Karp differencing method, a polynomial-time approximation in time $O(n \log n)$ that constructs a partition. The final algorithm always runs the approximation and, whenever the input allows it, one exact algorithm chosen by the size and the magnitude of the input, so that the decision is exact and the quality of the approximation can be compared with it.


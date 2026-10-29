## 7. Summary table (goes near the end of the report)

| Algorithm | Exact? | Time | Memory | Depends on magnitude? | Practical range |
|---|---|---|---|---|---|
| A. Brute force | yes | $O(n \cdot 2^n)$, $O(2^n)$ with Gray code | $O(n)$ | no | $n \le 30$ |
| B. Meet in the middle | yes | $O(n \cdot 2^{n/2})$ | $O(2^{n/2})$ | no | $n \le 44$ |
| C. Dynamic programming | yes | $O(n \cdot \Sigma)$ | $O(\Sigma)$ bits | **yes** | $\Sigma/2 \le 10^{10}$ |
| D. Karmarkar-Karp | no | $O(n \log n)$ | $O(n)$ | no | $n \le 10^6$ |
| E. Final (selection) | yes within limits, else approx. | max of the branch taken | max of the branch taken | via C only | any |

---


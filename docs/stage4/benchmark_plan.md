# Stage 4: benchmark plan

What we will measure for Stage 4 (due Dec 4, 2026), how, and which graphs go
into the report. The ranges below were checked with one
trial run of every experiment on the current program (section 6).

Stage 4 asks for "results of computational tests, including performance
characteristics (computation time as a function of problem size)" (`task.md`
section 4, item c). The same measurements also fix the parameters that the
Stage 2 report left open: $N_{MITM}$, $T_{DP}$, $W_{DP}$ and the rule that
chooses between dynamic programming and meet in the middle (`task.md`
section 6, all provisional).

## 1. Questions

| | Question | Report |
| --- | --- | --- |
| Q1 | Do the running times follow the bounds of the report? | Theorems 2.5, 3.5, 4.3, 5.4 |
| Q2 | Does the memory follow the estimates: $32 \cdot 2^{n/2}$ bytes for meet in the middle, $4T$ bytes for dynamic programming? | Sections 3.6, 4.7 |
| Q3 | Up to which $n$ and $T$ are the exact algorithms practical? This fixes $N_{MITM}$, $T_{DP}$, $W_{DP}$. | Section 1.4 |
| Q4 | When both exact algorithms are in range, which is faster? Is the rule $T/64 < 2^{n/2}$ right? | Proposition 4.4 |
| Q5 | How good is Karmarkar-Karp: how $d$ falls with $n$, how often it misses a balanced partition | Section 5.5 |
| Q6 | What does the number type cost: 64-bit, 128-bit, numbers of any size (only if the big numbers are implemented) | Section 1.4 |

## 2. Test inputs

Every input is one set in one file, made by a seeded generator, so that every
point can be reproduced. Four families:

| Family | Definition | Why |
| --- | --- | --- |
| `trap` | $n - 1$ values $4 r_i$ and one value $4r + 2$, with random $r_i$, $r$ | $S \equiv 2 \pmod 4$, so $T$ is odd while every subset sum is even: the answer is NO with an even sum. It is the worst case of all three exact algorithms: brute force visits all $2^{n-1}$ masks, meet in the middle sweeps both lists to the end, dynamic programming runs all $n$ steps. Karmarkar-Karp gives an even $d \ge 2$. |
| `random` | $n$ values with $D$ digits, uniform ($D$ = 3, 10, 25, 38) | the typical case; quality of Karmarkar-Karp |
| `yes` | random values, balanced by construction (as `large_yes_instance` in `tests/random_sets.hpp`) | the early exit of the exact algorithms; how often Karmarkar-Karp misses |
| examples | `examples/*.txt`, `raw_regulations/*.txt` | sanity check of `SOLVE` on the files of the demo |

The magnitude of the values does not change the time of brute force, meet in
the middle or Karmarkar-Karp (Section 1.4 of the report), so their series use
25-digit values, as in the assignment's example. Dynamic programming depends on
$T$, so its values are chosen to give the wanted $T$.

## 3. Experiments

| | Algorithm | Family | Varied | Measured | Expected (Q) |
| --- | --- | --- | --- | --- | --- |
| E1 | brute force (`bf`) | `trap`, 25 digits; also `yes` | $n$ = 16, 18, ..., 34 | time | doubles per element, $\Theta(2^n)$ (Q1) |
| E2 | meet in the middle (`mitm`, `--n-mitm 56`) | `trap`, 25 digits; also `yes` | $n$ = 20, 22, ..., 54, until memory runs out | time, peak memory | time and memory double every 2 elements; memory $\approx 32 \cdot 2^{n/2}$ bytes (Q1, Q2, Q3) |
| E3a | dynamic programming (`dp`, `--t-dp` raised) | `trap` | $T$ = $10^5$ to $10^9$ (×√10 steps), $n = 100$ | time, peak memory | time $\propto T$, memory $\approx 4T$ bytes (Q1, Q2, Q3) |
| E3b | dynamic programming | `trap` | $n$ = $10^2$ to $10^5$, $T = 10^7$ | time | time $\propto n$; with E3a, time $\propto n \cdot T$, which fixes $W_{DP}$ (Q1, Q3) |
| E4 | Karmarkar-Karp (`kk`) | `random`, 25 digits | $n$ = $10^3$ to $10^6$ (×√10) | time, peak memory | $O(n \log n)$ (Q1) |
| E5 | `dp` and `mitm` on the same sets | `trap` | grid $n$ = 24, 28, ..., 44 × $T$ = $10^5$ to $10^9$ | time and memory of both | the faster one changes at $T/64 = 2^{n/2}$; memory favours meet in the middle much earlier ($4T$ against $32 \cdot 2^{n/2}$) (Q4) |
| E6 | Karmarkar-Karp | `random` with $D$ = 3, 10, 25; `yes` with 3 digits | $n$ = 10 to $10^5$; 20 seeds per point | $d$; share of `yes` sets with $d > 0$ | $d$ falls very fast with $n$ [Yakir 1996]; misses only for small $n$ (Q5) |
| E7 | `SOLVE` (default) | examples; `random` 25 digits | $n$ = 10 to 1000 | time, which method decided, share of UNKNOWN | UNKNOWN once $n > N_{MITM}$ and $T > T_{DP}$ |
| E8 | `kk` and `mitm` | `random` with 18, 25, 38, 50, 100 digits | the number of digits | time per set | a step where the values stop fitting 64 and 128 bits (Q6); only with the big numbers |

Each series stops at the first point that takes more than 60 s or needs more
memory than half of the machine's.

## 4. How we measure

- **Timer.** The program has no timer yet (`task.md`: timing code is Stage 4).
  Timing it from outside (`/usr/bin/time`) also counts reading the file,
  writing the results file and, for an exact algorithm with NO, the
  Karmarkar-Karp partition for the file; and its resolution is 10 ms, so most
  small points read 0.00 s (section 6). Stage 4 therefore adds an option
  `--time` that prints the time of the algorithm call alone, measured with
  `std::chrono::steady_clock`, for every set.
- **Repetitions.** Every point is run 5 times (the same file for `trap`,
  5 seeds for the random families); we report the median and show the
  minimum and maximum. The first run after a build is discarded: the trial's
  first brute-force run took 0.40 s for a set that takes under 0.01 s.
- **Memory.** Peak resident memory of the process, from `/usr/bin/time -l` on
  macOS (bytes) or `/usr/bin/time -v` on Linux (kilobytes).
- **Build and machine.** The `make` build (`-O2`), the same binary for all
  points, one machine for all final numbers, nothing else running. The report
  states the CPU, memory, system and compiler. A few points are repeated on a
  lab machine with `dist/partition.exe` to show that the demo executable
  behaves the same.
- **Data.** One CSV line per run: experiment, algorithm, family, $n$, $T$,
  digits, seed, repetition, time (s), peak memory (MB), answer, $d$. Data in
  `docs/stage4/data/`, graphs in `docs/stage4/figures/`.

## 5. Graphs for the report

| | Graph | From |
| --- | --- | --- |
| G1 | time against $n$, logarithmic time axis, brute force and meet in the middle, with the fitted slopes next to $2^n$ and $n \cdot 2^{n/2}$ | E1, E2 |
| G2 | peak memory of meet in the middle against $n$, with the line $32 \cdot 2^{n/2}$ | E2 |
| G3 | time of dynamic programming against $n \cdot T$ (both series) | E3 |
| G4 | peak memory of dynamic programming against $T$, with $4T$ | E3a |
| G5 | time of Karmarkar-Karp against $n$, with $n \log n$ | E4 |
| G6 | map over $(n, T)$: which exact algorithm is faster, with the curve $T/64 = 2^{n/2}$ | E5 |
| G7 | $d$ of Karmarkar-Karp against $n$, logarithmic axes, one line per number of digits | E6 |
| G8 | share of balanced sets that Karmarkar-Karp misses, against $n$ | E6 |
| G9 | time against the number of digits | E8, if done |

## 6. Trial run (Nov 14, 2026)

One run per point, external timing, Apple M2 Pro (12 cores), 16 GB, macOS 27,
Apple clang 21, `make` build of branch `feature/homenda-output`; MB means
$2^{20}$ bytes. Its only purpose is to choose the ranges above; it is not the
data of the report.

| Experiment | Point | Time | Peak memory |
| --- | --- | --- | --- |
| E1 brute force, `trap` | $n$ = 24 / 26 / 28 / 30 | 0.01 / 0.04 / 0.15 / 0.63 s | 1.8 MB |
| E2 meet in the middle, `trap` | $n$ = 36 / 40 / 44 | 0.03 / 0.12 / 0.55 s | 10 / 34 / 130 MB |
| | $n$ = 46 / 48 | 1.19 / 2.41 s | 258 / 514 MB |
| E3a dynamic programming, `trap`, $n = 100$ | $T \approx 10^6$ / $10^7$ / $10^8$ / $3 \cdot 10^8$ | 0.00 / 0.02 / 0.25 / 0.86 s | 6 / 39 / 375 / 1186 MB |
| E3b dynamic programming, `trap`, $T \approx 10^7$ | $n$ = 100 / 1000 / 10 000 | 0.02 / 0.10 / 0.94 s | 39 / 42 / 42 MB |
| E4 Karmarkar-Karp, 25 digits | $n$ = $10^4$ / $10^5$ / $10^6$ | 0.00 / 0.09 / 1.23 s (with reading and writing) | 5 / 36 / 305 MB |
| E5 $n = 40$, $T \approx 10^8$ | `dp` / `mitm` | 0.15 / 0.12 s | 410 / 34 MB |
| E6 $d$ of Karmarkar-Karp, 25 digits, 5 seeds | $n$ = 10 / 100 / 1000 / 10 000 | median $d \approx 10^{23}$ / $10^{18}$ / $3 \cdot 10^{10}$ / 1 | |
| E6 `yes`, 3 digits, 40 sets | $n$ = 20 / 50 / 100 | Karmarkar-Karp misses 14 / 0 / 0 of 40 | |

What the trial shows:

- **The bounds hold.** Brute force grows about 4 times per 2 elements.
  Meet in the middle doubles in time and memory per 2 elements. Its memory
  is the estimate $32 \cdot 2^{n/2}$ almost exactly: 128 MB predicted, 130 MB
  measured at $n = 44$; 512 MB predicted, 514 MB at $n = 48$. Dynamic
  programming has memory $4T + T/8$ bytes (373 MB predicted, 375 MB measured
  at $T \approx 0.95 \cdot 10^8$; 1196 MB predicted, 1186 MB measured at
  $T \approx 3.04 \cdot 10^8$) and time proportional to $n$ at fixed $T$.
- **The ranges are right.**
  - Brute force reaches about 10 s near $n = 34$.
  - Meet in the middle reaches about 10 s and 2 GB near $n = 52$, so E2 can
    go up to $n \approx 54$ on 16 GB.
  - For dynamic programming, $n \cdot T = 10^{11}$ takes about 1 s, so the
    default $W_{DP} = 10^{12}$ is about 10 s.
  - $T_{DP} = 3 \cdot 10^8$ is 1.2 GB.
- **Proposition 4.4 agrees.** At $n = 32$, $36$ and $40$, every point where
  the two times differ measurably lies on the side that $T/64 < 2^{n/2}$
  predicts. At $n = 40$, $T \approx 10^8$, close to the predicted switch at
  $T \approx 6.7 \cdot 10^7$, the two times are within 25 %. Meet in the middle needs a twelfth of the memory there;
  E5 decides whether the rule should also look at memory.
- **Karmarkar-Karp is very good on random data.** Its $d$ falls from
  $10^{23}$ to about 1 as $n$ grows from 10 to 10 000. On balanced sets with
  small values it misses about a third at $n = 20$ and none from $n = 50$.
- **The external timer is too coarse** for most points. Hence `--time`
  (section 4).

## 7. What has to be built (Stage 4)

1. `--time`: the time of each algorithm call, printed per set, without I/O.
2. A generator of the four families with a seed (one set per file).
3. A runner that loops over the points of an experiment, repeats them and
   writes the CSV.
4. A script that draws G1 to G9 from the CSV.

## 8. Open decisions

- **Language of the scripts (2 to 4).** CONSTITUTION point 1 says all code is
  C++20. Generating inputs and drawing graphs is much shorter in Python
  (matplotlib); the program itself stays C++. Can we do it with Python?
- **The machine** for the final numbers, and whether a few points are also
  measured on a lab machine.
- **Big numbers** (E8): only if the numbers of any size are implemented.
- **Final $N_{MITM}$, $T_{DP}$, $W_{DP}$ and the rule** of `SOLVE`, after E2,
  E3 and E5; then `task.md` section 6 and the report (§6.2) are updated.


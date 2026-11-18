# Algorithms and Computability: Laboratory Assignment (fall 2026)

Sources: `raw_regulations/` (course website, emails of 16.10.26 and 23.10.26, input examples).

> **Current stage: Stage 3 (implementation and sample inputs), due Nov 20, 2026, with a live demo on the faculty computers.** Stage 2 (the report) is finished; its sources are in `docs/stage2/`.

## 1. The problem

Teams of up to 4 students. The course offers two versions; **we chose the basic version** (decision of Oct 28, 2026). The advanced version is listed only for the record and is not mentioned in our documents.

| Version | Task | Max grade |
|---|---|---|
| **a) Basic (chosen)** | Partition a set of numbers into **2 subsets with equal sums** | up to 3.5, can be improved to 4.0 |
| **b) Advanced** | Partition a set of numbers into **k subsets** whose sums are as close to each other as possible | full scale, up to 5.0 |

### Input format (same for all teams)

- Text file describing one or more sets.
- Line 1: the number of sets in the file.
- Each following line: the numbers of one set, separated by a single space.
- A set may contain several equal elements (duplicates), so each line is really a multiset. The algorithms and their analysis must handle repeated values.
- Examples: `raw_regulations/AaC_example_1.txt` (2 sets, small numbers) and `raw_regulations/AaC_example_2.txt` (1 set, very large numbers, up to about 25 digits, so plan for big integers).

## 2. Stages and grading

| Stage | Content | Due | Weight |
|---|---|---|---|
| 1 | Teams organised and a conceptual solution (an idea we can explain in words). No documents or code due. | Oct 8 (done) | none |
| 2 | Documentation of algorithms: pseudocode, exhaustive description, complexity analysis | Oct 30, 2026 (report finished) | 33% |
| **3 (current)** | **Implementation of the algorithms and sample input preparation. Live demo of working code on faculty computers without compilers installed.** | **Nov 20, 2026** | **33%** |
| 4 | Empirical testing and a report on the tests. Deliver everything: documentation, program, sources, examples. | Dec 4, 2026 | 34% |

Rules:

- We need at least half of the points at each of Stages 2-4 to pass.
- Absence at any stage costs 10% of that student's final grade.
- For Stages 2 and 4, a poorly edited document (many typos and so on) loses 10-20% of that stage's grade.
- Each team may set its own internal schedule and procedure.
- Teams are encouraged to submit earlier than the deadlines.

Lab classes are on Fridays in room 203. Remaining dates: Nov 6, Nov 20, Nov 27 (completing the assignments), Dec 4 (final submission) and Dec 11 (assessment of what was submitted by Dec 4).

Mail must go to **both** instructors: rabiatul.adawiyah.dokt@pw.edu.pl and wladyslaw.homenda@pw.edu.pl.

## 3. Deliverables

### Stage 3 (current): implementation, due Nov 20, 2026

- **Live demo** of the working program on the faculty computers, which have **no compilers**, so we bring a prebuilt, statically linked Windows executable (CONSTITUTION points 2 and 5).
- **Sample inputs** prepared by us, in the input format of section 1, covering YES and NO instances, duplicates, zeros, large values and each input error.
- The code implements the algorithms of the Stage 2 report and must match its pseudocode (CONSTITUTION point 6): brute force with Gray code (§2), meet in the middle (§3), dynamic programming (§4), Karmarkar-Karp (§5) and the final algorithm `SOLVE` (§6).
- The program reads the constraints of section 7 and the thresholds $N_{MITM}$, $T_{DP}$ from the command line, so Stage 4 can measure past them.

Plan of the code (agreed Nov 5, 2026; all steps written: PR 1 steps 1 to 4, PR 2 steps 5 to 8, PR 3 steps 9 and 10). Build with `make`, C++20 (section 6):

| Module | Content | Report |
|---|---|---|
| `number` | the alias `Value = unsigned __int128`, decimal parser and printer, checked addition | §1.4 |
| `input` | reading a file into a list of multisets, checks of section 7, error messages with line numbers | §1.1 |
| `partition` | shared types (`Verdict`, `Partition`, `ExactAnswer`), the preliminary checks: $S$, parity, $S = 0$, $T = S/2$, and the balanced partition from an index set $A$ | §1.2 |
| `algorithms/brute_force` | `FIND-SUBSET` (Gray code) and `BRUTE-FORCE`, returns a balanced partition or NO | §2.2 |
| `algorithms/meet_in_the_middle` | `SUBSET-SUMS` and `MEET-IN-THE-MIDDLE`, the subsets recovered with `FIND-SUBSET`; a balanced partition or NO | §3.2 |
| `algorithms/dynamic_programming` | `DP-PARTITION` on a bitset with the first-reach table; a balanced partition or NO | §4.3 |
| `algorithms/karmarkar_karp` | `KARMARKAR-KARP` with the edge list and 2-colouring, returns $(A, B, d)$ | §5.2 |
| `solve` | `SOLVE`: Karmarkar-Karp always, plus an exact algorithm in range | §6.2 |
| `output` | coloured and plain output (`--no-color`, `NO_COLOR`, not a terminal) | CONSTITUTION 4 |
| `memory` | refuses a table of meet in the middle or dynamic programming larger than the physical memory (Linux; Windows fails with `bad_alloc` in time) | |
| `cli` | command-line options | |
| `main` | reads the file, runs each set, catches input errors | |
| `tests/` | every exact algorithm and the $d$ of Karmarkar-Karp against brute force on random small inputs; parser tests; `cli_test.sh` for the exit codes and the same text with and without colours | CONSTITUTION 7 |

All exact algorithms share the preliminary checks in `partition` and take the multiset as a read-only span, so the test driver runs them interchangeably.

Steps (agreed Nov 5, 2026; one commit per step, three PRs into `dev`):

| Step | Content | Check |
|---|---|---|
| **PR 1: reading** | | |
| 1 | Skeleton: `Makefile`, `.gitignore`, empty `main.cpp`, `tests/check.hpp` | `make`, `make test`, `make clean` work |
| 2 | `number`: `Value`, `parse_value`, `to_string`, `checked_add` | tests at 0, leading zeros, $10^{38}-1$, $10^{38}$, junk |
| 3 | `input`: `read_instances`, the checks of section 7 with line numbers | one test per error |
| 4 | `output`, `cli`, `main` printing the sets read | the two example files, `--no-color`, `\| cat`, a broken file exits with code 1 |
| **PR 2: algorithms** | | |
| 5 | `partition.hpp` and brute force (§2.2) | hand-made cases, the examples of §2.3 |
| 6 | Karmarkar-Karp (§5.2) | §5.3 examples; random inputs: valid partition, sums differ by $d$, $d = 0$ implies YES |
| 7 | Meet in the middle (§3.2) | §3.3 examples; equals brute force on random sets with $n \le 20$, duplicates and zeros |
| 8 | Dynamic programming (§4.3), bitset by words | §4.4 examples; equals brute force; $T$ at word boundaries |
| **PR 3: the program** | | |
| 9 | `SOLVE` (§6.2), `--algorithm`, final output | example files; a DP case, a MITM case, an UNKNOWN case |
| 10 | `examples/`, `make windows`, README build section, `dist/partition.exe` | the `.exe` runs on Windows without extra DLLs |

Not in Stage 3: timing code (Stage 4), multithreading, a big-integer fallback, configuration files.

### Stage 2 (delivered): documentation of algorithms

- **Format:** PDF, sent by email. The website also allows a flash drive, but the email is the formal route.
- **Statement:** it must say clearly that the material is **Stage 2**. Sending it by the deadline with this statement is also the **formal selection of the procedure** for the assignment (formal mode or individual mode, see section 5).
- **Deadline:** end of day **Oct 30, 2026**. The email first said Oct 29 and corrected it to Oct 30. The website still says 29 X, so to be safe we can aim for Oct 29.
- **Contents:**
  1. Pseudocode of every algorithm.
  2. An exhaustive description of each algorithm.
  3. A computational complexity analysis of each algorithm.
- Quality of editing counts (typos and sloppy layout are penalised).

## 4. Requirements for the whole assignment (code and report)

### Report must contain

- **a) Concepts:** definitions of the relevant concepts, with justification or proof. They can come from the literature (cite the source and add a short justification) or be our own (add a short rationale for the definition).
- **b) Algorithms:** descriptions of the algorithms, with complexity analysis.
  - If an exact algorithm has **exponential** complexity, we must also develop a **polynomial-time approximation algorithm**.
- **c) Computational tests:** results, including performance characteristics (computation time as a function of problem size).
- **d) Technical description of the program:** a brief description, plus detailed instructions for compiling and running it.
- **e) Conclusions.**

### Program must

- Be compilable and runnable on the **Windows computers in the lab**.
- Be demonstrated on faculty computers **without compilers installed**, so we need a ready executable (Stage 3).
- Read the input format from section 1.
- Come with prepared sample inputs.

### Stage 2 and the final report

Stage 2 covers items a) and b) of the report. Items c), d) and e) come in Stages 3-4. Stage 4 delivers the whole package: documentation, program, sources and examples.

## 5. Modes of completion

- **Formal mode:** rules are on the course page https://homenda.mini.pw.edu.pl/pages/21 (reachable only from the university network).
- **Individual mode.**

The Stage 2 submission serves as the formal choice of procedure. We need to decide on the mode before sending it.

## 6. Open decisions

- [x] Version: **basic** (2 subsets with equal sums, grade cap 4.0). Decided Oct 28, 2026. The Stage 2 draft is in `docs/stage2/`.
- [ ] Mode: formal or individual.
- [x] Implementation language: **C++**. It must still compile and run on the Windows lab machines, and the demo machines have no compiler, so we need a prebuilt Windows executable (ideally statically linked so it needs no extra DLLs).
- [x] Quality measure for the approximation algorithm: the **discrepancy** $d = |\mathrm{sum}(A) - \mathrm{sum}(B)|$. It is 0 exactly when an equal-sum partition exists, so the exact algorithms decide the problem and the approximation minimises $d$.
- [x] Partition sides and minimal size: **both subsets must be non-empty, so every input set must have $n \ge 2$ elements**. Decided Oct 29, 2026. Rationale: the assignment asks for a partition into *two* subsets, which we read as two non-empty ones. The requirement is free of charge: if $S > 0$ both sides of a balanced partition have sum $S/2 > 0$ and are non-empty anyway, and if $S = 0$ (all zeros) the partition $\{1\}$ against the rest works. Only $n = 1$ is excluded and is reported as an input error. Documented in `docs/stage2/01_1_introduction.md` (*Constraints on the input*) and `docs/stage2/01_2_definitions.md` (*Trivial cases*).
- [x] Number representation: **unsigned 128-bit integers** (`unsigned __int128` on GCC/Clang, which is what we build the static Windows executable with). Decided Oct 28, 2026. Rationale: 64 bits (19 digits) cannot hold the 25-digit values in `AaC_example_2.txt`; 128 bits hold values up to $2^{128}-1 \approx 3.4 \cdot 10^{38}$ (39 digits); inputs are non-negative so no sign bit is wasted; every arithmetic operation is $O(1)$, which keeps complexity bounds free of a digit-length factor. The concrete limits are in section 7; parsing and summation check them and fail with a clear error, never silently. The type sits behind one alias so it can be swapped for a big-integer class if the instructors ever test larger inputs. Since `std::cin`/`std::cout` do not support `__int128`, we write our own decimal parser and printer.
- [x] Total sum: **$\Sigma(M) < 2^{128}$ is an assumption**; larger totals are rejected with an error. Decided Oct 29, 2026. Rationale: Karmarkar-Karp (differences only) and dynamic programming ($S/2 \le T_{DP}$) cannot overflow anyway; the only real limit is meet in the middle with 38-digit values, which is rejected. For 25-digit data the bound allows $n$ up to about $10^{13}$. A wider type would be the alternative if larger inputs ever appear.
- [ ] Ranges of the exact algorithms ($N_{MITM}$, $T_{DP}$): **not fixed in the Stage 2 report**; the values are determined by experiments once the algorithms are implemented. Decided Oct 29, 2026. Provisional values: $N_{MITM} = 44$, $T_{DP} = 3 \cdot 10^8$ ($10^{10}$ until Nov 9, 2026, when dynamic programming started to return the subset), $W_{DP} = 10^{12}$.
- [x] Brute force is **not used by the final algorithm** and has no range parameter ($N_{BF}$ dropped, Oct 30, 2026). Rationale: meet in the middle is faster for every $n \ge 5$ and its memory is negligible for the few tens of elements brute force can handle. Brute force stays in the report as the baseline and in the program as the reference for testing (CONSTITUTION point 7). Prototype measurements of Oct 29 (random 25-digit numbers, 12 cores, 15 GB): brute force with Gray code $n = 30$ takes 2.3 s; meet in the middle $n = 44$ takes 0.9 s and about 260 MB, $n = 48$ takes 4 s and 1 GB; one dynamic programming pass over $10^{10}$ bits (1.25 GB) takes 0.07 to 0.13 s, so the work $n \cdot T$ also needs a bound (about $6 \cdot 10^{12}$ for one minute).
- [x] Final algorithm in the Stage 2 report: Karmarkar-Karp always, plus an exact algorithm in range; when both are in range the report says only that the cheaper one for the input is chosen (estimates of Section 4.7) and fixes no rule, and it states no complexity theorem for the combination. Decided Oct 30, 2026. Rationale: the rule and the thresholds depend on measurements that come with Stage 3, so the report would otherwise commit to numbers that the experiments may change.
- [x] Build system: **`make`**. Decided Nov 5, 2026. A `Makefile` at the repository root builds the Linux binary (`make`), the static Windows executable with MinGW-w64 (`make windows`, `x86_64-w64-mingw32-g++ -static`), the tests (`make test`) and cleans up (`make clean`); the README lists the same compiler commands for building without `make` (CONSTITUTION point 5). No CMake.
- [x] C++ standard: **C++20** (`-std=c++20`). Decided Nov 5, 2026. Rationale: it has `std::span` and `std::countr_zero`, which the algorithms need, and is fully supported by GCC 13 and MinGW-w64 GCC; the earlier "C++21" in the CONSTITUTION was a typo (no such standard exists) and is corrected.
- [x] Output: **readable and coloured on both Linux and Windows, with colours optionally disabled**. Decided Nov 5, 2026. ANSI colours on Linux; on Windows the program enables ANSI processing in the console at startup (`SetConsoleMode` with `ENABLE_VIRTUAL_TERMINAL_PROCESSING`, Windows 10 and later) and falls back to plain output if that fails. Colours are off with `--no-color`, when `NO_COLOR` is set, or when stdout is not a terminal; the text is the same in both modes (CONSTITUTION point 4).
- [x] Code layout: **`src/` with the algorithms in `src/algorithms/`, tests in `tests/`, sample inputs in `examples/`, build output in `build/` (not committed), the prebuilt Windows executable in `dist/` (committed before the demo)**. Decided Nov 5, 2026. Module table in section 3. Shared types and the preliminary checks of §1.2 live once in `partition.hpp`; only `output` prints and only it contains Windows-specific code; input errors are exceptions caught in `main`.
- [x] Command line: `partition [--no-color] [--n-mitm N] [--t-dp T] [--w-dp W] [--algorithm solve|bf|mitm|dp|kk] FILE`. Decided Nov 5, 2026. `--algorithm` runs a single algorithm, for the Stage 4 timings.
- [x] Choice of the exact algorithm in `SOLVE` when both are in range (provisional, tuned in Stage 4): dynamic programming if $T/32 < 2^{n/2}$, otherwise meet in the middle. Decided Nov 5, 2026; adjusted Nov 9, 2026. Proposition 4.4 of the report gives $T/64 < 2^{n/2}$ for the bitset alone; since dynamic programming now also fills the 4-byte first-reach table (4T bytes, a scalar write per new sum), its cost is doubled. Measured on a development laptop (NO instances, one run each): $n = 40$, $T = 3.2 \cdot 10^7$: dynamic programming 0.15 s and 130 MB, meet in the middle 0.17 s and 36 MB; $n = 36$, $T = 8.7 \cdot 10^6$: 0.03 s against 0.04 s; so the two cost about the same when $T/64 \approx 0.5 \cdot 2^{n/2}$, which the factor 2 puts at the switch. Crude (single runs, one machine), tuned in Stage 4. Kept in one function.
- [x] Work bound of dynamic programming: **$n \cdot T \le W_{DP}$** (`--w-dp W`, default $10^{12}$), next to $T \le T_{DP}$. Decided Nov 5, 2026. Rationale: the time of dynamic programming is $O(n \cdot T/64)$, so $T \le T_{DP}$ alone allows hours on valid inputs (measured: $n = 201$, $T \approx 5.2 \cdot 10^9$, a NO instance with $d > 0$, 12 s; $n = 10^6$ would take many hours). $10^{12}$ is about 11 s in the worst case on the development laptop; tuned in Stage 4. **The Stage 2 report does not have this bound**: in Stage 4, `SOLVE` (§6.2) gets the condition $n \cdot T \le W_{DP}$ for dynamic programming and Theorem 6.1(4) reads "UNKNOWN only if $S$ is even, $d > 0$, $n > N_{MITM}$ and ($T > T_{DP}$ or $n \cdot T > W_{DP}$)". `--t-dp` is capped at $2^{63} - 1$, the largest target the bitset can index, so that the message "T > T_DP" is always true.
- [x] `--algorithm bf|mitm|dp|kk` runs one algorithm alone; meet in the middle and dynamic programming still respect `--n-mitm`, `--t-dp` and `--w-dp` and are skipped with a warning beyond them (raise the option to go further), brute force only has its hard limit $n \le 64$. An algorithm that cannot run on a set (that limit, or no memory) is an error for that set, the other sets still run, and the exit code is 3 (1: invalid input file, 2: invalid command line). Decided Nov 5, 2026.
- [x] **Every exact algorithm returns a balanced partition** when one exists, not only YES (requested by the instructor, Nov 9, 2026; the output file lists the subsets). Decided Nov 9, 2026. Brute force reads $A$ off the mask that hits $T$; its loop is the procedure `FIND-SUBSET(b, x)`, which finds a subset of $b$ with sum $x$ by the Gray code. Meet in the middle keeps only the sums in its sorted lists, so after the sweep finds $x + y = T$ it calls `FIND-SUBSET` on each half: $O(2^{n/2})$ more time, no more memory. Dynamic programming keeps next to the bitset a table `first[s]`, the step at which the sum $s$ became reachable (32 bits per sum), and reads $A$ back from $T$; the indices strictly decrease, so they are distinct. The table costs 32 times the memory of the bitset, hence $T_{DP}$ drops from $10^{10}$ to $3 \cdot 10^8$ (about 1.2 GB), which still covers many small numbers. Divide and conquer would keep the memory of the bitset at the cost of more code and a longer proof; not chosen. `SOLVE` returns the Karmarkar-Karp partition and, with YES, a balanced partition (that of Karmarkar-Karp when $d = 0$, otherwise that of the exact algorithm), so YES holds exactly when a balanced partition is returned. The Stage 2 report was updated to match on Nov 18, 2026 (title and abstract, Sections 1.1, 1.2, 1.4, 2, 3, 4, 5 and 6; PR #28).

## 7. Input constraints (our own, set Oct 28, 2026)

The instructors gave no limits beyond the two examples, so we fix our own and state them in the report and in the program's error messages. They follow from the 128-bit representation and from the memory of the exact algorithms. If the instructors answer the question in `TODO.md`, these get revised.

| Quantity | Constraint | Reason |
|---|---|---|
| Value of an element $a_i$ | integer, $0 \le a_i < 10^{38}$ (at most 38 decimal digits, leading zeros allowed) | fits in `unsigned __int128` with headroom; the examples use at most 25 digits |
| Total sum of one set $S = \sum a_i$ | $S < 2^{128}$, checked at runtime with overflow detection; larger totals are rejected | every intermediate sum in every algorithm is bounded by $S$ |
| Number of elements per set $n$ | $2 \le n \le 10^6$ for the approximation algorithm; meet in the middle is run only for $n \le 44$, otherwise the program reports that the exact method is skipped; brute force is a test reference only and is run for $n \le 30$ in the tests | memory $O(2^{n/2})$ of meet in the middle (about 100 MB per half at $n = 44$, 25 GB at $n = 60$); time $O(2^n)$ of brute force |
| Sides of the partition | both non-empty (hence $n \ge 2$; a line with one number is an input error) | the assignment asks for *two* subsets; see section 6 |
| Dynamic programming over sums | run only when $S/2 \le 3 \cdot 10^8$ (about 1.2 GB: the bitset and the first-reach table of 4 bytes per sum) and $n \cdot S/2 \le 10^{12}$, otherwise skipped with a message | table size is $O(S)$ words, time $O(n \cdot S/64 + S)$ |
| Number of sets in a file $N$ | $1 \le N \le 10^4$ | arbitrary; sets are processed independently, one after another |
| Format | decimal digits only, single spaces, Unix or Windows line endings, optional trailing newline | the examples |

Anything outside these bounds is rejected with a clear error message and a non-zero exit code (CONSTITUTION point 3). The thresholds for the exact algorithms are configurable on the command line so that the Stage 4 experiments can push past them deliberately.

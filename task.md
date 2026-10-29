# Algorithms and Computability: Laboratory Assignment (fall 2026)

Sources: `raw_regulations/` (course website, emails of 16.10.26 and 23.10.26, input examples).

> **Current stage: Stage 2 (documentation of algorithms), due Oct 30, 2026 (end of day).**

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
| **2 (current)** | **Documentation of algorithms: pseudocode, exhaustive description, complexity analysis** | **Oct 30, 2026** | **33%** |
| 3 | Implementation of the algorithms and sample input preparation. Live demo of working code on faculty computers **without compilers installed**. | Nov 20, 2026 | 33% |
| 4 | Empirical testing and a report on the tests. Deliver everything: documentation, program, sources, examples. | Dec 4, 2026 | 34% |

Rules:

- We need at least half of the points at each of Stages 2-4 to pass.
- Absence at any stage costs 10% of that student's final grade.
- For Stages 2 and 4, a poorly edited document (many typos and so on) loses 10-20% of that stage's grade.
- Each team may set its own internal schedule and procedure.
- Teams are encouraged to submit earlier than the deadlines.

Lab classes are on Fridays in room 203. Remaining dates: Nov 6, Nov 20, Nov 27 (completing the assignments), Dec 4 (final submission) and Dec 11 (assessment of what was submitted by Dec 4).

Mail must go to **both** instructors: rabiatul.adawiyah.dokt@pw.edu.pl and wladyslaw.homenda@pw.edu.pl.

## 3. Stage 2: what we must deliver now

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

- [x] Version: **basic** (2 subsets with equal sums, grade cap 4.0). Decided Oct 28, 2026. The Stage 2 draft is `docs/stage2_basic.md`.
- [ ] Mode: formal or individual.
- [x] Implementation language: **C++**. It must still compile and run on the Windows lab machines, and the demo machines have no compiler, so we need a prebuilt Windows executable (ideally statically linked so it needs no extra DLLs).
- [x] Quality measure for the approximation algorithm: the **discrepancy** $d = |\mathrm{sum}(A) - \mathrm{sum}(B)|$. It is 0 exactly when an equal-sum partition exists, so the exact algorithms decide the problem and the approximation minimises $d$.
- [x] Partition sides and minimal size: **both subsets must be non-empty, so every input set must have $n \ge 2$ elements**. Decided Oct 29, 2026. Rationale: the assignment asks for a partition into *two* subsets, which we read as two non-empty ones. The requirement is free of charge: if $S > 0$ both sides of a balanced partition have sum $S/2 > 0$ and are non-empty anyway, and if $S = 0$ (all zeros) the partition $\{1\}$ against the rest works. Only $n = 1$ is excluded and is reported as an input error. Documented in `docs/stage2_basic.md` section 1.1 (*Assumptions*) and 1.2 (*Trivial cases*).
- [x] Number representation: **unsigned 128-bit integers** (`unsigned __int128` on GCC/Clang, which is what we build the static Windows executable with). Decided Oct 28, 2026. Rationale: 64 bits (19 digits) cannot hold the 25-digit values in `AaC_example_2.txt`; 128 bits hold values up to $2^{128}-1 \approx 3.4 \cdot 10^{38}$ (39 digits); inputs are non-negative so no sign bit is wasted; every arithmetic operation is $O(1)$, which keeps complexity bounds free of a digit-length factor. The concrete limits are in section 7; parsing and summation check them and fail with a clear error, never silently. The type sits behind one alias so it can be swapped for a big-integer class if the instructors ever test larger inputs. Since `std::cin`/`std::cout` do not support `__int128`, we write our own decimal parser and printer.
- [x] Total sum: **$\Sigma(M) < 2^{128}$ is an assumption**; larger totals are rejected with an error. Decided Oct 29, 2026. Rationale: Karmarkar-Karp (differences only) and dynamic programming ($S/2 \le 10^{10}$) cannot overflow anyway; the only real limit is meet in the middle with 38-digit values, which is rejected. For 25-digit data the bound allows $n$ up to about $10^{13}$. A wider type would be the alternative if larger inputs ever appear.
- [x] Meet in the middle limit lowered from $n \le 60$ to **$n \le 44$** (about 100 MB per half; $n = 60$ needs about 25 GB). Decided Oct 29, 2026. Configurable on the command line.

## 7. Input constraints (our own, set Oct 28, 2026)

The instructors gave no limits beyond the two examples, so we fix our own and state them in the report and in the program's error messages. They follow from the 128-bit representation and from the memory of the exact algorithms. If the instructors answer the question in `TODO.md`, these get revised.

| Quantity | Constraint | Reason |
|---|---|---|
| Value of an element $a_i$ | integer, $0 \le a_i < 10^{38}$ (at most 38 decimal digits, leading zeros allowed) | fits in `unsigned __int128` with headroom; the examples use at most 25 digits |
| Total sum of one set $S = \sum a_i$ | $S < 2^{128}$, checked at runtime with overflow detection; larger totals are rejected | every intermediate sum in every algorithm is bounded by $S$ |
| Number of elements per set $n$ | $2 \le n \le 10^6$ for the approximation algorithm; exact algorithms are run only for $n \le 44$ (meet in the middle) and $n \le 30$ (brute force), otherwise the program reports that the exact method is skipped | memory $O(2^{n/2})$ of meet in the middle (about 100 MB per half at $n = 44$, 25 GB at $n = 60$); time $O(2^n)$ of brute force |
| Sides of the partition | both non-empty (hence $n \ge 2$; a line with one number is an input error) | the assignment asks for *two* subsets; see section 6 |
| Dynamic programming over sums | run only when $S/2 \le 10^{10}$ (about 1.25 GB as a bitset), otherwise skipped with a message | table size is $O(S)$ bits |
| Number of sets in a file $N$ | $1 \le N \le 10^4$ | arbitrary; sets are processed independently, one after another |
| Format | decimal digits only, single spaces, Unix or Windows line endings, optional trailing newline | the examples |

Anything outside these bounds is rejected with a clear error message and a non-zero exit code (CONSTITUTION point 3). The thresholds for the exact algorithms are configurable on the command line so that the Stage 4 experiments can push past them deliberately.

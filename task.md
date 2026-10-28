# Algorithms and Computability: Laboratory Assignment (fall 2026)

Sources: `raw_regulations/` (course website, emails of 16.10.26 and 23.10.26, input examples).

> **Current stage: Stage 2 (documentation of algorithms), due Oct 30, 2026 (end of day).**

## 1. The problem

Teams of up to 4 students. Both versions are currently considered, and we have not picked one yet.

| Version | Task | Max grade |
|---|---|---|
| **a) Basic** | Partition a set of numbers into **2 subsets with equal sums** | up to 3.5, can be improved to 4.0 |
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

- [ ] Version: **basic** (2 subsets, equal sums, grade cap 4.0) or **advanced** (k subsets, grade up to 5.0). Not decided by the team yet. The current Stage 2 draft (`docs/stage2_basic.md`, branch `feature/simple-task`) targets the **basic** version; its algorithms (Karmarkar-Karp in particular) extend to $k$ subsets if we switch.
- [ ] Mode: formal or individual.
- [x] Implementation language: **C++**. It must still compile and run on the Windows lab machines, and the demo machines have no compiler, so we need a prebuilt Windows executable (ideally statically linked so it needs no extra DLLs).
- [x] Objective for "sums as close as possible" (advanced version): **minimize max − min** over the $k$ subset sums (the spread). For $k=2$ this equals $|S - 2\,\mathrm{sum}(A)|$, and it is 0 exactly when an equal-sum partition exists, so the basic version is the case $k=2$ with spread 0. The standard bounds in the literature (Graham's LPT, MULTIFIT) are stated for the largest sum (makespan), so they do not carry over directly; we must cite or prove our own bounds for the spread.
- [x] Number representation: **signed 128-bit integers** (`__int128` on GCC/Clang, which is what we build the static Windows executable with). Decided Oct 28, 2026. Rationale: 64 bits (19 digits) cannot hold the 25-digit values in `AaC_example_2.txt`; 128 bits give 38 digits, so 13 digits of headroom for sums; every arithmetic operation is $O(1)$, which keeps complexity bounds free of a digit-length factor. Limits: each value and each total sum must fit in 38 digits; parsing and summation check for overflow and fail with a clear error, never silently. The type sits behind one alias so it can be swapped for a big-integer class if the instructors ever test larger inputs. Since `std::cin`/`std::cout` do not support `__int128`, we write our own decimal parser and printer.

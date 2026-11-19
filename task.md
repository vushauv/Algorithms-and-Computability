# Algorithms and Computability: Laboratory Assignment (fall 2026)

Sources: `raw_regulations/` (course website, emails of 16.10.26 and 23.10.26, the output format of 27.10.26 with its example `AaClab.txt` and `AaClab_output.txt` and its corrections of 28.10.26 and 29.10.26, Stage 3 of 07.11.26, input examples).

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
- From Homenda's Stage 3 email of Nov 7, 2026 (`raw_regulations/email_07.11.26.txt`): one program with the algorithm chosen by a command-line parameter is allowed; it is run from the console with the input file and the number of subsets $k$; the results are **saved to disk** in Homenda's format (`raw_regulations/email_27.10.26.txt`, corrected on 28.10 and 29.10); a short **`readme.txt`** says how to run it and where the results go (`deliverable/readme.txt`); examples to check that it works.

Plan of the code (agreed Nov 5, 2026; all steps written: PR 1 steps 1 to 4, PR 2 steps 5 to 8, PR 3 steps 9 and 10). Build with `make`, C++20 (section 6):

| Module | Content | Report |
|---|---|---|
| `number` | the three types of numbers (`Native = unsigned __int128`, `BoostInt`, `BigUint`) and what the algorithms need of each: decimal parser and printer, checked addition, parity, halving, conversion to 64 bits | §1.4 |
| `big_uint` | `BigUint`, our own long arithmetic with the base of `--base` | |
| `input` | reading a file into a list of multisets, the syntax checks and, for `native`, the limits of section 7, error messages with line numbers | §1.1 |
| `partition` | shared types (`Verdict`, `Partition`, `ExactAnswer`), the preliminary checks: $S$, parity, $S = 0$, $T = S/2$, and the balanced partition from an index set $A$, `values_at` (the elements of an index set) | §1.2 |
| `algorithms/brute_force` | `FIND-SUBSET` (Gray code) and `BRUTE-FORCE`, returns a balanced partition or NO | §2.2 |
| `algorithms/meet_in_the_middle` | `SUBSET-SUMS` and `MEET-IN-THE-MIDDLE`, the subsets recovered with `FIND-SUBSET`; a balanced partition or NO | §3.2 |
| `algorithms/dynamic_programming` | `DP-PARTITION` on a bitset with the first-reach table; a balanced partition or NO | §4.3 |
| `algorithms/karmarkar_karp` | `KARMARKAR-KARP` with the edge list and 2-colouring, returns $(A, B, d)$ | §5.2 |
| `solve` | `SOLVE`: Karmarkar-Karp always, plus an exact algorithm in range | §6.2 |
| `output` | coloured and plain output (`--no-color`, `NO_COLOR`, not a terminal), the messages `error:` and `warning:`, the Windows-specific code | CONSTITUTION 4 |
| `report` | the console report: the set read, the partitions, the answer and its reason (uses `output` for the colours) | |
| `run` | runs one set with `SOLVE` or the one algorithm of `--algorithm`, prints through `report`, returns the partition for the results file; the fallback partition of Karmarkar-Karp | |
| `result_file` | the results file in Homenda's format: its name, the header, one block per set with the subsets sorted | |
| `memory` | refuses a table of meet in the middle or dynamic programming larger than the physical memory (Linux; Windows fails with `bad_alloc` in time) | |
| `cli` | command-line options | |
| `main` | options, picks the arithmetic of `--arithmetic`, reads the file, loops over the sets (`run`), catches input errors and failures per set, writes the results file, exit codes | |
| `tests/` | every exact algorithm and the $d$ of Karmarkar-Karp against brute force on random small inputs; `BigUint` against the native numbers and Boost in seven bases; every algorithm in the long arithmetics against the native one; parser tests; the results file against Homenda's example; `cli_test.sh` for the exit codes, the results file and the same text with and without colours; `test_scale.cpp` and `generate.sh` for numbers of thousands of digits, $10^5$ numbers and $10^4$ sets; `make test-quick` runs the light part (budgets in `tests/README.md`: `make test` at most 90 s, `make test-quick` at most 15 s) | CONSTITUTION 7 |

All exact algorithms share the preliminary checks in `partition` and take the multiset as a read-only span, so the test driver runs them interchangeably. Since Nov 18, 2026 every module that handles numbers is a template over their type `V`, with the bodies in the `.cpp` files and explicit instantiations for the three types (section 6).

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
| 10 | `deliverable/examples/`, `make windows`, README build section, `deliverable/partition.exe` | the `.exe` runs on Windows without extra DLLs |

Not in Stage 3: timing code (Stage 4), multithreading, an automatic choice of the arithmetic (`--arithmetic` chooses it), configuration files.

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

The Stage 2 submission serves as the formal choice of procedure. **We work in the individual mode** (section 6).

## 6. Open decisions

- [x] Version: **basic** (2 subsets with equal sums, grade cap 4.0). Decided Oct 28, 2026. The Stage 2 draft is in `docs/stage2/`.
- [x] Mode: **individual**. Recorded Nov 18, 2026.
- [x] Implementation language: **C++**. It must still compile and run on the Windows lab machines, and the demo machines have no compiler, so we need a prebuilt Windows executable (ideally statically linked so it needs no extra DLLs).
- [x] Quality measure for the approximation algorithm: the **discrepancy** $d = |\mathrm{sum}(A) - \mathrm{sum}(B)|$. It is 0 exactly when an equal-sum partition exists, so the exact algorithms decide the problem and the approximation minimises $d$.
- [x] Partition sides and minimal size: **both subsets must be non-empty, so every input set must have $n \ge 2$ elements**. Decided Oct 29, 2026. Rationale: the assignment asks for a partition into *two* subsets, which we read as two non-empty ones. The requirement is free of charge: if $S > 0$ both sides of a balanced partition have sum $S/2 > 0$ and are non-empty anyway, and if $S = 0$ (all zeros) the partition $\{1\}$ against the rest works. Only $n = 1$ is excluded and is reported as an input error. Documented in `docs/stage2/01_1_introduction.md` (*Constraints on the input*) and `docs/stage2/01_2_definitions.md` (*Trivial cases*).
- [x] Number representation: **unsigned 128-bit integers** (`unsigned __int128` on GCC/Clang, which is what we build the static Windows executable with). Decided Oct 28, 2026. Rationale: 64 bits (19 digits) cannot hold the 25-digit values in `AaC_example_2.txt`; 128 bits hold values up to $2^{128}-1 \approx 3.4 \cdot 10^{38}$ (39 digits); inputs are non-negative so no sign bit is wasted; every arithmetic operation is $O(1)$, which keeps complexity bounds free of a digit-length factor. The concrete limits are in section 7; parsing and summation check them and fail with a clear error, never silently. The type sits behind one alias so it can be swapped for a big-integer class if the instructors ever test larger inputs. Since `std::cin`/`std::cout` do not support `__int128`, we write our own decimal parser and printer. Since Nov 18, 2026 this holds only with `--arithmetic native`; the default arithmetic has no such limit (entry "Numbers of any size" below).
- [x] Total sum: **$\Sigma(M) < 2^{128}$ is an assumption**; larger totals are rejected with an error. Decided Oct 29, 2026. Rationale: Karmarkar-Karp (differences only) and dynamic programming ($S/2 \le T_{DP}$) cannot overflow anyway; the only real limit is meet in the middle with 38-digit values, which is rejected. For 25-digit data the bound allows $n$ up to about $10^{13}$. A wider type would be the alternative if larger inputs ever appear. Since Nov 18, 2026 this holds only with `--arithmetic native`; the default arithmetic has no such limit (entry "Numbers of any size" below).
- [ ] Ranges of the exact algorithms ($N_{MITM}$, $T_{DP}$): **not fixed in the Stage 2 report**; the values are determined by experiments once the algorithms are implemented. Decided Oct 29, 2026. Provisional values: $N_{MITM} = 44$, $T_{DP} = 3 \cdot 10^8$ ($10^{10}$ until Nov 9, 2026, when dynamic programming started to return the subset), $W_{DP} = 10^{12}$. Since Nov 18, 2026 the default numbers are those of Boost, which take 32 bytes instead of 16, so meet in the middle needs about twice the memory at the same $n$ and the values were not measured again with them.
- [x] Brute force is **not used by the final algorithm** and has no range parameter ($N_{BF}$ dropped, Oct 30, 2026). Rationale: meet in the middle is faster for every $n \ge 5$ and its memory is negligible for the few tens of elements brute force can handle. Brute force stays in the report as the baseline and in the program as the reference for testing (CONSTITUTION point 7). Prototype measurements of Oct 29 (random 25-digit numbers, 12 cores, 15 GB): brute force with Gray code $n = 30$ takes 2.3 s; meet in the middle $n = 44$ takes 0.9 s and about 260 MB, $n = 48$ takes 4 s and 1 GB; one dynamic programming pass over $10^{10}$ bits (1.25 GB) takes 0.07 to 0.13 s, so the work $n \cdot T$ also needs a bound (about $6 \cdot 10^{12}$ for one minute).
- [x] Final algorithm in the Stage 2 report: Karmarkar-Karp always, plus an exact algorithm in range; when both are in range the report says only that the cheaper one for the input is chosen (estimates of Section 4.7) and fixes no rule, and it states no complexity theorem for the combination. Decided Oct 30, 2026. Rationale: the rule and the thresholds depend on measurements that come with Stage 3, so the report would otherwise commit to numbers that the experiments may change.
- [x] Build system: **`make`**. Decided Nov 5, 2026. A `Makefile` at the repository root builds the Linux binary (`make`), the static Windows executable with MinGW-w64 (`make windows`, `x86_64-w64-mingw32-g++ -static`), the tests (`make test`) and cleans up (`make clean`); the README lists the same compiler commands for building without `make` (CONSTITUTION point 5). No CMake.
- [x] C++ standard: **C++20** (`-std=c++20`). Decided Nov 5, 2026. Rationale: it has `std::span` and `std::countr_zero`, which the algorithms need, and is fully supported by GCC 13 and MinGW-w64 GCC; the earlier "C++21" in the CONSTITUTION was a typo (no such standard exists) and is corrected.
- [x] Output: **readable and coloured on both Linux and Windows, with colours optionally disabled**. Decided Nov 5, 2026. ANSI colours on Linux; on Windows the program enables ANSI processing in the console at startup (`SetConsoleMode` with `ENABLE_VIRTUAL_TERMINAL_PROCESSING`, Windows 10 and later) and falls back to plain output if that fails. Colours are off with `--no-color`, when `NO_COLOR` is set, or when stdout is not a terminal; the text is the same in both modes (CONSTITUTION point 4).
- [x] Code layout: **`src/` with the algorithms in `src/algorithms/`, tests in `tests/`, build output in `build/` (not committed), and `deliverable/` (committed) with everything handed over for the demo: the prebuilt Windows executable `partition.exe`, `readme.txt` and the sample inputs in `examples/`**. Decided Nov 5, 2026; `deliverable/` replaced `dist/` and the root `examples/` on Nov 19, 2026, so that each file has one place. Module table in section 3. Shared types and the preliminary checks of §1.2 live once in `partition.hpp`; the algorithms never print; the console report is in `report` and the choice and run of the algorithm in `run` (refactoring of Nov 19, 2026, both moved out of `main`), and only `output` contains Windows-specific code and the colour codes; input errors are exceptions caught in `main`. Boost.Multiprecision is vendored in `third_party/` (entry "Numbers of any size" below).
- [x] Command line: `partition [--no-color] [--n-mitm N] [--t-dp T] [--w-dp W] [--algorithm solve|bf|mitm|dp|kk] [--arithmetic boost|own|native] [--base B] FILE [K]`. Decided Nov 5, 2026; `K` added Nov 14, 2026; `--arithmetic` and `--base` added Nov 18, 2026. `--algorithm` runs a single algorithm, for the Stage 4 timings. `K` is the number of subsets, which Homenda passes to every program (`raw_regulations/email_07.11.26.txt`); it is optional, and any value other than 2 is an invalid command line (exit code 2).
- [x] Choice of the exact algorithm in `SOLVE` when both are in range (provisional, tuned in Stage 4): dynamic programming if $T/32 < 2^{n/2}$, otherwise meet in the middle. Decided Nov 5, 2026; adjusted Nov 9, 2026. Proposition 4.4 of the report gives $T/64 < 2^{n/2}$ for the bitset alone; since dynamic programming now also fills the 4-byte first-reach table (4T bytes, a scalar write per new sum), its cost is doubled. Measured on a development laptop (NO instances, one run each): $n = 40$, $T = 3.2 \cdot 10^7$: dynamic programming 0.15 s and 130 MB, meet in the middle 0.17 s and 36 MB; $n = 36$, $T = 8.7 \cdot 10^6$: 0.03 s against 0.04 s; so the two cost about the same when $T/64 \approx 0.5 \cdot 2^{n/2}$, which the factor 2 puts at the switch. Crude (single runs, one machine), tuned in Stage 4. Kept in one function.
- [x] Work bound of dynamic programming: **$n \cdot T \le W_{DP}$** (`--w-dp W`, default $10^{12}$), next to $T \le T_{DP}$. Decided Nov 5, 2026. Rationale: the time of dynamic programming is $O(n \cdot T/64)$, so $T \le T_{DP}$ alone allows hours on valid inputs (measured: $n = 201$, $T \approx 5.2 \cdot 10^9$, a NO instance with $d > 0$, 12 s; $n = 10^6$ would take many hours). $10^{12}$ is about 11 s in the worst case on the development laptop; tuned in Stage 4. The report has the bound since Nov 18, 2026: the range table of §1.4, §4.7, steps and pseudocode of `SOLVE` (§6.1, §6.2) and Theorem 6.1(4), "UNKNOWN only if $S$ is even, $d > 0$, $n > N_{MITM}$, and $T > T_{DP}$ or $n \cdot T > W_{DP}$". `--t-dp` is capped at $2^{63} - 1$, the largest target the bitset can index, so that the message "T > T_DP" is always true.
- [x] `--algorithm bf|mitm|dp|kk` runs one algorithm alone; meet in the middle and dynamic programming still respect `--n-mitm`, `--t-dp` and `--w-dp` and are skipped with a warning beyond them (raise the option to go further), brute force only has its hard limit $n \le 64$. An algorithm that cannot run on a set (that limit, or no memory) is an error for that set, the other sets still run, and the exit code is 3 (1: invalid input file, 2: invalid command line, 4: the results file cannot be written). Decided Nov 5, 2026.
- [x] **Every exact algorithm returns a balanced partition** when one exists, not only YES (requested by the instructor, Nov 9, 2026; the output file lists the subsets). Decided Nov 9, 2026. Brute force reads $A$ off the mask that hits $T$; its loop is the procedure `FIND-SUBSET(b, x)`, which finds a subset of $b$ with sum $x$ by the Gray code. Meet in the middle keeps only the sums in its sorted lists, so after the sweep finds $x + y = T$ it calls `FIND-SUBSET` on each half: $O(2^{n/2})$ more time, no more memory. Dynamic programming keeps next to the bitset a table `first[s]`, the step at which the sum $s$ became reachable (32 bits per sum), and reads $A$ back from $T$; the indices strictly decrease, so they are distinct. The table costs 32 times the memory of the bitset, hence $T_{DP}$ drops from $10^{10}$ to $3 \cdot 10^8$ (about 1.2 GB), which still covers many small numbers. Divide and conquer would keep the memory of the bitset at the cost of more code and a longer proof; not chosen. `SOLVE` returns the Karmarkar-Karp partition and, with YES, a balanced partition (that of Karmarkar-Karp when $d = 0$, otherwise that of the exact algorithm), so YES holds exactly when a balanced partition is returned. The Stage 2 report was updated to match on Nov 18, 2026 (title and abstract, Sections 1.1, 1.2, 1.4, 2, 3, 4, 5 and 6; PR #28).
- [x] **Results file in Homenda's format** (`raw_regulations/email_27.10.26.txt` and its two corrections, `email_28.10.26.txt` and `email_29.10.26.txt`; example `AaClab.txt`, `AaClab_output.txt` with $k = 5$). Decided Nov 14, 2026. Next to the console output, the program saves `FILE` with `_output.txt` in place of `.txt` (any other name gets `_output.txt` appended), in the directory of the input: the line "N 2"; then, after a blank line, for every set the line "S a1 ... an" in input order, the two subsets as "s x1 ...", the numbers non-increasing and the lines lexicographically non-increasing (not by their sums), and the difference of the two sums. Numbers are separated by single spaces, lines by `\n` also on Windows, and the file ends without a newline, as the example does; the example's one trailing space (after the second set line) is a slip and is not copied. The subsets written are the balanced partition when one is found, otherwise the Karmarkar-Karp partition with its $d$: for `SOLVE` the balanced one with YES and that of Karmarkar-Karp with NO or UNKNOWN; for an exact algorithm alone that answers NO, is skipped or fails, the program also runs Karmarkar-Karp, prints its partition under "For the results file:" and writes it, so every set has a partition in the file. An invalid input file writes no results file; a results file that cannot be written gives exit code 4. `tests/test_result_file.cpp` rebuilds Homenda's example from its subsets byte for byte (except the trailing space). Not done: the exact algorithms do not search for the smallest difference when the answer is NO, so with NO the file shows the difference of Karmarkar-Karp, which need not be the smallest.
- [x] **Extra spaces and tabs in the input are accepted** (Nov 18, 2026): one or more spaces or tabs separate the numbers, and those at the start and the end of a line are skipped; a line of blanks only counts as empty. Before, a second space, a tab or a space at the end of a line was an input error with exit code 1 and no results file. Rationale: Homenda's own output example (`raw_regulations/AaClab_output.txt`) has a space at the end of a line, so his input files may have one too, and one stray space must not lose a whole file at the demo. The format we write and document stays single spaces; the sample `examples/11_extra_spaces.txt` shows the accepted variants, and the three error samples for them are removed.
- [x] **Numbers of any size, three arithmetics** (decided Nov 18, 2026). The algorithms are templates over the type of the numbers and `--arithmetic` chooses it at run time: `boost` (**the default**, also in `deliverable/partition.exe`), `cpp_int` of Boost.Multiprecision 1.83, header-only, copied with Boost.Config into `third_party/` (3.2 MB, Boost Software License; built with `-isystem third_party -DBOOST_MP_STANDALONE`); `own`, our class `BigUint` (`src/big_uint.hpp`), cells of 32 bits in a base set by `--base B`, $2 \le B \le 2^{32}$, default $2^{32}$; `native`, `unsigned __int128` with its limits $a_i < 10^{38}$ and $S < 2^{128}$. Rationale: Homenda wants no limit on the input, and Stage 4 compares one machine operation (native) with long arithmetic; no library lets the base be chosen (Boost and GMP fix the cell at a machine word), so the base experiment needs our own class, and Boost is the tested default. The algorithms need only addition, subtraction, comparison, parity, halving and decimal reading and printing; dynamic programming converts the numbers to machine words once, as its target is at most $2^{63} - 1$. The output and the results file are the same in every arithmetic. Side effects: `--t-dp` and `--w-dp` are 64-bit values (`--w-dp` at most $2^{64} - 1$); the two samples of the old limits moved from `deliverable/examples/errors/` to `deliverable/examples/native_limits/`. First measurement (development laptop, meet in the middle, $n = 40$, 25-digit numbers, one run): native 0.18 s and 36 MB, Boost 0.34 s and 69 MB, own with base $2^{32}$ 0.81 s and 118 MB, base 10: 1.8 s and 282 MB, base 2: 3.3 s and 774 MB.
- [x] **No limit on the number of sets and of numbers in a set** (decided Nov 19, 2026; with the entry above this closes the request of Homenda, recorded Nov 18, 2026, that the input is not limited). `kMaxSets` ($10^4$) and `kMaxElements` ($10^6$) are removed from `input`; the only error left for the first line is a count that is 0 or does not fit in `std::size_t`, since no file can have that many lines. Rationale: no algorithm needs the limits; Karmarkar-Karp is $O(n \log n)$ and the exact algorithms are guarded by $N_{MITM}$, $T_{DP}$, $W_{DP}$ and the memory check. Tests read 10001 sets and one set of 1000001 numbers; a set of $2 \cdot 10^6$ numbers of 25 digits takes 4 s with `boost`, and 12000 small sets 0.1 s. Not done: $N_{MITM}$ and $T_{DP}$ were tuned for 16-byte numbers, while a number of Boost takes 32 bytes, so meet in the middle needs about twice the memory at the same $n$ (see the open entry on the ranges).
- [ ] **Report fixes for Stage 4** (list as of Nov 18, 2026; the work bound $W_{DP}$ was added to the report on that day): Section 1.4 states 128-bit numbers with the limits $10^{38}$ and $2^{128}$ and counts an arithmetic operation as $O(1)$, while the program now reads numbers of any size, for which an operation costs the length of the number; state the rule that chooses between the two exact algorithms (the code uses $T/32 < 2^{n/2}$, Proposition 4.4 has $T/64$ for the bitset alone) once it is tuned; the hard limits of the code (brute force $n \le 64$, `--n-mitm` at most 64, `--t-dp` at most $2^{63} - 1$) and the memory check belong to the technical description; with the answer NO the results file shows the difference of Karmarkar-Karp, which need not be the smallest.

## 7. Input constraints (our own, set Oct 28, 2026)

The instructors gave no limits beyond the two examples, so we first fixed our own (Oct 28, 2026), which followed from the 128-bit representation. Homenda has since answered that the input should not be limited (section 6, recorded Nov 18, 2026), so the table holds only with `--arithmetic native`, except for the rows on the ranges of the exact algorithms; there is no limit on $n$ or $N$ since Nov 19, 2026.

| Quantity | Constraint | Reason |
|---|---|---|
| Value of an element $a_i$ | non-negative integer of any size, leading zeros allowed; with `--arithmetic native` $0 \le a_i < 10^{38}$ (at most 38 decimal digits) | the long arithmetics have no limit; $10^{38}$ fits in `unsigned __int128` with headroom; the examples use at most 25 digits |
| Total sum of one set $S = \sum a_i$ | no limit; with `--arithmetic native` $S < 2^{128}$, checked at runtime with overflow detection, larger totals are rejected | every intermediate sum in every algorithm is bounded by $S$ |
| Number of elements per set $n$ | $n \ge 2$, no upper limit; the approximation algorithm runs for every $n$; meet in the middle is run only for $n \le 44$, otherwise the program reports that the exact method is skipped; brute force is a test reference only and is run for $n \le 30$ in the tests | memory $O(2^{n/2})$ of meet in the middle (about 100 MB per half at $n = 44$, 25 GB at $n = 60$); time $O(2^n)$ of brute force |
| Sides of the partition | both non-empty (hence $n \ge 2$; a line with one number is an input error) | the assignment asks for *two* subsets; see section 6 |
| Dynamic programming over sums | run only when $S/2 \le 3 \cdot 10^8$ (about 1.2 GB: the bitset and the first-reach table of 4 bytes per sum) and $n \cdot S/2 \le 10^{12}$, otherwise skipped with a message | table size is $O(S)$ words, time $O(n \cdot S/64 + S)$ |
| Number of sets in a file $N$ | $N \ge 1$, no upper limit (a count that does not fit in `std::size_t` is an error) | sets are processed independently, one after another |
| Format | decimal digits only, single spaces, Unix or Windows line endings, optional trailing newline; extra spaces and tabs between the numbers and at the ends of a line are accepted (section 6) | the examples |

Anything outside these bounds is rejected with a clear error message and a non-zero exit code (CONSTITUTION point 3). The thresholds for the exact algorithms are configurable on the command line so that the Stage 4 experiments can push past them deliberately.

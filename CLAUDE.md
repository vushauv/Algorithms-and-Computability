# CLAUDE.md

University lab project (Algorithms and Computability, fall 2026): the **partition problem**, in a team of up to 4.

@CONSTITUTION.md
@task.md

`raw_regulations/` holds the original course material (website, instructor emails, input examples). Cite it, do not edit it.

## Where we are

**Stage 3 (implementation and sample inputs), due Nov 20, 2026, with a live demo on lab machines that have no compiler.** We are **writing the code** of the algorithms specified in the Stage 2 report, from scratch from the report, in three PRs into `dev` (plan and steps in `task.md` section 3). The PRs are stacked: PR 1 (steps 1 to 4: build, `number`, `input`, `output`/`cli`/`main`) on `feature/stage3-reading`, PR 2 (steps 5 to 8, the algorithms) on `feature/stage3-algorithms`, PR 3 (steps 9 and 10: `SOLVE`, `--algorithm`, `deliverable/examples/`, `deliverable/partition.exe`) on `feature/stage3-program`. `make deliverable` rebuilds the committed Windows executable in `deliverable/` (with `readme.txt` and `examples/`, the folder handed over for the demo); rebuild and commit it after any code change before the demo. Build with `make`, C++20; decisions, including the code layout and the command line, are in `task.md` section 6.

Stage 2 is finished: the report and its PDF are in `docs/stage2/` on `dev`.

### Rules for the code

- **The Stage 2 report is the specification.** Each algorithm follows its pseudocode (§2.2 brute force, §3.2 meet in the middle, §4.3 dynamic programming, §5.2 Karmarkar-Karp, §6.2 `SOLVE`); names of functions and variables follow the pseudocode where reasonable. If the code needs to deviate, raise it with the user instead of silently diverging (CONSTITUTION point 6).
- **Reuse, don't duplicate:** one number module (the three types of numbers, parser, printer, checked addition, parity, halving), one input module, one module with the shared preliminary checks ($S$, parity, $S = 0$, $T$); the exact algorithms take a read-only span of values so tests run them interchangeably.
- **Every module that handles numbers is a template over their type `V`** (`Native`, `BoostInt`, `BigUint` in `src/number.hpp`), with the bodies in the `.cpp` files and explicit instantiations through `FOR_EACH_NUMBER_TYPE`. Generic code uses only `+`, `-`, comparisons and the functions of `number.hpp` (`is_odd`, `half`, `to_u64`, `checked_add`, `to_string`, `parse_value<V>`), never `%`, `/`, shifts or casts on a `V`. Callers name the type (`solve<V>(...)`) when they pass a vector, since a span parameter does not deduce it. `third_party/boost/` is a copy of Boost; do not edit it.
- **Output is nice and coloured on Linux and Windows**, colours optionally disabled: results, warnings and errors in distinct colours; on Windows enable ANSI in the console at startup (`SetConsoleMode` + `ENABLE_VIRTUAL_TERMINAL_PROCESSING`) and fall back to plain text; plain mode with `--no-color`, with `NO_COLOR` set, or when stdout is not a terminal; the same text in both modes. All of this lives in the `output` module, the algorithms never print.
- **Results are also saved to disk in Homenda's format** (`raw_regulations/email_27.10.26.txt`, corrected on 28.10 and 29.10, example `AaClab_output.txt`): `FILE_output.txt` next to the input, built only by the `result_file` module. The format is fixed by Homenda; match the example exactly, it is tested byte for byte. The command line takes `FILE [K]` with $K = 2$ only.
- Brute force is the test reference only, never called by `SOLVE`.
- Tests compare every exact algorithm with brute force on random small inputs, and check that Karmarkar-Karp returns a valid partition whose discrepancy is $d$.

Decided (details in `task.md` section 6):
- **Version: basic.** Partition a multiset into **2 subsets with equal sums**. The advanced version (k subsets) is out of scope; do not mention it in documents.
- **Numbers: any size, three arithmetics** (Nov 18, 2026): `--arithmetic boost` (default; Boost `cpp_int`, vendored in `third_party/`), `own` (`BigUint` with `--base B`, 2 to $2^{32}$) and `native` (`unsigned __int128`, values below $10^{38}$, sums below $2^{128}$). The output is the same in all three; Stage 4 compares their running times. Details in `task.md` section 6.
- **Language: C++20, built with `make`** (no CMake); prebuilt static Windows executable (MinGW-w64, `make windows`) for the demo.
- **Final algorithm:** the program always runs Karmarkar-Karp and adds an exact algorithm in range (dynamic programming if $T \le T_{DP}$, meet in the middle if $n \le N_{MITM}$). Dynamic programming also requires $n \cdot T \le W_{DP}$ (`--w-dp`, added Nov 5, 2026; in the report since Nov 18, 2026: Sections 1.4, 4.7, 6.1, 6.2 and Theorem 6.1). When both are in range, the report says only that the cheaper one for the input is chosen (Section 4.7 gives the estimates); the exact rule is fixed with the parameters after the experiments. The report has no complexity theorem for the final algorithm (removed Oct 30, 2026). **Brute force is not used by the final algorithm**; it is the baseline in the report and the test reference in the code (no $N_{BF}$ parameter, dropped Oct 30, 2026). The report describes only the Gray-code brute force; the direct enumeration is mentioned in one sentence.
- **Every exact algorithm returns a balanced partition** when one exists, or NO (since Nov 9, 2026; before, they decided only): brute force from its mask, meet in the middle by `FIND-SUBSET` on each half after the sweep, dynamic programming from a first-reach table. Karmarkar-Karp remains the approximation: it returns a partition for every input, with its discrepancy $d$. The Stage 2 report was updated to match on Nov 18, 2026 (PR #28; `task.md` section 6).
- **References** (Section 7): six entries (Graham, Korf and Schroeppel-Shamir dropped with their remarks on Oct 30, 2026), each verified against the publisher's record on Oct 30, 2026 and given a DOI/ISBN/URL. Karmarkar-Karp is cited as UCB/CSD-83-113, 1983 (Berkeley's catalogue; much of the literature says 1982).

- **Mode: individual** (recorded Nov 18, 2026).
- **Input reader:** extra spaces and tabs between the numbers and at the ends of a line are accepted (Nov 18, 2026).

Still open: **the rest of unlimited input**. The size of a number is unlimited; the limits $n \le 10^6$ and $N \le 10^4$ of `task.md` section 7 still hold and are to be revisited, and $N_{MITM}$ is still tuned for 16-byte numbers.

The report is split into one file per section in `docs/stage2/`, numbered in reading order (`00_title.md` with the title and the abstract, `01_1_introduction.md`, ..., `07_references.md`). Headings: `##` section, `###` subsection (both numbered in the text), `####` unnumbered subsubsection; the table of contents goes down to the subsections. `docs/stage2/stage2_report.md`, `.tex` and `.pdf` are generated by `make` in `docs/stage2/` (pandoc + XeLaTeX; preamble and Lua filter in `docs/stage2/tex/`, commands also in `README.md`); regenerate them after editing a section, never edit them directly. Check the PDF visually after a rebuild (`pdftoppm`), pandoc defaults are not good enough.

**All sections (1 to 7) are reviewed and frozen. Do not edit them unless asked.** If the implementation shows that the report is wrong, tell the user; fixes go in Stage 4.

### Review workflow (since Oct 29, 2026, for documents and code)

- `dev` is the integration branch; `main` holds submitted material. The Stage 2 report lives on `dev`; Stage 3 code goes to `dev` the same way.
- Each change gets its own branch and PR into `dev`. Keep a PR to one concern; one commit per section file.
- `dev` has a ruleset requiring an approving review. The owner merges with `gh pr merge --merge --admin` when the user says so; do not use `--admin` unprompted.
- The user marks the state of a reviewed section with a local tag such as `reviewed-1.1` and uses `git diff reviewed-1.1 -- docs/stage2/<file>` to see what changed since. Do not move or push these tags yourself.
- Commit and push only when the user asks.
- Old branches: `feature/algorithm-A`, `-B`, `-D`, `-c-dp` belong to teammates, are merged into `dev`, and PRs #5, #6, #7 on them are stale; leave them to their owners.

## Documents

The final documents are written in LaTeX and submitted as PDF. In this repo we draft them in Markdown and then transform them to LaTeX. So write Markdown that converts cleanly: math in `$...$` and `$$...$$`, pseudocode in fenced code blocks, no raw HTML.

### Style of the Stage 2 document

The files in `docs/stage2/` form a **mathematical report in the style of a paper**, not project notes.

- Formal mathematical language: definitions, lemmas, theorems and proofs, with algorithms as pseudocode and complexity bounds stated as theorems.
- A readable, coherent story with a clear order: motivation, problem, hardness, the algorithms, how they compare, and how they combine into the final one. The reader should want to read on; each section says why the next one is needed.
- The text never refers to project internals: no `CONSTITUTION.md`, `task.md`, `CLAUDE.md`, no "our team decided", no drafting notes, checklists or TODOs. Assumptions and limits are stated as part of the mathematical setting (for example "we assume $n \ge 2$ and $0 \le a_i < 10^{38}$"), with their reasons given in mathematical terms. Cross-references point only to sections of the report itself or to cited literature.

## How to work here

- This is the implementation phase: write code when asked, in small reviewable steps (one module or algorithm per PR into `dev`). Don't change the report in `docs/stage2/` unless asked.
- The Stage 3 deliverable is a program that builds on Linux and Windows, a prebuilt static Windows executable for the demo, and sample inputs.
- Any exact exponential algorithm needs a polynomial-time approximation next to it.
- Inputs are multisets with numbers of about 25 digits and possibly much longer, so handle duplicates and respect the input constraints in `task.md` section 7 (the 128-bit limits only with `--arithmetic native`).
- Cite sources with a short justification. Don't invent references; say so if one can't be verified.
- When a decision is made, update `task.md` section 6, not just the chat.

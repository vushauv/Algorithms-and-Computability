# CLAUDE.md

University lab project (Algorithms and Computability, fall 2026): the **partition problem**, in a team of up to 4.

@CONSTITUTION.md
@task.md

`raw_regulations/` holds the original course material (website, instructor emails, input examples). Cite it, do not edit it.

## Where we are

**Stage 2 (documentation of algorithms), due Oct 30, 2026.** No code yet. The Stage 2 report is **content-complete and reviewed** on `dev`; what remains is the conversion to LaTeX/PDF and the submission email.

Decided (details in `task.md` section 6):
- **Version: basic.** Partition a multiset into **2 subsets with equal sums**. The advanced version (k subsets) is out of scope; do not mention it in documents.
- **Numbers: unsigned 128-bit integers** (`unsigned __int128`), with input constraints in `task.md` section 7.
- **Language: C++**, prebuilt static Windows executable for the demo.
- **Final algorithm:** the program always runs Karmarkar-Karp and adds the cheapest exact algorithm in range (dynamic programming if $T \le T_{DP}$, meet in the middle if $n \le N_{MITM}$, chosen by the cost estimate of Proposition 4.4). **Brute force is not used by the final algorithm**; it is the baseline in the report and the test reference in the code (no $N_{BF}$ parameter, dropped Oct 30, 2026).
- **Exact algorithms decide only** (YES/NO); Karmarkar-Karp is the only algorithm that constructs a partition.
- **References** (Section 7): nine entries, each verified against the publisher's record on Oct 30, 2026 and given a DOI/ISBN/URL. Karmarkar-Karp is cited as UCB/CSD-83-113, 1983 (Berkeley's catalogue; much of the literature says 1982).

Still open: mode (formal or individual).

The report is split into one file per section in `docs/stage2/`, numbered in reading order (`00_title.md`, `01_1_introduction.md`, ..., `07_references.md`). `docs/stage2/stage2_report.md` is the concatenation (`cat docs/stage2/[0-9]*.md > docs/stage2/stage2_report.md`); regenerate it after editing a section, never edit it directly.

**All sections (1 to 7) are reviewed and frozen. Do not edit them unless asked.**

### Review workflow (since Oct 29, 2026)

- `dev` is the integration branch; `main` holds submitted material. The Stage 2 report lives on `dev`.
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

- This is a research and discussion phase: answer questions, compare options, point to sources. Don't write code or documents unless asked.
- The Stage 2 deliverable is a PDF with pseudocode, a detailed description and a complexity analysis for every algorithm. Editing quality is graded.
- Any exact exponential algorithm needs a polynomial-time approximation next to it.
- Inputs are multisets with numbers of about 25 digits, so handle duplicates and respect the 128-bit limits and input constraints in `task.md` section 7.
- Cite sources with a short justification. Don't invent references; say so if one can't be verified.
- When a decision is made, update `task.md` section 6, not just the chat.

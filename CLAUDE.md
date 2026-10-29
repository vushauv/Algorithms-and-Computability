# CLAUDE.md

University lab project (Algorithms and Computability, fall 2026): the **partition problem**, in a team of up to 4.

@CONSTITUTION.md
@task.md

`raw_regulations/` holds the original course material (website, instructor emails, input examples). Cite it, do not edit it.

## Where we are

**Stage 2 (documentation of algorithms), due Oct 30, 2026.** No code yet. We are writing the Stage 2 document.

Decided (details in `task.md` section 6):
- **Version: basic.** Partition a multiset into **2 subsets with equal sums**. The advanced version (k subsets) is out of scope; do not mention it in documents.
- **Numbers: unsigned 128-bit integers** (`unsigned __int128`), with input constraints in `task.md` section 7.
- **Language: C++**, prebuilt static Windows executable for the demo.
- **Final algorithm:** the program picks an algorithm depending on the input (small sums: dynamic programming; small n: meet in the middle; otherwise Karmarkar-Karp approximation). The individual algorithms are documented separately, then the selection algorithm on top (`docs/stage2_basic.md` section 6).

Still open: mode (formal or individual), which literature to reference.

The Stage 2 draft is `docs/stage2_basic.md`.

## Documents

The final documents are written in LaTeX and submitted as PDF. In this repo we draft them in Markdown and then transform them to LaTeX. So write Markdown that converts cleanly: math in `$...$` and `$$...$$`, pseudocode in fenced code blocks, no raw HTML.

### Style of the Stage 2 document

`docs/stage2_basic.md` is a **mathematical report in the style of a paper**, not project notes.

- Formal mathematical language: definitions, lemmas, theorems and proofs, with algorithms as pseudocode and complexity bounds stated as theorems.
- A readable, coherent story with a clear order: motivation, problem, hardness, the algorithms, how they compare, and how they combine into the final one. The reader should want to read on; each section says why the next one is needed.
- The text never refers to project internals: no `CONSTITUTION.md`, `task.md`, `CLAUDE.md`, no "our team decided", no drafting notes, checklists or TODOs. Assumptions and limits are stated as part of the mathematical setting (for example "we assume $n \ge 2$ and $0 \le a_i < 10^{38}$"), with their reasons given in mathematical terms. Cross-references point only to sections of the report itself or to cited literature.
- The existing draft still contains such references (for example "task.md section 7", "CONSTITUTION.md point 3") and notes for ourselves (sections 9 and 10 headed "not part of the Stage 2 PDF" or "to verify"). Remove or rewrite them when editing.

## How to work here

- This is a research and discussion phase: answer questions, compare options, point to sources. Don't write code or documents unless asked.
- The Stage 2 deliverable is a PDF with pseudocode, a detailed description and a complexity analysis for every algorithm. Editing quality is graded.
- Any exact exponential algorithm needs a polynomial-time approximation next to it.
- Inputs are multisets with numbers of about 25 digits, so handle duplicates and respect the 128-bit limits and input constraints in `task.md` section 7.
- Cite sources with a short justification. Don't invent references; say so if one can't be verified.
- When a decision is made, update `task.md` section 6, not just the chat.

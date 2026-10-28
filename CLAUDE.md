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

Still open: mode (formal or individual), which literature to reference.

The Stage 2 draft is `docs/stage2_basic.md`.

## Documents

The final documents are written in LaTeX and submitted as PDF. In this repo we draft them in Markdown and then transform them to LaTeX. So write Markdown that converts cleanly: math in `$...$` and `$$...$$`, pseudocode in fenced code blocks, no raw HTML.

## How to work here

- This is a research and discussion phase: answer questions, compare options, point to sources. Don't write code or documents unless asked.
- The Stage 2 deliverable is a PDF with pseudocode, a detailed description and a complexity analysis for every algorithm. Editing quality is graded.
- Any exact exponential algorithm needs a polynomial-time approximation next to it.
- Inputs are multisets with numbers of about 25 digits, so handle duplicates and respect the 128-bit limits and input constraints in `task.md` section 7.
- Cite sources with a short justification. Don't invent references; say so if one can't be verified.
- When a decision is made, update `task.md` section 6, not just the chat.

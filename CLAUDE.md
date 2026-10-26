# CLAUDE.md

University lab project (Algorithms and Computability, fall 2026): the **partition problem**, in a team of up to 4.

@CONSTITUTION.md
@task.md

`raw_regulations/` holds the original course material (website, instructor emails, input examples). Cite it, do not edit it.

## Where we are

**Stage 2 (documentation of algorithms), due Oct 30, 2026.** No code yet. We are in the **research phase**. We are deciding:
- which version to pick (basic: 2 subsets with equal sums; advanced: k subsets with sums as close as possible),
- which mode to use (formal or individual),
- how to represent huge numbers,
- which literature to reference.

Open decisions are in `task.md` section 6. Until they are settled, treat both versions as in scope.

## Documents

The final documents are written in LaTeX and submitted as PDF. In this repo we draft them in Markdown and then transform them to LaTeX. So write Markdown that converts cleanly: math in `$...$` and `$$...$$`, pseudocode in fenced code blocks, no raw HTML.

## How to work here

- This is a research and discussion phase: answer questions, compare options, point to sources. Don't write code or documents unless asked.
- The Stage 2 deliverable is a PDF with pseudocode, a detailed description and a complexity analysis for every algorithm. Editing quality is graded.
- Any exact exponential algorithm needs a polynomial-time approximation next to it.
- Inputs are multisets with numbers of 25+ digits, so handle duplicates and big integers.
- Cite sources with a short justification. Don't invent references; say so if one can't be verified.
- When a decision is made, update `task.md` section 6, not just the chat.

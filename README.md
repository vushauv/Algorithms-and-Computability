# Algorithms and Computability

Laboratory assignment, fall 2026: the partition problem (basic version, two
subsets with equal sums). See `task.md` for the assignment and
`CONSTITUTION.md` for the project rules.

Status: Stage 2 (the report in `docs/stage2/`) is finished; Stage 3 (the
program, due Nov 20, 2026) is written: for every set of the input file the
program runs the final algorithm of Section 6 of the report and prints the
answer (YES, NO or UNKNOWN) with the Karmarkar-Karp partition, and with YES a
balanced partition.

## Program

Requirements: GCC 13 or Clang 16 (C++20 and `unsigned __int128`); for the
Windows executable, MinGW-w64 (`x86_64-w64-mingw32-g++`). The one dependency,
Boost.Multiprecision (header-only), is in `third_party/` and needs no
installation.

```sh
make             # build/linux/partition
make test        # build and run the tests (from the repository root; about 35 s once built)
make test-quick  # the same without the slow checks (about 3 s); see tests/README.md
make windows     # static build/windows/partition.exe, needs no DLLs
make deliverable # the same, copied to deliverable/partition.exe
make clean
```

The same without `make`, from the repository root:

```sh
g++ -std=c++20 -O2 -Isrc -isystem third_party -DBOOST_MP_STANDALONE src/*.cpp src/algorithms/*.cpp -o partition
x86_64-w64-mingw32-g++ -std=c++20 -O2 -static -s -Isrc -isystem third_party -DBOOST_MP_STANDALONE src/*.cpp src/algorithms/*.cpp -o partition.exe
```

Run it on an input file, optionally followed by the number of subsets `K`
(only 2 is accepted):

```sh
./build/linux/partition raw_regulations/AaC_example_1.txt 2
./build/linux/partition --algorithm mitm --n-mitm 30 deliverable/examples/05_large_values.txt
./build/linux/partition --help
```

The results are printed and also saved next to the input, with `_output.txt`
in place of `.txt` (`AaC_example_1.txt` gives `AaC_example_1_output.txt`), in
Homenda's format (`raw_regulations/email_27.10.26.txt`, corrected on 28.10
and 29.10): the
line `N 2`, then for every set, after a blank line, its sum and numbers, the
two subsets with their sums (numbers largest first, the subsets in
lexicographically non-increasing order) and the difference of the two sums.
The subsets are the balanced partition when one is found, otherwise the
Karmarkar-Karp partition. These files are not committed (`.gitignore`).

Options: `--no-color`; `--n-mitm N`, `--t-dp T` and `--w-dp W`, the ranges of
meet in the middle ($n \le N$, default 44) and dynamic programming ($S/2 \le T$,
default $3 \cdot 10^8$, and $n \cdot S/2 \le W$, default $10^{12}$, which bounds its
time); `--algorithm solve|bf|mitm|dp|kk` to run one algorithm alone;
`--arithmetic boost|own|native` and `--base B`, the arithmetic of the numbers
(below). `make test` also runs `tests/cli_test.sh`, which checks the exit codes, the results file
and that the text is the same with and without colours.
Exit codes: 0 success, 1 invalid input file, 2 invalid command line, 3 an
algorithm could not be run on some set, 4 the results file could not be written.

### Arithmetic of the numbers

The numbers of the input may have any number of digits. The algorithms are
templates over the type of the numbers (`src/number.hpp`), and
`--arithmetic` chooses it, so that Stage 4 can compare the cost of one
machine operation with that of long arithmetic:

| `--arithmetic` | Type | Limits |
| --- | --- | --- |
| `boost` (default) | `cpp_int` of Boost.Multiprecision, cells of one machine word | none |
| `own` | `BigUint` (`src/big_uint.hpp`), our own long arithmetic; `--base B` sets the base of its cells, from 2 to $2^{32}$ (the default) | none |
| `native` | `unsigned __int128`, two machine instructions per addition | values below $10^{38}$, the sum of a set below $2^{128}$ |

The console output and the results file are the same in every arithmetic and
base (`tests/cli_test.sh` checks it on the sample inputs). Dynamic programming
works on machine words in all of them: its target is at most `--t-dp`.

### Demo on Windows (no compiler)

`deliverable/` is the folder to hand over: `partition.exe` (the prebuilt static
executable, `make deliverable`; it needs only the system DLLs), `readme.txt`
(the short guide Homenda asked for) and `examples/`. On the lab machine:

```bat
cd deliverable
partition.exe examples\10_homenda_example.txt 2
examples\run_examples.bat
```

The sample inputs and their expected results are described in
`deliverable/examples/README.md`.

## Stage 2 report

The report is written in Markdown, one file per section in `docs/stage2/`
(`00_title.md`, `01_1_introduction.md`, ..., `07_references.md`), and
converted to PDF with pandoc and XeLaTeX.

### Requirements

- pandoc (3.x)
- TeX Live with XeLaTeX and the packages `fontspec`, `unicode-math`,
  `geometry`, `microtype`, `parskip`, `titlesec`, `fancyvrb`, `booktabs`,
  `longtable`, `array`, `calc`, `enumitem`, `hyperref`
- the fonts TeX Gyre Pagella, TeX Gyre Pagella Math and DejaVu Sans Mono
  (all in TeX Live; on Debian/Ubuntu: `texlive-xetex texlive-fonts-extra
  fonts-dejavu`)

### Build

```sh
cd docs/stage2
make                     # builds stage2_report.pdf
make stage2_report.tex   # the LaTeX source only
make clean
```

The same without `make`, from `docs/stage2/`:

```sh
# 1. One Markdown file, sections in order, a blank line between files.
for f in [0-9]*.md; do cat "$f"; echo; done > stage2_report.md

# 2. PDF (pandoc runs XeLaTeX itself).
pandoc --from markdown-yaml_metadata_block-simple_tables-multiline_tables \
  --shift-heading-level-by=-1 --toc --toc-depth=2 --lua-filter=tex/filters.lua \
  --include-in-header=tex/preamble.tex --pdf-engine=xelatex \
  --metadata=date:"October 30, 2026" \
  --metadata=author:"Oleh Shuptar" --metadata=author:"Rafsan Rubayet" \
  --metadata=author:"Vasili Vushau" --metadata=author:"Zuzanna Wszołek" \
  -o stage2_report.pdf stage2_report.md

# 3. LaTeX source instead of the PDF: the same command with --standalone
#    and -o stage2_report.tex.
```

What the pieces do:

- `tex/preamble.tex`: page layout, fonts, framed pseudocode, table and list
  spacing, links.
- `tex/filters.lua`: drops the `---` separators between the section files,
  turns the `## Abstract` section into the abstract, starts the body on a new
  page after the table of contents, wraps pseudocode lines that would not fit
  in the frame, and writes the tables with a rule between the rows.
- `--from markdown-...`: the `---` separators must not be read as YAML
  metadata or as the rules of a simple table (the report uses pipe tables).
- `--shift-heading-level-by=-1`: the `#` title of `00_title.md` becomes the
  document title, `##` becomes `\section`, `###` becomes `\subsection` and
  `####` becomes `\subsubsection`. The sections are numbered in the text, so
  LaTeX does not number them.
- `--toc --toc-depth=2`: a table of contents down to the subsections. The
  `## Abstract` section of `00_title.md` is moved by the filter into the
  abstract of the title page, before the table of contents.

### Check the result

```sh
pdfinfo stage2_report.pdf | grep Pages
pdftoppm -r 70 -png stage2_report.pdf /tmp/page    # one PNG per page
xelatex -interaction=nonstopmode stage2_report.tex >/dev/null; \
  grep -E 'Overfull|Missing character' stage2_report.log   # should print nothing
```

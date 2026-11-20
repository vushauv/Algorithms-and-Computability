# Stage 4: benchmark framework (specification for the implementer)

What to build so that we can measure the program for Stage 4 and draw the
graphs from plain text files. The experiments themselves (why each one, what
we expect) are in `benchmark_plan.md`; this document replaces its sections 4
("How we measure") and 7 ("What has to be built").

The framework is three small pieces:

```
bench/generate.py   (Python, fixed seed)  ->  bench/inputs/*/*.txt   (not committed)
bench/bench.cpp     (C++, everything hardcoded)  ->  docs/stage4/data/*.tsv
bench/plots/*.py    (Python, later)  ->  docs/stage4/figures/*.pdf
```

## 0. What the instructor asks for

The Stage 4 email (Nov 2026, W. Homenda) asks for:

1. the (likely **average**) execution time for sets of a given size, over as
   wide a range of sizes as possible: from a few up to a dozen or several
   dozen elements for the exact algorithms; from a few up to several dozen,
   a hundred, two hundred and more for the approximation;
2. how the times were obtained, **in enough detail to repeat the
   measurements**;
3. Stages 2 and 4 combined into one document where possible.

We add a **comparison of the three arithmetics** (`native`, `boost`, `own`
with its base) on the same inputs, for numbers of different lengths and sets
of different sizes (`task.md` section 6, "Numbers of any size").

How the framework answers this:

- **Average:** each input file holds several random sets of one size; the
  time of a size is the mean over its sets.
- **Repeatable:** the inputs come from a script with a fixed seed and are in
  the course's input format, so they can also be run with `partition` or
  `partition.exe`; the result files record the commit, the compiler and the
  machine.
- **Same inputs in every arithmetic:** the bench program reads each file
  once per arithmetic.

## 1. The generator: `bench/generate.py`

A Python 3 script, standard library only. `python3 bench/generate.py`
writes all input files of section 4 to `bench/inputs/` (one directory per
experiment). Running it twice gives the same files byte for byte.

- **Seed:** one constant at the top of the script, `SEED = 20261204`. Each
  file uses its own generator `random.Random(f"{SEED}/{experiment}/{name}")`,
  so a file does not change when another experiment is added or changed.
  Use only `randrange`, `getrandbits` and `shuffle` of that object (their
  output for a given seed is the same in every Python 3 version since 3.2).
- **Format:** the course's input format: the number of sets on line 1,
  then one set per line, numbers separated by single spaces, `\n` line ends.
- **File names:** `bench/inputs/<experiment>/<name>.txt` with the parameters
  in the name, e.g. `E2/trap_n30.txt`, `A1/random_d1000.txt`,
  `E5/n32_T1e7.txt`.
- **Not committed:** `bench/inputs/` goes into `.gitignore`. Files of
  numbers with thousands of digits are large, and the script recreates them
  in seconds.

Families of sets (one function each in the script):

| Family | Parameters | Definition | Answer |
| --- | --- | --- | --- |
| `random` | $n$, $D$ | $n$ numbers of exactly $D$ digits (`randrange(10**(D-1), 10**D)`) | usually NO |
| `trap` | $n$, $D$ | $n - 1$ numbers $4 r_i$ and one $4 r + 2$, the $r_i$, $r$ random of $D$ digits | **NO** with an even sum ($T$ odd, every subset sum even): the worst case of every exact algorithm |
| `yes` | $n$, $D$ | random numbers of $D$ digits; one number of the first half is replaced so that both halves have the same sum; then shuffled | **YES** |
| `target` | $n$, $T$ | `trap` with $r_i$ in $0..\lfloor T/(2n) \rfloor$, so that $S/2 \approx T$ | **NO**; for dynamic programming |
| `small` | $n$, $M$ | $n$ numbers in $0..M$ (duplicates and zeros) | unknown |

## 2. The bench program: `bench/bench.cpp`

One C++ file (helpers in `bench/` if it grows), no command-line options:
**every parameter is a constant in the code**. `make bench` builds it as
`build/linux/bench` against the same objects as the program, and runs it.

### 2.1 The experiment table

At the top of `bench.cpp`, a table with one entry per experiment of
section 4:

```cpp
struct Experiment {
    std::string name;                       // "E2", also the directory in bench/inputs/
    Algorithm algorithm;                    // the enum of cli.hpp
    std::vector<std::string> files;         // in increasing size
    std::vector<ArithmeticRun> arithmetics; // e.g. {Native}, {Boost}, {Own, base 10}
    int reps;                               // runs of each set
};
```

Changing an experiment means changing this table and the matching list in
`generate.py`; nothing else.

### 2.2 What it does

For each experiment, for each arithmetic, for each file in order:

1. read the file with `read_instances<V>` (the program's own reader; for
   `own` call `BigUint::set_base` first, before any `BigUint` exists);
2. for each set, time the algorithm `reps` times (section 2.3) and verify
   the result (section 2.4);
3. append one row per set and run to the result file (section 3).

The algorithm is called directly: `brute_force<V>`, `meet_in_the_middle<V>`,
`dp_partition<V>`, `karmarkar_karp<V>`, `solve<V>` (with the default ranges
of `cli.hpp`). The program's `--n-mitm`, `--t-dp` and `--w-dp` limits do not
apply to the single algorithms, since we measure past them; the memory check
inside meet in the middle and dynamic programming still applies, and an
exception from it is written as a row with `status` = `out_of_memory`, which
ends that series.

Progress goes to the terminal (one line per file: experiment, arithmetic,
file, seconds), so a long run can be watched.

### 2.3 Timing

- Only the algorithm call is timed, with `std::chrono::steady_clock`.
  Reading, verification and writing are outside.
- **Fast calls:** a call on a few numbers takes microseconds, too little to
  measure once. Repeat the call $k = 1, 2, 4, \ldots$ times until the
  repetitions take at least 10 ms; the time of the run is the total divided
  by $k$. Keep the last result (and write it to a `volatile` variable) so
  that the compiler cannot drop the calls.
- **Warm-up:** one untimed call per set before the timed runs, skipped when
  that set already took over 1 s.

### 2.4 Verification

Each result is checked before its time is written, so that no time comes
from a wrong answer:

- with YES, $A$ and $B$ are non-empty, disjoint, cover all indices, and
  their sums are equal; Karmarkar-Karp: the same, with
  $d = |\mathrm{sum}(A) - \mathrm{sum}(B)|$;
- the answer equals the known one for `trap` (NO) and `yes` (YES);
- **all arithmetics agree**: the answer and $d$ of a set are kept from the
  first arithmetic and compared in the others.

A failed check stops the program with a message naming the file and the set.

### 2.5 The 5-minute rule

So that a full run stays short: **if one file in one arithmetic takes more
than 5 minutes in total, the larger files of that experiment are skipped
for that arithmetic**; the other arithmetics and experiments go on. The
slow file's rows are kept, with `status` = `over_limit`.

A single call cannot be interrupted, so the files of section 4 are chosen
so that no single call comes near 5 minutes: the last size of every series
should take at most a minute per call.

With this rule, a series that reaches the limit costs at most about 10
minutes (sizes grow geometrically, so all earlier files together take at
most as long as the last one). **A full run should take 1 to 3 hours.**
`bench` prints the total time at the end; if it is much longer, thin the
file lists or use fewer sets per file.

## 3. The result files

One file per experiment, `docs/stage4/data/<experiment>.tsv`, overwritten
by each run, committed (they are the data of the report).

Tab-separated, one header line, **one row per timed run of a set**; no
averaging in C++, the plot scripts compute means and spreads. First, lines
starting with `#`:

```
# experiment: E2
# date: 2026-11-28T14:03:11Z
# commit: 3e8124f
# compiler: 13.2.0 (from __VERSION__), flags: -std=c++20 -O2
# generator seed: 20261204
```

The commit and flags are passed by the `Makefile` (`-DBENCH_COMMIT=...`).
The machine is described by `docs/stage4/data/machine.txt`, written by
`make bench` with `lscpu`, `free -h`, `uname -a` and `g++ --version`.

Columns:

| Column | Example | Meaning |
| --- | --- | --- |
| `experiment` | `E2` | |
| `algorithm` | `mitm` | `bf`, `mitm`, `dp`, `kk`, `solve` |
| `arithmetic` | `own` | `native`, `boost`, `own` |
| `base` | `10` | base of `own`, `NA` for the others |
| `file` | `trap_n30.txt` | input file, in `bench/inputs/<experiment>/` |
| `set` | `3` | the set in the file, from 1 |
| `n` | `30` | numbers in the set |
| `digits` | `25` | digits of the largest number |
| `T` | `NA` | $S/2$ when it fits 64 bits (needed for dynamic programming), else `NA` |
| `rep` | `2` | run of the set, from 1 |
| `calls` | `1` | calls in the run (section 2.3) |
| `time_s` | `0.512` | seconds per call |
| `answer` | `NO` | `YES`, `NO`, `UNKNOWN` |
| `d` | `2` | discrepancy of Karmarkar-Karp (`kk`, `solve`), in full; else `NA` |
| `decided_by` | `NA` | `solve` only: `kk`, `odd`, `dp`, `mitm`, `none` |
| `status` | `ok` | `ok`, `over_limit`, `out_of_memory`, `unsupported` |

`unsupported` is a file that `native` cannot hold (a number of more than 38
digits or a sum of $2^{128}$ or more): `read_instances<Native>` rejects the
file, and one row with `NA` times is written for it, so the graph shows
where `native` ends.

Python reads a file with
`pandas.read_csv(path, sep="\t", comment="#")`.

## 4. The experiments

$I$ is the number of sets per file, $R$ the runs per set. "All three" means
`native`, `boost` and `own` with base $2^{32}$, on the same files.

### 4.1 Time against size (the email's request)

| Id | Algorithm | Files (family) | Sizes | Arithmetics | $I$ × $R$ |
| --- | --- | --- | --- | --- | --- |
| E1 | `bf` | `trap`, `random`, $D = 25$ | $n = 2, 3, \ldots, 12, 14, \ldots, 34$ | all three | 10 × 3 |
| E2 | `mitm` | `trap`, `random`, $D = 25$ | $n = 2, 3, \ldots, 12, 14, \ldots, 50$ | all three | 10 × 3 |
| E3a | `dp` | `target`, $n = 100$ | $T = 10^3, 10^{3.5}, \ldots, 10^9$ | all three | 5 × 3 |
| E3b | `dp` | `target`, $T = 10^7$ | $n = 2, 5, 10, 20, 50, 100, 300, 1000, 3000, 10^4, 3 \cdot 10^4, 10^5$ | all three | 5 × 3 |
| E4 | `kk` | `random`, $D = 25$ | $n = 2, 3, \ldots, 10, 15, 20, 30, 50, 75, 100, 150, 200, 300, 500, 10^3, 3 \cdot 10^3, \ldots, 10^6$ | all three | 20 × 3 |
| E5 | `dp`, `mitm` on the same files | `target` | $n = 24, 28, \ldots, 44$ × $T = 10^5, 10^6, 10^7, 10^8, 10^9$ | `boost` | 5 × 3 |
| E7 | `solve` | `random` $D = 25$; `small` $M = 1000$ | $n = 2, 5, 10, 20, 50, 100, 200, 500, 1000$ | `boost` | 20 × 3 |

### 4.2 Comparison of the arithmetics (same files)

| Id | Algorithm | Fixed | Varied | Arithmetics | $I$ × $R$ |
| --- | --- | --- | --- | --- | --- |
| A1 | `kk` | `random`, $n = 10^4$ | $D = 1, 5, 10, 19, 20, 25, 30, 36, 38, 50, 100, 300, 1000, 3000, 10^4$ | all three | 5 × 3 |
| A2 | `mitm` | `random`, $n = 30$ | the same $D$ | all three | 5 × 3 |
| A3 | `bf` | `random`, $n = 20$ | the same $D$ | all three | 5 × 3 |
| A4 | `kk`, `mitm` (as A1, A2) | $D = 25$ and $D = 1000$ | base of `own`: $2, 3, 10, 16, 2^8, 10^4, 2^{16}, 10^9, 2^{32}$ | `own`; `boost` and `native` as reference | 5 × 3 |

How the graphs look: A1 to A3 plot time against $D$ (log axes), one curve
per arithmetic; `native` ends where the numbers or their sum stop fitting
in 128 bits (rows `unsupported`; about $D = 35$ at $n = 10^4$, $D = 38$ at
$n = 20$). A4 plots time against the base, one curve per $D$, with
horizontal lines for `boost` and `native`. With E1, E2, E4 (time against
$n$ in each arithmetic) this covers both lengths: of the numbers and of the
set.

### 4.3 Quality of Karmarkar-Karp (no timing needed)

| Id | Algorithm | Files | Sizes | $I$ × $R$ |
| --- | --- | --- | --- | --- |
| E6 | `kk` | `random` $D = 3, 10, 25$; `yes` $D = 3$ | $n = 4, 6, 8, 10, 15, 20, 30, 50, 100, 200, 10^3, 10^4, 10^5$ | 50 × 1, `boost` |

Graphs from the `d` column: the median of $d$ against $n$, and for `yes`
the share of sets with $d > 0$ (sets where Karmarkar-Karp misses the
balanced partition).

## 5. Repeating the measurements

These steps go into `bench/README.md` and into the report:

```
python3 bench/generate.py      # inputs, fixed seed
make bench                     # builds and runs bench (-O2), writes docs/stage4/data/
```

- One machine for all final data, plugged in, nothing else running; it is
  described in `docs/stage4/data/machine.txt`.
- The same commit for all data; it is in every result file.
- Any single file can be rerun with the program:
  `build/linux/partition --algorithm mitm --arithmetic own --base 10 bench/inputs/E2/trap_n30.txt`.

## 6. Order of work (suggested PRs into `dev`)

1. `bench/generate.py`, `.gitignore` for `bench/inputs/`.
2. `bench/bench.cpp`, `make bench`, `bench/README.md`; a quick check with
   a few small files before the full run.
3. A full run on the chosen machine; `docs/stage4/data/` committed.

The plot scripts (Python, pandas and matplotlib, `bench/plots/`) come after,
from the committed data.

## 7. Open points

- The machine for the final run (chosen later; `machine.txt` records it).
- Peak memory is not measured: one process runs everything, and its peak
  memory is a high-water mark over all files. The graphs compare with the
  report's memory formulas instead. If measured memory is wanted for meet
  in the middle and dynamic programming, run a few files with
  `/usr/bin/time -v build/linux/partition --algorithm ...`.

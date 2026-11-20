# Stage 4: benchmark framework (specification for the implementer)

What to build so that we can measure the program for Stage 4 and later draw
the graphs from plain text files. This document says **what** to build and
**what the files look like**; the experiments themselves (why each one, what
we expect) are in `benchmark_plan.md`, whose sections 4 ("How we measure")
and 7 ("What has to be built") this document replaces.

## 0. What the instructor asks for

The Stage 4 email (Nov 2026, W. Homenda) asks for a document with:

1. **the time performance of the algorithms**: the (likely *average*)
   execution time for sets of a given size, over as wide a range of sizes as
   possible: *from a few up to a dozen or several dozen* elements for the
   exact algorithms, *from a few up to several dozen, a hundred, two hundred,
   etc.* for the approximation;
2. **how these times were obtained, in enough detail that someone else can
   repeat the measurements and get the same results**;
3. where possible, Stages 2 and 4 combined into one document.

Not in the email but required by us: **a comparison of the three
arithmetics** (`native`, `boost`, `own` with its base) on **the same
inputs**, for numbers of different lengths and sets of different sizes.
Since Nov 18, 2026 the program has three number types (`task.md` section 6),
and the comparison is the reason the `own` arithmetic with a free base exists.

Consequences for the framework:

| Requirement | What the framework does |
| --- | --- |
| average time | many different random sets per size (not one), each timed several times; mean, standard deviation, median, min, max per size |
| sizes from "a few" | every series starts at $n = 2$ (or the smallest valid size), so the inner-loop timer of section 4 is needed: a call on 4 numbers takes microseconds |
| as wide as possible | every series grows until the 5-minute limit (section 4.6) or the memory stops it, not at a fixed end |
| replicable | fixed seeds, a generator whose output does not depend on the compiler, the inputs can be written to files, every result file records the machine, compiler, commit and command line |
| same inputs in all arithmetics | the generator produces decimal strings, independent of the arithmetic; each arithmetic parses the same strings |
| graphs later | one tab-separated text file per experiment, one header line, one row per measurement, no aggregation that cannot be redone |

## 1. Overview

```
            bench/run.sh  (driver, POSIX sh)
                 |  loops over experiments and their grids,
                 |  one process per point, stops a series at the 5-minute limit
                 v
        build/linux/bench  (C++20, links the same objects as the program)
           generate set(s) as decimal strings  ->  parse into V
           time the algorithm call only (steady_clock)
           check the result, read peak memory
                 |
                 v
   docs/stage4/data/<experiment>.tsv          one row per point (summary)
   docs/stage4/data/raw/<experiment>.tsv      one row per timed run
   docs/stage4/data/environment.txt           machine, compiler, commit
                 |
                 v
        plots in Python (later, not part of this task): read the .tsv files
```

Why a separate binary and not timing `partition` from outside: the external
timer also counts reading the file, writing the results file and process
start, and has 10 ms resolution, so most small points read 0.00 s
(`benchmark_plan.md` section 6). The bench binary calls the algorithm
functions directly, the same templates the program uses, so what is measured
is exactly the code of `src/algorithms/` and `src/solve.cpp`. The program
`partition` and `deliverable/partition.exe` are **not changed** (no `--time`
option is needed any more).

## 2. Files to add

| Path | Content |
| --- | --- |
| `bench/bench.cpp` | `main`: options, one point (section 3), output rows |
| `bench/generate.hpp`, `.cpp` | the families of inputs (section 5), as decimal strings |
| `bench/measure.hpp`, `.cpp` | the timer with the inner loop (section 4), peak memory |
| `bench/verify.hpp`, `.cpp` | checks of each result (section 4.4) |
| `bench/table.hpp`, `.cpp` | writing the `.tsv` rows and the metadata lines (section 6) |
| `bench/run.sh` | the driver: all experiments of section 7, `quick` and `full` modes |
| `bench/environment.sh` | writes `docs/stage4/data/environment.txt` |
| `bench/check_arithmetics.sh` | checks that all arithmetics gave the same answers on the same inputs (section 8) |
| `bench/README.md` | how to build, run and repeat; the time a full run takes |
| `Makefile` | targets `bench` (build `build/linux/bench`), `bench-quick` (smoke run, under 2 minutes), `bench-full` (everything) |
| `docs/stage4/data/` | the results, committed (they are the data of the report) |

Rules of the repository apply (CLAUDE.md, CONSTITUTION): C++20, `make`, the
modules templated over `V` with `FOR_EACH_NUMBER_TYPE`; generic code uses
only `+`, `-`, comparisons and the functions of `number.hpp`. The generator
may use `BoostInt` internally for its arithmetic (multiplying by 4 etc.),
since it only produces strings. `bench` is Linux-only for memory; it should
still compile with MinGW, with the memory columns `NA`.

## 3. The bench binary: one point per process

One call of `bench` measures **one point**: one algorithm, one arithmetic,
one family, one size. One process per point, because peak memory is a
high-water mark of the whole process and because `BigUint::set_base` must
be called before any `BigUint` exists.

```
bench --experiment ID --algorithm bf|mitm|dp|kk|solve
      --arithmetic native|boost|own [--base B]
      --family random|trap|yes|target|small
      --n N [--digits D | --target T | --max-value M]
      [--instances I] [--reps R] [--seed S]
      [--min-time SECONDS] [--limit SECONDS]
      [--n-mitm N --t-dp T --w-dp W]            (solve only, as in partition)
      [--dump-inputs DIR]
      --out docs/stage4/data/ID.tsv --raw docs/stage4/data/raw/ID.tsv
```

| Option | Default | Meaning |
| --- | --- | --- |
| `--instances I` | 10 | different random sets of this size (the average is over them) |
| `--reps R` | 3 | timed runs of each set; the median of the reps is the time of that set |
| `--seed S` | 20261204 | master seed; the seed of each set is derived from it (section 5) |
| `--min-time` | 0.05 | the inner loop repeats a fast call until it has run this long (section 4.2) |
| `--limit` | 300 | the 5-minute rule of section 4.6: the point stops when its total time would pass this, exit code 10 (the driver stops the series) |
| `--dump-inputs DIR` | off | also write each set to `DIR/ID_family_n_D_i.txt` in the course's input format, so it can be run with `partition` or `partition.exe` |

Exit codes: 0 point measured; 10 stopped by the 5-minute limit (section 4.6); 11 not
measured: out of memory (`require_memory` threw, `std::bad_alloc`) or a hard
limit of the algorithm (brute force $n > 64$, ...); 12 not measured: the
arithmetic cannot hold the input (`native` with more than 38 digits, or
$S \ge 2^{128}$); 13 a result failed verification (a bug, must never
happen); 2 bad options. In cases 11 and 12 a row is still written, with the
status in the `status` column and `NA` in the times, so the graphs show where
a series ends and why.

The algorithm is called directly: `brute_force<V>`, `meet_in_the_middle<V>`,
`dp_partition<V>`, `karmarkar_karp<V>`, `solve<V>`. No `--n-mitm` / `--t-dp`
limit applies to the single algorithms (they are what we measure past); the
memory check inside meet in the middle and dynamic programming still
applies, and is reported as status `out_of_memory`.

## 4. Measuring

### 4.1 What is timed

Only the algorithm call. Generation, parsing into `V`, verification and
writing are outside the timed region. The input is a `std::vector<V>` built
before the timer starts and passed as a `std::span<const V>`.

Clock: `std::chrono::steady_clock`, the result in seconds as a `double`,
printed with 9 significant digits.

### 4.2 Inner loop for fast calls

A Karmarkar-Karp call on 10 numbers takes about a microsecond, below what a
single clock reading can measure. So each timed run is:

```
k = 1
loop:
    t0 = now(); repeat k times: result = algorithm(a); t1 = now()
    if t1 - t0 >= min_time: time = (t1 - t0) / k; stop
    k = 2 * k
```

The rows record `k` (column `calls`). The result of the last call is kept
and verified, and is written to a `volatile` sink so that the compiler cannot
drop the calls. The first, untimed call of each set (warm-up: page faults of
the allocations, caches) is not recorded; for a call that takes more than
1 s the warm-up is skipped (it would only double the run time).

### 4.3 Repetitions and the average

For one point: `I` sets, each run `R` times. The time of a set is the
median of its `R` runs (removes outliers caused by the machine); the time of
the point is the **mean** over the `I` sets (the average case the email asks
for), with the standard deviation, median, min and max over the sets next to
it. The raw file keeps every single run, so any other statistic can be
computed later.

A slow point measures fewer sets: see the 5-minute rule (section 4.6). The
actual number of sets measured is in the column `instances`.

### 4.4 Verification of every result

Not timed, but always done, so that a timing can never come from a wrong
answer:

- exact algorithms (`bf`, `mitm`, `dp`): if YES, $A$ and $B$ are disjoint,
  non-empty, cover $0..n-1$, and $\mathrm{sum}(A) = \mathrm{sum}(B)$; for the
  families with a known answer (`trap` NO, `yes` YES) the answer equals it;
- `kk`: the same partition checks, and $d = |\mathrm{sum}(A) -
  \mathrm{sum}(B)|$;
- `solve`: the partition of Karmarkar-Karp as above; with YES the balanced
  partition as above; with NO the family is not `yes`.

A failure prints the seed and the set and exits with code 13.

### 4.5 Memory

On Linux, `getrusage(RUSAGE_SELF).ru_maxrss` (kilobytes):
`mem_base_mb` read after the input is built and before the first call,
`mem_peak_mb` read at the end of the point. Their difference is the memory
of the algorithm. Next to it, `mem_model_mb` is the model of the report for
comparison: meet in the middle $2 \cdot 2^{\lceil n/2 \rceil} \cdot
\texttt{sizeof(V)}$ bytes (a lower bound for `own` and `boost`, whose large
numbers also use the heap), dynamic programming $4T + T/8$ bytes, otherwise
`NA`. MB means $2^{20}$ bytes.

### 4.6 The 5-minute rule

**No point takes more than 5 minutes** (`--limit 300`), so that a full run
stays short. The time counted is the wall time of the whole point in `bench`:
generation, warm-up, all runs and verification.

- **In `bench`:** before each run, if the elapsed time plus the duration of
  the previous run would pass the limit, the point stops. The rows measured
  so far are written, the summary is computed from the sets completed (a set
  with fewer than `R` runs counts with the runs it has), the status is
  `over_limit`, and the exit code is 10.
- **In the driver:** each `bench` call runs under `timeout 330` (the limit
  plus 30 s of margin). A single call that runs longer, so that `bench`
  cannot stop by itself, is killed; the driver then writes the summary row
  itself, with the options of the point, `NA` in the measurements and the
  status `timeout`.
- **Either way the series stops**: the next sizes are larger and would only
  take longer. The row of the stopped point is kept, so the graph shows where
  and why the series ends.

A point with status `over_limit` and at least 3 sets is still a valid
average and is plotted; one with fewer than 3 sets, or `timeout`, is plotted
only as the end mark of its series.

## 5. Inputs

### 5.1 Reproducible generator

- Engine: `std::mt19937_64`. Its output sequence is fixed by the C++
  standard. **Do not use `std::uniform_int_distribution`**: its algorithm is
  left to the implementation, so GCC, Clang and MSVC give different numbers
  from the same seed. Reduce the 64-bit outputs yourself (a digit: rejection
  of values $\ge 10 \lfloor 2^{64}/10 \rfloor$, then `% 10`).
- Seed of a set: from the master seed, the family, $n$, $D$ (or $T$, $M$)
  and the index $i$ of the set, combined with a fixed mixing function
  (for example SplitMix64 over the fields one after another). **The
  arithmetic and the algorithm are not part of the seed**: the set number
  $i$ of a point is the same in every arithmetic and for every algorithm.
- The generator writes decimal strings. Each arithmetic parses them with
  `parse_value<V>`. So all arithmetics get exactly the same numbers.
- `--dump-inputs` writes the strings in the course's format ("1", then the
  set on one line), so any point can be repeated with the program itself.

### 5.2 Families

| Family | Parameters | Definition | Answer |
| --- | --- | --- | --- |
| `random` | $n$, $D$ | $n$ numbers of exactly $D$ digits, first digit 1 to 9, other digits uniform | usually NO; unknown in advance |
| `trap` | $n$, $D$ | $n - 1$ numbers $4 r_i$ and one $4 r + 2$, with $r_i, r$ random of $D$ digits | **NO**, with an even sum ($T$ odd, all subset sums even): the worst case of every exact algorithm |
| `yes` | $n$, $D$ | random numbers of $D$ digits, the last one of the first half replaced so that the two halves have equal sums, then shuffled (as `large_yes_instance` in `tests/random_sets.hpp`) | **YES** |
| `target` | $n$, $T$ | `trap` with $r_i$ uniform in $0..\lfloor T / (2n) \rfloor$, so that $S/2 \approx T$ | **NO**; the actual $T$ is recorded |
| `small` | $n$, $M$ | $n$ numbers uniform in $0..M$ (many duplicates and zeros) | unknown |

The digit count $D$ is the length of each number; the column `digits` in
the output always holds it (for `target` and `small`, the length of the
largest number).

## 6. Output files

### 6.1 Format

Tab-separated text, UTF-8, `\n` line ends. At the top, lines starting with
`#` hold the metadata; then one header line with the column names; then one
row per point (summary file) or per timed run (raw file). Missing values are
`NA`. Numbers in plain decimal (`0.000123`, `1.5e-06` is fine); $d$, which
can have thousands of digits, is written in full in `d` and as
$\log_{10}(d + 1)$ in `d_log10` for plotting.

Readable without any conversion by pandas
(`read_csv(path, sep='\t', comment='#')`), gnuplot (`set datafile separator
'\t'`, `#` is a comment) and spreadsheets.

The binary **appends** rows; it writes the `#` lines and the header only when
the file is new or empty. The driver deletes the files of an experiment
before running it, so a file always holds one run of one experiment.

Metadata lines (written once per file, by the first point):

```
# experiment: E2
# created: 2026-11-28T14:03:11Z
# git: 3e8124f (clean)
# compiler: g++ 13.2.0, -std=c++20 -O2
# machine: see environment.txt
# command: bench --experiment E2 --algorithm mitm ... (the first point)
```

### 6.2 Columns of the summary file `<experiment>.tsv`

| Column | Example | Meaning |
| --- | --- | --- |
| `experiment` | `E2` | id of section 7 |
| `algorithm` | `mitm` | `bf`, `mitm`, `dp`, `kk`, `solve` |
| `arithmetic` | `own` | `native`, `boost`, `own` |
| `base` | `4294967296` | base of `own`; `NA` for the others |
| `family` | `trap` | section 5.2 |
| `n` | `40` | numbers in a set |
| `digits` | `25` | $D$ |
| `T` | `NA` | target $S/2$ (mean over the sets); for `target` the actual value |
| `instances` | `10` | sets measured |
| `reps` | `3` | runs per set |
| `calls` | `1` | inner-loop calls per run (the largest over the point) |
| `time_mean_s` | `0.5512` | mean over the sets of the per-set median |
| `time_sd_s` | `0.0123` | standard deviation over the sets |
| `time_median_s`, `time_min_s`, `time_max_s` | | over the sets |
| `mem_base_mb`, `mem_peak_mb`, `mem_model_mb` | | section 4.5 |
| `yes`, `no`, `unknown` | `0 10 0` | how many sets got each answer |
| `d_median`, `d_log10_median` | | median $d$ of Karmarkar-Karp (`kk` and `solve`), else `NA` |
| `kk_missed` | `0` | `yes` family only: sets where Karmarkar-Karp had $d > 0$ |
| `decided_mitm`, `decided_dp`, `decided_kk`, `decided_odd` | | `solve` only: how many sets each method decided |
| `status` | `ok` | `ok`, `over_limit` (stopped by the 5-minute rule), `timeout` (killed by the driver), `out_of_memory`, `too_large` (algorithm limit), `unsupported` (arithmetic) |

### 6.3 Columns of the raw file `raw/<experiment>.tsv`

One row per timed run: `experiment algorithm arithmetic base family n
digits T instance seed rep calls time_s answer d d_log10 status`, where
`instance` is $i$ and `seed` the seed of that set (so the set can be
generated again or dumped alone).

### 6.4 `environment.txt`

Written by `bench/environment.sh` at the start of every driver run: date
(UTC), `git rev-parse HEAD` and whether the tree is dirty, `uname -a`, CPU
model and number of cores (`/proc/cpuinfo` or `lscpu`), total memory
(`/proc/meminfo`), CPU frequency governor if readable, compiler version
(`g++ --version`), the `CXXFLAGS` of the `Makefile`, the driver's mode and
arguments, and the total wall time of the run (appended at the end).

## 7. Experiments run by the driver

`bench/run.sh full` runs them all; `bench/run.sh full E2 A2` only those
listed; `bench/run.sh quick` runs every experiment on a few small points
only (instances 2, reps 1, `--limit 5`), as a smoke test of the whole chain.
`bench/run.sh full --limit S` lowers the limit for a shorter trial run.

A **series** is the list of sizes of one line of the table, for one
arithmetic. The driver runs its points in increasing order and stops the
series at the first exit code 10, 11 or 12, or a `timeout` (the row of that
point is kept). The limit is 5 minutes per point (section 4.6).

"All three" below means `native`, `boost` and `own` with base $2^{32}$, each
on the same sets (section 5.1).

### 7.1 Time against size (the email's request)

| Id | Algorithm | Family, numbers | Sizes | Arithmetics | I × R |
| --- | --- | --- | --- | --- | --- |
| E1 | `bf` | `trap` and `random`, $D = 25$ | $n = 2, 3, \ldots, 12$, then $14, 16, \ldots$ until the limit (about 36) | all three | 10 × 3 |
| E2 | `mitm` | `trap` and `random`, $D = 25$ | $n = 2, 3, \ldots, 12$, then $14, 16, \ldots$ until the limit or memory (about 50) | all three | 10 × 3 |
| E3a | `dp` | `target`, $n = 100$ | $T = 10^3, 10^{3.5}, \ldots$ up to $10^9$ or memory | all three | 5 × 3 |
| E3b | `dp` | `target`, $T = 10^7$ | $n = 2, 5, 10, 20, 50, 10^2, \ldots, 10^5$ (steps ×√10 above 100) | all three | 5 × 3 |
| E4 | `kk` | `random`, $D = 25$ | $n = 2, 3, \ldots, 10, 15, 20, 30, 50, 75, 100, 150, 200, 300, 500$, then $10^3$ to $10^6$ (×√10) | all three | 20 × 3 |
| E5 | `dp` and `mitm` on the same sets | `target` | grid $n = 24, 28, \ldots, 44$ × $T = 10^5, 10^{5.5}, \ldots, 10^9$ | `boost` | 5 × 3 |
| E7 | `solve` (default limits) | `random`, $D = 25$; `small` with $M = 1000$ | $n = 2, 5, 10, 20, 50, 100, 200, 500, 1000$ | `boost` | 20 × 3 |

E1, E2 and E4 give the graphs "time against $n$" asked for in the email, with
one curve per arithmetic; the email's ranges ("a few" to "several dozen"
for exact, to "two hundred" and beyond for the approximation) are inside
them.

### 7.2 Comparison of the arithmetics (same inputs)

| Id | Algorithm | Fixed | Varied | Arithmetics | I × R |
| --- | --- | --- | --- | --- | --- |
| A1 | `kk` | `random`, $n = 10^4$ | $D = 1, 2, 5, 10, 15, 19, 20, 25, 30, 36, 38, 50, 100, 200, 500, 1000, 2000, 5000, 10^4$ | all three | 10 × 3 |
| A2 | `mitm` | `random`, $n = 30$ | the same $D$ list | all three | 5 × 3 |
| A3 | `bf` | `random`, $n = 20$ | the same $D$ list | all three | 5 × 3 |
| A4 | `kk` and `mitm` (as A1, A2) | $D = 25$ and $D = 1000$ | the base of `own`: $2, 3, 10, 16, 2^8, 10^4, 2^{16}, 10^9, 2^{32}$ | `own`, with `native` (only $D = 25$) and `boost` as reference rows | 5 × 3 |

How it looks: A1 to A3 are graphs of time against the number of digits $D$
(logarithmic axes), one curve per arithmetic. `native` ends at the length
where the numbers or their sum no longer fit 128 bits: its rows there have
status `unsupported` (from about $D = 35$ for $n = 10^4$, since the sum of
$10^4$ numbers of $D$ digits must stay below $2^{128} \approx 3.4 \cdot
10^{38}$; at $n = 20$ or $30$ it ends at $D = 38$). The long arithmetics should be
flat for small $D$ (one or two cells) and grow linearly with $D$ once the
numbers span many cells. A4 is a graph of time against the base (log axis),
one curve per $D$, with horizontal lines for `boost` and `native`. Together
with E1, E2, E4 (time against $n$ for each arithmetic, at $D = 25$) this
covers both "lengths": the length of the numbers and the size of the set.

Also measured in A1 to A4: `mem_peak_mb`, since the memory of meet in the
middle depends on the size of a number in each arithmetic.

### 7.3 Quality of Karmarkar-Karp (not timing)

| Id | Algorithm | Family | Sizes | I |
| --- | --- | --- | --- | --- |
| E6 | `kk` | `random` with $D = 3, 10, 25$; `yes` with $D = 3$ | $n = 4, 6, 8, 10, 15, 20, 30, 50, 100, 200, 10^3, 10^4, 10^5$ | 50 |

`--reps 1` here; the columns used are `d_median`, `d_log10_median` and
`kk_missed`.

### 7.4 Expected duration

With the 5-minute rule, no point takes more than 5 minutes, and a series
that reaches the limit costs at most about 10 minutes in total: its sizes
grow geometrically (brute force about ×4 per step, meet in the middle ×2),
so all earlier points together take at most as long as the last one. About
a dozen series can reach the limit (E1, E2, the slow arithmetics in A1 to
A3); the others end on their grid in a few minutes. **Estimate: 2 to 3
hours for a full run.** The driver prints the time of each experiment and the
total at the end. If a full run takes much longer, thin the grids or lower
`I` and `R`; the limit stays at 5 minutes. Each experiment can be rerun
alone.

## 8. Checks of the framework itself

- `bench/check_arithmetics.sh` reads the raw files and, for every
  (experiment, algorithm, family, $n$, $D$, instance), checks that all
  arithmetics gave the same `answer` and the same `d`. It prints the
  mismatches and exits non-zero if there is one. Run by the driver at the
  end.
- `make bench-quick` runs in under 2 minutes and must pass on a clean
  checkout. It is not part of `make test` (its budget is 90 s,
  `tests/README.md`).
- One point dumped with `--dump-inputs` and run with
  `build/linux/partition --algorithm X --arithmetic Y FILE` gives the same
  answer and $d$ as the raw row.

## 9. Rules for the measuring run

These go into `bench/README.md` and into the report (the email asks for
enough detail to repeat the measurements):

- the `make` build (`-O2`), the same commit for all final data, recorded;
- one machine for all final numbers, plugged in, nothing else running, the
  CPU governor `performance` if it can be set;
- the driver runs the points one after another (no parallel points: they
  would compete for memory bandwidth and caches);
- the command that produced all data: `make bench && sh bench/run.sh full`;
- to repeat one point: the row's columns give all options of `bench`;
- the data committed in `docs/stage4/data/`, with `environment.txt`.

## 10. Graphs (later, for orientation only)

The plots will be made in **Python 3 with pandas and matplotlib**
(`pd.read_csv(path, sep='\t', comment='#')`). They are not part of this task
and not part of the program (which stays C++, CONSTITUTION point 1); they
are listed so that the files have what the graphs need.

| Graph | x | y | Curves | File |
| --- | --- | --- | --- | --- |
| time of the exact algorithms | `n` | `time_mean_s` (log), error bars `time_sd_s` | algorithm × arithmetic | `E1.tsv`, `E2.tsv` |
| memory of meet in the middle | `n` | `mem_peak_mb - mem_base_mb`, `mem_model_mb` | arithmetic | `E2.tsv` |
| time of dynamic programming | `n * T` | `time_mean_s` | series | `E3a.tsv`, `E3b.tsv` |
| time of Karmarkar-Karp | `n` (log) | `time_mean_s` (log) | arithmetic | `E4.tsv` |
| faster exact algorithm | `n` | `T` (log) | which is faster | `E5.tsv` |
| time against number length | `digits` (log) | `time_mean_s` (log) | arithmetic | `A1.tsv` to `A3.tsv` |
| time against base | `base` (log) | `time_mean_s` | `digits` | `A4.tsv` |
| quality of Karmarkar-Karp | `n` (log) | `d_log10_median`; `kk_missed / instances` | `digits` | `E6.tsv` |

## 11. Order of work (suggested PRs into `dev`)

1. **Bench binary**: `generate`, `measure`, `verify`, `table`, `bench.cpp`,
   `make bench`; a unit test that the generator gives the same strings for
   the same seed and the known answers for `trap` and `yes`.
2. **Driver**: `run.sh`, `environment.sh`, `check_arithmetics.sh`,
   `bench/README.md`, `make bench-quick`, `make bench-full`.
3. **Data**: one full run on the chosen machine, `docs/stage4/data/`
   committed.

## 12. Open points

- The machine for the final run (to be chosen later; nothing in the
  framework depends on it, `environment.txt` records it).
- Whether a few points are also run on a lab machine with
  `deliverable/partition.exe` and the dumped inputs (external timing only).

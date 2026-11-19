# Sample inputs

Input files in the format of the assignment (line 1: the number of sets; each
next line: one multiset of non-negative integers separated by single spaces).
Run one with

```sh
./build/linux/partition examples/01_report_examples.txt 2     # Linux
dist\partition.exe examples\01_report_examples.txt 2           # Windows
```

or all of them on Windows with `examples\run_examples.bat`. Each run also saves
its results next to the input, for example `01_report_examples_output.txt`
(not committed).

## Valid inputs

| File | Sets | What it shows |
| --- | --- | --- |
| `01_report_examples.txt` | 4 | the examples of Section 6.3 of the report: balanced by Karmarkar-Karp, odd sum, YES that Karmarkar-Karp misses ($d = 2$), NO with an even sum |
| `02_yes.txt` | 6 | YES instances, from $n = 2$ up; the last one is YES with $d = 2$ |
| `03_no.txt` | 6 | NO instances: odd sums, $\max a_i > T$, even sums without a balanced partition |
| `04_duplicates_and_zeros.txt` | 8 | only zeros, zeros among other values, many equal values |
| `05_large_values.txt` | 7 | 25-digit values: the second example of the assignment, then YES and NO pairs with $n = 12, 24, 40$, decided by meet in the middle |
| `06_many_small_numbers.txt` | 4 | $n = 200$, $5000$ and $101$ small values, in the range of dynamic programming |
| `07_unknown.txt` | 2 | $n = 100$ and $1000$ values of 25 digits: no exact algorithm is in range, the answer is UNKNOWN with the Karmarkar-Karp partition |
| `08_limits.txt` | 3 | values $10^{38} - 1$ (the largest that `--arithmetic native` allows) and leading zeros |
| `09_windows_line_endings.txt` | 2 | CRLF line endings |
| `10_homenda_example.txt` | 2 | the input of Homenda's output example (`raw_regulations/AaClab.txt`): YES, and NO with an odd sum ($d = 1$) |
| `11_extra_spaces.txt` | 3 | spaces at the ends of the lines, two spaces and tabs between the numbers: accepted and read as single spaces |
| `12_beyond_128_bits.txt` | 6 | numbers of 39, 50 and 180 digits, which need the long arithmetic: $10^{38}$ twice; a sum above $2^{128}$; YES that Karmarkar-Karp misses, found by meet in the middle; NO with an even and with an odd sum; YES with 180 digits |

The YES sets of `05_large_values.txt` are made balanced by construction; the
others are random. The answers were checked against brute force
(`--algorithm bf`) for every set with $n \le 24$, and those of
`06_many_small_numbers.txt` against a separate dynamic programming script.

## Invalid inputs (`errors/`)

Each file has one error; the program prints it with the line and column and
exits with code 1.

| File | Error |
| --- | --- |
| `01_empty_file.txt` | the file is empty |
| `02_first_line_not_a_number.txt` | line 1 is not a number |
| `03_zero_sets.txt` | the number of sets is 0 |
| `04_too_few_lines.txt` | fewer sets than announced |
| `05_too_many_lines.txt` | more sets than announced |
| `06_empty_line.txt` | an empty line in place of a set |
| `07_one_element.txt` | a set with one number ($n \ge 2$ is required) |
| `08_negative_number.txt` | a negative number |
| `09_not_a_digit.txt` | a letter inside a number |
| `10_decimal_point.txt` | a non-integer value |
| `11_first_line_two_numbers.txt` | more than one number on line 1 |

## Limits of the native arithmetic (`native_limits/`)

Valid inputs for the default arithmetic and for `--arithmetic own`, which
have no limit on the size of a number. With `--arithmetic native` (128-bit
integers) each one is an error with exit code 1.

| File | Error with `--arithmetic native` |
| --- | --- |
| `01_value_too_large.txt` | a value of $10^{38}$ |
| `02_sum_too_large.txt` | the sum of a set reaches $2^{128}$ |

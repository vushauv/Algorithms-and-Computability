# Tests

| Command | What it runs | Time once built |
| --- | --- | --- |
| `make test` | all unit tests (`run_tests`) and all program checks (`cli_test.sh`) | about 37 s on the development laptop |
| `make test-quick` | the same without the slow checks | about 5 s |

**Warning: keep `make test` under 90 s and `make test-quick` under 15 s.**
The times above are for a build that is already done. A clean build takes
about 110 s of compiler time (about 40 s with `make -j8`), mostly Boost. A
test that pushes either run over its limit must be made smaller or moved
into the heavy group.

What the quick run leaves out (`tests/main.cpp`, `tests/cli_test.sh`):

- the unit test `scale` (numbers of thousands of digits, sets of $10^5$
  numbers, files of thousands of sets, in every arithmetic);
- in `cli_test.sh`, the samples `05_large_values.txt` and
  `14_beyond_128_bits.txt` in the comparison of the arithmetics (only
  `native` and `own --base 10` are compared on the others), most of the
  terminal runs, and the generated files of the scale test.

Everything else runs in both: the algorithms against brute force, the three
arithmetics against the native one, the input errors, the exit codes and the
results file.

## Files

| File | Content |
| --- | --- |
| `test_*.cpp` | unit tests, one file per module; `main.cpp` calls them (`run_tests --times` prints the seconds of each, `run_tests quick` runs the light ones) |
| `test_scale.cpp` | size: 3000-digit numbers, $10^5$ numbers, $2 \cdot 10^4$ sets; answers known by construction (a set repeated twice is YES) |
| `cli_test.sh` | the program as a whole: exit codes, results file, the same text in every arithmetic and with or without colours |
| `generate.sh` | large input files from a fixed seed: `sets N`, `numbers N DIGITS`, `huge DIGITS`; used by `cli_test.sh` and for `deliverable/examples/15_*` and `16_*` |

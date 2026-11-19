PARTITION PROBLEM
Splits a set of numbers into two subsets with equal sums.
Oleh Shuptar, Rafsan Rubayet, Vasili Vushau, Zuzanna Wszolek

RUN (from the console; partition.exe needs no installation)

    partition.exe INPUT_FILE [K] [options]

    INPUT_FILE  line 1: the number of sets; each next line: one set of
                non-negative integers of any size, separated by single spaces
                (extra spaces and tabs are accepted)
    K           the number of subsets: 2 (the default and the only value)

    Example, from the main folder of the project:

    dist\partition.exe examples\10_homenda_example.txt 2

RESULTS

    Printed in the console and saved next to the input file, with _output.txt
    in place of .txt, here examples\10_homenda_example_output.txt.
    For every set: its sum and numbers, the two subsets with their sums, and
    the difference of the two sums (0 for an equal split). When no equal
    split is found, the subsets are the split found by Karmarkar-Karp.

ALGORITHMS (option --algorithm A, default solve)

    solve  Karmarkar-Karp, plus an exact algorithm when the set allows it
    bf     brute force (exact, at most 64 numbers)
    mitm   meet in the middle (exact, at most 44 numbers; --n-mitm N)
    dp     dynamic programming (exact, half sum at most 3*10^8; --t-dp T, --w-dp W)
    kk     Karmarkar-Karp (approximation, any set)

ARITHMETIC (option --arithmetic X, default boost)

    boost   numbers of any size, the library Boost.Multiprecision
    own     numbers of any size, our own long arithmetic; --base B sets the
            base of its cells, from 2 to 4294967296 (the default)
    native  128-bit integers of the processor: numbers below 10^38, the sum
            of a set below 2^128

    The results are the same in all three; only the running time differs.

    Other options: --no-color, --help

EXAMPLES

    examples\run_examples.bat runs all sample inputs; examples\README.md says
    what each one shows; examples\errors\ has one invalid file per error;
    examples\native_limits\ has the two files that only --arithmetic native
    rejects.

EXIT CODES

    0 success, 1 invalid input file, 2 invalid command line,
    3 an algorithm could not run on some set, 4 the results file not written

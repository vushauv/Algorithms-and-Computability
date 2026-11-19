PARTITION PROBLEM
Splits a set of numbers into two subsets with equal sums.
Oleh Shuptar, Rafsan Rubayet, Vasili Vushau, Zuzanna Wszolek

RUN (from the console, in this folder; no installation needed)

    partition.exe INPUT_FILE [K] [options]

    INPUT_FILE  line 1: the number of sets; each next line: one set of
                non-negative integers of any size, separated by spaces
    K           the number of subsets: 2 (the default and the only value)

    Example:    partition.exe examples\10_homenda_example.txt 2

    All sample inputs at once: examples\run_examples.bat
    (examples\README.md says what each file shows).

RESULTS

    Printed in the console and saved next to the input file, with
    _output.txt in place of .txt:
    examples\10_homenda_example.txt -> examples\10_homenda_example_output.txt

    For every set: its sum and numbers, the two subsets with their sums, and
    the difference of the two sums (0 for an equal split). If no equal split
    exists, the subsets are the closest split found (Karmarkar-Karp).

OPTIONS

    --algorithm A   solve (default: Karmarkar-Karp plus an exact algorithm
                    when the set allows it), bf (brute force, up to 64
                    numbers), mitm (meet in the middle), dp (dynamic
                    programming), kk (Karmarkar-Karp)
    --arithmetic X  how the program stores and adds the numbers:
                      boost   (default) library for numbers of any size
                      own     our own code for numbers of any size
                              (--base B: base of its digits, 2 to 4294967296)
                      native  the processor's 128-bit integers: fastest,
                              but numbers must be below 10^38 and the sum
                              of a set below 2^128 (else an error)
                    The results are the same; only the speed differs.
    --no-color      plain text, no colours
    --help          all options

EXIT CODES

    0 success, 1 invalid input file, 2 invalid command line,
    3 an algorithm could not run on some set, 4 results file not written

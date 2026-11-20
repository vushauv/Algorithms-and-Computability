SAMPLE INPUTS

Input files in the format of the assignment: line 1 is the number of sets;
each next line is one multiset of non-negative integers separated by single
spaces.

Run one of them from the folder above this one (where partition.exe is):

    partition.exe examples\01_report_examples.txt 2

or all of them at once with examples\run_examples.bat. Each run also saves
its results next to the input, for example
examples\01_report_examples_output.txt.


VALID INPUTS

01_report_examples.txt (4 sets)
    The examples of Section 6.3 of the report: balanced by Karmarkar-Karp,
    odd sum, YES that Karmarkar-Karp misses (d = 2), NO with an even sum.

02_yes.txt (6 sets)
    YES instances, from n = 2 up; the last one is YES with d = 2.

03_no.txt (6 sets)
    NO instances: odd sums, a number larger than half the sum, even sums
    without a balanced partition.

04_duplicates_and_zeros.txt (8 sets)
    Only zeros, zeros among other values, many equal values.

05_large_values.txt (6 sets)
    25-digit values: YES and NO pairs with n = 12, 24 and 40, decided by
    meet in the middle.

06_many_small_numbers.txt (4 sets)
    n = 200, 5000 and 101 small values, in the range of dynamic
    programming.

07_unknown.txt (2 sets)
    n = 100 and 1000 values of 25 digits: no exact algorithm is in range,
    so the answer is UNKNOWN with the Karmarkar-Karp partition.

08_limits.txt (3 sets)
    Values 10^38 - 1 (the largest that --arithmetic native allows) and
    leading zeros.

09_windows_line_endings.txt (2 sets)
    CRLF line endings.

10_course_example.txt (2 sets)
    The input of the course's example of the results file: YES, and NO
    with an odd sum (d = 1).

11_extra_spaces.txt (3 sets)
    Spaces at the ends of the lines, two spaces and tabs between the
    numbers: accepted and read as single spaces.

12_course_example_1.txt (2 sets)
    The course's first input example, unchanged: small numbers, YES, and
    NO with an odd sum.

13_course_example_2.txt (1 set)
    The course's second input example, unchanged: one set of 25-digit
    values, NO with an odd sum.

14_beyond_128_bits.txt (6 sets)
    Numbers of 39, 50 and 180 digits, which need the long arithmetic:
    10^38 twice; a sum above 2^128; YES that Karmarkar-Karp misses, found
    by meet in the middle; NO with an even and with an odd sum; YES with
    180 digits.

15_many_sets.txt (500 sets)
    Sets of 2 to 6 numbers below 1000: a long file.

16_huge_numbers.txt (1 set)
    20 numbers of 1000 digits, every one twice: YES, found by
    Karmarkar-Karp; needs the long arithmetic.

The answers were checked against brute force (--algorithm bf) for every set
with n <= 24, and those of 06_many_small_numbers.txt against a separate
dynamic programming program.


INVALID INPUTS (errors\)

Each file has one error. The program prints it with the line and the column,
writes no results file and exits with code 1.

01_empty_file.txt                the file is empty
02_first_line_not_a_number.txt   line 1 is not a number
03_zero_sets.txt                 the number of sets is 0
04_too_few_lines.txt             fewer sets than announced
05_too_many_lines.txt            more sets than announced
06_empty_line.txt                an empty line in place of a set
07_one_element.txt               a set with one number (n >= 2 is required)
08_negative_number.txt           a negative number
09_not_a_digit.txt               a letter inside a number
10_decimal_point.txt             a number that is not an integer
11_first_line_two_numbers.txt    more than one number on line 1


LIMITS OF THE NATIVE ARITHMETIC (native_limits\)

Valid inputs for the default arithmetic and for --arithmetic own, which have
no limit on the size of a number. With --arithmetic native (128-bit
integers) each one is an error with exit code 1:

    partition.exe --arithmetic native examples\native_limits\01_value_too_large.txt 2

01_value_too_large.txt           a value of 10^38
02_sum_too_large.txt             the sum of a set reaches 2^128

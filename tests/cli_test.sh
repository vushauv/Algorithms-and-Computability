#!/bin/sh
# Tests of the program as a whole (run by make test, from the repository root):
# the exit codes, the results file, and the same text with and without colours
# (CONSTITUTION points 3 and 4).
#
#   sh tests/cli_test.sh build/linux/partition
#
# The program saves a results file next to its input, so the sample inputs are
# copied to a temporary directory and run there.

PROGRAM=$(cd "$(dirname "$1")" && pwd)/$(basename "$1")
passed=0
failed=0
TMP=$(mktemp -d)
trap 'rm -rf "$TMP"' EXIT
ESC=$(printf '\033')
cp -R deliverable raw_regulations "$TMP"/
cd "$TMP" || exit 1

pass() { passed=$((passed + 1)); }
fail() { failed=$((failed + 1)); echo "cli_test: $*" >&2; }

# expect CODE ARGS...: the program exits with CODE.
expect() {
    want=$1
    shift
    "$PROGRAM" "$@" >/dev/null 2>&1
    got=$?
    if [ "$got" -eq "$want" ]; then pass; else fail "exit $got, expected $want: $*"; fi
}

# 0: valid files, help, a skipped exact algorithm (a warning, not an error), K = 2.
for f in deliverable/examples/*.txt raw_regulations/AaC_example_*.txt; do
    case $f in *_output.txt) continue ;; esac  # results left by earlier runs
    expect 0 "$f"
done
expect 0 --help
expect 0 --algorithm mitm --n-mitm 4 deliverable/examples/02_yes.txt
expect 0 --algorithm dp --w-dp 1 deliverable/examples/02_yes.txt
expect 0 deliverable/examples/02_yes.txt 2

# 1: invalid input files.
for f in deliverable/examples/errors/*.txt; do expect 1 "$f"; done
expect 1 "$TMP/no_such_file.txt"
expect 1 examples

# 2: invalid command lines.
expect 2
expect 2 --bogus deliverable/examples/02_yes.txt
expect 2 deliverable/examples/02_yes.txt deliverable/examples/03_no.txt
expect 2 --n-mitm 65 deliverable/examples/02_yes.txt
expect 2 --t-dp 9223372036854775808 deliverable/examples/02_yes.txt
expect 2 --w-dp -1 deliverable/examples/02_yes.txt
expect 2 --algorithm all deliverable/examples/02_yes.txt
expect 2 deliverable/examples/02_yes.txt 5
expect 2 deliverable/examples/02_yes.txt 2 2

# 3: brute force cannot run on 65 numbers; the next set is still answered.
printf '2\n%s\n1 1\n' "$(seq 1 65 | tr '\n' ' ' | sed 's/ $//')" > "$TMP/bf.txt"  # seq -s adds a space on macOS
expect 3 --algorithm bf "$TMP/bf.txt"
if "$PROGRAM" --algorithm bf "$TMP/bf.txt" 2>/dev/null | grep -q 'Answer: YES'; then
    pass
else
    fail "the set after a failed one is not answered"
fi
# ... and both sets are in the results file, the first one with the partition
# of Karmarkar-Karp (S = 2145 is odd, d = 1).
if [ "$(awk 'NR==1||NR==6||NR==8||NR==11{print} NR==3{print $1}' "$TMP/bf_output.txt" |
        tr '\n' ' ')" = "2 2 2145 1 2 1 1 0 " ]; then
    pass
else
    fail "the results file of a failed set"
fi

# 4: the results file cannot be written (a directory has its name).
mkdir "$TMP/blocked_output.txt"
printf '1\n1 1\n' > "$TMP/blocked.txt"
expect 4 "$TMP/blocked.txt"
# The console results come first and are all there; the error follows them.
"$PROGRAM" --no-color "$TMP/blocked.txt" > "$TMP/blocked_out" 2>&1
if grep -q 'YES' "$TMP/blocked_out" && grep -q 'cannot write the results file' "$TMP/blocked_out" \
    && ! grep -q 'Results saved' "$TMP/blocked_out"; then
    pass
else
    fail "console results of a set whose results file cannot be written"
fi
# A failed set (exit 3) and an unwritable file (4): the exit code is 4.
{ echo 1; yes 1 | head -n 65 | tr '\n' ' ' | sed 's/ $//'; echo; } > "$TMP/blocked_two.txt"
mkdir "$TMP/blocked_two_output.txt"
expect 4 --algorithm bf "$TMP/blocked_two.txt"

# The results file: _output.txt in place of .txt, in Homenda's format, no
# newline at the end; none for an invalid input file.
"$PROGRAM" deliverable/examples/01_report_examples.txt >/dev/null 2>&1
printf '4 2\n\n10 3 1 4 2\n5 4 1\n5 3 2\n0\n\n9 2 3 4\n4 4\n5 3 2\n1\n\n%s\n%s\n%s\n0\n\n%s\n%s\n%s\n2' \
    '36 5 6 7 8 10' '18 10 8' '18 7 6 5' '6 1 1 4' '4 4' '2 1 1' > "$TMP/expected.txt"
if cmp -s "$TMP/expected.txt" deliverable/examples/01_report_examples_output.txt; then
    pass
else
    fail "the results file of 01_report_examples.txt"
fi
"$PROGRAM" deliverable/examples/errors/07_one_element.txt >/dev/null 2>&1
if [ -e deliverable/examples/errors/07_one_element_output.txt ]; then
    fail "a results file for an invalid input file"
else
    pass
fi

# The work bound: n * T > W_DP and n > N_MITM gives UNKNOWN with the reason.
if "$PROGRAM" --w-dp 10 --n-mitm 2 deliverable/examples/03_no.txt | grep -q 'n \* T = 3 \* 6 > W_DP = 10'; then
    pass
else
    fail "no UNKNOWN with the reason n * T > W_DP"
fi

# Plain output: not a terminal, --no-color, NO_COLOR.
for f in deliverable/examples/01_report_examples.txt deliverable/examples/errors/07_one_element.txt; do
    if "$PROGRAM" "$f" 2>&1 | grep -q "$ESC"; then fail "colours in a pipe: $f"; else pass; fi
done

# Under a pseudo-terminal (script of util-linux) the output is coloured, and
# without the colour codes it is the text of --no-color.
if command -v script >/dev/null 2>&1; then
    for f in deliverable/examples/0*.txt deliverable/examples/errors/07_one_element.txt; do
        case $f in *_output.txt) continue ;; esac  # results of the runs above
        env -u NO_COLOR script -qec "'$PROGRAM' '$f'" /dev/null > "$TMP/tty" 2>&1
        if grep -q "$ESC" "$TMP/tty"; then pass; else fail "no colours on a terminal: $f"; fi
        sed "s/$ESC\[[0-9;]*m//g" "$TMP/tty" | tr -d '\r' > "$TMP/stripped"
        "$PROGRAM" --no-color "$f" > "$TMP/plain" 2>&1
        if cmp -s "$TMP/stripped" "$TMP/plain"; then pass; else fail "coloured and plain text differ: $f"; fi
    done
    for option in "--no-color" ""; do
        NO_COLOR=$([ -z "$option" ] && echo 1) script -qec "'$PROGRAM' $option deliverable/examples/02_yes.txt" \
            /dev/null > "$TMP/tty" 2>&1
        if grep -q "$ESC" "$TMP/tty"; then fail "colours despite ${option:-NO_COLOR}"; else pass; fi
    done
else
    echo "cli_test: script(1) not found, terminal checks skipped" >&2
fi

echo "$passed program checks passed, $failed failed"
[ "$failed" -eq 0 ]

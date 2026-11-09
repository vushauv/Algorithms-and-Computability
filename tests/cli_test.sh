#!/bin/sh
# Tests of the program as a whole (run by make test, from the repository root):
# the exit codes, and the same text with and without colours (CONSTITUTION
# points 3 and 4).
#
#   sh tests/cli_test.sh build/linux/partition

PROGRAM=$1
passed=0
failed=0
TMP=$(mktemp -d)
trap 'rm -rf "$TMP"' EXIT
ESC=$(printf '\033')

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

# 0: valid files, help, a skipped exact algorithm (a warning, not an error).
for f in examples/*.txt raw_regulations/AaC_example_*.txt; do expect 0 "$f"; done
expect 0 --help
expect 0 --algorithm mitm --n-mitm 4 examples/02_yes.txt
expect 0 --algorithm dp --w-dp 1 examples/02_yes.txt

# 1: invalid input files.
for f in examples/errors/*.txt; do expect 1 "$f"; done
expect 1 "$TMP/no_such_file.txt"
expect 1 examples

# 2: invalid command lines.
expect 2
expect 2 --bogus examples/02_yes.txt
expect 2 examples/02_yes.txt examples/03_no.txt
expect 2 --n-mitm 65 examples/02_yes.txt
expect 2 --t-dp 9223372036854775808 examples/02_yes.txt
expect 2 --w-dp -1 examples/02_yes.txt
expect 2 --algorithm all examples/02_yes.txt

# 3: brute force cannot run on 65 numbers; the next set is still answered.
printf '2\n%s\n1 1\n' "$(seq -s ' ' 1 65)" > "$TMP/bf.txt"
expect 3 --algorithm bf "$TMP/bf.txt"
if "$PROGRAM" --algorithm bf "$TMP/bf.txt" 2>/dev/null | grep -q 'Answer: YES'; then
    pass
else
    fail "the set after a failed one is not answered"
fi

# The work bound: n * T > W_DP and n > N_MITM gives UNKNOWN with the reason.
if "$PROGRAM" --w-dp 10 --n-mitm 2 examples/03_no.txt | grep -q 'n \* T = 3 \* 6 > W_DP = 10'; then
    pass
else
    fail "no UNKNOWN with the reason n * T > W_DP"
fi

# Plain output: not a terminal, --no-color, NO_COLOR.
for f in examples/01_report_examples.txt examples/errors/08_two_spaces.txt; do
    if "$PROGRAM" "$f" 2>&1 | grep -q "$ESC"; then fail "colours in a pipe: $f"; else pass; fi
done

# Under a pseudo-terminal (script of util-linux) the output is coloured, and
# without the colour codes it is the text of --no-color.
if command -v script >/dev/null 2>&1; then
    for f in examples/0*.txt examples/errors/08_two_spaces.txt; do
        env -u NO_COLOR script -qec "'$PROGRAM' '$f'" /dev/null > "$TMP/tty" 2>&1
        if grep -q "$ESC" "$TMP/tty"; then pass; else fail "no colours on a terminal: $f"; fi
        sed "s/$ESC\[[0-9;]*m//g" "$TMP/tty" | tr -d '\r' > "$TMP/stripped"
        "$PROGRAM" --no-color "$f" > "$TMP/plain" 2>&1
        if cmp -s "$TMP/stripped" "$TMP/plain"; then pass; else fail "coloured and plain text differ: $f"; fi
    done
    for option in "--no-color" ""; do
        NO_COLOR=$([ -z "$option" ] && echo 1) script -qec "'$PROGRAM' $option examples/02_yes.txt" \
            /dev/null > "$TMP/tty" 2>&1
        if grep -q "$ESC" "$TMP/tty"; then fail "colours despite ${option:-NO_COLOR}"; else pass; fi
    done
else
    echo "cli_test: script(1) not found, terminal checks skipped" >&2
fi

echo "$passed program checks passed, $failed failed"
[ "$failed" -eq 0 ]

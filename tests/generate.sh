#!/bin/sh
# Writes a large input file to stdout, the same one for the same arguments
# (the seed is fixed), so the files need not be kept in the repository.
#
#   sh tests/generate.sh sets N              N sets of 2 to 6 numbers below 1000
#   sh tests/generate.sh numbers N DIGITS    one set of N numbers (N even) of DIGITS
#                                            digits, every number twice: YES
#   sh tests/generate.sh huge DIGITS         one set of 20 numbers of DIGITS digits,
#                                            every number twice: YES, and the
#                                            exact algorithm must find it
#
# Used by tests/cli_test.sh; the samples in deliverable/examples/ come from it.

kind=$1
shift
case $kind in
    sets) N=$1 ;;
    numbers) N=$1 DIGITS=$2 ;;
    huge) N=20 DIGITS=$1 ;;
    *) echo "usage: generate.sh sets N | numbers N DIGITS | huge DIGITS" >&2; exit 2 ;;
esac

awk -v kind="$kind" -v n="$N" -v digits="${DIGITS:-0}" '
function number(d,    s, i) {
    s = 1 + int(rand() * 9)
    for (i = 1; i < d; i++) s = s int(rand() * 10)
    return s
}
BEGIN {
    srand(20261120)
    if (kind == "sets") {
        print n
        for (k = 1; k <= n; k++) {
            m = 2 + int(rand() * 5)
            line = ""
            for (i = 0; i < m; i++) line = line (i ? " " : "") int(rand() * 1000)
            print line
        }
    } else {
        print 1
        half = n / 2
        line = ""
        for (i = 0; i < half; i++) {
            x = number(digits)
            line = line (i ? " " : "") x " " x
        }
        print line
    }
}'

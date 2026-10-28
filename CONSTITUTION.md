# Project Constitution

Principles for the partition-problem program (see `task.md`). Keep it short; change it only by team agreement.

1. **Language.** All code is C++21 (`-std=c++2b` on compilers that do not yet accept `-std=c++21`), standard library only unless a dependency is trivial to ship on Windows.
2. **Portability.** It must build and run on the Windows lab machines and on Linux. The demo machines have no compiler, so we ship a prebuilt, statically linked executable.
3. **Input format.** A text file. Line 1 is the number of sets `N`. Each of the next `N` lines is one set: non-negative integers separated by single spaces. Values may repeat (multiset) and may be large (about 25 digits in the examples). Numbers are read and stored as **signed 128-bit integers** (`__int128`, a GCC/Clang extension that is the one allowed exception to "standard library only"), behind a single type alias. Reading a value that does not fit, or a running sum that overflows, is an error with a clear message and a non-zero exit code; nothing is ever silently truncated. Examples are in `raw_regulations/AaC_example_*.txt`. Malformed input gives a clear error message and a non-zero exit code.
4. **Output.** Output is readable and colored (ANSI) by default: results, errors and warnings use distinct colors. A plain mode with no colors is mandatory, enabled by `--no-color` and also automatically when stdout is not a terminal or `NO_COLOR` is set. The text content must be identical in both modes.
5. **Build.** A `Makefile` is the main way to compile (`make`, `make clean`). The README also lists the same manual compiler commands, so the program can be built without `make`.
6. **Algorithms.** Every exact algorithm with exponential complexity comes with a polynomial-time approximation. Code, pseudocode and complexity analysis in the documents must match.
7. **Correctness first.** Every algorithm is checked against a brute-force solution on small inputs. Tests and sample inputs live in the repository.
8. **Simplicity.** Prefer clear code to clever code. Small functions, meaningful names, comments only where the reason is not obvious.

# Benchmarks

The benchmark framework uses only C++20 executables. Generate inputs and run
the measurements from the repository root:

```sh
make bench
```

To run only one experiment:

```powershell
Set-Location bench
..\build\windows\generate.exe E6
..\build\windows\bench.exe E6 *> bench-e6.log
Set-Location ..
```

Without an argument, `generate` and `bench` run all experiments. Valid
experiment names are `E1`, `E2`, `E3a`, `E3b`, `E4`, `E5`, `E6`, `E7`,
`A1`, `A2`, `A3`, and `A4`.

With GNU Make, the same scopes are available as:

```sh
mingw32-make generate-experiment EX=E6
mingw32-make bench-experiment EX=E6
mingw32-make bench-experiment-windows EX=E6
```

On Windows, build static benchmark executables first:

```sh
make bench-windows
```

Place `generate.exe` and `bench.exe` in an empty directory. `generate.exe`
writes `inputs/`, and `bench.exe` reads those files and writes `results/`.

The generator uses seed `20261204` and recreates all input files
deterministically. `bench/inputs/` is generated and is not committed.



For a repeatable final run:

1. Use one plugged-in machine and close other programs.
2. Build and run the same commit for every result.
3. Record CPU, memory, system details, compiler, and date in
   `bench/results/machine.txt`.
4. Do not edit or average generated TSV files.

TSV files are written below `bench/results/`. A line beginning with `#`
contains metadata. Plot scripts read one row per set and repetition.

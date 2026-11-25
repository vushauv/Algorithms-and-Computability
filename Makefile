# Partition problem: build of the program and the tests.
#
#   make           the program, build/linux/partition
#   make test      build and run the tests (at most 90 s once built; tests/README.md)
#   make test-quick   the tests without the slow checks (about 15 s once built)
#   make windows   static Windows executable, build/windows/partition.exe
#   make deliverable   the same, copied to deliverable/partition.exe (committed for the demo),
#                  and the sources copied to deliverable/source/ (deliverable-source)
#   make deliverable-source   only the sources: src/ and third_party/ copied to
#                  deliverable/source/, next to its own Makefile (kept by hand)
#   make clean     remove build/
#
# The README lists the same compiler commands for building without make.

CXX      = g++
MINGW    = x86_64-w64-mingw32-g++
CXXFLAGS = -std=c++20 -O2 -Wall -Wextra -Wpedantic
# Boost.Multiprecision (header-only, third_party/README.md); -isystem keeps
# the warnings to our own code.
INCLUDES = -Isrc -isystem third_party -DBOOST_MP_STANDALONE

BUILD    = build/linux
PROGRAM  = $(BUILD)/partition
TESTS    = $(BUILD)/run_tests
WINDOWS  = build/windows/partition.exe
DELIVERABLE = deliverable/partition.exe
DELIVERABLE_SOURCE = deliverable/source

MAIN_SRC = src/main.cpp
LIB_SRC  = $(filter-out $(MAIN_SRC),$(wildcard src/*.cpp src/algorithms/*.cpp))
TEST_SRC = $(wildcard tests/*.cpp)

LIB_OBJ  = $(LIB_SRC:%.cpp=$(BUILD)/%.o)
MAIN_OBJ = $(MAIN_SRC:%.cpp=$(BUILD)/%.o)
TEST_OBJ = $(TEST_SRC:%.cpp=$(BUILD)/%.o)

.PHONY: all test test-quick windows deliverable deliverable-source bench-build bench bench-windows generate-experiment bench-experiment bench-experiment-windows clean

ifeq ($(OS),Windows_NT)
MKDIR = powershell -NoProfile -Command "New-Item -ItemType Directory -Force '$(subst /,\,$1)' | Out-Null"
RUN_CLI_TEST = @echo "cli_test.sh requires a POSIX shell; unit tests passed"
else
MKDIR = mkdir -p $1
RUN_CLI_TEST = sh tests/cli_test.sh $(1) $(2)
endif

all: $(PROGRAM)

$(PROGRAM): $(MAIN_OBJ) $(LIB_OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(TESTS): $(TEST_OBJ) $(LIB_OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@

test: $(TESTS) $(PROGRAM)
	./$(TESTS)
	$(call RUN_CLI_TEST,$(PROGRAM),)

# The same without the slow checks (about 15 s once built; see tests/README.md).
test-quick: $(TESTS) $(PROGRAM)
	./$(TESTS) quick
	$(call RUN_CLI_TEST,$(PROGRAM),quick)

BENCH_BUILD = build/linux
BENCH_GENERATE = $(BENCH_BUILD)/generate
BENCH_PROGRAM = $(BENCH_BUILD)/bench
BENCH_WINDOWS_GENERATE = build/windows/generate.exe
BENCH_WINDOWS_PROGRAM = build/windows/bench.exe
empty :=
space := $(empty) $(empty)
comma := ,
BENCH_CXXFLAGS = $(subst $(space),$(comma),$(CXXFLAGS))
BENCH_MINGWFLAGS = $(subst $(space),$(comma),$(CXXFLAGS) -static -s)

bench-build:
	$(call MKDIR,$(BENCH_BUILD))
	$(CXX) $(CXXFLAGS) $(INCLUDES) bench/generate.cpp -o $(BENCH_GENERATE)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -DBENCH_COMMIT=\"$(shell git rev-parse --short HEAD)\" -DBENCH_FLAGS=\"$(BENCH_CXXFLAGS)\" bench/bench.cpp $(LIB_SRC) -o $(BENCH_PROGRAM)

bench-windows:
	$(call MKDIR,build/windows)
	$(MINGW) $(CXXFLAGS) -static -s $(INCLUDES) bench/generate.cpp -o $(BENCH_WINDOWS_GENERATE)
	$(MINGW) $(CXXFLAGS) -static -s $(INCLUDES) -DBENCH_COMMIT=\"$(shell git rev-parse --short HEAD)\" -DBENCH_FLAGS=\"$(BENCH_MINGWFLAGS)\" bench/bench.cpp $(LIB_SRC) -o $(BENCH_WINDOWS_PROGRAM)

ifeq ($(OS),Windows_NT)
bench: bench-windows
	powershell -NoProfile -Command "Set-Location bench; & ..\build\windows\generate.exe; if ($$LASTEXITCODE -ne 0) { exit $$LASTEXITCODE }; & ..\build\windows\bench.exe; exit $$LASTEXITCODE"

bench-experiment: bench-windows
	powershell -NoProfile -Command "Set-Location bench; & ..\build\windows\generate.exe '$(EX)'; if ($$LASTEXITCODE -ne 0) { exit $$LASTEXITCODE }; & ..\build\windows\bench.exe '$(EX)'; exit $$LASTEXITCODE"

bench-experiment-windows: bench-experiment
else
bench: bench-build
	cd bench && ../$(BENCH_GENERATE) && ../$(BENCH_PROGRAM)

bench-experiment: bench-build
	@test -n "$(EX)" || (echo "EX is required, e.g. EX=E6" >&2; exit 2)
	cd bench && ../$(BENCH_GENERATE) "$(EX)" && ../$(BENCH_PROGRAM) "$(EX)"

bench-experiment-windows: bench-windows
	@echo "The Windows benchmark executables can only be run on Windows." >&2
	@exit 2
endif

ifeq ($(OS),Windows_NT)
generate-experiment: bench-windows
	powershell -NoProfile -Command "Set-Location bench; & ..\build\windows\generate.exe '$(EX)'; exit $$LASTEXITCODE"
else
generate-experiment: bench-build
	@test -n "$(EX)" || (echo "EX is required, e.g. EX=E6" >&2; exit 2)
	cd bench && ../$(BENCH_GENERATE) "$(EX)"
endif

# One compiler call from the sources; -static so that no DLL beyond the
# system ones (KERNEL32, msvcrt) is needed, -s drops the symbols.
windows:
	mkdir -p build/windows
	$(MINGW) $(CXXFLAGS) -static -s $(INCLUDES) $(MAIN_SRC) $(LIB_SRC) -o $(WINDOWS)

deliverable: windows deliverable-source
	cp $(WINDOWS) $(DELIVERABLE)

# Removed first, so that a file deleted from src/ does not stay in the copy.
deliverable-source:
	rm -rf $(DELIVERABLE_SOURCE)/src $(DELIVERABLE_SOURCE)/third_party
	cp -R src third_party $(DELIVERABLE_SOURCE)/

# -MMD -MP: dependency files, so that a changed header rebuilds its users.
$(BUILD)/%.o: %.cpp
	@$(call MKDIR,$(dir $@))
	$(CXX) $(CXXFLAGS) $(INCLUDES) -MMD -MP -c $< -o $@

clean:
	rm -rf build

-include $(LIB_OBJ:.o=.d) $(MAIN_OBJ:.o=.d) $(TEST_OBJ:.o=.d)

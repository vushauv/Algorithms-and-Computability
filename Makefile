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

.PHONY: all test test-quick windows deliverable deliverable-source clean

all: $(PROGRAM)

$(PROGRAM): $(MAIN_OBJ) $(LIB_OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(TESTS): $(TEST_OBJ) $(LIB_OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@

test: $(TESTS) $(PROGRAM)
	./$(TESTS)
	sh tests/cli_test.sh $(PROGRAM)

# The same without the slow checks (about 15 s once built; see tests/README.md).
test-quick: $(TESTS) $(PROGRAM)
	./$(TESTS) quick
	sh tests/cli_test.sh $(PROGRAM) quick

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
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -MMD -MP -c $< -o $@

clean:
	rm -rf build

-include $(LIB_OBJ:.o=.d) $(MAIN_OBJ:.o=.d) $(TEST_OBJ:.o=.d)

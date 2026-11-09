# Partition problem: build of the program and the tests.
#
#   make           the program, build/linux/partition
#   make test      build and run the tests
#   make windows   static Windows executable, build/windows/partition.exe
#   make clean     remove build/
#
# The README lists the same compiler commands for building without make.

CXX      = g++
MINGW    = x86_64-w64-mingw32-g++
CXXFLAGS = -std=c++20 -O2 -Wall -Wextra -Wpedantic

BUILD    = build/linux
PROGRAM  = $(BUILD)/partition
TESTS    = $(BUILD)/run_tests
WINDOWS  = build/windows/partition.exe

MAIN_SRC = src/main.cpp
LIB_SRC  = $(filter-out $(MAIN_SRC),$(wildcard src/*.cpp src/algorithms/*.cpp))
TEST_SRC = $(wildcard tests/*.cpp)

LIB_OBJ  = $(LIB_SRC:%.cpp=$(BUILD)/%.o)
MAIN_OBJ = $(MAIN_SRC:%.cpp=$(BUILD)/%.o)
TEST_OBJ = $(TEST_SRC:%.cpp=$(BUILD)/%.o)

.PHONY: all test windows clean

all: $(PROGRAM)

$(PROGRAM): $(MAIN_OBJ) $(LIB_OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(TESTS): $(TEST_OBJ) $(LIB_OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@

test: $(TESTS)
	./$(TESTS)

# One compiler call from the sources; -static so that no DLL is needed.
windows:
	mkdir -p build/windows
	$(MINGW) $(CXXFLAGS) -static -Isrc $(MAIN_SRC) $(LIB_SRC) -o $(WINDOWS)

# -MMD -MP: dependency files, so that a changed header rebuilds its users.
$(BUILD)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -Isrc -MMD -MP -c $< -o $@

clean:
	rm -rf build

-include $(LIB_OBJ:.o=.d) $(MAIN_OBJ:.o=.d) $(TEST_OBJ:.o=.d)

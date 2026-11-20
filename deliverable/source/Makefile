# Partition problem: build of the program from these sources.
#
#   make           the program, build/linux/partition
#   make windows   static Windows executable, build/windows/partition.exe
#   make clean     remove build/
#
# Without make (the same commands):
#   g++ -std=c++20 -O2 -Isrc -isystem third_party -DBOOST_MP_STANDALONE \
#       src/*.cpp src/algorithms/*.cpp -o partition
#   x86_64-w64-mingw32-g++ -std=c++20 -O2 -static -s -Isrc -isystem third_party \
#       -DBOOST_MP_STANDALONE src/*.cpp src/algorithms/*.cpp -o partition.exe

CXX      = g++
MINGW    = x86_64-w64-mingw32-g++
CXXFLAGS = -std=c++20 -O2 -Wall -Wextra -Wpedantic
# Boost.Multiprecision (header-only, third_party/README.md); -isystem keeps
# the warnings to our own code.
INCLUDES = -Isrc -isystem third_party -DBOOST_MP_STANDALONE

BUILD    = build/linux
PROGRAM  = $(BUILD)/partition
WINDOWS  = build/windows/partition.exe

SRC      = $(wildcard src/*.cpp src/algorithms/*.cpp)
OBJ      = $(SRC:%.cpp=$(BUILD)/%.o)

.PHONY: all windows clean

all: $(PROGRAM)

$(PROGRAM): $(OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@

# One compiler call from the sources; -static so that no DLL beyond the
# system ones (KERNEL32, msvcrt) is needed, -s drops the symbols.
windows:
	mkdir -p build/windows
	$(MINGW) $(CXXFLAGS) -static -s $(INCLUDES) $(SRC) -o $(WINDOWS)

# -MMD -MP: dependency files, so that a changed header rebuilds its users.
$(BUILD)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -MMD -MP -c $< -o $@

clean:
	rm -rf build

-include $(OBJ:.o=.d)

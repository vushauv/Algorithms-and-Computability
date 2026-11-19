// Runs the tests: all of them, or with the argument "quick" only the light ones
// (make test-quick). With "--times" it prints the seconds of each test.
#include <chrono>
#include <cstring>

#include "check.hpp"
#include "tests.hpp"

namespace {

struct Test {
    const char* name;
    void (*run)();
    bool heavy;  // left out by "quick"
};

const Test kTests[] = {
    {"number", test_number, false},
    {"big_uint", test_big_uint, false},
    {"input", test_input, false},
    {"cli", test_cli, false},
    {"brute_force", test_brute_force, false},
    {"karmarkar_karp", test_karmarkar_karp, false},
    {"meet_in_the_middle", test_meet_in_the_middle, false},
    {"dynamic_programming", test_dynamic_programming, false},
    {"solve", test_solve, false},
    {"result_file", test_result_file, false},
    {"arithmetics", test_arithmetics, false},
    {"scale", test_scale, true},
};

}  // namespace

int main(int argc, char** argv) {
    bool quick = false;
    bool times = false;
    for (int i = 1; i < argc; ++i) {
        quick = quick || std::strcmp(argv[i], "quick") == 0;
        times = times || std::strcmp(argv[i], "--times") == 0;
    }
    for (const Test& test : kTests) {
        if (quick && test.heavy) continue;
        const auto start = std::chrono::steady_clock::now();
        test.run();
        if (times) {
            const std::chrono::duration<double> seconds = std::chrono::steady_clock::now() - start;
            std::cout << test.name << ": " << seconds.count() << " s\n";
        }
    }

    std::cout << check::passed << " checks passed, " << check::failed << " failed\n";
    return check::failed == 0 ? 0 : 1;
}

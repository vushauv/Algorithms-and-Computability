#include "check.hpp"
#include "input.hpp"
#include "tests.hpp"

#include <sstream>
#include <string>

namespace {

std::vector<Instance> read(const std::string& text) {
    std::istringstream in(text);
    return read_instances(in);
}

// The line of the InputError thrown for the text, or -1 if none is thrown.
long error_line(const std::string& text) {
    try {
        read(text);
    } catch (const InputError& error) {
        return static_cast<long>(error.line());
    }
    return -1;
}

}  // namespace

void test_input() {
    // Valid files.
    auto sets = read("2\n1 2 3\n4 4\n");
    CHECK(sets.size() == 2);
    CHECK(sets[0].values.size() == 3);
    CHECK(sets[0].sum == 6);
    CHECK(sets[0].line == 2);
    CHECK(sets[1].values[1] == 4);
    CHECK(sets[1].line == 3);

    CHECK(read("1\n5 5").size() == 1);                // no trailing newline
    CHECK(read("1\r\n5 5\r\n").size() == 1);          // Windows line endings
    CHECK(read("1\n5 5\n\n\n").size() == 1);          // empty lines at the end
    CHECK(read("1\n0 0 007\n")[0].values[2] == 7);    // zeros and leading zeros

    const std::string max38(38, '9');
    CHECK(read("1\n" + max38 + " " + max38 + "\n")[0].sum == 2 * (kValueLimit - 1));

    // The example files of the course.
    CHECK(read_instances_from_file("raw_regulations/AaC_example_1.txt").size() == 2);
    auto big = read_instances_from_file("raw_regulations/AaC_example_2.txt");
    CHECK(big.size() == 1);
    CHECK(to_string(big[0].values[0]) == "3141592653589793238462643");

    // Errors, with the line they are reported on.
    CHECK(error_line("") == 0);                       // empty file
    CHECK(error_line("x\n1 2\n") == 1);               // N not a number
    CHECK(error_line("0\n") == 1);                    // N = 0
    CHECK(error_line("10001\n") == 1);                // N too large
    CHECK(error_line("1 1\n1 2\n") == 1);             // more than N on the first line
    CHECK(error_line("2\n1 2\n") == 0);               // fewer sets than announced
    CHECK(error_line("1\n1 2\n3 4\n") == 3);          // more sets than announced
    CHECK(error_line("1\n\n") == 2);                  // empty set
    CHECK(error_line("1\n5\n") == 2);                 // one element
    CHECK(error_line("1\n1  2\n") == 2);              // two spaces
    CHECK(error_line("1\n 1 2\n") == 2);              // leading space
    CHECK(error_line("1\n1 2 \n") == 2);              // trailing space
    CHECK(error_line("1\n1\t2\n") == 2);              // tab
    CHECK(error_line("1\n1 -2\n") == 2);              // negative number
    CHECK(error_line("1\n1 2.5\n") == 2);             // not an integer
    CHECK(error_line("2\n1 2\n1 1" + std::string(38, '0') + "\n") == 3);  // 10^38

    // A sum of 2^128 or more: 4 elements just below 10^38 exceed 2^128 ~ 3.4 * 10^38.
    CHECK(error_line("1\n" + max38 + " " + max38 + " " + max38 + " " + max38 + "\n") == 2);

    // The column of the offending number.
    try {
        read("1\n12 3x 4\n");
        CHECK(false);
    } catch (const InputError& error) {
        CHECK(error.column() == 4);
        CHECK(std::string(error.what()) == "line 2, column 4: '3x' is not a non-negative integer (digits only)");
    }

    try {
        read("1\n1\t2\n");
        CHECK(false);
    } catch (const InputError& error) {
        CHECK(std::string(error.what()).find("not a non-negative integer") != std::string::npos);
    }

    CHECK_THROWS(read_instances_from_file("no/such/file.txt"), InputError);
}

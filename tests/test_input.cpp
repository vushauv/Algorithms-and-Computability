#include "check.hpp"
#include "input.hpp"
#include "tests.hpp"
#include "value.hpp"

#include <sstream>
#include <string>

namespace {

std::vector<Instance<Value>> read(const std::string& text) {
    std::istringstream in(text);
    return read_instances<Value>(in);
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
    CHECK(read("\xEF\xBB\xBF" "1\r\n5 5\r\n").size() == 1);  // UTF-8 byte order mark

    // Extra spaces and tabs are skipped; the columns still count them.
    CHECK(read("1\n1  2\n")[0].values.size() == 2);            // two spaces
    CHECK(read("1\n 1 2\n")[0].values.size() == 2);            // leading space
    CHECK(read("1\n1 2 \n")[0].values.size() == 2);            // trailing space
    CHECK(read("1\n1\t2\n")[0].values.size() == 2);            // tab
    CHECK(read(" 1 \r\n\t1  2 \t\r\n \r\n")[0].sum == 3);     // everywhere, with CRLF
    CHECK(read("1\r\r\n1 2\r\r\n")[0].sum == 3);               // a doubled \r at the end of a line
    try {
        read("1\n 12  3x\n");
        CHECK(false);
    } catch (const InputError& error) {
        CHECK(error.column() == 6);
    }

    const std::string max38(38, '9');
    CHECK(read("1\n" + max38 + " " + max38 + "\n")[0].sum == 2 * (kNativeLimit - 1));

    // No limit on the number of sets or of numbers in a set.
    std::string many_sets = "10001\n";
    for (int i = 0; i < 10001; ++i) many_sets += "1 1\n";
    CHECK(read(many_sets).size() == 10001);
    std::string many_numbers = "1\n";
    for (int i = 0; i < 1'000'001; ++i) many_numbers += "1 ";
    CHECK(read(many_numbers)[0].values.size() == 1'000'001);

    // The example files of the course.
    CHECK(read_instances_from_file<Value>("raw_regulations/AaC_example_1.txt").size() == 2);
    auto big = read_instances_from_file<Value>("raw_regulations/AaC_example_2.txt");
    CHECK(big.size() == 1);
    CHECK(to_string(big[0].values[0]) == "3141592653589793238462643");

    // Errors, with the line they are reported on.
    CHECK(error_line("") == 0);                       // empty file
    CHECK(error_line("x\n1 2\n") == 1);               // N not a number
    CHECK(error_line("0\n") == 1);                    // N = 0
    CHECK(error_line("99999999999999999999999999\n") == 1);  // N does not fit in size_t
    CHECK(error_line("1 1\n1 2\n") == 1);             // more than N on the first line
    CHECK(error_line("2\n1 2\n") == 0);               // fewer sets than announced
    CHECK(error_line("1\n1 2\n3 4\n") == 3);          // more sets than announced
    CHECK(error_line("1\n\n") == 2);                  // empty set
    CHECK(error_line("1\n5\n") == 2);                 // one element
    CHECK(error_line("1\n  \n") == 2);               // a set of spaces only
    CHECK(error_line("1\n1,2\n") == 2);              // comma
    CHECK(error_line("1\n1 -2\n") == 2);              // negative number
    CHECK(error_line("1\n1 2.5\n") == 2);             // not an integer
    CHECK(error_line("2\n1 2\n1 1" + std::string(38, '0') + "\n") == 3);  // 10^38

    // A sum of 2^128 or more: 4 elements just below 10^38 exceed 2^128 ~ 3.4 * 10^38.
    CHECK(error_line("1\n" + max38 + " " + max38 + " " + max38 + " " + max38 + "\n") == 2);
    try {
        read("1\n" + std::string(39, '9') + " 0\n");
        CHECK(false);
    } catch (const InputError& error) {
        CHECK(error.kind() == InputErrorKind::NativeLimit);
    }
    try {
        read("1\n" + max38 + " " + max38 + " " + max38 + " " + max38 + "\n");
        CHECK(false);
    } catch (const InputError& error) {
        CHECK(error.kind() == InputErrorKind::NativeLimit);
    }

    // The column of the offending number.
    try {
        read("1\n12 3x 4\n");
        CHECK(false);
    } catch (const InputError& error) {
        CHECK(error.column() == 4);
        CHECK(std::string(error.what()) == "line 2, column 4: '3x' is not a non-negative integer (digits only)");
    }

    try {
        read_instances_from_file<Value>("no/such/file.txt");
        CHECK(false);
    } catch (const InputError& error) {
        CHECK(error.kind() == InputErrorKind::InvalidInput);
    }
    try {
        read_instances_from_file<Value>("raw_regulations");
        CHECK(false);
    } catch (const InputError& error) {
        CHECK(std::string(error.what()) == "is a directory, not a file");
    }
}

// A minimal test helper: CHECK and CHECK_THROWS count failures and print
// where they happened; tests/main.cpp runs the test functions and reports.
#pragma once

#include <iostream>

namespace check {

inline int passed = 0;
inline int failed = 0;

inline void report(bool ok, const char* expression, const char* file, int line) {
    if (ok) {
        ++passed;
        return;
    }
    ++failed;
    std::cerr << file << ":" << line << ": CHECK failed: " << expression << "\n";
}

}  // namespace check

#define CHECK(condition) ::check::report(static_cast<bool>(condition), #condition, __FILE__, __LINE__)

#define CHECK_THROWS(statement, Exception)                                         \
    ::check::report(                                                               \
        [&] {                                                                      \
            try {                                                                  \
                statement;                                                         \
            } catch (const Exception&) {                                           \
                return true;                                                       \
            } catch (...) {                                                        \
            }                                                                      \
            return false;                                                          \
        }(),                                                                       \
        #statement " throws " #Exception, __FILE__, __LINE__)

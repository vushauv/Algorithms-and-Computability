#include "check.hpp"
#include "tests.hpp"

int main() {
    test_number();

    std::cout << check::passed << " checks passed, " << check::failed << " failed\n";
    return check::failed == 0 ? 0 : 1;
}

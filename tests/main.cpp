#include "check.hpp"
#include "tests.hpp"

int main() {
    test_number();
    test_input();
    test_cli();
    test_brute_force();
    test_karmarkar_karp();

    std::cout << check::passed << " checks passed, " << check::failed << " failed\n";
    return check::failed == 0 ? 0 : 1;
}

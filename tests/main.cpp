#include "check.hpp"
#include "tests.hpp"

int main() {
    test_number();
    test_big_uint();
    test_input();
    test_cli();
    test_brute_force();
    test_karmarkar_karp();
    test_meet_in_the_middle();
    test_dynamic_programming();
    test_solve();
    test_result_file();
    test_arithmetics();

    std::cout << check::passed << " checks passed, " << check::failed << " failed\n";
    return check::failed == 0 ? 0 : 1;
}

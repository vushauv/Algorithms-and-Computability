#include "check.hpp"
#include "tests.hpp"

#include <boost/multiprecision/cpp_int.hpp>

#include <random>
#include <span>

#define BENCH_GENERATOR_NO_MAIN
#include "../bench/generate.cpp"
#include "algorithms/brute_force.hpp"

void test_benchmark_generator() {
    const std::uint64_t seed = bench_hash_seed("E2", "trap_n30.txt");
    std::mt19937_64 engine(seed);
    std::vector<std::string> trap =
        generate_set(GeneratorFamily::Trap, 30, 25, 0, 0, engine);
    CHECK(trap.size() == 30);

    boost::multiprecision::cpp_int sum = 0;
    for (const std::string& value : trap) sum += boost::multiprecision::cpp_int(value);
    CHECK((sum % 4) == 2);

    std::mt19937_64 yes_engine(bench_hash_seed("E6", "yes_d3_n20.txt"));
    std::vector<std::string> yes =
        generate_set(GeneratorFamily::Yes, 20, 3, 0, 0, yes_engine);
    std::vector<BoostInt> yes_values;
    for (const std::string& text : yes)
        yes_values.push_back(parse_value<BoostInt>(text).value);
    CHECK(verdict_of(brute_force<BoostInt>(std::span<const BoostInt>(yes_values))) == Verdict::Yes);

    std::mt19937_64 first_engine(seed);
    std::mt19937_64 second_engine(seed);
    for (int i = 0; i < 100; ++i)
        CHECK(bench_randrange(1000, first_engine) == bench_randrange(1000, second_engine));

    std::mt19937_64 trap_engine_a(seed);
    std::mt19937_64 trap_engine_b(seed);
    CHECK(generate_set(GeneratorFamily::Trap, 30, 25, 0, 0, trap_engine_a) ==
          generate_set(GeneratorFamily::Trap, 30, 25, 0, 0, trap_engine_b));
}

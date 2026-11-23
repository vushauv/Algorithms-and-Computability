#pragma once

#include <cstdint>
#include <random>
#include <string>
#include <vector>

#include "experiments.hpp"

std::uint64_t bench_randrange(std::uint64_t limit, std::mt19937_64& engine);
std::uint64_t bench_hash_seed(const std::string& experiment,
                              const std::string& file_name);
std::vector<std::string> generate_set(GeneratorFamily family, std::size_t n,
                                      std::size_t digits, std::uint64_t target,
                                      std::uint64_t small_max,
                                      std::mt19937_64& engine);
void generate_file(const ExperimentFile& file, const std::string& experiment,
                   const std::string& directory);

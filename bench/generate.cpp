#include "generate.hpp"

#include <boost/multiprecision/cpp_int.hpp>

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <limits>
#include <random>
#include <string>

namespace {

using BenchBoostInt = boost::multiprecision::cpp_int;

std::string random_digits(std::size_t digits, std::mt19937_64& engine) {
    std::string text;
    text.reserve(digits);
    text.push_back(static_cast<char>('1' + bench_randrange(9, engine)));
    for (std::size_t i = 1; i < digits; ++i)
        text.push_back(static_cast<char>('0' + bench_randrange(10, engine)));
    return text;
}

BenchBoostInt parse_decimal(const std::string& text) {
    return BenchBoostInt(text);
}

std::string small_number(std::uint64_t limit, std::mt19937_64& engine) {
    return std::to_string(bench_randrange(limit + 1, engine));
}

std::vector<std::string> random_set(std::size_t n, std::size_t digits,
                                    std::mt19937_64& engine) {
    std::vector<std::string> values;
    for (std::size_t i = 0; i < n; ++i)
        values.push_back(random_digits(digits, engine));
    return values;
}

void fisher_yates(std::vector<std::string>& values, std::mt19937_64& engine) {
    for (std::size_t i = values.size(); i > 1; --i)
        std::swap(values[i - 1], values[bench_randrange(i, engine)]);
}

}  // namespace

std::uint64_t bench_randrange(std::uint64_t limit, std::mt19937_64& engine) {
    if (limit == 0) return 0;
    if (limit == 1) return 0;
    const std::uint64_t cutoff =
        (std::numeric_limits<std::uint64_t>::max() - std::numeric_limits<std::uint64_t>::max() % limit);
    std::uint64_t value;
    do {
        value = engine();
    } while (value >= cutoff);
    return value % limit;
}

std::uint64_t bench_hash_seed(const std::string& experiment,
                              const std::string& file_name) {
    constexpr std::uint64_t offset = 14695981039346656037ULL;
    constexpr std::uint64_t prime = 1099511628211ULL;
    std::uint64_t hash = offset;
    std::string stem = file_name;
    if (stem.ends_with(".txt")) stem.resize(stem.size() - 4);
    const std::string text = "20261204/" + experiment + "/" + stem;
    for (unsigned char c : text) {
        hash ^= c;
        hash *= prime;
    }
    return hash;
}

std::vector<std::string> generate_set(GeneratorFamily family, std::size_t n,
                                      std::size_t digits, std::uint64_t target,
                                      std::uint64_t small_max,
                                      std::mt19937_64& engine) {
    std::vector<std::string> values;
    if (family == GeneratorFamily::Random)
        return random_set(n, digits, engine);

    if (family == GeneratorFamily::Trap) {
        for (std::size_t i = 0; i + 1 < n; ++i) {
            const BenchBoostInt r(parse_decimal(random_digits(digits, engine)));
            BenchBoostInt value = r * 4;
            values.push_back(value.str());
        }
        const BenchBoostInt r(parse_decimal(random_digits(digits, engine)));
        BenchBoostInt value = r * 4 + 2;
        values.push_back(value.str());
        fisher_yates(values, engine);
        return values;
    }

    if (family == GeneratorFamily::Target) {
        const std::uint64_t limit = target / n;
        for (std::size_t i = 0; i + 1 < n; ++i) {
            const std::uint64_t r = bench_randrange(limit + 1, engine);
            values.push_back(std::to_string(r * 4));
        }
        const std::uint64_t r = bench_randrange(limit + 1, engine);
        values.push_back(std::to_string(r * 4 + 2));
        fisher_yates(values, engine);
        return values;
    }

    if (family == GeneratorFamily::Small) {
        for (std::size_t i = 0; i < n; ++i)
            values.push_back(small_number(small_max, engine));
        return values;
    }

    // Construct a balanced first/second half, then shuffle.
    for (;;) {
        values = random_set(n, digits, engine);
        const std::size_t half = n / 2;
        BenchBoostInt first = 0;
        BenchBoostInt second = 0;
        for (std::size_t i = 0; i < half; ++i) first += parse_decimal(values[i]);
        for (std::size_t i = half; i < n; ++i) second += parse_decimal(values[i]);
        const std::size_t index = bench_randrange(half, engine);
        const BenchBoostInt old = parse_decimal(values[index]);
        const BenchBoostInt replacement = second - (first - old);
        if (replacement >= 0) {
            values[index] = replacement.str();
            fisher_yates(values, engine);
            return values;
        }
    }
}

void generate_file(const ExperimentFile& file, const std::string& experiment,
                   const std::string& directory) {
    std::filesystem::create_directories(directory);
    const std::string path = directory + "/" + file.name;
    std::ofstream out(path, std::ios::binary);
    if (!out) throw std::runtime_error("cannot write " + path);
    out << file.sets << '\n';
    std::mt19937_64 engine(bench_hash_seed(experiment, file.name));
    for (std::size_t set = 0; set < file.sets; ++set) {
        const std::vector<std::string> values =
            generate_set(file.family, file.n, file.digits, file.target,
                         file.small_max, engine);
        for (std::size_t i = 0; i < values.size(); ++i) {
            if (i != 0) out << ' ';
            out << values[i];
        }
        out << '\n';
    }
}

#ifndef BENCH_GENERATOR_NO_MAIN
int main(int argc, char** argv) {
    if (argc > 2 || (argc == 2 && std::string(argv[1]).empty())) {
        std::cerr << "usage: generate [E1|E2|E3a|E3b|E4|E5|E6|E7|A1|A2|A3|A4]\n";
        return 2;
    }

    const std::string selected = argc == 2 ? argv[1] : "";
    if (!selected.empty()) {
        const auto experiment = std::find_if(
            experiments::kExperiments.begin(), experiments::kExperiments.end(),
            [&selected](const Experiment& candidate) { return candidate.name == selected; });
        if (experiment == experiments::kExperiments.end()) {
            std::cerr << "unknown experiment: " << selected << '\n';
            return 2;
        }

        const std::string directory = "inputs/" + experiment->name;
        for (const ExperimentFile& file : experiment->files)
            generate_file(file, experiment->name, directory);
        return 0;
    }

    const std::string root = "inputs";
    for (const Experiment& experiment : experiments::kExperiments) {
        const std::string directory = root + "/" + experiment.name;
        for (const ExperimentFile& file : experiment.files)
            generate_file(file, experiment.name, directory);
    }
    return 0;
}
#endif

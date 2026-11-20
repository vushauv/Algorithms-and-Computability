// Reads the sets from the input file, runs SOLVE or the single
// algorithm from --algorithm on each, prints the results and saves the results file.
//
// Exit codes: 0 ok, 1 bad input file, 2 bad command line, 3 an algorithm couldn't run
// on some set (the other sets still run), 4 the results file couldn't be written.

#include <exception>
#include <fstream>
#include <iostream>
#include <new>
#include <optional>
#include <string>
#include <span>
#include <string_view>
#include <vector>

#include "cli.hpp"
#include "input.hpp"
#include "output.hpp"
#include "report.hpp"
#include "result_file.hpp"
#include "run.hpp"

namespace {

// --no-color has to work for the error about a bad command line too, and that one is
// printed before the options are parsed.
bool no_color_requested(int argc, char** argv) {
    for (int i = 1; i < argc; ++i)
        if (std::string_view(argv[i]) == "--no-color") return true;
    return false;
}

bool save_results(const std::string& path, const std::string& text) {
    std::ofstream file(path, std::ios::binary);  // binary, so "\n" stays "\n" on Windows
    file << text;
    file.close();
    return static_cast<bool>(file);
}

// Reads the file and runs every set in the arithmetic V; returns the exit code.
template <class V>
int process(const Options& options) {
    std::vector<Instance<V>> instances;
    try {
        instances = read_instances_from_file<V>(options.path);
    } catch (const InputError& error) {
        output::error(options.path + ": " + error.what());
        return 1;
    }

    const std::string results_path = result_file::path_for(options.path);
    std::string results = result_file::header(instances.size(), options.subsets);

    int exit_code = 0;
    for (std::size_t i = 0; i < instances.size(); ++i) {
        if (i > 0) std::cout << "\n";
        report::print_instance(i + 1, instances[i]);
        std::optional<Partition<V>> found;
        try {
            found = run::run_set(i + 1, instances[i], options);
        } catch (const std::bad_alloc&) {
            output::error(run::set_name(i + 1, instances[i]) + ": not enough memory");
            exit_code = 3;
        } catch (const std::exception& error) {
            output::error(run::set_name(i + 1, instances[i]) + ": " + error.what());
            exit_code = 3;
        }
        const std::span<const V> a = instances[i].values;
        if (!found) found = run::fallback_partition(a);
        results += "\n\n" + result_file::format_set(a, *found);
    }

    if (!save_results(results_path, results)) {
        std::cout << "\n";
        output::error("cannot write the results file " + results_path);
        return 4;  // after the console output; wins over 3
    }
    std::cout << "\nResults saved to " << results_path << "\n";
    return exit_code;
}

}  // namespace

int main(int argc, char** argv) {
    Options options;
    try {
        options = parse_options(argc, argv);
    } catch (const UsageError& error) {
        output::init(no_color_requested(argc, argv));
        output::error(error.what());
        std::cerr << "Try 'partition --help'.\n";
        return 2;
    }
    output::init(options.no_color);

    if (options.help) {
        std::cout << usage_text();
        return 0;
    }
    // --quiet: a failed stream drops everything written to it. Errors and warnings
    // go to stderr and stay.
    if (options.quiet) std::cout.setstate(std::ios::badbit);

    switch (options.arithmetic) {
        case Arithmetic::Native:
            return process<Native>(options);
        case Arithmetic::Own:
            BigUint::set_base(options.base);
            return process<BigUint>(options);
        case Arithmetic::Boost:
            break;
    }
    return process<BoostInt>(options);
}

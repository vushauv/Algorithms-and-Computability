// Partition problem: reads the sets of the input file and, for now, prints
// them; the algorithms are added in the next steps of the plan.
//
// Exit codes: 0 success, 1 invalid input file, 2 invalid command line.

#include <iostream>
#include <string_view>

#include "cli.hpp"
#include "input.hpp"
#include "output.hpp"

using output::paint;
using output::Style;

namespace {

// --no-color must hold also for the message about an invalid command line,
// which is printed before the options are known.
bool no_color_requested(int argc, char** argv) {
    for (int i = 1; i < argc; ++i)
        if (std::string_view(argv[i]) == "--no-color") return true;
    return false;
}

void print_instance(std::size_t index, const Instance& instance) {
    std::cout << paint("Set " + std::to_string(index), Style::Heading)
              << paint(" (line " + std::to_string(instance.line) + ")", Style::Dim) << ": n = "
              << instance.values.size() << ", S = " << to_string(instance.sum) << "\n"
              << "  " << output::format_values(instance.values) << "\n";
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

    std::vector<Instance> instances;
    try {
        instances = read_instances_from_file(options.path);
    } catch (const InputError& error) {
        output::error(options.path + ": " + error.what());
        return 1;
    }

    for (std::size_t i = 0; i < instances.size(); ++i) {
        if (i > 0) std::cout << "\n";
        print_instance(i + 1, instances[i]);
    }
    return 0;
}

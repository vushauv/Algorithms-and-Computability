#include "result_file.hpp"

#include <algorithm>
#include <cctype>
#include <functional>

namespace result_file {

namespace {

// The numbers separated by single spaces, each preceded by one.
std::string spaced(std::span<const Value> values) {
    std::string text;
    for (Value v : values) text += " " + to_string(v);
    return text;
}

Value sum_of(std::span<const Value> values) {
    Value sum = 0;
    for (Value v : values) sum += v;  // at most S < 2^128
    return sum;
}

std::vector<Value> values_at(std::span<const Value> a, const std::vector<std::size_t>& indices) {
    std::vector<Value> values;
    values.reserve(indices.size());
    for (std::size_t i : indices) values.push_back(a[i]);
    return values;
}

}  // namespace

std::string path_for(std::string_view input_path) {
    constexpr std::string_view kExtension = ".txt";
    std::string path(input_path);
    if (path.size() >= kExtension.size()) {
        std::string ending = path.substr(path.size() - kExtension.size());
        for (char& c : ending) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
        if (ending == kExtension) path.resize(path.size() - kExtension.size());
    }
    return path + "_output.txt";
}

std::string header(std::size_t sets, std::size_t subsets) {
    return std::to_string(sets) + " " + std::to_string(subsets);
}

std::string format_set(std::span<const Value> a, std::vector<std::vector<Value>> subsets) {
    for (std::vector<Value>& subset : subsets) std::ranges::sort(subset, std::greater<>());
    // Larger content first: the first larger number decides, and of two lines
    // where one continues the other, the longer one is larger.
    std::ranges::sort(subsets, [](const std::vector<Value>& x, const std::vector<Value>& y) {
        return std::ranges::lexicographical_compare(y, x);
    });

    std::string text = to_string(sum_of(a)) + spaced(a);
    Value smallest = 0;
    Value largest = 0;
    for (std::size_t i = 0; i < subsets.size(); ++i) {
        const Value sum = sum_of(subsets[i]);
        if (i == 0 || sum < smallest) smallest = sum;
        if (i == 0 || sum > largest) largest = sum;
        text += "\n" + to_string(sum) + spaced(subsets[i]);
    }
    return text + "\n" + to_string(largest - smallest);
}

std::string format_set(std::span<const Value> a, const Partition& partition) {
    return format_set(a, {values_at(a, partition.A), values_at(a, partition.B)});
}

}  // namespace result_file

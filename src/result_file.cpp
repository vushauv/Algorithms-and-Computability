#include "result_file.hpp"

#include <algorithm>
#include <cctype>
#include <functional>

namespace result_file {

namespace {

// The numbers separated by single spaces, each preceded by one.
template <class V>
std::string spaced(std::span<const V> values) {
    std::string text;
    for (const V& v : values) text += " " + to_string(v);
    return text;
}

template <class V>
V sum_of(std::span<const V> values) {
    V sum = 0;
    for (const V& v : values) sum += v;  // at most S
    return sum;
}

template <class V>
std::vector<V> values_at(std::span<const V> a, const std::vector<std::size_t>& indices) {
    std::vector<V> values;
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

template <class V>
std::string format_set(std::span<const V> a, std::vector<std::vector<V>> subsets) {
    for (std::vector<V>& subset : subsets) std::ranges::sort(subset, std::greater<>());
    // Larger content first: the first larger number decides, and of two lines
    // where one continues the other, the longer one is larger.
    std::ranges::sort(subsets, [](const std::vector<V>& x, const std::vector<V>& y) {
        return std::ranges::lexicographical_compare(y, x);
    });

    std::string text = to_string(sum_of(a)) + spaced(a);
    V smallest = 0;
    V largest = 0;
    for (std::size_t i = 0; i < subsets.size(); ++i) {
        const V sum = sum_of(std::span<const V>(subsets[i]));
        if (i == 0 || sum < smallest) smallest = sum;
        if (i == 0 || sum > largest) largest = sum;
        text += "\n" + to_string(sum) + spaced(std::span<const V>(subsets[i]));
    }
    return text + "\n" + to_string(V(largest - smallest));
}

template <class V>
std::string format_set(std::span<const V> a, const Partition<V>& partition) {
    return format_set<V>(a, {values_at(a, partition.A), values_at(a, partition.B)});
}

#define INSTANTIATE(V)                                                               \
    template std::string format_set(std::span<const V>, std::vector<std::vector<V>>); \
    template std::string format_set(std::span<const V>, const Partition<V>&);
FOR_EACH_NUMBER_TYPE(INSTANTIATE)
#undef INSTANTIATE

}  // namespace result_file

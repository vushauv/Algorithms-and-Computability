#include "input.hpp"

#include <filesystem>
#include <fstream>
#include <string_view>

namespace {

constexpr std::string_view kUtf8Bom = "\xEF\xBB\xBF";

std::string location(std::size_t line, std::size_t column) {
    if (line == 0) return "";
    std::string text = "line " + std::to_string(line);
    if (column > 0) text += ", column " + std::to_string(column);
    return text + ": ";
}

// Next line without its line ending. A file converted twice can end in \r\r\n, so all
// trailing \r are dropped.
bool next_line(std::istream& in, std::string& line) {
    if (!std::getline(in, line)) return false;
    while (!line.empty() && line.back() == '\r') line.pop_back();
    return true;
}

struct Field {
    std::string_view text;
    std::size_t column;
};

bool is_blank(char c) {
    return c == ' ' || c == '\t';
}

// Splits a line into numbers. Extra spaces and tabs, also at the ends, are skipped so a
// stray space doesn't reject the whole file.
std::vector<Field> split_fields(std::string_view line) {
    std::vector<Field> fields;
    std::size_t i = 0;
    while (i < line.size()) {
        if (is_blank(line[i])) {
            ++i;
            continue;
        }
        const std::size_t start = i;
        while (i < line.size() && !is_blank(line[i])) ++i;
        fields.push_back({line.substr(start, i - start), start + 1});
    }
    return fields;
}

template <class V>
V parse_field(const Field& field, std::size_t line_number) {
    ParsedValue<V> parsed = parse_value<V>(field.text);
    switch (parsed.status) {
        case ParseStatus::Ok:
            return std::move(parsed.value);
        case ParseStatus::TooLarge:  // Native only
            throw InputError(line_number, field.column,
                             "the number has to be below 10^38 (at most 38 digits) with "
                             "--arithmetic native");
        case ParseStatus::Empty:
        case ParseStatus::NotDigit:
            break;
    }
    std::string shown(field.text.substr(0, 20));
    if (field.text.size() > 20) shown += "...";
    throw InputError(line_number, field.column,
                     "'" + shown + "' is not a non-negative integer (digits only)");
}

std::size_t read_set_count(std::istream& in) {
    std::string line;
    if (!next_line(in, line)) throw InputError(0, 0, "the file is empty");
    // Some Windows editors add a UTF-8 byte order mark.
    if (line.starts_with(kUtf8Bom)) line.erase(0, kUtf8Bom.size());
    std::vector<Field> fields = split_fields(line);
    if (fields.size() != 1)
        throw InputError(1, 0, "the first line must contain only the number of sets");
    // In the long arithmetic too: a count of any length is still out of range.
    const BoostInt count = parse_field<BoostInt>(fields[0], 1);
    if (count < 1 || count > kMaxSets)
        throw InputError(1, 1, "the number of sets must be between 1 and " + std::to_string(kMaxSets));
    return static_cast<std::size_t>(count);
}

template <class V>
Instance<V> read_set(const std::string& line, std::size_t line_number) {
    std::vector<Field> fields = split_fields(line);
    if (fields.empty())
        throw InputError(line_number, 0, "empty line; a set needs at least " +
                                             std::to_string(kMinElements) + " numbers");
    if (fields.size() > kMaxElements)
        throw InputError(line_number, 0, "a set may have at most " + std::to_string(kMaxElements) +
                                             " numbers, found " + std::to_string(fields.size()));

    // The numbers are parsed before their count is checked, so that "1,2"
    // is reported as a bad character and not as a set with one element.
    Instance<V> instance{{}, 0, line_number};
    instance.values.reserve(fields.size());
    for (const Field& field : fields) {
        V value = parse_field<V>(field, line_number);
        std::optional<V> sum = checked_add(instance.sum, value);
        if (!sum)  // Native only
            throw InputError(line_number, field.column,
                             "the sum of the set exceeds 2^128 - 1 with --arithmetic native");
        instance.sum = std::move(*sum);
        instance.values.push_back(std::move(value));
    }

    if (instance.values.size() < kMinElements)
        throw InputError(line_number, 0, "a set needs at least " + std::to_string(kMinElements) +
                                             " numbers, found " + std::to_string(instance.values.size()));
    return instance;
}

}  // namespace

InputError::InputError(std::size_t line, std::size_t column, const std::string& message)
    : std::runtime_error(location(line, column) + message), line_(line), column_(column) {}

template <class V>
std::vector<Instance<V>> read_instances(std::istream& in) {
    std::size_t count = read_set_count(in);

    std::vector<Instance<V>> instances;
    std::string line;
    std::size_t line_number = 1;
    while (instances.size() < count) {
        if (!next_line(in, line))
            throw InputError(0, 0, "the first line announces " + std::to_string(count) +
                                       " sets, but the file has only " +
                                       std::to_string(instances.size()));
        ++line_number;
        instances.push_back(read_set<V>(line, line_number));
    }

    // Blank lines at the end are fine, anything else is an error.
    while (next_line(in, line)) {
        ++line_number;
        if (!split_fields(line).empty())
            throw InputError(line_number, 0, "the first line announces " + std::to_string(count) +
                                                 " sets, but the file has more lines");
    }
    return instances;
}

template <class V>
std::vector<Instance<V>> read_instances_from_file(const std::string& path) {
    // A directory opens without error on Linux and then reads as empty.
    std::error_code ignored;
    if (std::filesystem::is_directory(path, ignored))
        throw InputError(0, 0, "is a directory, not a file");
    std::ifstream file(path, std::ios::binary);  // binary, next_line handles \r\n
    if (!file) throw InputError(0, 0, "cannot open the file");
    return read_instances<V>(file);
}

#define INSTANTIATE(V)                                                 \
    template std::vector<Instance<V>> read_instances<V>(std::istream&); \
    template std::vector<Instance<V>> read_instances_from_file<V>(const std::string&);
FOR_EACH_NUMBER_TYPE(INSTANTIATE)
#undef INSTANTIATE

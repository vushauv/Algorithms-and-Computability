#include "input.hpp"

#include <fstream>
#include <string_view>

namespace {

std::string location(std::size_t line, std::size_t column) {
    if (line == 0) return "";
    std::string text = "line " + std::to_string(line);
    if (column > 0) text += ", column " + std::to_string(column);
    return text + ": ";
}

// Reads the next line without its line ending (\n or \r\n).
bool next_line(std::istream& in, std::string& line) {
    if (!std::getline(in, line)) return false;
    if (!line.empty() && line.back() == '\r') line.pop_back();
    return true;
}

// The fields of a line separated by single spaces, with their columns.
struct Field {
    std::string_view text;
    std::size_t column;
};

std::vector<Field> split_fields(std::string_view line, std::size_t line_number) {
    std::vector<Field> fields;
    std::size_t start = 0;
    while (true) {
        std::size_t end = line.find(' ', start);
        std::string_view text = line.substr(start, end == std::string_view::npos ? end : end - start);
        if (text.empty()) {
            std::string what = start == 0 ? "the line starts with a space"
                             : end == std::string_view::npos ? "the line ends with a space"
                             : "two spaces in a row";
            throw InputError(line_number, start + 1, what + "; numbers are separated by single spaces");
        }
        fields.push_back({text, start + 1});
        if (end == std::string_view::npos) return fields;
        start = end + 1;
    }
}

Value parse_field(const Field& field, std::size_t line_number) {
    ParsedValue parsed = parse_value(field.text);
    switch (parsed.status) {
        case ParseStatus::Ok:
            return parsed.value;
        case ParseStatus::TooLarge:
            throw InputError(line_number, field.column,
                             "the number has to be below 10^38 (at most 38 digits)");
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
    std::vector<Field> fields = split_fields(line, 1);
    if (fields.size() != 1)
        throw InputError(1, 0, "the first line must contain only the number of sets");
    Value count = parse_field(fields[0], 1);
    if (count < 1 || count > kMaxSets)
        throw InputError(1, 1, "the number of sets must be between 1 and " + std::to_string(kMaxSets));
    return static_cast<std::size_t>(count);
}

Instance read_set(const std::string& line, std::size_t line_number) {
    if (line.empty())
        throw InputError(line_number, 0, "empty line; a set needs at least " +
                                             std::to_string(kMinElements) + " numbers");
    std::vector<Field> fields = split_fields(line, line_number);
    if (fields.size() > kMaxElements)
        throw InputError(line_number, 0, "a set may have at most " + std::to_string(kMaxElements) +
                                             " numbers, found " + std::to_string(fields.size()));

    // The numbers are parsed before their count is checked, so that "1<tab>2"
    // is reported as a bad character and not as a set with one element.
    Instance instance{{}, 0, line_number};
    instance.values.reserve(fields.size());
    for (const Field& field : fields) {
        Value value = parse_field(field, line_number);
        std::optional<Value> sum = checked_add(instance.sum, value);
        if (!sum) throw InputError(line_number, field.column, "the sum of the set exceeds 2^128 - 1");
        instance.sum = *sum;
        instance.values.push_back(value);
    }

    if (instance.values.size() < kMinElements)
        throw InputError(line_number, 0, "a set needs at least " + std::to_string(kMinElements) +
                                             " numbers, found " + std::to_string(instance.values.size()));
    return instance;
}

}  // namespace

InputError::InputError(std::size_t line, std::size_t column, const std::string& message)
    : std::runtime_error(location(line, column) + message), line_(line), column_(column) {}

std::vector<Instance> read_instances(std::istream& in) {
    std::size_t count = read_set_count(in);

    std::vector<Instance> instances;
    std::string line;
    std::size_t line_number = 1;
    while (instances.size() < count) {
        if (!next_line(in, line))
            throw InputError(0, 0, "the first line announces " + std::to_string(count) +
                                       " sets, but the file has only " +
                                       std::to_string(instances.size()));
        ++line_number;
        instances.push_back(read_set(line, line_number));
    }

    // Empty lines at the end are tolerated, anything else is not.
    while (next_line(in, line)) {
        ++line_number;
        if (!line.empty())
            throw InputError(line_number, 0, "the first line announces " + std::to_string(count) +
                                                 " sets, but the file has more lines");
    }
    return instances;
}

std::vector<Instance> read_instances_from_file(const std::string& path) {
    std::ifstream file(path, std::ios::binary);  // binary: \r\n is handled by next_line
    if (!file) throw InputError(0, 0, "cannot open the file '" + path + "'");
    return read_instances(file);
}

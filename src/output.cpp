#include "output.hpp"

#include <cstdlib>
#include <iostream>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <io.h>
#include <windows.h>
#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif
#else
#include <unistd.h>
#endif

namespace output {

namespace {

bool stdout_color = false;
bool stderr_color = false;

constexpr int kStdout = 1;
constexpr int kStderr = 2;

#ifdef _WIN32
// Windows 10 and later understand ANSI codes once the console is asked to.
bool terminal_with_color(int fd) {
    if (!_isatty(fd)) return false;
    HANDLE handle = GetStdHandle(fd == kStdout ? STD_OUTPUT_HANDLE : STD_ERROR_HANDLE);
    DWORD mode = 0;
    if (handle == INVALID_HANDLE_VALUE || !GetConsoleMode(handle, &mode)) return false;
    if (mode & ENABLE_VIRTUAL_TERMINAL_PROCESSING) return true;
    return SetConsoleMode(handle, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING) != 0;
}
#else
bool terminal_with_color(int fd) {
    return isatty(fd) != 0;
}
#endif

std::string_view code(Style style) {
    switch (style) {
        case Style::Plain:   return "";
        case Style::Heading: return "\033[1m";     // bold
        case Style::Yes:     return "\033[1;32m";  // bold green
        case Style::No:      return "\033[1;31m";  // bold red
        case Style::Warning: return "\033[33m";    // yellow
        case Style::Error:   return "\033[1;31m";  // bold red
        case Style::Dim:     return "\033[2m";     // faint
    }
    return "";
}

std::string wrap(std::string_view text, Style style, bool color) {
    std::string_view start = code(style);
    if (!color || start.empty()) return std::string(text);
    return std::string(start) + std::string(text) + "\033[0m";
}

}  // namespace

void init(bool no_color) {
    const char* no_color_env = std::getenv("NO_COLOR");
    bool allowed = !no_color && (no_color_env == nullptr || no_color_env[0] == '\0');
    stdout_color = allowed && terminal_with_color(kStdout);
    stderr_color = allowed && terminal_with_color(kStderr);
}

std::string paint(std::string_view text, Style style) {
    return wrap(text, style, stdout_color);
}

void error(std::string_view message) {
    std::cout.flush();
    std::cerr << wrap("error:", Style::Error, stderr_color) << " " << message << "\n";
}

void warning(std::string_view message) {
    std::cout.flush();
    std::cerr << wrap("warning:", Style::Warning, stderr_color) << " " << message << "\n";
}

template <class V>
std::string format_values(std::span<const V> values, std::size_t max_shown) {
    const std::size_t n = values.size();
    const std::size_t head = n <= max_shown ? n : max_shown - max_shown / 4;
    const std::size_t tail = n <= max_shown ? 0 : max_shown / 4;

    std::string text = "{";
    for (std::size_t i = 0; i < head; ++i) {
        if (i > 0) text += ", ";
        text += to_string(values[i]);
    }
    if (n > max_shown) text += ", ... (" + std::to_string(n - head - tail) + " more) ...";
    for (std::size_t i = n - tail; i < n; ++i) text += ", " + to_string(values[i]);
    return text + "}";
}

#define INSTANTIATE(V) template std::string format_values(std::span<const V>, std::size_t);
FOR_EACH_NUMBER_TYPE(INSTANTIATE)
#undef INSTANTIATE

}  // namespace output

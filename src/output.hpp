// Coloured terminal output (CONSTITUTION point 4). The only module that talks to the
// terminal and the only one with Windows-specific code.
//
// stdout and stderr are decided separately: colours only if the stream is a terminal,
// NO_COLOR is unset and --no-color was not given. On Windows we turn on ANSI processing
// first and fall back to plain text if that fails. The text is the same either way.
#pragma once

#include <cstddef>
#include <span>
#include <string>
#include <string_view>

#include "number.hpp"

namespace output {

enum class Style { Plain, Heading, Yes, No, Warning, Error, Dim };

// Call once, before printing. Decides the colours.
void init(bool no_color);

// Wraps the text in the colour codes of the style, if stdout uses colours.
std::string paint(std::string_view text, Style style);

// "error: <message>" on stderr, in red when stderr uses colours.
void error(std::string_view message);

// "warning: <message>" on stderr, in yellow when stderr uses colours.
void warning(std::string_view message);

// "{a, b, c}". With more than max_shown values: the first and last few, and how many
// were left out.
template <class V>
std::string format_values(std::span<const V> values, std::size_t max_shown = 10);

}  // namespace output

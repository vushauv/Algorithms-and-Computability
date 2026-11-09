// Coloured terminal output (CONSTITUTION point 4). This module is the only
// one that writes to the terminal and the only one with Windows-specific code.
//
// Colours are used on stdout and stderr independently, each only when the
// stream is a terminal, NO_COLOR is not set and --no-color was not given.
// On Windows the console is switched to ANSI processing first; if that is
// not possible the output is plain. The text is the same in both modes.
#pragma once

#include <cstddef>
#include <span>
#include <string>
#include <string_view>

#include "number.hpp"

namespace output {

enum class Style { Plain, Heading, Yes, No, Warning, Error, Dim };

// Decides on colours for stdout and stderr; call once, before printing.
void init(bool no_color);

// The text wrapped in the colour codes of the style when stdout uses colours.
std::string paint(std::string_view text, Style style);

// "error: <message>" on stderr, in red when stderr uses colours.
void error(std::string_view message);

// "warning: <message>" on stderr, in yellow when stderr uses colours.
void warning(std::string_view message);

// "{a, b, c}"; for more than max_shown values the first and last few,
// with the number of the omitted ones in between.
std::string format_values(std::span<const Value> values, std::size_t max_shown = 10);

}  // namespace output

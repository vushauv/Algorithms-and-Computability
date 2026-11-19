// Checks the size of a table before an exact algorithm allocates it.
//
// Linux overcommits: a table bigger than RAM is allocated without error and the
// process is killed later, while filling it. So we refuse up front. On Windows the
// allocation fails with std::bad_alloc right away, so there is nothing to check.
#pragma once

#include <cstdint>
#include <string_view>

#include "number.hpp"

// Physical memory in bytes, 0 if unknown.
std::uint64_t physical_memory_bytes();

// Throws std::runtime_error if the table needs more than the physical memory.
void require_memory(std::string_view algorithm, Value bytes);

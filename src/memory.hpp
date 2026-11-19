// A check of the memory of the exact algorithms before they allocate.
//
// Linux overcommits: an allocation larger than the free memory succeeds and
// the process is killed later, while the table is being filled, instead of
// getting std::bad_alloc. So a table larger than the physical memory is
// refused up front. Windows commits memory at allocation time, where
// std::bad_alloc comes in time, so there the size is unknown and not checked.
#pragma once

#include <cstdint>
#include <string_view>

#include "number.hpp"

// The physical memory of the machine in bytes, or 0 if it is not known.
std::uint64_t physical_memory_bytes();

// Throws std::runtime_error if the table of the algorithm needs more bytes
// than the physical memory has.
void require_memory(std::string_view algorithm, Native bytes);

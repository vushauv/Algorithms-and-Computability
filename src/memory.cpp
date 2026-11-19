#include "memory.hpp"

#include <stdexcept>
#include <string>

#ifdef __linux__
#include <unistd.h>
#endif

std::uint64_t physical_memory_bytes() {
#ifdef __linux__
    const long pages = sysconf(_SC_PHYS_PAGES);
    const long page_size = sysconf(_SC_PAGESIZE);
    if (pages > 0 && page_size > 0) {
        return static_cast<std::uint64_t>(pages) * static_cast<std::uint64_t>(page_size);
    }
#endif
    return 0;
}

void require_memory(std::string_view algorithm, Native bytes) {
    const std::uint64_t physical = physical_memory_bytes();
    if (physical == 0 || bytes <= physical) return;
    constexpr Native kMiB = Native{1} << 20;
    throw std::runtime_error(std::string(algorithm) + " needs " + to_string((bytes + kMiB - 1) / kMiB) +
                             " MiB, the machine has " + to_string(physical / kMiB) + " MiB");
}

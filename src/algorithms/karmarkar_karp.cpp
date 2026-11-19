#include "algorithms/karmarkar_karp.hpp"

#include <cstddef>
#include <queue>
#include <utility>
#include <vector>

Partition karmarkar_karp(std::span<const Value> a) {
    const std::size_t n = a.size();

    // Max-heap of the pairs (a[i], i); ties are broken by the index.
    using Entry = std::pair<Value, std::size_t>;
    std::vector<Entry> entries;
    entries.reserve(n);
    for (std::size_t i = 0; i < n; ++i) entries.emplace_back(a[i], i);
    std::priority_queue<Entry> H(std::less<Entry>{}, std::move(entries));

    // The edges of E as adjacency lists: u and v are in opposite parts.
    std::vector<std::vector<std::size_t>> E(n);
    while (H.size() > 1) {
        const auto [x, u] = H.top();
        H.pop();
        const auto [y, v] = H.top();  // x ≥ y
        H.pop();
        H.emplace(x - y, u);  // the difference keeps the index of the larger element
        E[u].push_back(v);
        E[v].push_back(u);
    }
    const Value d = H.top().first;

    std::vector<int> colour(n, -1);
    std::queue<std::size_t> queue;
    colour[0] = 0;
    queue.push(0);
    while (!queue.empty()) {
        const std::size_t u = queue.front();
        queue.pop();
        for (std::size_t v : E[u]) {
            if (colour[v] == -1) {
                colour[v] = 1 - colour[u];
                queue.push(v);
            }
        }
    }

    Partition result{{}, {}, d};
    for (std::size_t i = 0; i < n; ++i) {
        (colour[i] == 0 ? result.A : result.B).push_back(i);
    }
    return result;
}

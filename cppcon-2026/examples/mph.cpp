// Minimal perfect-hash lookup (qlibs/mph) vs a linear scan, 128 keys.
// Build: g++ -std=c++20 -O3 -mbmi2 -c mph.cpp -o mph.o
// Bench: perf bench func mph_find --exec mph.o --mode latency -e cycles
//   --config.branch=unpredictable --config.memory=cold   # mph stays flat,
//                                                        # the scan explodes
#include <array>
#include <cstdint>

#include "/home/kris/projects/qlibs/mph/mph"

static constexpr auto pairs = [] {
    std::array<std::pair<unsigned, unsigned>, 128> a{};
    for (unsigned i = 0; i < 128; ++i) {
        a[i] = {i * 2654435761u % 100003u, i};
    }
    return a;
}();

extern "C" int mph_find(std::uint64_t key) {
    return mph::lookup<pairs>(key); // pext + single table load, no branches
}

extern "C" int scan_find(std::uint64_t key) {
    for (unsigned i = 0; i < 128; ++i) {
        if (pairs[i].first == key) {
            return pairs[i].second;
        }
    }
    return -1;
}

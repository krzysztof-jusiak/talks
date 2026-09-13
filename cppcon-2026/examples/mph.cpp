// Minimal perfect-hash lookup (qlibs/mph) vs an if/else chain.
// Build: g++ -std=c++20 -O3 -mbmi2 -c mph.cpp -o mph.o
// Bench: perf bench func mph_find --exec mph.o --mode latency -e branch-misses
#include <array>
#include <cstdint>

#include "/home/kris/projects/qlibs/mph/mph"

extern "C" int mph_find(std::uint64_t key) {
    static constexpr std::array ids{
        std::pair{54u, 91u},
        std::pair{64u, 324u},
        std::pair{91u, 234u},
        std::pair{324u, 54u},
        std::pair{234u, 64u},
    };
    return mph::lookup<ids>(key);
}

extern "C" int ifelse_find(std::uint64_t key) {
    if (key == 54) {
        return 91;
    } else if (key == 64) {
        return 324;
    } else if (key == 91) {
        return 234;
    } else if (key == 324) {
        return 54;
    } else if (key == 234) {
        return 64;
    }
    return -1;
}

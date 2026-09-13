#!/usr/bin/env bash
# qlibs/mph: branchless pext+lut lookup vs an if/else chain.
# mph stays flat (~1 cycle, ~0 branch-misses); the chain is data-dependent.
set -euo pipefail
cd "$(dirname "$0")"

g++ -std=c++20 -O3 -mbmi2 -c mph.cpp -o mph.o

echo "### executed asm: single pext + table load, no branches"
perf bench func mph_find --exec mph.o -S | head -n 12

for func in mph_find ifelse_find; do
  echo "### ${func} (unpredictable, branch-misses)"
  perf bench func "${func}" --exec mph.o --mode latency \
    -e cycles,branch-misses --config.branch=unpredictable
done

echo "### data-dependent chain: first key vs last key"
perf bench func ifelse_find --exec mph.o --mode latency \
  -e cycles --data.rdi=54
perf bench func ifelse_find --exec mph.o --mode latency \
  -e cycles --data.rdi=234
perf bench func mph_find --exec mph.o --mode latency \
  -e cycles --data.rdi=54
perf bench func mph_find --exec mph.o --mode latency \
  -e cycles --data.rdi=234

#!/usr/bin/env bash
# qlibs/mph (128 keys): branchless pext+lut vs a linear scan, across
# branch predictability and cache state. mph stays flat (~1-5 cycles);
# the scan pays ~4x for unpredictable branches and ~20-70x for cold memory.
# Numbers below are Alder Lake i7-12650H medians; yours will differ.
set -euo pipefail
cd "$(dirname "$0")"

g++ -std=c++20 -O3 -mbmi2 -c mph.cpp -o mph.o

echo "### executed asm: single pext + table load, no branches"
perf bench func mph_find --exec mph.o -S | head -n 12

for func in mph_find scan_find; do
  for branch in predictable unpredictable; do
    for memory in hot cold; do
      echo "### ${func} branch=${branch} memory=${memory}"
      perf bench func "${func}" --exec mph.o --mode latency \
        -e cycles,branch-misses \
        --config.branch="${branch}" --config.memory="${memory}" \
        -o data/mph
    done
  done
done
perf plot -t ecdf -e cycles -- data/mph

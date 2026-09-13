#!/usr/bin/env bash
# Branch predictability on fizz_buzz (see slides: Branch effects).
# Predictable pins one path; unpredictable samples all four paths.
# Numbers below are Alder Lake i7-12650H medians; yours will differ.
set -euo pipefail
cd "$(dirname "$0")"

g++ -O2 -o fizz fizz.c

echo "### predictable (branch-misses ~0.0/op, cycles ~1.0)"
perf bench func fizz_buzz --exec fizz --mode latency \
  -e cycles,branch-misses --config.branch=predictable

echo "### unpredictable (branch-misses ~1.0/op, cycles ~1-2)"
perf bench func fizz_buzz --exec fizz --mode latency \
  -e cycles,branch-misses --config.branch=unpredictable

echo "### one pinned input (FizzBuzz path)"
perf bench func fizz_buzz --exec fizz --mode latency \
  -e cycles,instructions --data.rdi=15

#!/usr/bin/env bash
# Branch predictability on fizz_buzz (see slides: Branch effects).
# Predictable pins one path; unpredictable samples all four paths.
# Numbers below are Alder Lake i7-12650H medians; yours will differ.
set -euo pipefail
cd "$(dirname "$0")"

g++ -O2 -o fizz fizz.c

echo "### predictable (pinned path, ~0-1 miss/op path-dependent)"
perf bench func fizz_buzz --exec fizz --mode latency \
  -e cycles,branch-misses --config.branch=predictable

echo "### unpredictable (new path each op, ~1 miss/op stable)"
perf bench func fizz_buzz --exec fizz --mode latency \
  -e cycles,branch-misses --config.branch=unpredictable

echo "### one pinned input (FizzBuzz path)"
perf bench func fizz_buzz --exec fizz --mode latency \
  -e cycles,instructions --data.rdi=15

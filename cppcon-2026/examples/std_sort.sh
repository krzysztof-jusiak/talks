#!/usr/bin/env bash
# `std::sort` scaling over input size, reverse-sorted vs shuffled inputs.
# See the deck section `libstdc++.so`.
#
# Adaptations of the slide snippet (which as written cannot run):
#   - `std::sort` has no symbol in `libstdc++.so` (header-only template,
#     see examples/example2.md), so we bench the `std_sort` wrapper from
#     examples/std_sort.cpp (same `(u8*, n)` ABI as examples/sort.cpp,
#     still linked against `libstdc++.so`).
#   - `--name sort-$n` became `--name sort-rev` / `sort-shuf` (one constant
#     label per input pattern): per-n names give a single point per hue so
#     `-t line` cannot draw a scaling line, and sharing one name across both
#     patterns would merge two distributions into one line. Note `perf`
#     appends `-<data_hash>` to `name` on save, so plots group by
#     `config.branch` instead (reverse-sorted input takes predictable
#     branches, shuffled input unpredictable — the same split the deck
#     studies for `fizz_buzz`).
set -euo pipefail
cd "$(dirname "$0")"

rm -f std_sort.so
# -fvisibility=hidden: direct calls only (`perf bench func` has no dynamic
# loader, so PLT/GOT lazy binding would fault); -fno-builtin: keep the local
# `memmove` a plain loop. See std_sort.cpp for details.
g++ -O3 -fno-builtin -shared -fPIC -fvisibility=hidden -o std_sort.so std_sort.cpp

rm -rf ../data/std_sort
for n in 1 4 8 16 32 64 128; do
  rev="$(seq "$n" -1 1 | paste -sd, -)"
  rnd="$(shuf -i "1-$n" | paste -sd, -)"

  perf bench func std_sort --name sort-rev \
    --mode latency \
    --exec std_sort.so \
    --data.arg0=0x10000000 \
    --data.arg1="$n" \
    "--data[0x10000000:]=[${rev}]" \
    --config.branch=predictable \
    --event duration_time \
    --event topdown-retiring,topdown-bad-spec,topdown-fe-bound,topdown-be-bound \
    --output ../data/std_sort

  perf bench func std_sort --name sort-shuf \
    --mode latency \
    --exec std_sort.so \
    --data.arg0=0x10000000 \
    --data.arg1="$n" \
    "--data[0x10000000:]=[${rnd}]" \
    --config.branch=unpredictable \
    --event duration_time \
    --event topdown-retiring,topdown-bad-spec,topdown-fe-bound,topdown-be-bound \
    --output ../data/std_sort
done

# Latency scaling: lines with error bands (seaborn lineplot, mean +/- CI).
perf plot -g config.branch -t line -x data.arg1 -e duration_time \
  -o ../images/sort1.png -- ../data/std_sort

# Top-down L1 breakdown, all four events together (comma form shares one row).
perf plot -g config.branch -t line -x data.arg1 \
  -e topdown-retiring,topdown-bad-spec,topdown-fe-bound,topdown-be-bound \
  -o ../images/sort2.png -- ../data/std_sort

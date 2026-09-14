#!/usr/bin/env bash
# `std::sort` scaling over input size: reverse-sorted vs shuffled inputs
# (predictable vs unpredictable branches) crossed with hot vs cold cache.
# See the deck section `libstdc++.so`.
#
# Adaptations of the slide snippet (which as written cannot run):
#   - `std::sort` has no symbol in `libstdc++.so` (header-only template,
#     see examples/example2.md), so we bench the `std_sort` wrapper from
#     examples/std_sort.cpp (same `(u8*, n)` ABI as examples/sort.cpp,
#     still linked against `libstdc++.so`).
#   - `--name sort-$n` became one constant label per (pattern, cache)
#     combination: per-n names give a single point per hue so `-t line`
#     cannot draw a scaling line, and sharing one name across patterns
#     would merge distributions into one line. Note `perf` appends
#     `-<data_hash>` to `name` on save, so plots group by
#     `config.branch,config.cache` instead (reverse-sorted input takes
#     predictable branches, shuffled input unpredictable — the same split
#     the deck studies for `fizz_buzz`; hot/cold sets the `cache` shortcut).
#   - Events are `cycles,instructions` (one `-e`, comma form shares a single
#     measurement row so `instructions/cycles` = IPC is sample-aligned),
#     not `duration_time`: scaling is shown as cycles, instructions, and IPC.
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

  for cache in hot cold; do
    perf bench func std_sort --name "sort-rev-${cache}" \
      --mode latency \
      --exec std_sort.so \
      --data.arg0=0x10000000 \
      --data.arg1="$n" \
      "--data[0x10000000:]=[${rev}]" \
      --config.branch=predictable \
      --config.cache="${cache}" \
      --event cycles,instructions \
      --output ../data/std_sort

    perf bench func std_sort --name "sort-shuf-${cache}" \
      --mode latency \
      --exec std_sort.so \
      --data.arg0=0x10000000 \
      --data.arg1="$n" \
      "--data[0x10000000:]=[${rnd}]" \
      --config.branch=unpredictable \
      --config.cache="${cache}" \
      --event cycles,instructions \
      --output ../data/std_sort
  done
done

# Cycles scaling: lines with error bands (seaborn lineplot, mean +/- CI).
perf plot -g config.branch,config.cache -t line -x data.arg1 -e cycles \
  -o ../images/sort1.png -- ../data/std_sort

# Instructions scaling: same axes/grouping.
perf plot -g config.branch,config.cache -t line -x data.arg1 -e instructions \
  -o ../images/sort2.png -- ../data/std_sort

# IPC (instructions/cycles): same axes/grouping.
perf plot -g config.branch,config.cache -t line -x data.arg1 -e instructions/cycles \
  -o ../images/sort3.png -- ../data/std_sort

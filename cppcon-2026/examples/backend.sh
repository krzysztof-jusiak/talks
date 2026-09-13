#!/usr/bin/env bash
# Backend bound: compiler axis (gcc vs clang, -O2 vs -O3) crossed with
# cache state (hot vs cold). clang is ~25% faster hot; cold costs ~1.7x
# for both. Numbers below are Alder Lake i7-12650H medians; yours differ.
set -euo pipefail
cd "$(dirname "$0")"

g++ -O2 -std=c++23 -c process.cpp -o process_gcc_O2.o
g++ -O3 -std=c++23 -c process.cpp -o process_gcc_O3.o
clang++ -O2 -std=c++23 -c process.cpp -o process_clang_O2.o
clang++ -O3 -std=c++23 -c process.cpp -o process_clang_O3.o

PAYLOAD="$(python3 -c 'print(",".join(str((i*37)%256) for i in range(256)))')"
rm -rf data/backend
for obj in process_gcc_O2 process_gcc_O3 process_clang_O2 process_clang_O3; do
  for memory in hot cold; do
    echo "### ${obj} memory=${memory}"
    perf bench func process --exec "${obj}.o" --mode latency -e cycles \
      --data.arg0=0x10000000 --data.arg1=256 \
      "--data[0x10000000:]=[${PAYLOAD}]" --config.memory="${memory}" \
      -o data/backend
  done
done
perf plot -t ecdf -e cycles -- data/backend

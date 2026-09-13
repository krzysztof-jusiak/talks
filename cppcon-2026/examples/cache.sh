#!/usr/bin/env bash
# Cache effects: one load (L1d hit vs DRAM miss) and a 256B scan.
# Numbers below are Alder Lake i7-12650H medians; yours will differ.
set -euo pipefail
cd "$(dirname "$0")"

echo "### add r11, [rax]: hot (L1d hit, ~0.5 cycles)"
perf bench asm 'add r11, [rax]' --mode latency -e cycles \
  --config.memory=hot

echo "### add r11, [rax]: cold (DRAM miss, ~5-16 cycles)"
perf bench asm 'add r11, [rax]' --mode latency -e cycles \
  --config.memory=cold

g++ -std=c++23 -O3 -c process.cpp -o process.o
PAYLOAD="$(python3 -c 'print(",".join(str((i*37)%256) for i in range(256)))')"

echo "### process(arr, 256): hot (~750 cycles)"
perf bench func process --exec process.o --mode latency -e cycles \
  --data.arg0=0x10000000 --data.arg1=256 \
  "--data[0x10000000:]=[${PAYLOAD}]" --config.memory=hot

echo "### process(arr, 256): cold (~1200-1360 cycles)"
perf bench func process --exec process.o --mode latency -e cycles \
  --data.arg0=0x10000000 --data.arg1=256 \
  "--data[0x10000000:]=[${PAYLOAD}]" --config.memory=cold

echo "### topdown split for the same function"
perf bench func process --exec process.o --mode latency \
  -e topdown-retiring,topdown-bad-spec,topdown-fe-bound,topdown-be-bound \
  --data.arg0=0x10000000 --data.arg1=256 \
  "--data[0x10000000:]=[${PAYLOAD}]" --config.memory=hot

#!/usr/bin/env bash
# Codegen/layout: function order and alignment knobs.
# This tiny loop fits L1I, so both orders measure ~19 cycles here;
# the effect grows with frontend footprint (see perf studies/frontend_bound).
set -euo pipefail
cd "$(dirname "$0")"

g++ -O3 -c codegen.cpp -o codegen.o
PAYLOAD="$(python3 -c 'print(",".join(str(i) for i in range(64)))')"

echo "### sequential layout"
perf bench func hot_func --exec codegen.o --mode latency -e cycles \
  --data.arg0=0x10000000 --data.arg1=64 \
  "--data[0x10000000:]=[${PAYLOAD}]" \
  --config.function.order=sequential

echo "### randomized layout"
perf bench func hot_func --exec codegen.o --mode latency -e cycles \
  --data.arg0=0x10000000 --data.arg1=64 \
  "--data[0x10000000:]=[${PAYLOAD}]" \
  --config.function.order=random

echo "### 64B alignment"
perf bench func hot_func --exec codegen.o --mode latency -e cycles \
  --data.arg0=0x10000000 --data.arg1=64 \
  "--data[0x10000000:]=[${PAYLOAD}]" \
  --config.function.alignment=64

echo "### save + ECDF (layout is ±4 cycles even on a tiny loop)"
rm -rf data/codegen
for order in sequential random; do
  for alignment in 16 64; do
    perf bench func hot_func --exec codegen.o --mode latency -e cycles \
      --data.arg0=0x10000000 --data.arg1=64 \
      "--data[0x10000000:]=[${PAYLOAD}]" \
      --config.function.order="${order}" \
      --config.function.alignment="${alignment}" \
      -o data/codegen
  done
done
perf plot -t ecdf -e cycles -- data/codegen

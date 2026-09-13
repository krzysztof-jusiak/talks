#!/usr/bin/env bash
# Top-down triage over every function, then one section per bottleneck.
# Expected split (Alder Lake i7-12650H medians; yours will differ):
#   process  61% retiring / 38% backend bound  -> backend bound section
#   branchy  mixed, bad-spec + frontend heavy  -> bad speculation section
#   hot_func 86% retiring                      -> retiring section
#   chain    58% retiring                      -> retiring (dep chain) section
set -euo pipefail
cd "$(dirname "$0")"

g++ -std=c++23 -O3 -c topdown.cpp -o topdown.o

BASE=0x10000000
PAYLOAD="$(python3 -c 'print(",".join(str((i*37)%256) for i in range(256)))')"
PAYLOAD64="$(python3 -c 'print(",".join(str(i) for i in range(64)))')"
EVENTS="topdown-retiring,topdown-bad-spec,topdown-fe-bound,topdown-be-bound"

echo "### all functions, one run each"
perf bench func "process" --exec topdown.o --mode latency -e "${EVENTS}" \
  --data.arg0=${BASE} --data.arg1=256 "--data[${BASE}:]=[${PAYLOAD}]"
perf bench func "branchy" --exec topdown.o --mode latency -e "${EVENTS}"
perf bench func "hot_func" --exec topdown.o --mode latency -e "${EVENTS}" \
  --data.arg0=${BASE} --data.arg1=64 "--data[${BASE}:]=[${PAYLOAD64}]"
perf bench func "chain" --exec topdown.o --mode latency -e "${EVENTS}" \
  --data.rdi=42

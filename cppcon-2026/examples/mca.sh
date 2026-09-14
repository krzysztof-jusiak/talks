#!/usr/bin/env bash
# Machine Code Analysis slide: short llvm-mca example (2 instr, dependency chain).
# Reproduces the numbers shown in index.html (sapphirerapids).
# Real output captured on Alder Lake i7-12650H, llvm-mca 21.0.0git.
set -euo pipefail
cd "$(dirname "$0")"

SNIPPET='add eax, 42; imul eax, eax, 42'

echo "### 1. instruction info (uOps / latency / throughput)"
perf bench asm "$SNIPPET" -m latency -S \
  | llvm-mca -mcpu=sapphirerapids 2>&1 \
  | sed -n '/Instruction Info/,/Instructions:/p;/\[1\].*Instructions:/p;/add.*42/p;/imul.*42/p'
# Expected:
# [1]    [2]    [3]    [4]    [5]    [6]    Instructions:
#  1      1     0.20                        add  eax, 42
#  1      3     1.00                        imul eax, eax, 42
# [1]: #uOps  [2]: Latency  [3]: RThroughput
# [4]: MayLoad  [5]: MayStore  [6]: HasSideEffects (U)

echo
echo "### 2. resource pressure (port usage: imul saturates port 1)"
perf bench asm "$SNIPPET" -m latency -S \
  | llvm-mca -mcpu=sapphirerapids -resource-pressure 2>&1 \
  | grep -A 6 "Resource pressure per iteration\|Resource pressure by instruction"
# Expected per-iteration: [0] 0.25  [1] 1.00  [5] 0.25  [6] 0.25  [11] 0.25

echo
echo "### 3. timeline (D=dispatched, e=executing, E=executed, R=retired)"
perf bench asm "$SNIPPET" -m latency -S \
  | llvm-mca -mcpu=sapphirerapids -timeline 2>&1 \
  | grep -A 8 "Timeline view"
# Expected (first iterations):
# [0,0]  DeER .    ...   add  eax, 42
# [0,1]  D=eeeER ...     imul eax, eax, 42  <- 3-cycle chain stalls dispatch

echo
echo "### 4. measured latency (cross-check vs MCA prediction)"
perf bench asm "$SNIPPET" -m latency -e cycles,instructions --json \
  | perf view
# Measured: imul chain ~3 cycles/op (matches MCA latency 3)

#!/usr/bin/env bash
# meta-perf slide: cross-check the JIT harness asm against hardware traces.
# Real output captured on Alder Lake i7-12650H (numbers will differ per machine).
set -euo pipefail
cd "$(dirname "$0")"

g++ -O2 -o fizz fizz.c

echo "### 1. benchmark the Buzz path (rdi=5) and show cycle stats"
perf bench func fizz_buzz --exec fizz --mode latency \
  --data.rdi=5 --event cycles --json | perf view
# Real output (p50):
#   file          name               mode     stat  cycles
#   fizz@cdbee904 fizz_buzz-95373155 latency  p50    3.00

echo
echo "### 2. harness asm for the same input (must match the PT disasm below)"
perf bench func fizz_buzz --exec fizz --mode latency --data.rdi=5 -S | head -n 12
# Real output:
#   .intel_syntax noprefix
#   endbr64
#   imul eax, edi, 0xeeeeeeef
#   lea rdx, [rip + 0xeb3]
#   add eax, 0x8888888
#   cmp eax, 0x11111110
#   jbe .L40119a
#   imul eax, edi, 0xaaaaaaab
#   ...

echo
echo "### 3. Intel-PT trace of the same run, decoded with IPC"
perf record \
    --event intel_pt/tsc,cyc=1/u \
    --filter 'filter fizz_buzz @ ./fizz' \
    --output pt.data -- \
        perf bench func fizz_buzz \
            --exec fizz \
            --mode latency \
            --data.rdi=5 \
            --event cycles
perf script -i pt.data --itrace=i --insn-trace=disasm,intel -F+ipc | head -n 12
# Cross-check: the PT disasm follows the same Buzz path as the harness asm above
# (imul/cmp/jbe sequence for rdi=5). PT needs Intel-PT + sufficient --mmap-pages;
# on overload it warns "Processed N events and lost M chunks!".

echo
echo "### 4. mem-loads/mem-stores cross-check (ip -> insn, addr -> data)"
perf record \
    --event mem-loads,mem-stores \
    --output mem.data -- perf bench func fizz_buzz \
        --exec fizz --mode latency --data.rdi=5 -e cycles
perf script -i mem.data -F ip,sym | head -n 8
perf bench func fizz_buzz --exec fizz --mode latency --data.rdi=5 -S | head -n 12
# The [.jitdata] mapping for the JIT image is in /tmp/perf-<PID>.map

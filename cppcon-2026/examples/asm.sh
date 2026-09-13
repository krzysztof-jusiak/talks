perf bench asm 'imul eax, 0' --mode latency -e cycles -o a
perf bench asm 'add  eax, 0' --mode latency -e cycles -o a
perf bench asm 'sub  eax, 0' --mode latency -e cycles -o a
perf bench asm 'cdq; idiv ecx;' --mode latency -e cycles -o a

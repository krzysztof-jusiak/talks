#!/usr/bin/env bash
# JIT Code Generation slide: bench.o -> link -> run locally or on a remote server.
# Reproduces the fixed ssh workflow shown in index.html.
set -euo pipefail
cd "$(dirname "$0")"

g++ -O2 -o fizz fizz.c

echo "### 1. emit relocatable object for fizz_buzz"
perf bench func fizz_buzz --exec fizz -c -o bench.o
file bench.o
nm bench.o | grep -E " T (perf_bench_fizz|_Z9fizz_buzz)"

echo "### 2. link into your own driver and run locally"
cat > main.cpp <<'EOF'
#include <cstdio>
const char* fizz_buzz(int);
int main() { return fizz_buzz(15) == nullptr; } // runs, returns 0
EOF
g++ main.cpp bench.o -o a.out -Wl,--allow-multiple-definition
./a.out # -> exit 0 proves the object links and runs

echo "### 3. run the same binary on a remote server"
echo "scp a.out \"\$server:\" && ssh \"\$server\" ./a.out"
# Real remote run (uncomment with a reachable host):
# server="${server:-myserver}"
# scp a.out "$server:" && ssh "$server" ./a.out

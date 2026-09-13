# Examples

Small, runnable examples for every command. All output below was produced on
real hardware (Alder Lake i7-12650H); your numbers will differ, the effects
won't. Every CLI example has a Python equivalent and vice versa.

Setup first (user-space `rdpmc`, needed for everything except `duration_time`):

```sh
echo 2 | sudo tee /sys/devices/cpu_core/rdpmc
```

The helper binary used below:

```sh
cat > fizz.c <<'EOF'
const char* fizz_buzz(int n){
  if(n%15==0) return "FizzBuzz";
  else if(n%3==0) return "Fizz";
  else if(n%5==0) return "Buzz";
  else return "Unknown";
}
int main(){ return 0; }
EOF
g++ -O2 -o fizz fizz.c
```

## 1. `info`: what can be benchmarked

```sh
./bin/perf info cpu -c cpu,core,arch,L1d,L2,L3
./bin/perf info metadata --exec fizz
./bin/perf info metadata --exec fizz --kind func -c name,start,end,size
```

```py
import perf
perf.cpuinfo()[["cpu", "arch", "L1d", "L2", "L3"]].head()
perf.metadata("fizz", "func")[["name", "start", "end"]]
```

## 2. `bench asm`: raw snippets

Latency of one instruction (`repeat` backend: one snippet per iteration):

```sh
./bin/perf bench asm 'mov eax, 42' -m latency --backend repeat
```

Throughput of a snippet, and the generic differential (`unroll`, the default):

```sh
./bin/perf bench asm 'mov eax, 42' -m throughput
./bin/perf bench asm 'mov eax, 42' -m latency --backend unroll --config.unroll_n=3
```

Stateful instructions (`idiv`/`div` take one operand) need `repeat` plus
explicit state:

```sh
./bin/perf bench asm 'idiv ecx' -m latency --backend repeat \
  --data.eax=100 --data.edx=0 --data.ecx=42
```

```py
df = perf.bench(asm="mov eax, 42", mode="latency", backend="repeat")
df = perf.bench(asm="idiv ecx", mode="latency", backend="repeat",
                data={"eax": 100, "edx": 0, "ecx": 42})
```

Any register may be used in a snippet — the harness preserves its loop
registers (`r8`/`r9`/`r10`) and counter baselines on the stack, so even this
is safe (balanced `push`/`pop` in the snippet itself is still required):

```sh
./bin/perf bench asm 'mov r8, 42' -m latency
./bin/perf bench asm 'mov r11, 42' -m throughput
```

## 3. `bench func`: functions inside a binary

```sh
./bin/perf bench func fizz_buzz -m latency --exec fizz --data.rdi=15
./bin/perf bench func fizz_buzz -m throughput --exec fizz --data.arg0=15
```

`rdi`/`arg0` are the same first argument (aliases `arg0..arg5` cover
`rdi,rsi,rdx,rcx,r8,r9`); results carry both `data.rdi` and `data.arg0`
columns either way:

```py
df = perf.bench(exec="fizz", func="fizz_buzz", mode="latency", data={"arg0": 15})
df.filter(regex="^data").iloc[0].to_dict()
```

Result frames carry `data.*` columns for the inputs; the run config is
not expanded into `config.*` columns — it lives in `df.attrs["config"]`
(`df.attrs["data"]` holds the inputs). `config.*` still works in
`plot -x config.<key>`: it is resolved on demand from the stored config.

A custom label (`-n`) names the result instead of the function:

```sh
./bin/perf bench func fizz_buzz -m latency --exec fizz --data.rdi=15 -n buzz15
```

## 4. Hardware effects

Read counters with `-e` (`a,b` shares one measurement, repeated `-e` are
separate measurements merged on `samples`):

Cycle cost of an L1d hit vs a DRAM miss (`repeat` so every iteration misses):

```sh
./bin/perf bench asm 'mov rax, [rdi]' -m latency --backend repeat \
  --data.rdi=0x42000000000 --data[0x42000000000]=123 \
  --config.memory=hot -e cache-misses,cycles
# hot:  cache-misses ~0.0, cycles ~2     (L1d hit)

./bin/perf bench asm 'mov rax, [rdi]' -m latency --backend repeat \
  --data.rdi=0x42000000000 --data[0x42000000000]=123 \
  --config.memory=cold -e cache-misses,cycles
# cold: cache-misses ~1.0, cycles ~350   (DRAM miss)
```

Branch predictability (`branch-misses` drops when the outcome is pinned):

```sh
./bin/perf bench func fizz_buzz -m latency --exec fizz \
  --config.branch=predictable -e branch-misses
./bin/perf bench func fizz_buzz -m latency --exec fizz \
  --config.branch=unpredictable -e branch-misses
```

Instruction-level parallelism — two independent multiplies retire together,
a dependent chain does not (`cycles/instructions` is evaluated by `view`):

```sh
./bin/perf bench asm 'imul eax, ebx, 3; imul ecx, edx, 5' \
  -m latency --backend repeat --data.ebx=7 --data.edx=11 \
  -e cycles,instructions --json | ./bin/perf view -e cycles,instructions,cycles/instructions
```

```py
df = perf.bench(asm="imul eax, ebx, 3; imul ecx, edx, 5", mode="latency",
                backend="repeat", data={"ebx": 7, "edx": 11},
                event="cycles,instructions")
```

## 5. `bench region`: labelled code ranges (C++, Rust, Zig)

```cpp
#include <perf/perf.hpp>   // lib/perf/perf.rs / perf.zig are equivalent

extern "C" int hot(int n) {   // extern "C" keeps the symbol linkable as `hot`
  PERF_LABEL(hot_begin);
  int s = 0;
  for (int i = 0; i < n; ++i) s += i;
  PERF_LABEL(hot_end);
  return s;
}
```

```sh
g++ -O2 -I lib -o hot hot.cpp
./bin/perf info metadata --exec hot --kind region
./bin/perf bench region hot_begin..hot_end -m latency --exec hot --data.rdi=256
```

```py
df = perf.bench(exec="hot", region="hot_begin..hot_end",
                mode="latency", data={"rdi": 256})
```

## 6. Saving, `view`, `plot`

Save under a directory (nothing is printed unless piped/`--json`), then
aggregate and plot. Scaling over an input uses the `data.*` columns every
result carries:

```sh
for n in 8 64 256; do
  ./bin/perf bench func hot -m latency --exec hot --data.rdi=$n \
    -e cycles,instructions -o data/
done
./bin/perf view -- data/
./bin/perf view -g '' -s '' -c data.rdi -e duration_time -- data/
./bin/perf view -e cycles,cycles/instructions -- data/
# dotted data.*/config.* columns need backticks inside --query
./bin/perf view -q '`data.rdi` > 10' -s min,median,max -- data/
./bin/perf plot -t ecdf -e cycles -- data/
./bin/perf plot -t line -x data.rdi -g '' -e duration_time -- data/
```

Speedup of one group over another (sample-aligned baseline):

```sh
./bin/perf view -e instructions/'hot@<hash>'.cycles -- data/
```

```py
df = perf.bench(exec="hot", func="hot", mode="latency", data={"rdi": 64})
perf.plot(df, "ecdf", event="duration_time")
perf.plot(df, "line", event="duration_time", x="data.rdi", groupby=[])
```

## 7. `-S` and `-c`: asm dump and object output

Executed target asm (pipe it to `llvm-mca`); object file for linking into
your own driver:

```sh
./bin/perf bench func fizz_buzz --exec fizz --data.rdi=15 -S | head
./bin/perf bench func fizz_buzz --exec fizz --data.rdi=15 -S | llvm-mca
./bin/perf bench region hot_begin..hot_end --exec hot -S | head
./bin/perf bench func fizz_buzz -m latency --exec fizz -c -o bench.o
```

```c
// driver.c
int hot(int n);
int main(void) { return hot(256); }
```

```sh
gcc -Wl,--allow-multiple-definition driver.c bench.o -o bench_run && ./bench_run
echo $?   # 128 == hot(256) == 32640 mod 256: the object really runs
```

```py
print(perf.target_asm(exec_path="fizz", func="fizz_buzz", data={"rdi": 15}))
perf.write_object(exec_path="fizz", func="fizz_buzz", path="bench.o")
```

## 8. Config and data reference

```sh
# branch + cache + sampling knobs (CLI and dict forms are equivalent)
./bin/perf bench func fizz_buzz -m latency --exec fizz \
  --config.branch=predictable --config.memory=hot \
  --config.samples=200 --config.runs=5 --config.unroll_n=3
```

```py
df = perf.bench(exec="fizz", func="fizz_buzz", mode="latency",
                config={"branch": "predictable", "memory": "hot",
                        "samples": 200, "runs": 5, "unroll_n": 3})
```

```sh
# data: registers, aliases, single addresses, value lists, byte ranges
./bin/perf bench func fizz_buzz -m latency --exec fizz --data.rdi=15
./bin/perf bench func fizz_buzz -m latency --exec fizz --data.arg0=15
./bin/perf bench asm 'mov rax, [rdi]' -m latency \
  --data.rdi=0x10000000 --data[0x10000000:]=[1,2,3,4]
```

```py
df = perf.bench(asm="mov rax, [rdi]", mode="latency",
                data={"rdi": 0x10000000, hex(0x10000000) + ":": [1, 2, 3, 4]})
```

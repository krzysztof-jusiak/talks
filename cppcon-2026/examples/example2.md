# Examples

## `std::sort` via `libstdc++` (header-only)

`std::sort` has no symbol in `libstdc++.so` (header-only template):

```sh
nm -D /lib/x86_64-linux-gnu/libstdc++.so.6 | grep " sort" || echo "no sort symbol"
```

Wrap it in a tiny binary (links `libstdc++.so`):

```cpp
// sort_bench.cpp
#include <algorithm>
extern "C" void bench_sort(int* a, int n) { std::sort(a, a + n); }
int main() { return 0; }
```

```sh
g++ -O2 -o sort_bench sort_bench.cpp
ldd sort_bench | grep stdc++
perf info metadata --exec sort_bench --kind func | grep bench_sort
```

Bench predictable (sorted) vs unpredictable (random) inputs.
Each `int` is 4 bytes, 8 ints need 32 bytes at `0x10000000`:

```sh
# predictable: already sorted
perf bench func bench_sort \
  --exec sort_bench \
  --mode latency \
  --data.arg0=0x10000000 \
  --data.arg1=8 \
  '--data[0x10000000:]=[1,2,3,4,5,6,7,8,1,2,3,4,5,6,7,8,1,2,3,4,5,6,7,8,1,2,3,4,5,6,7,8]' \
  --config.branch=predictable \
  --config.iterations=128 \
  --event duration_time

# unpredictable: shuffled
perf bench func bench_sort \
  --exec sort_bench \
  --mode latency \
  --data.arg0=0x10000000 \
  --data.arg1=8 \
  '--data[0x10000000:]=[5,2,8,1,9,3,7,4,5,2,8,1,9,3,7,4,5,2,8,1,9,3,7,4,5,2,8,1,9,3,7,4]' \
  --config.branch=unpredictable \
  --config.iterations=128 \
  --event branch-misses
```

Python API:

```py
import perf
base = 0x10000000
payload = [5,2,8,1,9,3,7,4]*4  # 32 bytes
df = perf.bench(
    exec="sort_bench",
    func="bench_sort",
    mode="latency",
    data={"arg0": base, "arg1": 8, hex(base)+":": payload},
    config={"branch": "unpredictable", "iterations": 128},
    event="duration_time",
)
print(df.duration_time.describe())
```

Direct `.so` bench also works for exported symbols. Example minimal `.so`:

```sh
cat > minlib.cpp <<'CPP'
extern "C" long myadd(long a, long b) { return a + b; }
CPP
g++ -shared -fPIC -O2 -o minlib.so minlib.cpp
perf bench func myadd --exec minlib.so --mode latency --event duration_time
```

## Dynamic `--config` robustness

Unknown keys fail fast:

```sh
perf bench asm 'nop' --mode latency --config.FFF=1
# perf: error: unknown argument: --config.FFF (...)
```

Layered cache overrides merge instead of discarding the base:

```sh
perf bench asm 'nop' \
  --mode latency \
  --config.cache=cold \
  --config.cache.L1i=100 \
  --debug 2>&1 | head
# cache: {'L1d': 0, 'L2': 0, 'L3': 0, 'L1i': 100}  (cold data, hot L1i)
```

## Linking `obj()` output with `g++`

`perf.obj()` writes a relocatable `.o` with a `perf_bench_*` harness.
Image symbols are local except the target, so it links cleanly:

```py
import perf
perf.obj(exec_path="a.out", func="myfunc", path="bench.o")
```

```sh
cat > driver.cpp <<'CPP'
extern "C" long myfunc(long);
extern "C" long perf_bench_myfunc();
int main() { return (int)(myfunc(41) - 124); }
CPP
g++ driver.cpp bench.o -o driver && ./driver; echo $?
```

## Region on address range

Regions accept `begin..end` as labels, funcs, hex, or mixes with `+/-offset`:

```sh
perf bench region hot_begin..hot_end --exec a.out --mode latency
perf bench region 0x401000..0x401020 --exec a.out --mode latency
perf bench region main..main+0x20 --exec a.out --mode latency
perf bench region main+0x10..0x401020 --exec a.out --mode latency
perf info metadata --exec a.out --kind region
```

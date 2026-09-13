// Codegen/layout example: hot loop plus a cold function to separate layout.
// Build: g++ -O3 -c codegen.cpp -o codegen.o
// Bench: perf bench func hot_func --exec codegen.o --mode latency \
//          --data.arg0=0x10000000 --data.arg1=64 \
//          --config.function.order=sequential   # vs random
extern "C" int hot_func(const int* v, int n) {
    int sum = 0;
    for (int i = 0; i < n; ++i) {
        sum += v[i] & 63;
    }
    return sum;
}

extern "C" int cold_func() {
    volatile int s = 0;
    for (int i = 0; i < 100000; ++i) {
        s += i;
    }
    return s;
}

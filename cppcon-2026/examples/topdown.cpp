// Top-down triage: one function per bottleneck class.
// Build: g++ -std=c++23 -O3 -c topdown.cpp -o topdown.o
// Bench: perf bench func ".*" --exec topdown.o --mode latency \
//          -e topdown-retiring,topdown-bad-spec,topdown-fe-bound,topdown-be-bound
extern "C" long process(const unsigned char* arr, unsigned long n) {
    long total = 0; // backend bound: streams memory + branch
    for (unsigned long i = 0; i < n; ++i) {
        unsigned char v = arr[i];
        if (v & 1) {
            total += v;
        } else {
            total -= v;
        }
    }
    return total;
}

extern "C" int branchy(int n) { // bad speculation: modulo dispatch
    if (n % 15 == 0) {
        return 0;
    } else if (n % 3 == 0) {
        return 1;
    } else if (n % 5 == 0) {
        return 2;
    }
    return 3;
}

extern "C" int hot_func(const int* v, int n) { // retiring: tight loop
    int sum = 0;
    for (int i = 0; i < n; ++i) {
        sum += v[i] & 63;
    }
    return sum;
}

extern "C" long chain(long x) { // retiring: serial dependency chain
    x = x * 31 + 7;
    x = x * 31 + 7;
    x = x * 31 + 7;
    x = x * 31 + 7;
    x = x * 31 + 7;
    return x;
}

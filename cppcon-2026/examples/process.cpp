// Backend-bound example: strided accumulation with a data-dependent branch.
// Build: g++ -std=c++23 -O3 -c process.cpp -o process.o
// Bench: perf bench func process --exec process.o --mode latency \
//          --data.arg0=0x10000000 --data.arg1=256 \
//          "--data[0x10000000:]=[$(python3 -c 'print(",".join(str((i*37)%256) for i in range(256)))')]"
extern "C" long process(const unsigned char* arr, unsigned long n) {
    long total = 0;
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

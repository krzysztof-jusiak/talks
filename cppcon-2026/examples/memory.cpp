#include <perf/perf.hpp>

extern "C" {

int arr[64];

void init() {
    for (int i = 0; i < 64; ++i) {
        arr[i] = i * 37;
    }
}

int process(int n) {
    int sum = 0;
    for (int i = 0; i < n; ++i) {
        int val = arr[i & 63];
        if (val & 1) {
            sum += val;
        } else {
            sum -= val;
        }
    }
    return sum;
}

}

int main() {}

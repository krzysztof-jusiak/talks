// The MIT License (MIT)
//
// Copyright (c) 2026 Kris Jusiak <kris@jusiak.net>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

// `std::sort` is a header-only template: it has no symbol in `libstdc++.so`
// (see examples/example2.md), so bench this thin wrapper instead (it still
// links `libstdc++.so`). Same `(data, n)` ABI as examples/sort.cpp so the
// numbers are directly comparable.
//
// NOTE: `perf bench func` maps the target object into its own address space
// without a dynamic loader, so PLT/GOT calls (lazy binding) fault. Build
// this file with `-fvisibility=hidden` so every call is direct, and provide
// a local `memmove` (used by libstdc++ to shift elements) for the same
// reason. Build with `-fno-builtin` so the local `memmove` itself stays a
// plain loop instead of being turned back into a `memmove` call.

#include <algorithm>
#include <cstddef>
#include <cstdint>

extern "C" __attribute__((visibility("hidden"))) void* memmove(void* d,
                                                               const void* s,
                                                               std::size_t n) {
    auto* dst = static_cast<unsigned char*>(d);
    const auto* src = static_cast<const unsigned char*>(s);
    if (dst < src) {
        for (std::size_t i = 0; i < n; ++i) {
            dst[i] = src[i];
        }
    } else if (dst > src) {
        for (std::size_t i = n; i-- > 0;) {
            dst[i] = src[i];
        }
    }
    return d;
}

int64_t std_sort(std::uint8_t* data, std::size_t n) {
    std::sort(data, data + n);
    return n ? data[n - 1] : 0;
}

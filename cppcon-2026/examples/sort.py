import perf

                #["topdown-retiring", "topdown-bad-spec", "topdown-fe-bound", "topdown-be-bound"],
for size in (1, 4):
    for branch in ("predictable", "unpredictable"):
        for cache in ("hot", "cold"):
            perf.bench(
                exec="/tmp/sort.o", 
                func="sort",
                label=f"sort-{branch}-{cache}-{size}",
                mode="latency",
                config={"branch": branch, "cache": cache},
                data={"arg1": 0},
                event=["cycles", "instructions"],
            )

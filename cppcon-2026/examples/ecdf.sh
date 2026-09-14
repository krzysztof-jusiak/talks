#!/usr/bin/env bash
# Regenerate every ECDF chart referenced in the CppCon 2026 deck.
#
# Each chart is plotted from measured data:
#   - data/cache, data/fizz, data/codegen, data/backend, data/mph (perf bench -o ...)
#   - perf.data                                   (perf record -e cycles ...)
#
# The per-topic scripts (cache.sh, fizz.sh, codegen.sh, backend.sh, mph.sh)
# re-generate the data/ trees; this script only re-plots the images.
#
# Plots are written next to the deck so `npm run build`/`serve` picks them up.
set -euo pipefail
cd "$(dirname "$0")/.."   # repo root: data/ and images/ resolve from here

ROOT="$(pwd)"
[ -d "${ROOT}/images" ] || mkdir -p "${ROOT}/images"

# Pin figure size so re-runs are stable regardless of the perf default.
PLOT_CONFIG="$(mktemp)"
trap 'rm -f "${PLOT_CONFIG}"' EXIT
cat > "${PLOT_CONFIG}" <<'JSON'
{ "figure.figsize": [11, 5], "figure.dpi": 100 }
JSON

chart() { # chart event data...
  local name="$1" event="$2"
  shift 2
  for src in "$@"; do
    if [ ! -e "${ROOT}/${src}" ]; then
      echo "skip ${name} (missing ${src})"
      return
    fi
  done
  echo "### ${name} <- ${event} ${*}"
  perf plot --config "${PLOT_CONFIG}" -t ecdf -e "${event}" \
    -o "${ROOT}/images/${name}" -- "$@"
}

chart cache_lat.png   cycles        data/cache
chart branch_miss.png branch-misses data/fizz
chart codegen_ecdf.png cycles       data/codegen
chart fizz_ecdf.png   cycles        data/fizz
chart backend_ecdf.png cycles       data/backend
chart mph_ecdf.png    cycles        data/mph

# perf.data is a `perf record` output; create one from a real workload if absent.
if [ ! -e "${ROOT}/perf.data" ]; then
  echo "### record perf.data (a.out, cycles + branch/cache misses)"
  g++ -O2 -o /tmp/.perf_a.out "${ROOT}/examples/trace.cpp"
  perf record -e cycles,branch-misses,cache-misses,cache-references \
    -o "${ROOT}/perf.data" -- /tmp/.perf_a.out 2000000000
  rm -f /tmp/.perf_a.out
fi
chart perf_data.png cycles perf.data

echo "done: $(ls -1 "${ROOT}"/images/*.png | wc -l) images"
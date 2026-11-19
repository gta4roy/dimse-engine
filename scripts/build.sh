#!/usr/bin/env bash
# Configure and build dimse-engine against an out-of-tree DCMTK install.
# Does not compile DCMTK — expects shared libs already present under $DCMTK_LIB.
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${BUILD_DIR:-${ROOT}/build}"

if [[ -z "${DCMTK_LIB:-}" ]]; then
  echo "error: DCMTK_LIB is not set" >&2
  echo "  export DCMTK_LIB=/path/to/dcmtk/install" >&2
  echo "  (prefix containing include/ and lib/)" >&2
  exit 1
fi

if [[ ! -d "${DCMTK_LIB}/include/dcmtk" ]]; then
  echo "error: ${DCMTK_LIB}/include/dcmtk not found" >&2
  exit 1
fi

if [[ ! -d "${DCMTK_LIB}/lib" ]]; then
  echo "error: ${DCMTK_LIB}/lib not found" >&2
  exit 1
fi

echo "DCMTK_LIB=${DCMTK_LIB}"
echo "BUILD_DIR=${BUILD_DIR}"

cmake -S "${ROOT}" -B "${BUILD_DIR}" -DDCMTK_LIB="${DCMTK_LIB}"
cmake --build "${BUILD_DIR}" -j"$(nproc)"

echo
echo "Built: ${BUILD_DIR}/dimse-engine"
echo "Runtime libs: ${DCMTK_LIB}/lib (rpath embedded)"

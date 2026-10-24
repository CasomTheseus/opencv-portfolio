#!/usr/bin/env bash
# Build all three projects. CMake is resolved from PATH first, then CLion's
# bundled copy (handy on machines that only have the IDE installed).
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT"

CMAKE_BIN="${CMAKE_BIN:-}"
if [[ -z "$CMAKE_BIN" ]]; then
    if command -v cmake >/dev/null 2>&1; then
        CMAKE_BIN="$(command -v cmake)"
    elif [[ -x "/Applications/CLion.app/Contents/bin/cmake/mac/aarch64/bin/cmake" ]]; then
        CMAKE_BIN="/Applications/CLion.app/Contents/bin/cmake/mac/aarch64/bin/cmake"
    elif [[ -x "/Applications/CLion.app/Contents/bin/cmake/mac/x86_64/bin/cmake" ]]; then
        CMAKE_BIN="/Applications/CLion.app/Contents/bin/cmake/mac/x86_64/bin/cmake"
    else
        echo "cmake not found. Install it with 'brew install cmake' or set CMAKE_BIN." >&2
        exit 1
    fi
fi

BUILD_TYPE="${BUILD_TYPE:-Release}"
JOBS="${JOBS:-$(sysctl -n hw.ncpu 2>/dev/null || nproc 2>/dev/null || echo 4)}"

echo "==> cmake: $CMAKE_BIN"
echo "==> build: $BUILD_TYPE, $JOBS jobs"
"$CMAKE_BIN" -S "$ROOT" -B "$ROOT/build" -DCMAKE_BUILD_TYPE="$BUILD_TYPE"
"$CMAKE_BIN" --build "$ROOT/build" -j "$JOBS"

echo "==> binaries:"
ls -1 "$ROOT/build/bin"

#!/usr/bin/env sh
# Build the native library of LiteScript.Net for the current platform and copy it in
# dotnet/artifacts/runtimes/<rid>/native (used by the tests and by "dotnet pack")
#
# Usage: dotnet/build-native.sh [rid] [-- meson options...]
#   rid   The runtime identifier (detected by default: linux-x64, linux-arm64, osx-arm64...)
#
# Needs Meson and Ninja (python -m pip install meson ninja) and a C++20 compiler.

set -e

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
RID=""
if [ $# -gt 0 ] && [ "$1" != "--" ]; then
    RID="$1"
    shift
fi
if [ $# -gt 0 ] && [ "$1" = "--" ]; then
    shift
fi

if [ -z "$RID" ]; then
    case "$(uname -s)" in
        Linux) OS=linux ;;
        Darwin) OS=osx ;;
        *) echo "Unknown system, give the runtime identifier"; exit 2 ;;
    esac
    case "$(uname -m)" in
        x86_64|amd64) ARCH=x64 ;;
        aarch64|arm64) ARCH=arm64 ;;
        *) echo "Unknown architecture, give the runtime identifier"; exit 2 ;;
    esac
    RID="$OS-$ARCH"
fi

if command -v meson >/dev/null 2>&1; then
    MESON="meson"
else
    MESON="python3 -m mesonbuild.mesonmain"
fi

BUILD_DIR="$ROOT/builddir/dotnet-$RID"
OPTIONS="--buildtype=release -Ddotnet=true -Dtests=false -Dexamples=false -Dplayground=false -Dbenchmarks=false"

if [ ! -f "$BUILD_DIR/meson-private/coredata.dat" ]; then
    $MESON setup "$BUILD_DIR" "$ROOT" $OPTIONS "$@"
elif [ $# -gt 0 ]; then
    $MESON configure "$BUILD_DIR" "$@"
fi
$MESON compile -C "$BUILD_DIR"

OUT_DIR="$ROOT/dotnet/artifacts/runtimes/$RID/native"
mkdir -p "$OUT_DIR"
for LIB in "$BUILD_DIR"/dotnet/native/liblitescript_net.so "$BUILD_DIR"/dotnet/native/liblitescript_net.dylib; do
    if [ -f "$LIB" ]; then
        cp "$LIB" "$OUT_DIR/"
        echo "Native library: $OUT_DIR/$(basename "$LIB")"
    fi
done

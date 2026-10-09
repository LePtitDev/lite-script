#!/usr/bin/env sh
# Build the library, the tests, the examples and the playground (litescript-pg)
#
# Usage: scripts/build.sh [release|debug|debugoptimized] [clean] [test] [-- meson options...]
#   release, debug...  Build type (release by default), each type has its directory builddir/<type>
#   clean              Delete the build directory before building
#   test               Run the unit tests after the build
#   -- options         Options given to "meson setup" (or "meson configure" if already configured),
#                      for example: scripts/build.sh -- -Dtests=false -Dhttp=disabled
#
# Needs Meson and Ninja (python3 -m pip install meson ninja) and a C++20 compiler,
# the HTTP client of the playground uses libcurl (libcurl4-openssl-dev on Debian/Ubuntu).

set -eu

ROOT=$(cd "$(dirname "$0")/.." && pwd)
BUILD_TYPE=release
CLEAN=0
TEST=0

usage() {
    echo "Usage: scripts/build.sh [release|debug|debugoptimized] [clean] [test] [-- meson options...]"
}

while [ $# -gt 0 ]; do
    case "$1" in
        release|debug|debugoptimized) BUILD_TYPE=$1 ;;
        clean) CLEAN=1 ;;
        test) TEST=1 ;;
        -h|--help) usage; exit 0 ;;
        --) shift; break ;;
        *) echo "Unknown argument \"$1\""; usage; exit 2 ;;
    esac
    shift
done
# The remaining arguments are the options of Meson

BUILD_DIR="$ROOT/builddir/$BUILD_TYPE"

# Find Meson: the command "meson" or the Python module
if command -v meson >/dev/null 2>&1; then
    MESON=meson
else
    MESON=
    for python in python3 python; do
        if command -v "$python" >/dev/null 2>&1 && "$python" -c "import mesonbuild" >/dev/null 2>&1; then
            MESON="$python -m mesonbuild.mesonmain"
            break
        fi
    done
    if [ -z "$MESON" ]; then
        echo "Meson isn't installed, install it with: python3 -m pip install meson ninja"
        exit 1
    fi
fi

if [ "$CLEAN" = 1 ] && [ -d "$BUILD_DIR" ]; then
    echo "Deleting $BUILD_DIR"
    rm -rf "$BUILD_DIR"
fi

if [ ! -f "$BUILD_DIR/meson-private/coredata.dat" ]; then
    $MESON setup "$BUILD_DIR" "$ROOT" --buildtype="$BUILD_TYPE" "$@"
elif [ $# -gt 0 ]; then
    $MESON configure "$BUILD_DIR" "$@"
fi

$MESON compile -C "$BUILD_DIR"

if [ "$TEST" = 1 ]; then
    $MESON test -C "$BUILD_DIR" --print-errorlogs
fi

if [ -f "$BUILD_DIR/playground/litescript-pg" ]; then
    echo
    echo "Playground: $BUILD_DIR/playground/litescript-pg"
fi

#!/usr/bin/env sh
# Build and run the unit tests (same arguments as build.sh)
#
# Usage: scripts/test.sh [release|debug|debugoptimized] [clean] [-- meson options...]

exec "$(dirname "$0")/build.sh" test "$@"

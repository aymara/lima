#!/usr/bin/env bash
#
# Copyright 2026 CEA LIST
# SPDX-License-Identifier: MIT
#
# populate-hwcaps.sh OPTIMIZED_PREFIX BASELINE_PREFIX [HWCAPS_LEVEL]
#
# Runtime CPU dispatch for the whole LIMA C++ codebase without any dispatcher
# code, using glibc-hwcaps (glibc >= 2.33). For every shared library produced by
# an *optimized* build (e.g. -march=x86-64-v3), install a copy into the
# "glibc-hwcaps/<level>/" subdirectory next to its *baseline* counterpart. The
# dynamic loader then auto-selects the optimized library on CPUs that advertise
# that micro-architecture level and transparently falls back to the baseline
# library everywhere else (older CPUs, older glibc). Same ABI/soname, so the two
# copies are drop-in interchangeable.
#
# Typical use (two installs of the same source tree):
#   cmake ... -DWITH_ARCH=OFF ..                       # baseline (-msse4.2)
#   ninja && ninja install                             # -> BASELINE_PREFIX
#   cmake ... -DLIMA_ARCH_FLAGS="-march=x86-64-v3" ..  # optimized
#   ninja && DESTDIR=... ninja install                 # -> OPTIMIZED_PREFIX
#   populate-hwcaps.sh OPTIMIZED_PREFIX BASELINE_PREFIX x86-64-v3

set -euo pipefail

OPT="${1:?usage: populate-hwcaps.sh OPTIMIZED_PREFIX BASELINE_PREFIX [HWCAPS_LEVEL]}"
BASE="${2:?usage: populate-hwcaps.sh OPTIMIZED_PREFIX BASELINE_PREFIX [HWCAPS_LEVEL]}"
LEVEL="${3:-x86-64-v3}"

if [ ! -d "$OPT" ] || [ ! -d "$BASE" ]; then
  echo "populate-hwcaps: OPTIMIZED_PREFIX and BASELINE_PREFIX must both exist" >&2
  exit 1
fi

count=0
# Mirror every shared object (real files *and* the soname/dev symlinks) into the
# matching glibc-hwcaps/<level>/ directory, preserving the relative layout so the
# soname symlink and its target land side by side and resolve there.
while IFS= read -r -d '' rel; do
  rel="${rel#./}"
  dir="$(dirname "$rel")"
  name="$(basename "$rel")"
  dest="$BASE/$dir/glibc-hwcaps/$LEVEL"
  mkdir -p "$dest"
  # -a preserves symlinks as symlinks (relative, same-dir targets are copied too).
  cp -a "$OPT/$rel" "$dest/$name"
  count=$((count + 1))
done < <(cd "$OPT" && find . \( -type f -o -type l \) \( -name '*.so' -o -name '*.so.*' \) -print0)

echo "populate-hwcaps: installed $count shared-object entries into $LEVEL under $BASE"

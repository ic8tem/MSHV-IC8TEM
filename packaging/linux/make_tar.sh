#!/bin/sh
# Portable folder: run ./mshv-ic8tem from the extracted folder (data stay inside it,
# like the Windows version). Needs Qt5, ALSA, PulseAudio and FFTW3 from the system.
# usage: make_tar.sh <outdir> <name-suffix>   e.g. dist linux-x86_64
set -e
OUT="$1"; SUF="$2"; V="${MSHV_VERSION:-dev}"
T="$(mktemp -d)"; D="$T/MSHV-IC8TEM"
mkdir -p "$D" "$OUT"
cp -a bin/. "$D/"
find "$D" -name .gitkeep -delete
cp COPYING.txt README.md CHANGES_IC8TEM.md "$D/"
cp packaging/linux/README-UNIX.txt "$D/" 2>/dev/null || true
printf '%s' "MSHV-IC8TEM-{V}-$SUF.tar.gz" > "$D/mshv-package.txt"   # for the updater
tar -C "$T" -czf "$OUT/MSHV-IC8TEM-$V-$SUF.tar.gz" MSHV-IC8TEM
rm -rf "$T"
ls -la "$OUT"

#!/bin/sh
# Build MSHV-IC8TEM on Linux/BSD with qmake.
# usage: build_unix.sh <file.pro> [extra qmake -after assignments...]
# env:   QMAKE (default: first qmake found), MAKE (default: make)
set -e
PRO="$1"; shift
if [ -z "$QMAKE" ]; then
  for q in /usr/lib/qt5/bin/qmake /usr/lib/*/qt5/bin/qmake /usr/bin/qmake-qt5 /usr/local/lib/qt5/bin/qmake /usr/local/bin/qmake-qt5 qmake; do
    if command -v "$q" >/dev/null 2>&1; then QMAKE="$q"; break; fi
  done
fi
JOBS="$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 2)"
echo "qmake: $QMAKE  jobs: $JOBS"
"$QMAKE" -v
"$QMAKE" "$PRO" -after "TARGET=mshv-ic8tem" "$@"
${MAKE:-make} -j"$JOBS"
test -x bin/mshv-ic8tem
echo "built bin/mshv-ic8tem"

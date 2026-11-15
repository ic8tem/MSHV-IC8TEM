#!/bin/sh
# usage: make_deb.sh <outdir> <deb-arch> <name-suffix>
set -e
OUT="$1"; DA="$2"; SUF="$3"; V="${MSHV_VERSION:-0.0}"
DV="$(echo "$V" | sed 's/-ic8tem\./+ic8tem/')"
T="$(mktemp -d)"; R="$T/root"
sh packaging/linux/stage_root.sh "$R"
printf '%s' "MSHV-IC8TEM-{V}-$SUF.deb" > "$R/opt/mshv-ic8tem/mshv-package.txt"   # for the updater
mkdir -p "$T/debian" "$R/DEBIAN" "$OUT"
printf 'Source: mshv-ic8tem\n\nPackage: mshv-ic8tem\nArchitecture: any\n' > "$T/debian/control"
DEPS="$(cd "$T" && dpkg-shlibdeps -O "-e$R/opt/mshv-ic8tem/mshv-ic8tem" 2>/dev/null | sed 's/^shlibs:Depends=//')"
SIZE="$(du -sk "$R" | cut -f1)"
cat > "$R/DEBIAN/control" <<EOC
Package: mshv-ic8tem
Version: $DV
Architecture: $DA
Maintainer: IC8TEM <ic8tem@gmail.com>
Installed-Size: $SIZE
Depends: $DEPS
Section: hamradio
Priority: optional
Homepage: https://github.com/ic8tem/MSHV-IC8TEM
Description: MSHV-IC8TEM - weak signal digital modes for amateur radio
 Modified version of MSHV 2.76.7 by LZ2HV: Doppler tracking (CFOM) via CAT,
 LoTW users marking and filter, fictitious log reset and more (IC8TEM).
 User data are kept in ~/.local/share/MSHV-IC8TEM.
EOC
dpkg-deb --root-owner-group --build "$R" "$OUT/MSHV-IC8TEM-$V-$SUF.deb"
rm -rf "$T"

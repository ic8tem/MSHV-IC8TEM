#!/bin/sh
# Slackware package (.txz) from the staged tree.
# usage: make_slackpkg.sh <outdir> <slack-arch>
set -e
OUT="$(mkdir -p "$1" && cd "$1" && pwd)"; SA="$2"; V="${MSHV_VERSION:-0.0}"
SV="$(echo "$V" | tr '-' '_')"
T="$(mktemp -d)"; R="$T/root"
sh packaging/linux/stage_root.sh "$R"
printf '%s' "mshv-ic8tem-{VU}-$SA-1_ic8tem.txz" > "$R/opt/mshv-ic8tem/mshv-package.txt"   # for the updater
mkdir -p "$R/install" "$OUT"
cat > "$R/install/slack-desc" <<'EOD'
           |-----handy-ruler------------------------------------------------------|
mshv-ic8tem: mshv-ic8tem (MSHV with Doppler tracking and LoTW, IC8TEM mod)
mshv-ic8tem:
mshv-ic8tem: Modified version of MSHV 2.76.7 by LZ2HV for weak signal digital
mshv-ic8tem: modes (FT8, FT4, Q65, MSK144, JT65 ...): Doppler tracking (CFOM)
mshv-ic8tem: via CAT, LoTW users marking and filter, fictitious log reset.
mshv-ic8tem:
mshv-ic8tem: Requires: qt5, alsa-lib, pulseaudio, fftw.
mshv-ic8tem: User data: ~/.local/share/MSHV-IC8TEM
mshv-ic8tem:
mshv-ic8tem: https://github.com/ic8tem/MSHV-IC8TEM
mshv-ic8tem:
EOD
( cd "$R" && tar --owner=0 --group=0 -cJf "$OUT/mshv-ic8tem-$SV-$SA-1_ic8tem.txz" . )
rm -rf "$T"

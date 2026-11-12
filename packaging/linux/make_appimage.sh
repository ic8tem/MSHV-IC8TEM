#!/bin/sh
# Self-contained AppImage (bundles Qt). User data in ~/.local/share/MSHV-IC8TEM.
# usage: make_appimage.sh <outdir> <arch: x86_64|aarch64>
set -e
SRC="$(pwd)"; OUT="$(mkdir -p "$1" && cd "$1" && pwd)"; A="$2"; V="${MSHV_VERSION:-dev}"
T="$(mktemp -d)"; AD="$T/AppDir"
mkdir -p "$AD/usr/bin" "$AD/usr/share/mshv-ic8tem"
cp bin/mshv-ic8tem "$AD/usr/bin/"
cp -a bin/. "$AD/usr/share/mshv-ic8tem/"
rm -f "$AD/usr/share/mshv-ic8tem/mshv-ic8tem"
find "$AD" -name .gitkeep -delete
cat > "$T/AppRun" <<'EOS'
#!/bin/sh
HERE="$(dirname "$(readlink -f "$0")")"
export MSHV_DATA_DIR="${XDG_DATA_HOME:-$HOME/.local/share}/MSHV-IC8TEM"
export MSHV_SEED_DIR="$HERE/usr/share/mshv-ic8tem"
exec "$HERE/usr/bin/mshv-ic8tem" "$@"
EOS
chmod 755 "$T/AppRun"
cd "$T"
wget -q "https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-$A.AppImage"
wget -q "https://github.com/linuxdeploy/linuxdeploy-plugin-qt/releases/download/continuous/linuxdeploy-plugin-qt-$A.AppImage"
chmod +x linuxdeploy-*.AppImage
export APPIMAGE_EXTRACT_AND_RUN=1
export QMAKE="${QMAKE:-$(ls /usr/lib/qt5/bin/qmake /usr/lib/*/qt5/bin/qmake 2>/dev/null | head -1)}"
export LDAI_OUTPUT="MSHV-IC8TEM-$V-linux-$A.AppImage"
export OUTPUT="$LDAI_OUTPUT"
"./linuxdeploy-$A.AppImage" --appdir "$AD" \
   -d "$SRC/packaging/common/mshv-ic8tem.desktop" \
   -i "$SRC/packaging/common/mshv-ic8tem.png" \
   --custom-apprun "$T/AppRun" \
   --plugin qt --output appimage
cd "$SRC"
mv "$T"/MSHV-IC8TEM-*.AppImage "$OUT/"
rm -rf "$T"

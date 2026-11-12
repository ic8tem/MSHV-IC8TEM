#!/bin/sh
# Stage an installed tree (for .deb/.rpm/Slackware) from the built bin/ folder.
# usage: stage_root.sh <rootdir>
#   /opt/mshv-ic8tem/            binary + seed data (settings, ...)
#   /usr/bin/mshv-ic8tem         launcher: user data in ~/.local/share/MSHV-IC8TEM
set -e
R="$1"
mkdir -p "$R/opt/mshv-ic8tem" "$R/usr/bin" "$R/usr/share/applications" \
         "$R/usr/share/icons/hicolor/256x256/apps" "$R/usr/share/doc/mshv-ic8tem"
cp -a bin/. "$R/opt/mshv-ic8tem/"
find "$R/opt/mshv-ic8tem" -name .gitkeep -delete
cat > "$R/usr/bin/mshv-ic8tem" <<'EOS'
#!/bin/sh
export MSHV_DATA_DIR="${XDG_DATA_HOME:-$HOME/.local/share}/MSHV-IC8TEM"
export MSHV_SEED_DIR=/opt/mshv-ic8tem
exec /opt/mshv-ic8tem/mshv-ic8tem "$@"
EOS
chmod 755 "$R/usr/bin/mshv-ic8tem"
cp packaging/common/mshv-ic8tem.desktop "$R/usr/share/applications/"
cp packaging/common/mshv-ic8tem.png "$R/usr/share/icons/hicolor/256x256/apps/"
cp COPYING.txt README.md CHANGES_IC8TEM.md "$R/usr/share/doc/mshv-ic8tem/"

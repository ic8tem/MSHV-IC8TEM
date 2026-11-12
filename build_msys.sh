#!/bin/bash
# Compilazione MSHV dentro MSYS2/MINGW64 (chiamato da compila.bat)
export PATH="/mingw64/bin:/mingw64/qt5/bin:/mingw64/lib/qt5/bin:$PATH"
cd "$(dirname "$0")" || exit 1

findtool() { # $1 = nome base (qmake, windeployqt)
  for c in "$1-qt5" "$1"; do
    p=$(command -v "$c" 2>/dev/null) && { echo "$p"; return 0; }
  done
  for p in /mingw64/qt5/bin/$1.exe /mingw64/lib/qt5/bin/$1.exe /mingw64/bin/$1-qt5.exe; do
    [ -x "$p" ] && { echo "$p"; return 0; }
  done
  return 1
}

QMAKE=$(findtool qmake)
if [ -z "$QMAKE" ]; then
  echo "qmake di Qt5 non trovato. File presenti:"
  pacman -Ql mingw-w64-x86_64-qt5-base | grep -i "qmake" | head
  exit 1
fi
echo "Uso qmake: $QMAKE"
"$QMAKE" -v | head -2

"$QMAKE" MSHV_WIN64.pro -after "QMAKE_CXXFLAGS -= -pedantic-errors" || exit 1
mingw32-make -j"$(nproc)" || exit 1

WDQ=$(findtool windeployqt)
if [ -n "$WDQ" ]; then
  "$WDQ" --release --no-translations --no-angle --no-opengl-sw bin/MSHV_WIN64.exe || echo "(windeployqt: avviso ignorato)"
else
  echo "windeployqt non trovato: copio solo le DLL con ldd"
fi
# copia tutte le DLL MinGW/Qt necessarie (3 passaggi per le dipendenze annidate)
for i in 1 2 3; do
  for f in bin/*.exe bin/*.dll bin/*/*.dll; do ldd "$f" 2>/dev/null; done \
    | grep -i '/mingw64/' | awk '{print $3}' | sort -u | xargs -r cp -u -t bin/
done
# plugin piattaforma, se windeployqt non c'era
if [ ! -f bin/platforms/qwindows.dll ]; then
  P=$(ls /mingw64/share/qt5/plugins/platforms/qwindows.dll /mingw64/qt5/plugins/platforms/qwindows.dll /mingw64/lib/qt5/plugins/platforms/qwindows.dll 2>/dev/null | head -1)
  [ -n "$P" ] && mkdir -p bin/platforms && cp -u "$P" bin/platforms/
fi
exit 0

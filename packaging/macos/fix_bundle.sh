#!/bin/bash
# MSHV-IC8TEM: complete a macdeployqt bundle.
# macdeployqt does not follow @rpath dependencies of the libraries it copies
# (e.g. libbrotlidec -> @rpath/libbrotlicommon). This copies every missing
# Homebrew library into Contents/Frameworks, points all references to
# @executable_path/../Frameworks, and fails if anything still points outside
# the bundle or the system.
set -u
APP="$1"
FW="$APP/Contents/Frameworks"
BREW="$(brew --prefix)"
mkdir -p "$FW"

machos() {
  find "$APP/Contents" -type f \( -name "*.dylib" -o -perm -u+x \) | while read -r f; do
    file "$f" | grep -q "Mach-O" && echo "$f"
  done
}
find_src() {   # library name -> path in Homebrew
  local n="$1" p
  for p in "$BREW/lib/$n" "$BREW"/opt/*/lib/"$n" "$BREW"/Cellar/*/*/lib/"$n"; do
    [ -f "$p" ] && { echo "$p"; return 0; }
  done
  return 1
}
is_external() {  # dependency that must live inside the bundle
  case "$1" in
    /System/*|/usr/lib/*) return 1 ;;
    @executable_path/../Frameworks/*) return 1 ;;
  esac
  return 0
}

for pass in 1 2 3 4 5 6; do
  changed=0
  while read -r f; do
    chmod u+w "$f"
    otool -L "$f" | tail -n +2 | awk '{print $1}' | while read -r dep; do
      is_external "$dep" || continue
      case "$dep" in *.framework/*) continue ;; esac     # Qt frameworks: handled by macdeployqt
      n="$(basename "$dep")"
      [ "$(basename "$f")" = "$n" ] && continue          # own install name
      if [ ! -f "$FW/$n" ]; then
        src="$(find_src "$n")" || { echo "MISSING source for $n (needed by $f)"; continue; }
        cp -L "$src" "$FW/$n"; chmod u+w "$FW/$n"
        install_name_tool -id "@executable_path/../Frameworks/$n" "$FW/$n" 2>/dev/null
        echo "added $n (for $(basename "$f"))"
        touch "$FW/.changed"
      fi
      install_name_tool -change "$dep" "@executable_path/../Frameworks/$n" "$f" 2>/dev/null
    done
  done < <(machos)
  [ -f "$FW/.changed" ] && { rm -f "$FW/.changed"; changed=1; }
  [ $changed = 0 ] && break
done

# final check: every dependency must resolve inside the bundle or the system
bad=0
while read -r f; do
  otool -L "$f" | tail -n +2 | awk '{print $1}' | while read -r dep; do
    case "$dep" in
      /System/*|/usr/lib/*) ;;
      @executable_path/../Frameworks/*)
        t="$APP/Contents/Frameworks/${dep#@executable_path/../Frameworks/}"
        [ -e "$t" ] || { echo "BROKEN: $f -> $dep"; echo x >> "$FW/.bad"; } ;;
      @rpath/*|@loader_path/*|/*)
        [ "$(basename "$dep")" = "$(basename "$f")" ] && continue
        case "$dep" in *.framework/*) t="$APP/Contents/Frameworks/${dep#@rpath/}"; [ -e "$t" ] && continue ;; esac
        echo "OUTSIDE: $f -> $dep"; echo x >> "$FW/.bad" ;;
    esac
  done
done < <(machos)
if [ -f "$FW/.bad" ]; then rm -f "$FW/.bad"; echo "bundle check FAILED"; exit 1; fi
echo "bundle check OK"

#!/bin/bash
# Build a self-contained "Logic Analyze.app" from the native build.
#   packaging/macos/package.sh [--sign "Developer ID Application: ..."] [--notarize KEYCHAIN_PROFILE]
# Output: dist/Logic Analyze.app (unsigned unless --sign is given).
set -euo pipefail

SRC="$(cd "$(dirname "$0")/../.." && pwd)"
NAME="Logic Analyze"
EXE="LogicAnalyze"
BUNDLE_ID="com.lanscapes.LogicAnalyzer"
VERSION="${VERSION:-1.0.0}"
BUILD="${BUILD:-1}"
PYVER=3.14
PYSRC="$(brew --prefix python@$PYVER)/Frameworks/Python.framework/Versions/$PYVER"
QTBIN="$(brew --prefix qtbase)/bin"
QTSVGLIB="$(brew --prefix qtsvg)/lib"
SIGN_ID=""
NOTARY_PROFILE=""
while [ $# -gt 0 ]; do
  case "$1" in
    --sign) SIGN_ID="$2"; shift 2 ;;
    --notarize) NOTARY_PROFILE="$2"; shift 2 ;;
    *) echo "unknown argument: $1"; exit 2 ;;
  esac
done
[ -n "$NOTARY_PROFILE" ] && [ -z "$SIGN_ID" ] && { echo "--notarize requires --sign"; exit 2; }
# Homebrew's bottles are built for this macOS; the bundle cannot run on older.
MIN_MACOS="${MIN_MACOS:-26.0}"

DIST="$SRC/dist"
APP="$DIST/$NAME.app"
C="$APP/Contents"
rm -rf "$APP"
mkdir -p "$C/MacOS" "$C/Resources" "$C/Frameworks"

echo "== executables"
cp "$SRC/build.dir/DSView" "$C/MacOS/$EXE"
cp "$SRC/build.dir/dslcap" "$C/MacOS/dslcap"

echo "== data (Contents/Resources; GetAppDataDir looks here first)"
cp -R "$SRC/DSView/res" "$SRC/DSView/demo" "$SRC/lang" "$C/Resources/"
cp -R "$SRC/libsigrokdecode4DSL/decoders" "$C/Resources/decoders"
rm -rf "$C/Resources/decoders/ir_irmp"  # needs the native libirmp, which is not built
cp "$SRC/NEWS25" "$SRC/NEWS31" "$SRC/ug25.pdf" "$SRC/ug31.pdf" "$C/Resources/"
cp "$SRC/DSView/icons/showDoc25.png" "$SRC/DSView/icons/showDoc31.png" "$C/Resources/"
cp "$SRC/packaging/macos/icon/$EXE.icns" "$C/Resources/$EXE.icns"
sips -Z 256 "$SRC/packaging/macos/icon/$EXE-1024.png" --out "$C/Resources/about-icon.png" >/dev/null
mkdir -p "$C/Resources/licenses"
cp "$SRC/COPYING" "$C/Resources/licenses/GPL-3.0.txt"
cp "$SRC/DSView/res/license.txt" "$C/Resources/licenses/DreamSourceLab-firmware-MIT.txt"
find "$C/Resources" -name '__pycache__' -type d -prune -exec rm -rf {} +

cat > "$C/Info.plist" <<PLIST
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0"><dict>
<key>CFBundleExecutable</key><string>$EXE</string>
<key>CFBundleIconFile</key><string>$EXE.icns</string>
<key>CFBundleIdentifier</key><string>$BUNDLE_ID</string>
<key>CFBundleName</key><string>$NAME</string>
<key>CFBundleDisplayName</key><string>$NAME</string>
<key>CFBundlePackageType</key><string>APPL</string>
<key>CFBundleShortVersionString</key><string>$VERSION</string>
<key>CFBundleVersion</key><string>$BUILD</string>
<key>LSMinimumSystemVersion</key><string>$MIN_MACOS</string>
<key>LSApplicationCategoryType</key><string>public.app-category.developer-tools</string>
<key>NSHighResolutionCapable</key><true/>
<key>NSPrincipalClass</key><string>NSApplication</string>
<key>NSHumanReadableCopyright</key><string>GPL-3.0-or-later. Based on DSView by DreamSourceLab.</string>
</dict></plist>
PLIST

echo "== Python $PYVER framework (trimmed)"
PF="$C/Frameworks/Python.framework"
PV="$PF/Versions/$PYVER"
mkdir -p "$PV/lib"
cp "$PYSRC/Python" "$PV/Python"
cp -R "$PYSRC/Resources" "$PV/Resources"
rm -rf "$PV/Resources/Python.app"
rsync -a --copy-unsafe-links \
  --exclude '__pycache__' --exclude 'site-packages' --exclude 'test' --exclude 'tests' \
  --exclude 'idlelib' --exclude 'tkinter' --exclude 'turtledemo' --exclude 'ensurepip' \
  --exclude 'pydoc_data' --exclude 'config-*' --exclude 'lib2to3' --exclude 'venv' \
  "$PYSRC/lib/python$PYVER" "$PV/lib/"
DYN="$PV/lib/python$PYVER/lib-dynload"
# Modules that link other Homebrew libraries, or exist only for tests/terminals.
rm -f "$DYN"/_decimal.* "$DYN"/_hashlib.* "$DYN"/_ssl.* "$DYN"/_lzma.* "$DYN"/_sqlite3.* "$DYN"/_zstd.* \
      "$DYN"/_test*.* "$DYN"/_xxtestfuzz.* "$DYN"/xx*.* "$DYN"/_ctypes_test.* \
      "$DYN"/readline.* "$DYN"/_curses*.* "$DYN"/_dbm.* "$DYN"/_gdbm.* "$DYN"/_tkinter.*
rm -f "$PV/lib/python$PYVER/sitecustomize.py"  # Homebrew's: adds /opt/homebrew site-packages
mkdir -p "$PV/lib/python$PYVER/site-packages"
ln -s "$PYVER" "$PF/Versions/Current"
ln -s Versions/Current/Python "$PF/Python"
ln -s Versions/Current/Resources "$PF/Resources"
chmod -R u+w "$PF"
install_name_tool -id "@rpath/Python.framework/Versions/$PYVER/Python" "$PV/Python"
for exe in "$C/MacOS/$EXE" "$C/MacOS/dslcap"; do
  install_name_tool -change "$PYSRC/Python" "@executable_path/../Frameworks/Python.framework/Versions/$PYVER/Python" "$exe"
done

echo "== SVG plugins (checkbox and toolbar graphics are SVG)"
PLUG="$C/PlugIns"
QTPLUG="$(brew --prefix)/share/qt/plugins"
for p in imageformats/libqsvg.dylib iconengines/libqsvgicon.dylib; do
  mkdir -p "$PLUG/$(dirname "$p")"; cp "$QTPLUG/$p" "$PLUG/$p"; chmod u+w "$PLUG/$p"
done

echo "== precompiling Python (the bundle is read-only at run time)"
"$(brew --prefix python@$PYVER)/bin/python$PYVER" -m compileall -q -j 0 \
  -d "" "$PV/lib/python$PYVER" "$C/Resources/decoders" >/dev/null

echo "== Qt and other libraries (macdeployqt)"
"$QTBIN/macdeployqt" "$APP" -executable="$C/MacOS/dslcap" -libpath="$QTSVGLIB" -always-overwrite -verbose=1 > "$DIST/macdeployqt.log" 2>&1 \
  || { cat "$DIST/macdeployqt.log"; echo "FAIL: macdeployqt"; exit 1; }
grep -v '^Log: ' "$DIST/macdeployqt.log" || true

echo "== required plugins"
for p in platforms/libqcocoa.dylib imageformats/libqsvg.dylib iconengines/libqsvgicon.dylib; do
  [ -f "$C/PlugIns/$p" ] || { echo "FAIL: missing PlugIns/$p"; exit 1; }
done

# Every Mach-O file in the bundle, found by content rather than permissions.
macho_files() {
  find "$APP" -type f -print0 | while IFS= read -r -d '' f; do
    if file -b "$f" | grep -q 'Mach-O'; then printf '%s\0' "$f"; fi
  done
  return 0
}

echo "== scrubbing search paths and IDs that leave the bundle"
python3 "$SRC/packaging/macos/macho_audit.py" scrub "$APP"

echo "== auditing every Mach-O slice (dependency resolution, rpaths, symlinks, minimum macOS)"
python3 "$SRC/packaging/macos/macho_audit.py" audit "$APP" "$MIN_MACOS"

echo "== third-party notices (every bundled library traced to its Homebrew keg)"
python3 "$SRC/packaging/macos/third_party_notices.py" "$APP" "$SRC" "$C/Resources/licenses/THIRD-PARTY-NOTICES.txt"

# Sign every Mach-O file individually, inside out, then frameworks and the app.
# codesign --deep does not reach loose libraries such as Python's lib-dynload.
#   sign_tree APP IDENTITY [developer-id]
sign_tree() {
  local app="$1" id="$2" mode="${3:-adhoc}" opts=()
  [ "$mode" = "developer-id" ] && opts=(--timestamp --options runtime)
  local ent="$SRC/packaging/macos/entitlements-developer-id.plist"
  find "$app" -type f -print0 | while IFS= read -r -d '' f; do
    case "$f" in "$app/Contents/MacOS/"*) continue ;; esac
    if file -b "$f" | grep -q 'Mach-O'; then
      codesign --force ${opts[@]+"${opts[@]}"} --sign "$id" "$f" 2>&1 | grep -v 'replacing existing signature' || true
    fi
  done
  for fw in "$app/Contents/Frameworks"/*.framework; do
    codesign --force ${opts[@]+"${opts[@]}"} --sign "$id" "$fw" 2>&1 | grep -v 'replacing existing signature' || true
  done
  local entopt=()
  [ "$mode" = "developer-id" ] && entopt=(--entitlements "$ent")
  codesign --force ${opts[@]+"${opts[@]}"} ${entopt[@]+"${entopt[@]}"} --sign "$id" "$app/Contents/MacOS/dslcap"
  codesign --force ${opts[@]+"${opts[@]}"} ${entopt[@]+"${entopt[@]}"} --sign "$id" "$app"
  codesign --verify --deep --strict "$app"
}

echo "== runtime load check (dyld's own record of every loaded image)"
# The hardened runtime ignores DYLD_* variables, so check an ad-hoc signed copy.
if [ -z "${SKIP_RUNTIME_CHECK:-}" ]; then
  RT="$DIST/runtime-check"
  rm -rf "$RT"; mkdir -p "$RT"
  ditto "$APP" "$RT/$NAME.app"
  sign_tree "$RT/$NAME.app" - > "$RT/sign.log" 2>&1 || { cat "$RT/sign.log"; echo "FAIL: ad-hoc signing for the runtime check"; exit 1; }
  DYLD_PRINT_LIBRARIES=1 "$RT/$NAME.app/Contents/MacOS/dslcap" --list > "$RT/dslcap.log" 2>&1 || true
  DYLD_PRINT_LIBRARIES=1 "$RT/$NAME.app/Contents/MacOS/$EXE" > "$RT/app.log" 2>&1 &
  pid=$!
  sleep "${RUNTIME_CHECK_SECONDS:-12}"
  if ! kill -0 "$pid" 2>/dev/null; then
    tail -20 "$RT/app.log"
    echo "FAIL: the app exited during the runtime check (crash or code-signature kill; see ~/Library/Logs/DiagnosticReports)"
    exit 1
  fi
  kill "$pid" 2>/dev/null || true; wait "$pid" 2>/dev/null || true
  python3 - "$RT" "$RT/$NAME.app" <<'PY'
import os, re, sys
rt, app = sys.argv[1], os.path.realpath(sys.argv[2])
bad, seen = [], 0
for log in ("dslcap.log", "app.log"):
    for line in open(os.path.join(rt, log), errors="replace"):
        m = re.match(r"dyld\[\d+\]: <[^>]*> (.+)$", line.strip())
        if not m:
            continue
        seen += 1
        p = os.path.realpath(m.group(1))
        if not (p.startswith(app + os.sep) or p.startswith(("/System/", "/usr/lib/"))):
            bad.append(f"{log}: {m.group(1)}")
if seen == 0:
    sys.exit("FAIL: runtime check saw no loaded images (did the binaries start?)")
for b in bad:
    print("loaded from outside the bundle:", b)
if bad:
    sys.exit(f"FAIL: {len(bad)} image(s) loaded from outside the bundle")
print(f"runtime check clean: {seen} images loaded, all from the bundle or the OS")
PY
  rm -rf "$RT"
fi

if [ -n "$SIGN_ID" ]; then
  echo "== signing with $SIGN_ID"
  sign_tree "$APP" "$SIGN_ID" developer-id
  codesign --verify --deep --strict --verbose=2 "$APP" 2>&1 | tail -2
else
  echo "== ad-hoc signing (no --sign given; for local use only)"
  sign_tree "$APP" -
fi

if [ -n "$NOTARY_PROFILE" ]; then
  echo "== notarizing with keychain profile $NOTARY_PROFILE"
  ZIP="$DIST/$EXE-$VERSION.zip"
  rm -f "$ZIP"
  ditto -c -k --keepParent "$APP" "$ZIP"
  out=$(xcrun notarytool submit "$ZIP" --keychain-profile "$NOTARY_PROFILE" --wait --output-format json)
  echo "$out"
  id=$(echo "$out" | python3 -c 'import json,sys; print(json.load(sys.stdin)["id"])')
  status=$(echo "$out" | python3 -c 'import json,sys; print(json.load(sys.stdin)["status"])')
  xcrun notarytool log "$id" --keychain-profile "$NOTARY_PROFILE" "$DIST/notary-$id.json"
  [ "$status" = "Accepted" ] || { echo "FAIL: notarization $status; see $DIST/notary-$id.json"; exit 1; }
  xcrun stapler staple "$APP"
  xcrun stapler validate "$APP"
  spctl -a -vv "$APP"
  rm -f "$ZIP"
  ditto -c -k --keepParent "$APP" "$ZIP"   # re-zip with the stapled ticket for distribution
  echo "Notarized: $ZIP"
fi

du -sh "$APP"
echo "Built $APP"

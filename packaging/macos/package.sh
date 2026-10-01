#!/bin/bash
# Build a self-contained "Logic Analyze.app" from the native build.
#   packaging/macos/package.sh [--sign "Developer ID Application: ..."]
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
[ "${1:-}" = "--sign" ] && SIGN_ID="$2"

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
cp "$SRC/NEWS25" "$SRC/NEWS31" "$SRC/ug25.pdf" "$SRC/ug31.pdf" "$C/Resources/"
cp "$SRC/DSView/icons/showDoc25.png" "$SRC/DSView/icons/showDoc31.png" "$C/Resources/"
cp "$SRC/DSView.icns" "$C/Resources/$EXE.icns"
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
<key>LSMinimumSystemVersion</key><string>13.0</string>
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
"$QTBIN/macdeployqt" "$APP" -executable="$C/MacOS/dslcap" -libpath="$QTSVGLIB" -always-overwrite -verbose=1 2>&1 | grep -v '^Log: ' || true

echo "== checking for paths outside the bundle"
leaks=""
while IFS= read -r -d '' f; do
  out=$(otool -L "$f" 2>/dev/null | tail -n +2 | grep -E "/opt/homebrew|/usr/local" || true)
  [ -n "$out" ] && leaks+="$f:"$'\n'"$out"$'\n'
done < <(find "$APP" -type f \( -perm -u+x -o -name '*.dylib' -o -name '*.so' \) -print0)
if [ -n "$leaks" ]; then echo "$leaks"; echo "FAIL: external library references remain"; exit 1; fi
echo "no external references"

if [ -n "$SIGN_ID" ]; then
  echo "== signing with $SIGN_ID"
  ENT="$SRC/packaging/macos/entitlements-developer-id.plist"
  # Inside out: every Mach-O file, then frameworks, then the app.
  find "$APP" -type f \( -name '*.dylib' -o -name '*.so' \) -print0 \
    | xargs -0 codesign --force --timestamp --options runtime --sign "$SIGN_ID"
  codesign --force --timestamp --options runtime --sign "$SIGN_ID" "$PV/Python"
  for fw in "$C/Frameworks"/*.framework; do
    codesign --force --timestamp --options runtime --sign "$SIGN_ID" "$fw"
  done
  codesign --force --timestamp --options runtime --entitlements "$ENT" --sign "$SIGN_ID" "$C/MacOS/dslcap"
  codesign --force --timestamp --options runtime --entitlements "$ENT" --sign "$SIGN_ID" "$APP"
  codesign --verify --deep --strict --verbose=2 "$APP"
fi

du -sh "$APP"
echo "Built $APP"

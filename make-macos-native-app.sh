#!/bin/sh
# Package the native arm64 build as "DSView Native.app" next to the vendor app.
set -e
SRC="$(cd "$(dirname "$0")" && pwd)"
APP="${1:-/Applications/DSView Native.app}"
rm -rf "$APP"
mkdir -p "$APP/Contents/MacOS" "$APP/Contents/Resources"
cp "$SRC/build.dir/DSView" "$APP/Contents/MacOS/DSView"
cp -R "$SRC/DSView/res" "$APP/Contents/MacOS/res"
cp -R "$SRC/DSView/demo" "$APP/Contents/MacOS/demo"
cp -R "$SRC/libsigrokdecode4DSL/decoders" "$APP/Contents/MacOS/decoders"
cp -R "$SRC/lang" "$APP/Contents/MacOS/lang"
cp "$SRC/DSView.icns" "$APP/Contents/Resources/DSView.icns"
cat > "$APP/Contents/Info.plist" <<PLIST
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0"><dict>
<key>CFBundleExecutable</key><string>DSView</string>
<key>CFBundleIconFile</key><string>DSView.icns</string>
<key>CFBundleIdentifier</key><string>local.dsview.native</string>
<key>CFBundleName</key><string>DSView Native</string>
<key>CFBundlePackageType</key><string>APPL</string>
<key>CFBundleShortVersionString</key><string>$(git -C "$SRC" describe --tags --always)-native</string>
<key>LSMinimumSystemVersion</key><string>13.0</string>
<key>NSHighResolutionCapable</key><true/>
<key>NSPrincipalClass</key><string>NSApplication</string>
</dict></plist>
PLIST
codesign --force --deep --sign - "$APP"
echo "Built $APP"

#!/usr/bin/env bash
# Package everything into a "Grabber.dmg" file at the root of the git repository

set -e

# Create the application directory
APP_ROOT="Grabber.app"
APP_DIR="$APP_ROOT/Contents/MacOS"
APP_RESOURCES="$APP_ROOT/Contents/Resources"
mkdir -p "$APP_DIR" "$APP_RESOURCES"

# Package data as resources, keeping only executables in Contents/MacOS
./scripts/package.sh "$APP_RESOURCES"
mv "$APP_RESOURCES/Grabber" "$APP_RESOURCES/Grabber-cli" "$APP_DIR/"
rm "$APP_RESOURCES/settings.ini"
cp -r src/dist/macos/* "$APP_ROOT/Contents"

# Prepare the app bundle (doesn't create the DMG file)
macdeployqt $APP_ROOT
codesign --verify --deep --strict --verbose=4 "$APP_ROOT"

# Create LZMA compressed DMG using hdiutil (format ULMO)
hdiutil create \
  -volname "Grabber" \
  -srcfolder "$APP_ROOT" \
  -fs APFS \
  -format ULMO \
  -ov \
  "Grabber.dmg"

# Cleanup
rm -rf $APP_DIR

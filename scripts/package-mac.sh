#!/usr/bin/env bash
# Package everything into a "Grabber.dmg" file at the root of the git repository

set -e

# Create the application directory
APP_ROOT="Grabber.app"
APP_DIR="$APP_ROOT/Contents/MacOS"
mkdir -p $APP_DIR

# Copy all required files to the application directory
./scripts/package.sh $APP_DIR
rm "$APP_DIR/settings.ini"
cp -r src/dist/macos/* "$APP_ROOT/Contents"

# Prepare the app bundle (doesn't create the DMG file)
macdeployqt $APP_ROOT

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

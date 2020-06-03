#!/usr/bin/env bash
# Package everything into a "Grabber.dmg" file at the root of the git repository

# Create the application directory
APP_ROOT="Grabber.app"
APP_DIR="$APP_ROOT/Contents/MacOS"
mkdir -p $APP_DIR

# Copy all required files to the application directory
./scripts/package.sh $APP_DIR
cp -r src/dist/macos/* "$APP_ROOT/Contents"

# Create the DMG file
macdeployqt $APP_ROOT -dmg

# Cleanup
rm -rf $APP_DIR

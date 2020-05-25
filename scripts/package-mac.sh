#!/usr/bin/env bash
# Package everything into a "Grabber.dmg" file at the root of the git repository

# Create the application directory
APP_ROOT="Grabber.app"
APP_DIR="$APP_ROOT/Contents/MacOS"
mkdir -p $APP_DIR

# Copy all required files to the application directory
cp "build/gui/Grabber" $APP_DIR
cp -r release/* $APP_DIR
cp -r dist/common/* $APP_DIR
cp -r dist/macos/* "$APP_ROOT/Contents"
cp build/languages/*.qm "$APP_DIR/languages/"
touch "$APP_DIR/settings.ini"
rm -rf "$APP_DIR/sites/node_modules/"

# Create the DMG file
macdeployqt $APP_ROOT -dmg

# Cleanup
rm -rf $APP_DIR

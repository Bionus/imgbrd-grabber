#!/usr/bin/env bash
# Copy all relevant files into a given directory
# Usage: ./scripts/package.sh "target_directory"

# Create the application directory
APP_DIR=$1
mkdir -p $APP_DIR

# Copy all required files to the release directory
cp "build/gui/Grabber" $APP_DIR
cp -r sites/ $APP_DIR
cp -r dist/common/* $APP_DIR
cp build/languages/*.qm "$APP_DIR/languages/"
touch "$APP_DIR/settings.ini"
rm -rf "$APP_DIR/sites/node_modules/"

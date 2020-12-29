#!/usr/bin/env bash
# Copy all relevant files into a given directory
# Usage: ./scripts/package.sh "target_directory"

# Create the application directory
APP_DIR=$1
mkdir -p $APP_DIR

# Copy built files
cp "build/gui/Grabber" $APP_DIR 2> /dev/null
cp "build/cli/Grabber-cli" $APP_DIR 2> /dev/null
cp build/languages/*.qm "$APP_DIR/languages/" 2> /dev/null

# Copy other required files to the release directory
rsync -ar --exclude-from="src/sites/exclude.txt" src/sites $APP_DIR
cp -r src/dist/common/* $APP_DIR
touch "$APP_DIR/settings.ini"

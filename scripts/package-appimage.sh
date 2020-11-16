#!/usr/bin/env bash
# Package everything into an AppImage file at the root of the git repository

set -e

# Create the application directory
APP_DIR="release"
mkdir -p $APP_DIR

# Application binary
mkdir -p $APP_DIR/usr/bin
cp "build/gui/Grabber" $APP_DIR/usr/bin

# Desktop file
mkdir -p $APP_DIR/usr/share/applications
cp "src/dist/linux/Grabber.desktop" $APP_DIR/usr/share/applications

# Icon
mkdir -p $APP_DIR/usr/share/icons/hicolor
cp "src/gui/resources/images/icon.png" $APP_DIR/usr/share/icons/hicolor/Grabber.png

# Copy other files in $PREFIX/share/Grabber where they can be found
mkdir -p $APP_DIR/usr/share/Grabber
rsync -ar --exclude="node_modules" --exclude="CMakeLists.txt" --exclude="package*.json" --exclude="*.config.js" --exclude="tsconfig.json" --exclude="tslint.json" --exclude="**/*.ts" --exclude="**/resources" --exclude="Sankaku" --exclude="Tumblr" src/sites $APP_DIR/usr/share/Grabber
cp -r src/dist/common/* $APP_DIR/usr/share/Grabber
touch "$APP_DIR/usr/share/Grabber/settings.ini"

# See https://github.com/probonopd/linuxdeployqt
export VERSION=$GRABBER_VERSION
./vendor/linuxdeployqt.AppImage "$APP_DIR/usr/share/applications/Grabber.desktop" -appimage
mv "Grabber-$VERSION-x86_64.AppImage" "Grabber_$VERSION-x86_64.AppImage"

# Cleanup
rm -rf $APP_DIR

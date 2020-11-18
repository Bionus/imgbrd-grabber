#!/usr/bin/env bash
# Package everything into an AppImage file at the root of the git repository

set -e

# Create the application directory
APP_DIR="release"
mkdir -p $APP_DIR

# Application binary
mkdir -p $APP_DIR/usr/bin
cp "build/gui/Grabber" $APP_DIR/usr/bin/grabber

# Desktop file
mkdir -p $APP_DIR/usr/share/applications
cp "src/dist/linux/grabber.desktop" $APP_DIR/usr/share/applications

# AppData file
mkdir -p $APP_DIR/usr/share/metainfo
cp "src/dist/linux/grabber.appdata.xml" $APP_DIR/usr/share/metainfo

# Icon
mkdir -p $APP_DIR/usr/share/icons/hicolor
cp "src/dist/linux/grabber.png" $APP_DIR/usr/share/icons/hicolor

# Copy other files in $PREFIX/share/Grabber where they can be found
mkdir -p $APP_DIR/usr/share/Grabber
rsync -ar --exclude="node_modules" --exclude="CMakeLists.txt" --exclude="package*.json" --exclude="*.config.js" --exclude="tsconfig.json" --exclude="tslint.json" --exclude="**/*.ts" --exclude="**/resources" --exclude="Sankaku" --exclude="Tumblr" src/sites $APP_DIR/usr/share/Grabber
cp -r src/dist/common/* $APP_DIR/usr/share/Grabber
touch "$APP_DIR/usr/share/Grabber/settings.ini"

# See https://github.com/probonopd/linuxdeployqt
export VERSION=$GRABBER_VERSION
./vendor/linuxdeployqt.AppImage "$APP_DIR/usr/share/applications/grabber.desktop" -appimage
mv "Grabber-$VERSION-x86_64.AppImage" "Grabber_$VERSION-x86_64.AppImage"

# Cleanup
rm -rf $APP_DIR

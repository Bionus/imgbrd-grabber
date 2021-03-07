#!/usr/bin/env bash
# Package everything into an AppImage file at the root of the git repository

set -e

# Create the application directory
APP_DIR="release"
mkdir -p $APP_DIR

# "make install" into the app directory
pushd build
    make DESTDIR=../$APP_DIR install
popd

# See https://github.com/probonopd/linuxdeployqt
export VERSION=$GRABBER_VERSION
./vendor/linuxdeployqt.AppImage "$APP_DIR/usr/share/applications/grabber.desktop" -appimage
mv "Grabber-$VERSION-x86_64.AppImage" "Grabber_$VERSION-x86_64.AppImage"

# Cleanup
rm -rf $APP_DIR

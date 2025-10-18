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
./vendor/linuxdeployqt.AppImage "$APP_DIR/usr/share/applications/org.bionus.Grabber.desktop" -appimage -extra-plugins=platforms/libqoffscreen.so -exclude-libs=libqsqloci -updateinformation="gh-releases-zsync|Bionus|imgbrd-grabber|latest|Grabber_*-x86_64.AppImage.zsync"
mv "Grabber-$VERSION-x86_64.AppImage" "Grabber_$VERSION-x86_64.AppImage"
mv "Grabber-$VERSION-x86_64.AppImage.zsync" "Grabber_$VERSION-x86_64.AppImage.zsync"

# Cleanup
rm -rf $APP_DIR

#!/usr/bin/env bash
mv TEMP-Grabber.app/Contents/MacOS/TEMP-Grabber TEMP-Grabber.app/Contents/MacOS/Grabber
mkdir -p grabber-release
ln -s  /Applications grabber-release/Applications 
mv TEMP-Grabber.app grabber-release/Grabber.app
macdeployqt grabber-release/Grabber.app
hdiutil create -volname grabber -srcfolder grabber-release/ -ov -format UDRW -o grabber
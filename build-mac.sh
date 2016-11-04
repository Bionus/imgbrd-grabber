#!/usr/bin/env sh

ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
brew install qt5 --with-docs --with-developer --with-d-bus --with-mysql
brew install gcc

QT_SELECT=5 qmake "Grabber.pro"
make

appDir=release/Grabber.app/Contents/MacOS
mv gui/Grabber.app release
mv release/* $appDir
touch $appDir/settings.ini

echo "Grabber has been compiled in the release directory. To run it, type 'open ./release/Grabber.app'"

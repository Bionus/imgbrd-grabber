#!/bin/sh
appDir=release/Grabber.app/Contents/MacOS
ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
brew install qt5 --with-docs --with-developer --with-d-bus --with-mysql
brew install gcc
qmake Grabber.pro
make
mv gui/Grabber.app release
mv release/languages $appDir/
mv release/sites $appDir/
mv release/words.txt $appDir/
mv release/CDR.exe $appDir/
touch $appDir/settings.ini
echo "Grabber has been compiled in the release directory. To run it, type 'open ./release/Grabber.app'"
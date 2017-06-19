#!/usr/bin/env sh

if [ "$(which pacman)" = "/usr/bin/pacman" ]
then
  sudo pacman -Sy
  sudo pacman -S "qt" "gcc" "cmake" "libpulse"
else
  add-apt-repository --yes "ppa:ubuntu-sdk-team/ppa"
  apt-get update -qq
  apt-get install -qq "qt5-qmake" "qtbase5-dev" "qtdeclarative5-dev" "qtscript5-dev" "qtmultimedia5-dev" "libpulse-dev" "qt5-default" "qttools5-dev-tools"
  apt-get install -qq "g++" "cmake"
fi

mkdir build
cd build
cmake ..
make -j8
cd ..

mv "build/gui/gui" "release/Grabber"
touch "release/settings.ini"

echo "Grabber has been compiled in the release directory. To run it, type './release/Grabber'"

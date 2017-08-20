#!/usr/bin/env sh

# Install required packages
if type pacman > /dev/null 2>&1
then
  sudo pacman -Sy
  sudo pacman -S "qt" "gcc" "cmake" "libpulse"
else
  sudo apt-get install -qq "qt5-qmake" "qtbase5-dev" "qtdeclarative5-dev" "qtscript5-dev" "qtmultimedia5-dev" "libpulse-dev" "qt5-default" "qttools5-dev-tools"
  sudo apt-get install -qq "g++" "cmake" "libssl-dev"
fi

# Build the project in the build directory
mkdir build
cd build
cmake ..
make -j8
cd ..

# Move the built binary to the release folder with its config
mv "build/gui/Grabber" "release/"
touch "release/settings.ini"

echo "Grabber has been compiled in the release directory. To run it, type './release/Grabber'"

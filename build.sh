#!/usr/bin/env sh

# Restore submodules in case the repo was clone without --recursive
git submodule update --init --recursive

# Install required packages
if type pacman > /dev/null 2>&1
then
  sudo pacman -Sy
  sudo pacman -S "qt" "gcc" "cmake" "libpulse" "nodejs" "npm"
else
  sudo apt-get install -qq "qtbase5-dev" "qtscript5-dev" "qtmultimedia5-dev" "qtdeclarative5-dev" "qttools5-dev" "qttools5-dev-tools"
  sudo apt-get install -qq "g++" "cmake" "libssl-dev" "nodejs"
fi

# Build the project in the build directory
./scripts/build.sh gui cli

# Move the built binary to the release folder with its config
mv "build/gui/Grabber" "release/"
touch "release/settings.ini"

echo "Grabber has been compiled in the release directory. To run it, type './release/Grabber'"

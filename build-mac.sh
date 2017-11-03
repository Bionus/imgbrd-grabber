#!/usr/bin/env sh

# Install required packages
ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
brew install qt5 --with-docs --with-developer --with-d-bus --with-mysql
brew install gcc cmake

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

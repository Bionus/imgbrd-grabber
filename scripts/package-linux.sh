#!/usr/bin/env bash
# Package everything into a "Grabber.tar.gz" file at the root of the git repository

# Copy all required files to the release directory
cp "build/gui/Grabber" "release/Grabber"
cp build/languages/*.qm "release/languages/"
touch "release/settings.ini"
cp -r dist/common/* "release/"

# Zip the whole directory
pushd release
    tar -czvf "../Grabber.tar.gz" --exclude="./sites/node_modules" .
popd

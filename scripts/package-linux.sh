#!/usr/bin/env bash
# Package everything into a "Grabber.zip" file at the root of the git repository

# Copy all required files to the release directory
cp "build/gui/Grabber" "release/Grabber"
touch "release/settings.ini"
rm "release/CDR.exe"

# Zip the whole directory
pushd release
    zip -r "../Grabber.zip" . -x "sites/node_modules/*"
popd

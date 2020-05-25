#!/usr/bin/env bash
# Package everything into a "Grabber.tar.gz" file at the root of the git repository

APP_DIR="release"

# Copy all required files to the release directory
./scripts/package.sh $APP_DIR
cp -r dist/linux/* $APP_DIR

# Zip the whole directory
pushd release
    tar -czvf "../Grabber.tar.gz" .
popd

# Cleanup
rm -rf $APP_DIR

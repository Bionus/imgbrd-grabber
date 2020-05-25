#!/usr/bin/env bash

set -e

mkdir -p build
pushd build
    cmake ..
    make $@ -j8
popd

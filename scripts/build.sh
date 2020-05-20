#!/usr/bin/env bash

set -e

mkdir build
pushd build
    cmake ..
    make $@ -j8
popd

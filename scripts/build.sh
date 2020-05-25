#!/usr/bin/env bash

set -e

mkdir -p build
pushd build
    cmake ../src
    make $@ -j8
popd

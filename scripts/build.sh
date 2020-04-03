#!/usr/bin/env bash

mkdir build
pushd build
    cmake ..
    make gui cli -j8
popd

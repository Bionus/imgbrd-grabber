#!/usr/bin/env bash

mkdir build
pushd build
    cmake ..
    make -j8
popd

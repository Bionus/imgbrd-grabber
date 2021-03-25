#!/usr/bin/env bash

set -e

mkdir -p build
declare -i ret=0
pushd build
    cmake ../src || ret=1
    if ((!ret)); then make $@ -j$(($(nproc)+1)) || ret=2; fi
popd
exit "$ret"

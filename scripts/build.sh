#!/usr/bin/env bash
set -e

rm -rf build
mkdir -p build
cd build

cmake -G Ninja -DCMAKE_BUILD_TYPE=Release ..
ninja

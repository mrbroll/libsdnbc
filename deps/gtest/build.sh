#!/usr/bin/env sh

cd deps/gtest/src
if [ ! -d "build" ]; then
    mkdir build
fi
cd build
cmake ..
make
cd ../..
cp src/build/libgtest.a lib
cp src/build/libgtest_main.a lib
cp -r src/include/gtest include
cd ..

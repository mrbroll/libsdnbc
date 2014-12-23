#!/usr/bin/env sh

cd gtest
if [ ! -d "build" ]; then
    mkdir build
fi
cd build
cmake ..
make
cd ../..
cp gtest/build/libgtest.a deps/gtest/lib
cp gtest/build/libgtest_main.a deps/gtest/lib
cp -r gtest/include/gtest deps/gtest/include
rm -rf gtest

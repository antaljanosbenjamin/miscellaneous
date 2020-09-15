#!/bin/sh

PREVIOUS_DIR=`pwd`
git clone -b 1.90 https://github.com/danmar/cppcheck.git /tmp/cppcheck
cd /tmp/cppcheck
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make MATCHCOMPILER=yes FILESDIR=/usr/share/cppcheck HAVE_RULES=yes CXXFLAGS="-O2 -DNDEBUG -Wall -Wno-sign-compare -Wno-unused-function" -j
sudo make -s install
cppcheck --version
cd $PREVIOUS_DIR

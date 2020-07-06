#!/bin/sh

sudo apt install cmake
pip install --user conan
conan user
conan profile new default --detect
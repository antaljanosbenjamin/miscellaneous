#!/bin/sh

pip install --user conan
conan user
conan profile new default --detect
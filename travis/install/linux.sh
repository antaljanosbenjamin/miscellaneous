#!/bin/sh

pip install --user conan
conan user
conan profile new default --detect

curl https://sh.rustup.rs -sSf | sh
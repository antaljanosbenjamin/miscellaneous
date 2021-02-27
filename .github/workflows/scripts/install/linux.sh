#!/bin/sh

conan user
conan profile new default --detect

if test ! -z "$INSTALL_PACKAGES" 
then
  sudo apt install $INSTALL_PACKAGES
fi
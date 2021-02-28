#!/bin/sh

./.github/workflows/scripts/install/delete_default_conan_profile.sh

conan user
conan profile new default --detect

sudo apt install libgtk2.0-dev libgl-dev

if test ! -z "$INSTALL_PACKAGES" 
then
  sudo apt install $INSTALL_PACKAGES
fi

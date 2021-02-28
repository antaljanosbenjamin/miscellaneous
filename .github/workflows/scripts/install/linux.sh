#!/bin/sh

./.github/workflows/scripts/install/delete_default_conan_profile.sh

conan user
conan profile new default --detect

sudo apt libgtk2.0-dev

if test ! -z "$INSTALL_PACKAGES" 
then
  sudo apt install $INSTALL_PACKAGES
fi

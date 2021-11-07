#!/bin/bash

./.github/workflows/scripts/install/delete_default_conan_profile.sh

conan user
conan profile new default --detect
conan config set general.revisions_enabled=1

if [ "$COMPILER" = "MSVC" ]
then
  conan profile update settings.compiler="Visual Studio" default
  conan profile update settings.compiler.version=16 default
elif [ "$COMPILER" = "clang" ]
then
  conan profile update settings.compiler="clang" default
  conan profile update settings.compiler.version=9 default
  conan profile update settings.compiler.libcxx=libstdc++ default
else
  echo "Unknown compiler type: $COMPILER"
  exit 1
fi

if test ! -z "$INSTALL_PACKAGES"
then
  choco install $INSTALL_PACKAGES
fi

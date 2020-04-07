#!/bin/bash

choco install python3 --params "/InstallDir:C:\Python"
choco install visualstudio2019community --package-parameters "--add Microsoft.VisualStudio.Component.VC.Tools.x86.x64 --add Microsoft.VisualStudio.Component.VC.CMake.Project --add Microsoft.VisualStudio.Component.VC.Llvm.ClangToolset --no-includeRecommended"

export PATH=/c/Python:/c/Python/Scripts:$PATH

pip install conan
conan user
conan profile new default --detect

if [ "$COMPILER" = "MSVC" ]
then
  conan profile update settings.compiler="Visual Studio" default
  conan profile update settings.compiler.version=16 default
  conan profile remove settings.compiler.libcxx default
elif [ "$COMPILER" = "clang" ]
then
  conan profile update settings.compiler="clang" default
  conan profile update settings.compiler.version=9 default
  conan profile update settings.compiler.libcxx=libstdc++ default
else
  echo "Unknown compiler type: $COMPILER"
  exit 1
fi


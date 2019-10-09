choco install python
choco install visualstudio2019community --package-parameters "--add Microsoft.VisualStudio.Component.VC.Tools.x86.x64 --add Microsoft.VisualStudio.Component.VC.CMake.Project --add Microsoft.VisualStudio.Component.VC.Llvm.ClangToolset --no-includeRecommended"

export PATH=/c/Python37:/c/Python37/Scripts:/c/Users/travis/AppData/Roaming/Python/Python37/Scripts:$PATH

pip install --user conan
conan user
conan profile new default --detect

if [ "$COMPILER" = "MSVC" ]
then
  conan profile update settings.compiler="Visual Studio" default
  conan profile update settings.compiler.version=16 default
  conan profile remove settings.compiler.libcxx default
elif [ "$COMPILER" = "clang" ]
  conan profile update settings.compiler="clang" default
  conan profile update settings.compiler.version=9 default
  conan profile update settings.compiler.libcxx=libstdc++ default
else
  echo "Unknown compiler type: $COMPILER"
  exit 1
fi


#!/bin/sh
echo "Removing CMake installed from source from PATH..."
PATH=`echo $PATH | sed -re 's/\/usr\/local\/cmake-[[:digit:]]\.[[:digit:]][[:digit:]]\.[[:digit:]]+\/bin//'`
echo "The new value of PATH: $PATH"

if [ "$INSTALL_CPPCHECK" = true ] ; then
  travis/before_install/install_cppcheck.sh
fi

if [ -f "$HOME/.conan/profiles/default" ]; then
    echo "Default profile exists, deleting it."
    rm -f "$HOME/.conan/profiles/default"
else 
    echo "Default profile doesn't exist"
fi
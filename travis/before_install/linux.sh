#!/bin/sh
echo "Removing CMake installed from source from PATH..."
PATH=`echo $PATH | sed -re 's/\/usr\/local\/cmake-[[:digit:]]\.[[:digit:]][[:digit:]]\.[[:digit:]]+\/bin//'`
echo "The new value of PATH: $PATH"

if [ "$INSTALL_CPPCHECK" = true ] ; then
  . ./install_cppcheck.sh
fi

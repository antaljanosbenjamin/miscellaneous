#!/bin/sh
echo $PATH
PATH=`echo $PATH | sed -re 's/\/usr\/local\/cmake-[[:digit:]].[[:digit:]][[:digit:]]*\/bin(:)?//'`
echo $PATH

#!/bin/sh

if [ -f "$HOME/.conan/profiles/default" ]; then
    echo "Default profile exists, deleting it."
    rm -f "$HOME/.conan/profiles/default"
else 
    echo "Default profile doesn't exist"
fi

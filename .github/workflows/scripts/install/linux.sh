#!/bin/sh

./.github/workflows/scripts/install/delete_default_conan_profile.sh

conan user
conan profile new default --detect
conan config set general.revisions_enabled=1

sudo apt update
sudo apt install -y --no-install-recommends libgtk2.0-dev libgl-dev libx11-xcb-dev libxcb-dri3-dev libxcb-icccm4-dev libxcb-image0-dev libxcb-keysyms1-dev libxcb-randr0-dev libxcb-render-util0-dev libxcb-shape0-dev libxcb-sync-dev libxcb-util-dev libxcb-xfixes0-dev libxcb-xinerama0-dev libxcb-xkb-dev xorg-dev


if test ! -z "$INSTALL_PACKAGES"
then
  sudo apt install -y --no-install-recommends $INSTALL_PACKAGES
fi

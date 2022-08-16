#!/bin/bash -x
mkdir release
cp -f PKGBUILD release/
cd release
#makepkg -Acs
source PKGBUILD && pacman -Syu --noconfirm --needed --asdeps "${makedepends[@]}" "${depends[@]}"
rm -Rf PKGBUILD
rm -Rf XOpcodeCalc
cd ..

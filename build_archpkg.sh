#!/bin/bash -x
mkdir release
cp -f PKGBUILD release/
cd release
env EUID=1 makepkg -Acs --noconfirm
#makepkg -Acs
rm -Rf PKGBUILD
rm -Rf XOpcodeCalc
cd ..

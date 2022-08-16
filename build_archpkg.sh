#!/bin/bash -x
mkdir release
cp -f PKGBUILD release/
cd release
makepkg -Acs --noconfirm
rm -Rf PKGBUILD
rm -Rf XOpcodeCalc
cd ..

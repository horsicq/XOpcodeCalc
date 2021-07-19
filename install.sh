#!/bin/bash -x
export X_SOURCE_PATH=$PWD

cp -f $X_SOURCE_PATH/build/release/xocalc                            /usr/bin/
cp -f $X_SOURCE_PATH/DEBIAN/xocalc.desktop                           /usr/share/applications/
cp -Rf $X_SOURCE_PATH/DEBIAN/hicolor/                               /usr/share/icons/

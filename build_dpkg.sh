#!/bin/bash -x
export QMAKE_PATH=/usr/bin/qmake

export X_SOURCE_PATH=$PWD
export X_BUILD_NAME=xopcodecalc_linux_portable
export X_RELEASE_VERSION=$(cat "release_version.txt")

source build_tools/linux.sh

check_file $QMAKE_PATH

if [ -z "$X_ERROR" ]; then
    make_init
    make_build "$X_SOURCE_PATH/xopcodecalc_source.pro"

    check_file "$X_SOURCE_PATH/build/release/xocalc"
    if [ -z "$X_ERROR" ]; then
        create_deb_app_dir xocalc
        
        export X_PACKAGENAME='xopcodecalculator'
        export X_MAINTAINER='hors <horsicq@gmail.com>'
        
        export X_HOMEPAGE='http://ntinfo.biz'
        export X_DESCRIPTION='x86/64 Opcode calculator'
        
        if [ "$X_DEBIAN_VERSION" -ge "11" ]; then
            export X_DEPENDS='libqt5core5a, libqt5gui5, libqt5widgets5, libqt5dbus5'
        else
            export X_DEPENDS='qt5-default, libqt5core5a, libqt5gui5, libqt5widgets5, libqt5dbus5'
        fi
        
        create_deb_control $X_SOURCE_PATH/release/$X_BUILD_NAME/DEBIAN/control
        
        cp -f $X_SOURCE_PATH/build/release/xocalc                           $X_SOURCE_PATH/release/$X_BUILD_NAME/usr/bin/
        cp -f $X_SOURCE_PATH/LINUX/xocalc.desktop                           $X_SOURCE_PATH/release/$X_BUILD_NAME/usr/share/applications/
        sed -i "s/#VERSION#/$X_RELEASE_VERSION/"                            $X_SOURCE_PATH/release/$X_BUILD_NAME/usr/share/applications/xocalc.desktop
        cp -Rf $X_SOURCE_PATH/LINUX/hicolor/                                $X_SOURCE_PATH/release/$X_BUILD_NAME/usr/share/icons/
        cp -Rf $X_SOURCE_PATH/images/                                       $X_SOURCE_PATH/release/$X_BUILD_NAME/usr/lib/xocalc/

        make_deb
        mv $X_SOURCE_PATH/release/$X_BUILD_NAME.deb $X_SOURCE_PATH/release/xopcodecalc_${X_RELEASE_VERSION}_${X_OS_VERSION}_${X_ARCHITECTURE}.deb
        make_clear
    fi
fi

#!/bin/sh -x
export QMAKE_PATH=$HOME/Qt/5.15.2/clang_64/bin/qmake

export X_SOURCE_PATH=$PWD
export X_BUILD_NAME=xopcodecalc_mac
export X_RELEASE_VERSION=$(cat "release_version.txt")

source build_tools/mac.sh

check_file $QMAKE_PATH

if [ -z "$X_ERROR" ]; then
    make_init
    make_build "$X_SOURCE_PATH/xopcodecalc_source.pro"

    check_file "$X_SOURCE_PATH/build/release/XOcalc.app/Contents/MacOS/XOcalc"
    if [ -z "$X_ERROR" ]; then
        cp -R "$X_SOURCE_PATH/build/release/XOcalc.app"    "$X_SOURCE_PATH/release/$X_BUILD_NAME"

        fiximport "$X_SOURCE_PATH/build/release/XOcalc.app/Contents/MacOS/XOcalc"

        deploy_qt_library QtWidgets XOcalc
        deploy_qt_library QtGui XOcalc
        deploy_qt_library QtCore XOcalc

        deploy_qt_plugin platforms libqcocoa XOcalc
        deploy_qt_plugin platforms libqminimal XOcalc
        deploy_qt_plugin platforms libqoffscreen XOcalc

        make_release XOcalc
        make_clear
    fi
fi
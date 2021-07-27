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

    check_file "$X_SOURCE_PATH/build/release/xocalc.app/Contents/MacOS/xocalc"
    if [ -z "$X_ERROR" ]; then
        cp -R "$X_SOURCE_PATH/build/release/xocalc.app"    "$X_SOURCE_PATH/release/$X_BUILD_NAME"

        mkdir -p $X_SOURCE_PATH/release/$X_BUILD_NAME/xocalc.app/Contents/Resources/signatures

        fiximport "$X_SOURCE_PATH/build/release/xocalc.app/Contents/MacOS/xocalc"

        deploy_qt_library QtWidgets xocalc
        deploy_qt_library QtGui xocalc
        deploy_qt_library QtCore xocalc
        deploy_qt_library QtDBus xocalc
        deploy_qt_library QtPrintSupport xocalc

        deploy_qt_plugin platforms libqcocoa xocalc
        deploy_qt_plugin platforms libqminimal xocalc
        deploy_qt_plugin platforms libqoffscreen xocalc

        make_release xocalc
        make_clear
    fi
fi
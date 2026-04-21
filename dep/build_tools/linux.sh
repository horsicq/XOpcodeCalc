function check_file
{
    if test -f "$1"; then
        echo "Check file $1: TRUE"
    else
        export X_ERROR="true"
        echo "Cannot find file: $1"
    fi
}

function make_init
{
    export X_REVISION=$(date "+%Y%m%d")

    X_ARCHITECTURE=$(dpkg --print-architecture)
    
    export X_ARCHITECTURE
    
    if command -v lsb_release &>/dev/null; then
        X_OS_NAME=$(lsb_release -is)
        X_OS_NUMBER=$(lsb_release -rs)
    else
        X_OS_NAME=$(sed -nE 's/^ID=(\S+)$/\1/p' /etc/os-release)
        X_OS_NUMBER=$(sed -nE 's/^VERSION_ID="?([-0-9a-z._]+)"?$/\1/p' /etc/os-release)
    fi

    export X_OS_VERSION=${X_OS_NAME}_${X_OS_NUMBER}
    export X_QT_VERSION=$($QMAKE_PATH -query QT_VERSION)
    export X_QT_INSTALL_BINS=$($QMAKE_PATH -query QT_INSTALL_BINS)
    export X_QT_INSTALL_PLUGINS=$($QMAKE_PATH -query QT_INSTALL_PLUGINS)
    export X_QT_INSTALL_LIBS=$($QMAKE_PATH -query QT_INSTALL_LIBS)
    export X_QMAKE_SPEC=$($QMAKE_PATH -query QMAKE_SPEC)
    mkdir -p "$X_SOURCE_PATH/release"
    mkdir -p "$X_SOURCE_PATH/release/$X_BUILD_NAME"
    if [[ "$X_BUILD_NAME" == *portable* ]]; then
        touch "$X_SOURCE_PATH/release/$X_BUILD_NAME/portable"
    fi
    
    # get DEBIAN version
    # TODO other Linuxes

    X_DEBIAN_VERSION="$X_OS_NUMBER"
    
    echo "$X_DEBIAN_VERSION"
    
    export X_DEBIAN_VERSION
    
    cp -f $X_SOURCE_PATH/build_tools/build.pri                  $X_SOURCE_PATH/
}

function make_build
{
    if test -f "Makefile"; then
        make clean
    fi
    
    $QMAKE_PATH "$1" -spec $X_QMAKE_SPEC
    make -f Makefile
}

function make_translate 
{
    "$X_QT_INSTALL_BINS/lupdate" $1
    "$X_QT_INSTALL_BINS/lrelease" $1
    # TODO copy
}

function make_release
{
    # TODO
    echo "make_release"
}

function create_deb_app_dir
{
    mkdir -p $X_SOURCE_PATH/release/$X_BUILD_NAME/DEBIAN
    mkdir -p $X_SOURCE_PATH/release/$X_BUILD_NAME/usr
    mkdir -p $X_SOURCE_PATH/release/$X_BUILD_NAME/usr/bin
    mkdir -p $X_SOURCE_PATH/release/$X_BUILD_NAME/usr/lib
    mkdir -p $X_SOURCE_PATH/release/$X_BUILD_NAME/usr/lib/$1
    mkdir -p $X_SOURCE_PATH/release/$X_BUILD_NAME/usr/lib/$1/lang
    mkdir -p $X_SOURCE_PATH/release/$X_BUILD_NAME/usr/share
    mkdir -p $X_SOURCE_PATH/release/$X_BUILD_NAME/usr/share/applications
    mkdir -p $X_SOURCE_PATH/release/$X_BUILD_NAME/usr/share/icons
    mkdir -p $X_SOURCE_PATH/release/$X_BUILD_NAME/usr/share/metainfo
}

function create_deb_control
{
    # Compute Installed-Size in KiB excluding DEBIAN control dir (Debian policy)
    control_file="$1"
    build_dir="$X_SOURCE_PATH/release/$X_BUILD_NAME"
    debian_dir="$build_dir/DEBIAN"

    size_total=0
    size_debian=0
    if [ -d "$build_dir" ]; then
        size_total=$(du -sk "$build_dir" | awk '{print $1}')
        if [ -d "$debian_dir" ]; then
            size_debian=$(du -sk "$debian_dir" | awk '{print $1}')
        fi
    fi
    size_installed=$(( size_total - size_debian ))
    if [ "$size_installed" -lt 0 ] 2>/dev/null; then
        size_installed=0
    fi

    {
        if [ -n "$X_PACKAGENAME" ]; then
            printf 'Package: %s\n' "$X_PACKAGENAME"
        fi
        if [ -n "$X_RELEASE_VERSION" ]; then
            printf 'Version: %s\n' "$X_RELEASE_VERSION"
        fi
        if [ -n "$X_PRIORITY" ]; then
            printf 'Priority: %s\n' "$X_PRIORITY"
        fi
        if [ -n "$X_SECTION" ]; then
            printf 'Section: %s\n' "$X_SECTION"
        fi
        if [ -n "$X_ARCHITECTURE" ]; then
            printf 'Architecture: %s\n' "$X_ARCHITECTURE"
        fi
        if [ -n "$X_MAINTAINER" ]; then
            printf 'Maintainer: %s\n' "$X_MAINTAINER"
        fi
        printf 'Installed-Size: %s\n' "$size_installed"
        if [ -n "$X_DEPENDS" ]; then
            printf 'Depends: %s\n' "$X_DEPENDS"
        fi
        if [ -n "$X_HOMEPAGE" ]; then
            printf 'Homepage: %s\n' "$X_HOMEPAGE"
        fi
        if [ -n "$X_DESCRIPTION" ]; then
            desc="$X_DESCRIPTION"
            # Collapse any newlines to spaces to keep single-line short description
            desc=${desc//$'\n'/ }
            printf 'Description: %s\n' "$desc"
        fi
    } > "$control_file"
}

function create_image_app_dir
{
    mkdir -p $X_SOURCE_PATH/release/appDir
    mkdir -p $X_SOURCE_PATH/release/appDir/usr
    mkdir -p $X_SOURCE_PATH/release/appDir/usr/bin
    mkdir -p $X_SOURCE_PATH/release/appDir/usr/lib
    mkdir -p $X_SOURCE_PATH/release/appDir/usr/lib/$1
    mkdir -p $X_SOURCE_PATH/release/appDir/usr/lib/$1/lang
    mkdir -p $X_SOURCE_PATH/release/appDir/usr/share
    mkdir -p $X_SOURCE_PATH/release/appDir/usr/share/applications
    mkdir -p $X_SOURCE_PATH/release/appDir/usr/share/icons
    mkdir -p $X_SOURCE_PATH/release/appDir/usr/share/metainfo
}

function create_run_shell 
{
    {
        echo "#!/bin/sh"
        echo 'CWD=$(dirname $0)'
        echo 'export LD_LIBRARY_PATH="$CWD/base:$LD_LIBRARY_PATH"'
        echo "\$CWD/base/$2 \$*"
    } > $1
    
    chmod +x $1
}

function make_deb
{
    dpkg -b $X_SOURCE_PATH/release/$X_BUILD_NAME
}

function make_rpm
{
    # TODO
    echo "make_rpm"
}

function make_targz
{
    tar -cvf $2.tar $1
    gzip --best $2.tar
}

function make_clear
{
    rm -rf $X_SOURCE_PATH/release/$X_BUILD_NAME
}

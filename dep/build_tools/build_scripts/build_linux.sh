#!/usr/bin/env bash
set -euo pipefail

# Resolve script and source directories
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
SOURCE_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"

echo "Using source: $SOURCE_DIR"

# Use a local tmp_build inside the project
cd "$SOURCE_DIR"
mkdir -p tmp_build
cd tmp_build

# Detect Qt installation prefix
QT_PREFIX_PATH=""
QT_VERSION=""

if command -v qmake >/dev/null 2>&1; then
	QT_PREFIX_PATH="$(qmake -query QT_INSTALL_PREFIX 2>/dev/null || echo "")"
	QT_VERSION="$(qmake -query QT_VERSION 2>/dev/null || echo "")"
elif command -v qmake-qt5 >/dev/null 2>&1; then
	QT_PREFIX_PATH="$(qmake-qt5 -query QT_INSTALL_PREFIX 2>/dev/null || echo "")"
	QT_VERSION="$(qmake-qt5 -query QT_VERSION 2>/dev/null || echo "")"
fi

if [[ -z "$QT_PREFIX_PATH" ]]; then
	for qt_path in \
		"/usr" "/usr/local" \
		"/opt/Qt6" "/opt/qt6" \
		"/opt/Qt5" "/opt/qt5" \
		"/usr/lib/qt5" "/usr/lib/x86_64-linux-gnu/qt5" \
		"/home/$USER/Qt/6.5.0/gcc_64" \
		"/home/$USER/Qt/6.2.4/gcc_64" \
		"/home/$USER/Qt/5.15.3/gcc_64" \
		"/home/$USER/Qt/5.15.2/gcc_64" \
		"/home/$USER/Qt/5.15.1/gcc_64" \
		"/home/$USER/Qt/5.14.2/gcc_64" \
		"/home/$USER/Qt/5.12.12/gcc_64"; do
		if [[ -d "$qt_path" ]] && { [[ -f "$qt_path/bin/qmake" ]] || [[ -f "$qt_path/lib/cmake/Qt5/Qt5Config.cmake" ]] || [[ -f "$qt_path/lib/cmake/Qt6/Qt6Config.cmake" ]]; }; then
			QT_PREFIX_PATH="$qt_path"
			if [[ -f "$qt_path/bin/qmake" ]]; then
				QT_VERSION="$($qt_path/bin/qmake -query QT_VERSION 2>/dev/null || echo "")"
			fi
			break
		fi
	done
fi

if [[ -n "$QT_PREFIX_PATH" ]]; then
	echo "Using Qt prefix: $QT_PREFIX_PATH ${QT_VERSION:+(v$QT_VERSION)}"
	cmake -DCMAKE_BUILD_TYPE=MinSizeRel -DCMAKE_PREFIX_PATH="$QT_PREFIX_PATH" ..
else
	echo "Warning: Qt not found. Proceeding without CMAKE_PREFIX_PATH (build may fail)."
	cmake -DCMAKE_BUILD_TYPE=MinSizeRel ..
fi

# Build with parallel jobs
if command -v nproc >/dev/null 2>&1; then
	JOBS="$(nproc)"
else
	JOBS=4
fi
make -j"$JOBS"

# Package and copy artifacts
cpack
cd ..
cp -Rf tmp_build/packages .

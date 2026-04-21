#!/usr/bin/env bash
set -euo pipefail

# Optional: sudo ~/Qt/Tools/CMake/CMake.app/Contents/bin/cmake-gui --install

# Resolve script and source directories
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
SOURCE_DIR="$(dirname "$SCRIPT_DIR")/.."

# Use a local tmp_build next to this script's parent (preserves original behavior)
cd "$SOURCE_DIR"
mkdir -p tmp_build
cd tmp_build

# Detect Qt prefix path
QT_PREFIX_PATH=""

# 1) If qmake is available, query its install prefix
if command -v qmake >/dev/null 2>&1; then
	QT_PREFIX_PATH="$(qmake -query QT_INSTALL_PREFIX 2>/dev/null || echo "")"
elif command -v qmake-qt5 >/dev/null 2>&1; then
	QT_PREFIX_PATH="$(qmake-qt5 -query QT_INSTALL_PREFIX 2>/dev/null || echo "")"
fi

# 2) Fallbacks: Home path and Homebrew
if [[ -z "$QT_PREFIX_PATH" ]]; then
	# Common user install
	if [[ -d "${HOME}/Qt/6.5.0/macos" ]]; then QT_PREFIX_PATH="${HOME}/Qt/6.5.0/macos"; fi
	if [[ -z "$QT_PREFIX_PATH" && -d "${HOME}/Qt/5.15.2/clang_64" ]]; then QT_PREFIX_PATH="${HOME}/Qt/5.15.2/clang_64"; fi
	# Homebrew (Qt6 and Qt5)
	if [[ -z "$QT_PREFIX_PATH" ]] && command -v brew >/dev/null 2>&1; then
		BREW_QT_PREFIX="$(brew --prefix qt 2>/dev/null || true)"
		BREW_QT5_PREFIX="$(brew --prefix qt@5 2>/dev/null || true)"
		if [[ -n "$BREW_QT_PREFIX" && -d "$BREW_QT_PREFIX" ]]; then
			QT_PREFIX_PATH="$BREW_QT_PREFIX"
		elif [[ -n "$BREW_QT5_PREFIX" && -d "$BREW_QT5_PREFIX" ]]; then
			QT_PREFIX_PATH="$BREW_QT5_PREFIX"
		fi
	fi
fi

echo "Using source: $SOURCE_DIR"
if [[ -n "$QT_PREFIX_PATH" ]]; then
	echo "Using Qt prefix: $QT_PREFIX_PATH"
	cmake -DCMAKE_BUILD_TYPE=MinSizeRel -DCMAKE_PREFIX_PATH="$QT_PREFIX_PATH" ./..
else
	echo "Warning: Qt not found. Proceeding without CMAKE_PREFIX_PATH (build may fail)."
	cmake -DCMAKE_BUILD_TYPE=MinSizeRel ./..
fi

# Build (use parallel jobs if available)
JOBS="$(command -v sysctl >/dev/null 2>&1 && sysctl -n hw.ncpu 2>/dev/null || echo 4)"
make -j"$JOBS"

# Package
cpack

# Copy packages up one level
cd ..
cp -Rf tmp_build/packages .

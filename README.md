[![Donate](https://img.shields.io/badge/Donate-PayPal-green.svg)](https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=NF3FBD3KHMXDN)
[![GitHub tag (latest SemVer)](https://img.shields.io/github/tag/horsicq/XOpcodeCalc.svg)](https://github.com/horsicq/XOpcodeCalc/releases)
[![GitHub All Releases](https://img.shields.io/github/downloads/horsicq/XOpcodeCalc/total.svg)](https://github.com/horsicq/XOpcodeCalc/releases)

X86/X64 Opcode calculator. The program works on OSX, Linux and Windows.

![Opcode calculator logo](https://github.com/horsicq/XOpcodeCalc/blob/master/mascots/XOpcodeCalc.png "Mascot")

# How to build on Linux(Debian package)

Install packages:

- `sudo apt-get install qtbase5-dev -y`
- `sudo apt-get install qttools5-dev-tools -y`
- `sudo apt-get install git -y`
- `sudo apt-get install build-essential -y`
- `sudo apt-get install qt5-default -y`
- `git clone --recursive https://github.com/horsicq/XOpcodeCalc.git`
- `cd XOpcodeCalc`

Run build script: `bash -x build_dpkg.sh`

Install deb package: `sudo dpkg -i release/xopcodecalc\_[Version].deb`

Run: _xocalc_

# How to build on Linux(Automake)

Qt framework has to be installed on the system.

(Ubuntu)Install Qt Framework: `sudo apt-get install --quiet --assume-yes build-essential qt5-default qtbase5-dev qttools5-dev-tools`

Clone project: `git clone --recursive https://github.com/horsicq/XOpcodeCalc.git`

- `chmod a+x configure`
- `./configure`
- `make`
- `sudo make install`

Run: _xocalc_

# How to build on OSX

Install Qt 5.15.2: https://github.com/horsicq/build_tools

Clone project: `git clone --recursive https://github.com/horsicq/XOpcodeCalc.git`

Edit build_mac.sh ( check QMAKE_PATH variable)

Run build script: `bash -x build_mac.sh`

# How to build on Windows(XP) with Visual Studio

Install Visual Studio 2013: https://github.com/horsicq/build_tools

Install Qt 5.6.3 for VS2013: https://github.com/horsicq/build_tools

Install 7-Zip: https://github.com/horsicq/build_tools

Clone project: `git clone --recursive https://github.com/horsicq/XOpcodeCalc.git`

Edit build_msvc_winxp.bat ( check VSVARS_PATH, SEVENZIP_PATH, QMAKE_PATH variables)

Run build_msvc_winxp.bat

# How to build on Windows(7-10) with Visual Studio

Install Visual Studio 2019: https://github.com/horsicq/build_tools

Install Qt 5.15.2 for VS2019: https://github.com/horsicq/build_tools

Install 7-Zip: https://github.com/horsicq/build_tools

Install Inno Setup: https://github.com/horsicq/build_tools

Clone project: `git clone --recursive https://github.com/horsicq/XOpcodeCalc.git`

Edit build_msvc_win32.bat ( check VSVARS_PATH, SEVENZIP_PATH, INNOSETUP_PATH, QMAKE_PATH variables)

Edit build_msvc_win64.bat ( check VSVARS_PATH, SEVENZIP_PATH, INNOSETUP_PATH, QMAKE_PATH variables)

Run build_msvc_win32.bat

Run build_msvc_win64.bat

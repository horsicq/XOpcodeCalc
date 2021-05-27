[![Donate](https://img.shields.io/badge/Donate-PayPal-green.svg)](https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=NF3FBD3KHMXDN)
[![GitHub tag (latest SemVer)](https://img.shields.io/github/tag/horsicq/XOpcodeCalc.svg)](https://github.com/horsicq/XOpcodeCalc/releases)
[![GitHub All Releases](https://img.shields.io/github/downloads/horsicq/XOpcodeCalc/total.svg)](https://github.com/horsicq/XOpcodeCalc/releases)

Opcode calculator. The program works on OSX, Linux and Windows.

![alt text](https://github.com/horsicq/XOpcodeCalc/blob/master/screenshot_gui.jpg "Screenshot gui")

How to build on Windows
=======

- Install Visual Studio 2013: https://github.com/horsicq/build_tools
- Install Qt 5.6.3 for VS2013: https://github.com/horsicq/build_tools
- Install Qt 5.6.3 for VS2013: https://github.com/horsicq/build_tools
- Install 7-Zip: https://github.com/horsicq/build_tools

Clone project: git clone --recursive https://github.com/horsicq/XOpcodeCalc.git

Edit build_win32.bat ( check VS_PATH,  SEVENZIP_PATH, QT_PATH32 variables)

Edit build_win64.bat ( check VS_PATH,  SEVENZIP_PATH, QT_PATH64 variables)

Run build_win32.bat

Run build_win64.bat

How to build on Linux
=======

Install Qt 5.6.3: https://github.com/horsicq/build_tools

Clone project: git clone --recursive https://github.com/horsicq/XOpcodeCalc.git

Edit build_lin64.sh (check QT_PATH variable)

Run build_lin64.sh

How to build on OSX
=======

Install Qt 5.6.3: https://github.com/horsicq/build_tools

Clone project: git clone --recursive https://github.com/horsicq/XOpcodeCalc.git

Edit build_mac.sh (check QT_PATH variable)

Run build_mac.sh

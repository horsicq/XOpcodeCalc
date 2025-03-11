# Instructions to build XOpcodeCalc

## Building on Linux based on Debian

- Install prerequisite packages:

For Ubuntu 14.04-20.04

```bash
sudo apt-get install --quiet --assume-yes git \
  build-essential \
  qtbase5-dev \
  qttools5-dev-tools \
  qt5-default
```

For Ubuntu 22.04

```bash
sudo apt-get install --quiet --assume-yes git \
  build-essential \
  qtbase5-dev \
  qttools5-dev-tools \
  qt5-qmake
```

- Clone this repository recursively:

```bash
git clone --recursive https://github.com/horsicq/XOpcodeCalc.git
```

- Run build script:

```bash
cd XOpcodeCalc
bash -x build_dpkg.sh
```

- Install deb package:
```bash
sudo dpkg -i release/nfd_[Version].deb
```

Run: *xocalc*

## How to build on Linux(Automake)

Qt framework has to be installed on the system.

- Install git:

```bash
sudo apt-get install --quiet --assume-yes git
```

- For (Ubuntu 20.04) Install Qt Framework:

```bash
sudo apt-get install --quiet --assume-yes git \
  build-essential \
  qt5-default \
  qtbase5-dev \
  qttools5-dev-tools
```

- Clone this repository recursively:

```bash
git clone --recursive https://github.com/horsicq/XOpcodeCalc.git
```

- Build and install

```bash
cd XOpcodeCalc
chmod a+x configure
./configure
make
sudo make install
```

Run: *xocalc*

## How to build on OSX

- Install [Qt 5.15.2](https://github.com/horsicq/build_tools)

- Clone this project recursively:

```bash
git clone --recursive https://github.com/horsicq/XOpcodeCalc.git
cd XOpcodeCalc
```

- Edit build_mac.bat (check QT_PATH variable)

- Run build_mac.bat

## How to build on Windows(XP)

- Install [Visual Studio 2013](https://github.com/horsicq/build_tools)

- Install [Qt 5.6.3 for VS2013](https://github.com/horsicq/build_tools)

- Install [7-Zip](https://www.7-zip.org/)

- Clone this project recursively:

```bash
git clone --recursive https://github.com/horsicq/XOpcodeCalc.git
cd XOpcodeCalc
```

- Edit build_winxp.bat ( check VS_PATH, SEVENZIP_PATH, QT_PATH variables)

- Run build_winxp.bat

## How to build on Windows(7-11)

- Install [Visual Studio 2019](https://github.com/horsicq/build_tools)

- Install [Qt 5.15.2 for VS2019](https://github.com/horsicq/build_tools)

- Install [7-Zip](https://www.7-zip.org)

- Install [Inno Setup](https://github.com/horsicq/build_tools)

- Clone this project recursively:

```bash
git clone --recursive https://github.com/horsicq/XOpcodeCalc.git
cd XOpcodeCalc
```

- Edit build_msvc_win32.bat ( check VSVARS_PATH, SEVENZIP_PATH, INNOSETUP_PATH, QMAKE_PATH variables)

- Edit build_msvc_win64.bat ( check VSVARS_PATH, SEVENZIP_PATH, INNOSETUP_PATH, QMAKE_PATH variables)

- Run build_win32.bat

- Run build_win64.bat

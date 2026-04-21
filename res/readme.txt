# XOpcodeCalc

`XOpcodeCalc` is a small CMake + Qt Widgets desktop application for calculating a handful of common x86 opcode encodings.

The project is intentionally restricted to `x86` / `x86_64` targets:

- CMake aborts on non-x86 architectures.
- The executable links an x86 assembly source file.
- The GUI shows a tiny assembly-backed probe to confirm the ASM module is active.

## Features

- Qt Widgets GUI
- CMake build
- Supports both Qt 5 and Qt 6 Widgets
- Encodes several fixed and immediate-based x86 instructions
- Accepts decimal or `0x`-prefixed hexadecimal input

## Included instruction forms

- `NOP`
- `INT3`
- `RET`
- `RET imm16`
- `INT imm8`
- `MOV EAX, imm32`
- `ADD EAX, imm32`
- `SUB EAX, imm32`
- `XOR EAX, EAX`
- `JMP rel8`
- `CALL rel32`
- `ENTER imm16, imm8`

## Build

Project convention:

- build trees should live in the system temp directory, not in the repository root
- `release/` should contain only ready-to-use package outputs
- the helper scripts in `run/` and `packaging/` already follow this rule

Configure with a Qt installation visible to CMake, for example through `CMAKE_PREFIX_PATH`.

```powershell
$buildDir = Join-Path $env:TEMP "xopcodecalc_manual"
cmake -S . -B $buildDir -DCMAKE_PREFIX_PATH="C:/Qt/5.15.2/msvc2019_64"
cmake --build $buildDir --config Release
```

If you already have Qt in your environment, the extra prefix argument is usually unnecessary.

## Run And Package

- `run/run_xocalc_qt5_msvc2019_32.cmd` and `run/run_xocalc_qt5_msvc2019_64.cmd` build and run from `%TEMP%`
- `packaging/windows/build_portable_win32.bat` and `packaging/windows/build_portable_win64.bat` build in `%TEMP%` and write final portable outputs to `release/`
- `release/` is the only repository directory meant to hold finished portable packages


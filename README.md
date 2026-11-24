# CHIP-8-Emulator
A CHIP-8 emulator written in C++ using SDL3 for graphics, input, and timing.

## Features
- Full implementation of standard CHIP-8 opcodes, delay/sound timers, and 64x32 monochrome display
- Modern keyboard mapping for the CHIP-8 keypad

## Quick Start (Windows Release)
If you just want to run the emulator without building from source:
1. Download the latest `.zip` from the [Releases](../../releases) page.
2. Extract the archive (keep `chip8_emulator.exe` and `SDL3.dll` in the same folder)
3. Run from a terminal with a path to a valid CHIP-8 ROM:
    ```powershell
    .\chip8_emulator.exe path\to\rom
    ```

## Building from Source
### Prerequisites
- C++17 (or newer) compiler
- CMake
- SDL3

### Build Steps
```powershell
# Configure
cmake -B build-release

# Compile in Release mode
cmake --build build-release --config Release
```
Ensure `SDL3.dll` is placed alongside the compiled executable before running it.

## Controls
The 16-key CHIP-8 keypad is mapped to a modern keyboard as follows:

| CHIP-8 Keypad | Keyboard |
| :-------------: | :--------: |
| 1 2 3 C | 1 2 3 4 |
| 4 5 6 D | Q W E R |
| 7 8 9 E | A S D F |
| A 0 B F | Z X C V |

For example, in *Space Invaders*: Q moves left, E moves right, and W shoots.

## Demo Gameplay
https://github.com/user-attachments/assets/46b231f4-d455-4130-9bd6-2b0c6291a016

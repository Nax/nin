# nin

Nin is an advanced Nintendo Entertainement System and Famicom emulator that is actively being developped.

## Features

Right now, the emulator is not very polished on the UX side, but can run many games without issues.  
A gamepad is supported. The exact mapping will depend on the vendor's layout.
On a keyboard, the arrows are mapped to the NES D-Pad, Space is Select, Enter is Start, Z is A and X is B.
Multiple controllers or other kinds of controller are not currently supported.

## Supported Mappers

 * NROM
 * MMC1
 * MMC2
 * MMC3
 * MMC4
 * UxROM (Standard, 180)
 * CNROM
 * AxROM
 * GxROM
 * Color Dreams

MMC3 support is still experimental.

## Build

Nin requires cmake and Qt 5.12 or newer.

Basic build instructions under an Unix-like OS:

    git clone https://github.com/Nax/nin
    cd nin
    mkdir build
    cd build
    cmake .. -DCMAKE_BUILD_TYPE=Release
    make

Under windows, MSVC 2017 and 2019 are the prefered toolchains.

## License

Nin is distributed under the [BSD 2-Clause License](LICENSE).

## Author

Nin was developped by [Maxime Bacoux "Nax"](https://github.com/Nax).

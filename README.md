<p align="center">
  <img alt="nin logo" with="256" height="256" src="https://raw.githubusercontent.com/Nax/nin/master/data/nin.png"/>
  <p align="center">The advanced NES emulator</p>
</p>

![badge-version](https://img.shields.io/github/v/release/Nax/nin?include_prereleases)
![badge-license](https://img.shields.io/badge/license-GPLv2%20%7C%20Commercial-brightgreen)
![badge-build](https://img.shields.io/github/workflow/status/Nax/nin/Nin)
![badge-stars](https://img.shields.io/github/stars/Nax/nin)
![badge-downloads](https://img.shields.io/github/downloads/Nax/nin/total)

## Overview

Nin is an advanced Nintendo Entertainement System and Famicom emulator that is actively being developped.

<p align="center">
  <img with="256" height="240" src="https://raw.githubusercontent.com/Nax/nin/master/screenshots/mario.png" alt="Super Mario Bros."/>
  <img with="256" height="240" src="https://raw.githubusercontent.com/Nax/nin/master/screenshots/kirby.png" alt="Kirby's Adventure"/>
  <img with="256" height="240" src="https://raw.githubusercontent.com/Nax/nin/master/screenshots/zelda.png" alt="The Legend of Zelda"/>
  <img with="256" height="240" src="https://raw.githubusercontent.com/Nax/nin/master/screenshots/megaman2.png" alt="Mega Man 2"/>
  <img with="256" height="240" src="https://raw.githubusercontent.com/Nax/nin/master/screenshots/mario3.png" alt="Super Mario Bros. 3"/>
  <img with="256" height="240" src="https://raw.githubusercontent.com/Nax/nin/master/screenshots/punchout.png" alt="Punch-Out!!"/>
</p>

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
 * MMC5
 * UxROM (Standard, 180)
 * CNROM
 * AxROM
 * GxROM
 * Color Dreams
 * Action 52

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

Nin is [dual licensed](LICENSE) under the follwing licenses:

 * The [GNU General Public License Version 2](LICENSE-GPLv2.txt)
 * A commercial license, upon request

## Author

Nin was developped by [Maxime Bacoux "Nax"](https://github.com/Nax).

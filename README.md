![Microchip Technology Inc.](docs/microchip_logo.png)
# VSC8258EV for Win32 (MEPA) using MCP2210-HIDAPI Library

This code uses a library called MCP2210-HIDAPI to be able to run 
MEPA for Malibu PHY on Windows x86/x64.

## Hardware Setup

### Setup 1
* MCP2210 Breakout Module - [ADM00419](https://www.microchip.com/en-us/development-tool/adm00419)

* VSC8258EV Evaluation Board - [VSC8258EV](https://www.microchip.com/en-us/development-tool/vsc8258ev)
    * Make sure to add jumper/short in `J33 CLK to GND`.

| MCP2210 Breakout Module  | VSC8258 Evaluation Board |
| ------------------------ |:------------------------:|
| Pin 1   GPIO0            | J48 SSN                  |
| Pin 6   MOSI             | J48 MOSI                 |
| Pin 7   SCK              | J48 SCLK                 |
| Pin 8   MISO             | J48 MISO                 |
| Pin 13  GND              | J48 GND                  |

## Software Setup
### Pre-requisites 
* __MCP2210-HIDAPI__ Library 
    * [Branch: `main`](https://github.com/joem-mchp/mcp2210-hidapi)
    * Install [MSYS2](https://www.msys2.org/):
        * Run __MSYS2 UCRT64__ and __install__ the following:
            ```
            pacman -S mingw-w64-ucrt-x86_64-toolchain 
            ```
        * Select "__All__" and "__Yes__".
* __Fork of Microchip Ethernet Switch API__ 
    * [Branch: `v2026.03`](https://github.com/microchip-ung/sw-mepa)

### Build Instructions
1. Initialize submodule
    ```
    git submodule init
    git submodule update
    ```
2. Open bash, create build directory and make sure to run CMake with "MinGW Makefiles". See [CMake Generate Logs](docs/cmake-generate%20logs.txt).
    ```
    bash
    mkdir build
    cd build
    cmake -G "MinGW Makefiles" ../
    ```
3. Run mingw32-make. See [mingw32-make.exe logs](docs/mingw32-make%20logs.txt).
    ```
    mingw32-make.exe
    ```
4. Running sample application. See [app-runtime logs.txt](docs/app-runtime%20logs.txt)
   ```
   Run build\mepa_app_malibu_mcp2210-win32.exe
   ```
### Run Instructions 
Make sure that __hidapi.dll__ is alongside __mepa_app_malibu_mcp2210-win32.exe__. \
They can be copied anywhere. 

## Testing Instructions
Please see [Running MEPA on Windows - VSC8258](to_do)

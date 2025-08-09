[![Build](https://github.com/willll/mednafenSSDev/actions/workflows/build-ci.yml/badge.svg)](https://github.com/willll/mednafenSSDev/actions/workflows/build-ci.yml)

# mednafenSSDev


A managed Mednafen Git repository with relevant fixes to support the Sega Saturn Developer Community.

Inspired by [mednafenPceDev](https://github.com/pce-devel/mednafenPceDev).

## How to Build

Run the following commands:

```bash
./configure --enable-dev-build --enable-debugger
make
```

or

```bash
mkdir build
cd build
export PATH="/opt/mxe-w64/usr/bin/:$PATH"
x86_64-w64-mingw32.static-cmake .. -DWINDOWS=1 -DMDFN_ENABLE_DEV_BUILD=1 -DWANT_DEBUGGER=1
make
```

## How to Configure `mednafen.cfg`

To enable the expansion cart, add the following line to your `mednafen.cfg` file:

```ini
; Expansion cart
ss.cart debug
```

## How to Use in Your Code

... TODO: Add usage instructions ...
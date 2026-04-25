[![Build](https://github.com/willll/mednafenSSDev/actions/workflows/build-ci.yml/badge.svg)](https://github.com/willll/mednafenSSDev/actions/workflows/build-ci.yml)

# mednafenSSDev


A managed Mednafen Git repository with relevant fixes to support the Sega Saturn Developer Community.

Inspired by [mednafenPceDev](https://github.com/pce-devel/mednafenPceDev).

## How to Build

This project uses CMake presets.

### Linux debug build

```bash
cmake --preset debug
cmake --build --preset debug --target mednafen
```

### Rebuild and reinstall from scratch (Linux)

This rebuilds from a clean `build/` directory and installs the executable to `/usr/games/mednafen`.

```bash
cd /home/will/tmp/mednafenSSDev && rm -rf build && cmake --preset debug && cmake --build --preset debug --target mednafen && sudo cmake --install build
```

### Win32 cross-build (MXE toolchain)

```bash
cmake --preset win32
cmake --build --preset win32
```

## How to Configure `mednafen.cfg`

To enable the expansion cart, add the following line to your `mednafen.cfg` file:

```ini
; Expansion cart
ss.cart debug
```

## How to Use in Your Code

... TODO: Add usage instructions ...
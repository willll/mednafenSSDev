# MXE cross-compilation toolchain for x86_64-w64-mingw32.static
# Used inside the Docker image built from .gitlab/Dockerfile

set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

set(MXE_TARGET x86_64-w64-mingw32.static)
set(MXE_ROOT /opt/mxe-w64/usr)

set(CMAKE_C_COMPILER   ${MXE_ROOT}/bin/${MXE_TARGET}-gcc)
set(CMAKE_CXX_COMPILER ${MXE_ROOT}/bin/${MXE_TARGET}-g++)
set(CMAKE_RC_COMPILER  ${MXE_ROOT}/bin/${MXE_TARGET}-windres)

set(CMAKE_FIND_ROOT_PATH ${MXE_ROOT}/${MXE_TARGET})
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

# Point pkg-config at the MXE sysroot
set(PKG_CONFIG_EXECUTABLE "${MXE_ROOT}/bin/${MXE_TARGET}-pkg-config" CACHE FILEPATH "MXE pkg-config wrapper")
set(ENV{PKG_CONFIG_PATH} "")
set(ENV{PKG_CONFIG_LIBDIR} "${MXE_ROOT}/${MXE_TARGET}/lib/pkgconfig")
set(ENV{PKG_CONFIG_SYSROOT_DIR} "${MXE_ROOT}/${MXE_TARGET}")

cmake_minimum_required(VERSION 3.15)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(Mednafen)

include(FindPkgConfig)
include(CheckTypeSize)


include_directories(${CMAKE_BINARY_DIR}) # config.h, trio etc ...
include_directories(BEFORE "include")
include_directories(BEFORE "ares")


# shared
add_compile_definitions("HAVE_CONFIG_H")

# win32 config
if (WIN32)
    add_compile_definitions("WIN32" "UNICODE" "_UNICODE")
else()    
    add_compile_definitions(LOCALEDIR="/usr/local/share/locale/")
endif()

set(CMAKE_CXX_FLAGS -fdiagnostics-color=always)
set(CMAKE_C_FLAGS -fdiagnostics-color=always)



set(SRCS 
"${CMAKE_BINARY_DIR}/config.h"
    src/debug.cpp
    src/error.cpp
    src/mempatcher.cpp
    src/settings.cpp
    src/endian.cpp
    src/mednafen.cpp
    src/git.cpp
    src/file.cpp
    src/general.cpp
    src/memory.cpp
    src/netplay.cpp
    src/state.cpp
    src/state_rewind.cpp
    src/movie.cpp
    src/player.cpp
    src/PSFLoader.cpp
    src/SSFLoader.cpp
    src/SNSFLoader.cpp
    src/SPCReader.cpp
    src/tests.cpp
    src/testsexp.cpp
    src/qtrecord.cpp
    src/IPSPatcher.cpp
    src/VirtualFS.cpp
    src/NativeVFS.cpp
    src/Stream.cpp
    src/MemoryStream.cpp
    src/ExtMemStream.cpp
    src/FileStream.cpp
    src/MTStreamReader.cpp
    # video
    src/video/surface.cpp
    src/video/convert.cpp
    src/video/tblur.cpp
    src/video/Deinterlacer.cpp
    src/video/Deinterlacer_Simple.cpp
    src/video/Deinterlacer_Blend.cpp
    src/video/resize.cpp
    src/video/video.cpp
    src/video/primitives.cpp
    src/video/png.cpp
    src/video/text.cpp
    src/video/font-data.cpp
    src/video/font-data-18x18.c
    src/video/font-data-12x13.c
    # sound
    src/sound/Fir_Resampler.cpp
    src/sound/WAVRecord.cpp
    src/sound/okiadpcm.cpp
    src/sound/DSPUtility.cpp
    src/sound/SwiftResampler.cpp
    src/sound/OwlResampler.cpp
    src/sound/CassowaryResampler.cpp
    # string
    src/string/escape.cpp src/string/string.cpp
    # compress
    src/compress/ArchiveReader.cpp
    src/compress/ZIPReader.cpp
    src/compress/GZFileStream.cpp
    src/compress/DecompressFilter.cpp
    src/compress/ZstdDecompressFilter.cpp
    src/compress/ZLInflateFilter.cpp
    # zstd
    src/zstd/common/entropy_common.c
    src/zstd/common/error_private.c
    src/zstd/common/fse_decompress.c
    src/zstd/common/xxhash.c
    src/zstd/common/zstd_common.c
    src/zstd/decompress/huf_decompress.c
    src/zstd/decompress/zstd_ddict.c
    src/zstd/decompress/zstd_decompress_block.c
    src/zstd/decompress/zstd_decompress.c
    # quicklz
    src/quicklz/quicklz.c
    # minilzo
    src/minilzo/minilzo.c
    # trio
    src/trio/trio.c src/trio/trionan.c src/trio/triostr.c
    # hash
    src/hash/md5.cpp src/hash/sha1.cpp src/hash/sha256.cpp src/hash/crc.cpp
    # cdrom
    src/cdrom/crc32.cpp
    src/cdrom/galois.cpp
    src/cdrom/l-ec.cpp
    src/cdrom/recover-raw.cpp
    src/cdrom/lec.cpp
    src/cdrom/CDUtility.cpp
    src/cdrom/CDInterface.cpp
    src/cdrom/CDInterface_MT.cpp
    src/cdrom/CDInterface_ST.cpp
    src/cdrom/CDAccess.cpp
    src/cdrom/CDAccess_Image.cpp
    src/cdrom/CDAccess_CCD.cpp
    src/cdrom/CDAFReader.cpp
    src/cdrom/CDAFReader_Vorbis.cpp
    src/cdrom/CDAFReader_MPC.cpp
    src/cdrom/CDAFReader_PCM.cpp
    src/cdrom/scsicd.cpp
    #mpdec
    src/mpcdec/huffman.c
    src/mpcdec/mpc_decoder.c
    src/mpcdec/mpc_reader.c
    src/mpcdec/requant.c
    src/mpcdec/streaminfo.c
    src/mpcdec/synth_filter.c
    src/mpcdec/mpc_bits_reader.c
    src/mpcdec/mpc_demux.c
    src/mpcdec/crc32.c
    # tremor
    src/tremor/mdct.c
    src/tremor/block.c
    src/tremor/window.c
    src/tremor/synthesis.c
    src/tremor/info.c
    src/tremor/floor1.c
    src/tremor/floor0.c
    src/tremor/vorbisfile.c
    src/tremor/res012.c
    src/tremor/mapping0.c
    src/tremor/registry.c
    src/tremor/codebook.c
    src/tremor/sharedbook.c
    src/tremor/framing.c
    src/tremor/bitwise.c
    # cputest
    src/cputest/cputest.c src/cputest/x86_cpu.c

    #
    src/resampler/resample.c
    # xeyal
    src/sexyal/sexyal.cpp
    src/sexyal/convert.cpp
    src/sexyal/drivers/dummy.cpp
    src/sexyal/drivers/sdl.cpp

    # net
    src/net/Net.cpp

)

if (WIN32)    
    set(SRCS 
    ${SRCS}
        src/mthreading/MThreading_Win32.cpp
        src/time/Time_Win32.cpp
        src/sexyal/drivers/sdl.cpp
        src/sexyal/drivers/wasapi.cpp
        src/sexyal/drivers/wasapish.cpp
        src/net/Net_WS2.cpp
    )
else()
    # linux
    set(SRCS 
    ${SRCS}

    # tmp
    src/cdrom/CDAFReader_FLAC.cpp

        src/mthreading/MThreading_POSIX.cpp
        src/time/Time_POSIX.cpp
        src/sexyal/drivers/alsa.cpp
        src/sexyal/drivers/oss.cpp
        src/net/Net_POSIX.cpp
    )
endif()

set(DRIVERS 
    src/drivers/main.cpp
    src/drivers/args.cpp
    src/drivers/help.cpp
    src/drivers/ers.cpp
    src/drivers/sound.cpp
    src/drivers/netplay.cpp
    src/drivers/input.cpp
    src/drivers/mouse.cpp
    src/drivers/keyboard.cpp
    src/drivers/Joystick.cpp
    src/drivers/Joystick_SDL.cpp
    src/drivers/TextEntry.cpp
    src/drivers/console.cpp
    src/drivers/cheat.cpp
    src/drivers/fps.cpp
    src/drivers/video-state.cpp
    src/drivers/remote.cpp
    src/drivers/rmdui.cpp
    src/drivers/opengl.cpp
    src/drivers/shader.cpp
    src/drivers/nongl.cpp
    src/drivers/nnx.cpp
    src/drivers/video.cpp
    src/drivers/hqxx-common.cpp
    src/drivers/hq2x.cpp
    src/drivers/hq3x.cpp
    src/drivers/hq4x.cpp
    src/drivers/scale2x.c
    src/drivers/scale3x.c
    src/drivers/scalebit.c
    src/drivers/2xSaI.cpp
    src/drivers/debugger.cpp
    src/drivers/gfxdebugger.cpp
    src/drivers/memdebugger.cpp
    src/drivers/logdebugger.cpp
    src/drivers/prompt.cpp
    # linux
    src/drivers/Joystick_Linux.cpp
)

#================ 

set(DRIVERS_UI 
    src/drivers_imgui/main.cpp
    src/drivers_imgui/args.cpp
    src/drivers_imgui/help.cpp
    src/drivers_imgui/ers.cpp
    src/drivers_imgui/sound.cpp
    src/drivers_imgui/netplay.cpp
    src/drivers_imgui/input.cpp
    src/drivers_imgui/mouse.cpp
    src/drivers_imgui/keyboard.cpp
    src/drivers_imgui/Joystick.cpp
    src/drivers_imgui/Joystick_SDL.cpp
    src/drivers_imgui/TextEntry.cpp
    src/drivers_imgui/console.cpp
    src/drivers_imgui/cheat.cpp
    src/drivers_imgui/fps.cpp
    src/drivers_imgui/video-state.cpp
    src/drivers_imgui/remote.cpp
    src/drivers_imgui/rmdui.cpp
    src/drivers_imgui/opengl.cpp
    src/drivers_imgui/shader.cpp
    src/drivers_imgui/nongl.cpp
    src/drivers_imgui/nnx.cpp
    src/drivers_imgui/video.cpp
    src/drivers_imgui/debugger.cpp
    src/drivers_imgui/gfxdebugger.cpp
    src/drivers_imgui/memdebugger.cpp
    src/drivers_imgui/logdebugger.cpp
    src/drivers_imgui/prompt.cpp
    # linux
    # wip
    src/drivers_imgui/med_imgui.cpp
    src/drivers_imgui/profiler.cpp
    src/drivers_imgui/elf_parser.cpp
)
set(DRIVERS ${DRIVERS_UI}
    src/drivers_imgui/imgui/backends/imgui_impl_opengl2.cpp
    src/drivers_imgui/imgui/backends/imgui_impl_sdl2.cpp
    src/drivers_imgui/imgui/imgui.cpp
    src/drivers_imgui/imgui/imgui_draw.cpp
    src/drivers_imgui/imgui/imgui_tables.cpp
    src/drivers_imgui/imgui/imgui_widgets.cpp
    src/drivers_imgui/imgui/imgui_demo.cpp 
    # sat    
   src/ss/debugui.cpp 
)

if (WIN32)    
    set(SRCS 
        ${SRCS}
        src/win32-common.cpp
        src/drivers_imgui/Joystick_SDL.cpp
        src/drivers_imgui/Joystick_DX5.cpp
        src/drivers_imgui/Joystick_XInput.cpp
    )
else()
    # linux
    set(SRCS 
        ${SRCS}
        src/drivers_imgui/Joystick_Linux.cpp
    )
endif()

add_compile_definitions("DBG_IMGUI")
include_directories(BEFORE "src/drivers_imgui/imgui" "src/drivers_imgui/imgui/backends")
# ================

set(SATURN 
    src/ss/db.cpp
    src/ss/stvio.cpp
    src/ss/ak93c45.cpp
    src/ss/cdb.cpp
    src/ss/sound.cpp
    src/ss/cart.cpp
    src/ss/cart/backup.cpp
    src/ss/cart/cs1ram.cpp
    src/ss/cart/bootrom.cpp
    src/ss/cart/debug.cpp
    src/ss/cart/extram.cpp
    src/ss/cart/rom.cpp
    src/ss/cart/ar4mp.cpp
    src/ss/cart/stv.cpp
    src/ss/ss.cpp
    src/ss/scu_dsp_gen.cpp
    src/ss/scu_dsp_mvi.cpp
    src/ss/scu_dsp_jmp.cpp
    src/ss/scu_dsp_misc.cpp
    src/ss/vdp1.cpp
    src/ss/vdp1_line.cpp
    src/ss/vdp1_sprite.cpp
    src/ss/vdp1_poly.cpp
    src/ss/vdp2.cpp
    src/ss/vdp2_render.cpp
    src/ss/smpc.cpp
    src/ss/input/multitap.cpp
    src/ss/input/gamepad.cpp
    src/ss/input/3dpad.cpp
    src/ss/input/mouse.cpp
    src/ss/input/wheel.cpp
    src/ss/input/mission.cpp
    src/ss/input/gun.cpp
    src/ss/input/keyboard.cpp
    src/ss/input/jpkeyboard.cpp
    # m68k
    src/hw_cpu/m68k/m68k.cpp
    # drivers
    src/ss/ssf.cpp

)
# ================
# NALL (from ares) + GDB
set(GDB_NALL
    #ares/nall/gdb/server.cpp
    #ares/nall/tcptext/tcp-socket.cpp
    #ares/nall/tcptext/tcptext-server.cpp
    src/drivers_imgui/nall-gdb.cpp
)
if (WIN32)
set(GDB_NALL ${GDB_NALL} ares/nall/platform.cpp)
endif()

# ================


set(SASPLAY src/sasplay/sasplay.cpp)

set(SYSTEMS ${SATURN} ${SASPLAY} src/demo/demo.cpp src/cdplay/cdplay.cpp)


# add sdl2
pkg_search_module(SDL2 REQUIRED sdl2)
include_directories(${SDL2_INCLUDE_DIRS})
pkg_search_module(FLAC REQUIRED flac)

find_package(OpenGL REQUIRED)
find_package(ZLIB REQUIRED)
find_package(ZLIB REQUIRED)


set(OS_LIBS )
# Linux
if (UNIX)

pkg_search_module(ALSA REQUIRED alsa)
endif()

if (WIN32)
set(BUILD_SHARED_LIBS OFF)

find_package(Iconv  REQUIRED)
set(OS_LIBS ${OS_LIBS} ws2_32 dinput -static -mconsole)
endif()

# elf/bfd
find_library(BFD_LIBRARY NAMES bfd)
if (BFD_LIBRARY)
    add_compile_definitions("ELF_BFD")
    set(OS_LIBS ${OS_LIBS} ${BFD_LIBRARY})
endif()

if (WIN32)
    find_library(LIBIBERTY_LIBRARIES NAMES iberty)
    set(OS_LIBS ${OS_LIBS} ${LIBIBERTY_LIBRARIES})
endif()


add_executable(${CMAKE_PROJECT_NAME} ${SRCS} ${DRIVERS} ${SYSTEMS} ${GDB_NALL})
target_link_libraries(${CMAKE_PROJECT_NAME} ${SDL2_LIBRARIES} ${OPENGL_LIBRARY} ${FLAC_LINK_LIBRARIES} ${ZLIB_LIBRARIES} ${ALSA_LIBRARIES} ${Iconv_LIBRARIES} ${OS_LIBS} )
target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE -fsigned-char -fwrapv -fno-fast-math -fno-unsafe-math-optimizations -fno-aggressive-loop-optimizations  -Os -fvisibility=hidden )
target_link_options(${CMAKE_PROJECT_NAME} PRIVATE )
set_property(TARGET ${CMAKE_PROJECT_NAME} PROPERTY CXX_STANDARD 23)


set_property(TARGET ${CMAKE_PROJECT_NAME} PROPERTY POSITION_INDEPENDENT_CODE FALSE)

# original build system, copy some folders in include dir
file(COPY src/trio/ DESTINATION ${CMAKE_BINARY_DIR}/trio)
file(COPY src/minilzo/ DESTINATION ${CMAKE_BINARY_DIR}/minilzo)
file(COPY include/minilzo_internal/minilzo.h DESTINATION ${CMAKE_BINARY_DIR}/minilzo/)
file(COPY src/zstd/ DESTINATION ${CMAKE_BINARY_DIR}/zstd)



# config file
# autoconf create a config.h file, attempt to create it from cmake
# =====

check_type_size(char SIZEOF_CHAR)
check_type_size(short SIZEOF_SHORT)
check_type_size(int SIZEOF_INT)
check_type_size(long SIZEOF_LONG)
check_type_size("void *" SIZEOF_VOID_P)
check_type_size("long long" SIZEOF_LONG_LONG)
check_type_size(off_t SIZEOF_OFF_T)
check_type_size(size_t SIZEOF_SIZE_T)
check_type_size(ptrdiff_t SIZEOF_PTRDIFF_T)
check_type_size(__int64 SIZEOF___INT64)


if (WIN32)
    set(MDFN_PSS_STYLE 2)
    set(HAVE_WASAPI 1)
else()
    set(HAVE_ALSA 1)
    set(HAVE_GETPWUID 1)
    set(MDFN_PSS_STYLE 1)
    set(ENABLE_NLS 1)
    set(HAVE_SIGLONGJMP 1)
    set(HAVE_SIGACTION 1)
    set(HAVE_SIGNAL 1)
    set(HAVE_FCNTL 1)
    set(HAVE_FCNTL_H 1)
    set(HAVE_STRERROR_R 1)
    set(MINILZO_HAVE_CONFIG_H 1)
    set(HAVE_MMAP 1)
    set(HAVE_MUNMAP 1)
    set(HAVE_ALLOCA 1)
    set(HAVE_ALLOCA_H 1)
    set(HAVE_LINUX_JOYSTICK 1)
    set(HAVE_OSSDSP 1)
    set(HAVE_POSIX_SOCKETS 1)
    set(HAVE_LIBFLAC 1)
endif()

set(HAVE_ICONV 1)
set(ICONV_CONST " ")
# shared
set(ARCH_X86 1)
set(ARCH_X86_64 1)
set(HAVE_ACCEPT 1)
set(HAVE_ARGZ_COUNT 1)
set(HAVE_ARGZ_H 1)
set(HAVE_ARGZ_NEXT 1)
set(HAVE_ARGZ_STRINGIFY 1)
set(HAVE_ASPRINTF 1)
set(HAVE_BIND 1)
set(HAVE_BUILTIN_EXPECT 1)
set(HAVE_CLOCK_GETTIME 1)
set(HAVE_CLOSE 1)
set(HAVE_CONNECT 1)
set(HAVE_DCGETTEXT 1)
set(HAVE_DECL_FEOF_UNLOCKED 1)
set(HAVE_DECL_FGETS_UNLOCKED 1)
set(HAVE_DECL_GETC_UNLOCKED 1)
set(HAVE_DECL__SNPRINTF 0)
set(HAVE_DECL__SNWPRINTF 0)
set(HAVE_DUP2 1)
set(HAVE_FENV_H 1)
set(HAVE_FORK 1)
set(HAVE_FREEADDRINFO 1)
set(HAVE_FSTAT64 1)
set(HAVE_FTRUNCATE64 1)
set(HAVE_FWPRINTF 1)
set(HAVE_GAI_STRERROR 1)
set(HAVE_GETADDRINFO 1)
set(HAVE_GETCWD 1)
set(HAVE_GETEGID 1)
set(HAVE_GETENV 1)
set(HAVE_GETEUID 1)
set(HAVE_GETGID 1)
set(HAVE_GETHOSTBYADDR 1)
set(HAVE_GETHOSTBYNAME 1)
set(HAVE_GETPAGESIZE 1)
set(HAVE_GETSOCKOPT 1)
set(HAVE_GETTEXT 1)
set(HAVE_GETTIMEOFDAY 1)
set(HAVE_GETUID 1)
set(HAVE_GMTIME_R 1)
set(HAVE_INLINEASM_AVX 1)
set(HAVE_INTMAX_T 1)
set(HAVE_INTTYPES_H 1)
set(HAVE_INTTYPES_H_WITH_UINTMAX 1)
set(HAVE_LANGINFO_CODESET 1)
set(HAVE_LC_MESSAGES 1)
set(HAVE_LIBASOUND 1)
set(HAVE_LIMITS_H 1)
set(HAVE_LISTEN 1)
set(HAVE_LOCALTIME_R 1)
set(HAVE_LONG_LONG_INT 1)
set(HAVE_LSEEK64 1)
set(HAVE_MADVISE 1)
set(HAVE_MBRTOWC 1)
set(HAVE_MEMCMP 1)
set(HAVE_MEMCPY 1)
set(HAVE_MEMMOVE 1)
set(HAVE_MEMPCPY 1)
set(HAVE_MEMSET 1)
set(HAVE_MKDIR 1)
set(HAVE_NANOSLEEP 1)
set(HAVE_NEARBYINT 1)
set(HAVE_NEARBYINTF 1)
set(HAVE_NEWLOCALE 1)
set(HAVE_PIPE 1)
set(HAVE_POLL 1)
set(HAVE_POSIX_PRINTF 1)
set(HAVE_PTHREAD_CONDATTR_SETCLOCK 1)
set(HAVE_PTHREAD_CREATE 1)
set(HAVE_PTHREAD_GETAFFINITY_NP 1)
set(HAVE_PTHREAD_H 1)
set(HAVE_PTHREAD_MUTEX_RECURSIVE 1)
set(HAVE_PTHREAD_RWLOCK 1)
set(HAVE_PTHREAD_SETAFFINITY_NP 1)
set(HAVE_PTHREAD_SETNAME_NP 1)
set(HAVE_PUTENV 1)
set(HAVE_RECV 1)
set(HAVE_RINT 1)
set(HAVE_RINTF 1)
set(HAVE_ROUND 1)
set(HAVE_SCHED_H 1)
set(HAVE_SDL 1)
set(HAVE_SELECT 1)
set(HAVE_SEM_CLOCKWAIT 1)
set(HAVE_SEM_INIT 1)
set(HAVE_SEM_TIMEDWAIT 1)
set(HAVE_SEND 1)
set(HAVE_SETENV 1)
set(HAVE_SETLOCALE 1)
set(HAVE_SETSOCKOPT 1)
set(HAVE_SNPRINTF 1)
set(HAVE_SOCKET 1)
set(HAVE_STDDEF_H 1)
set(HAVE_STDINT_H 1)
set(HAVE_STDINT_H_WITH_UINTMAX 1)
set(HAVE_STDLIB_H 1)
set(HAVE_STPCPY 1)
set(HAVE_STRCASECMP 1)
set(HAVE_STRDUP 1)
set(HAVE_STRERROR 1)
set(HAVE_STRINGS_H 1)
set(HAVE_STRING_H 1)
set(HAVE_STRNLEN 1)
set(HAVE_STRTOUL 1)
set(HAVE_SYS_PARAM_H 1)
set(HAVE_SYS_STAT_H 1)
set(HAVE_SYS_TYPES_H 1)
set(HAVE_TSEARCH 1)
set(HAVE_UINTMAX_T 1)
set(HAVE_UNISTD_H 1)
set(HAVE_UNSIGNED_LONG_LONG_INT 1)
set(HAVE_USELOCALE 1)
set(HAVE_USLEEP 1)
set(HAVE_VISIBILITY 1)
set(HAVE_WCHAR_T 1)
set(HAVE_WCRTOMB 1)
set(HAVE_WCSLEN 1)
set(HAVE_WCSNLEN 1)
set(HAVE_WINT_T 1)
set(HAVE_WORKING_O_NOATIME 1)
set(HAVE_WORKING_O_NOFOLLOW 1)
set(HAVE__LONGJMP 1)
set(HAVE___FSETLOCKING 1)
set(INTDIV0_RAISES_SIGFPE 1)
set(LSB_FIRST 1)
set(MPC_FIXED_POINT 1)
set(PTHREAD_AFFINITY_NP cpu_set_t)

set(SIZEOF_CHAR         ${SIZEOF_CHAR})
set(SIZEOF_INT          ${SIZEOF_INT})
set(SIZEOF_LONG         ${SIZEOF_LONG})
set(SIZEOF_LONG_LONG    ${SIZEOF_LONG_LONG})
set(SIZEOF_OFF_T        ${SIZEOF_OFF_T})
set(SIZEOF_PTRDIFF_T    ${SIZEOF_PTRDIFF_T})
set(SIZEOF_SHORT        ${SIZEOF_SHORT})
set(SIZEOF_SIZE_T       ${SIZEOF_SIZE_T})
set(SIZEOF_VOID_P       ${SIZEOF_VOID_P})
set(SIZEOF___INT64      ${SIZEOF___INT64})

set(STDC_HEADERS 1)
set(USE_POSIX_THREADS 1)
set(USE_POSIX_THREADS_WEAK 1)
#
set(WANT_DEBUGGER 1)
set(WANT_FANCY_SCALERS 1)
set(WANT_INTERNAL_CJK 1)
set(WANT_SASPLAY_EMU 1)
set(WANT_SSFPLAY_EMU 1)
set(WANT_SS_EMU 1)
#
set(PACKAGE "\"mednafen\"")
set(PACKAGE_BUGREPORT "")
set(PACKAGE_NAME "")
set(PACKAGE_STRING "")
set(PACKAGE_TARNAME "")
set(PACKAGE_URL "")
set(PACKAGE_VERSION "")
#
set(MEDNAFEN_VERSION "\"1.32.1\"")
set(MEDNAFEN_VERSION_NUMERIC 0x00103201)
set(VERSION "\"1.32.1\"")

configure_file("cmake/cmake.config.h.in" "${CMAKE_BINARY_DIR}/config.h")
cmake_minimum_required(VERSION 3.15)

include(FindPkgConfig)

project(Mednafen)

include_directories(BEFORE "include")


# linux x64 config

add_compile_definitions("HAVE_CONFIG_H")
add_compile_definitions("LSB_FIRST")
add_compile_definitions("MDFN_PSS_STYLE")
add_compile_definitions(PACKAGE="mednafen")
add_compile_definitions(LOCALEDIR="/usr/local/share/locale/")
add_compile_definitions(MEDNAFEN_VERSION="1.32.1")
add_compile_definitions(MEDNAFEN_VERSION_NUMERIC=0x00103201)


set(SRCS 
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
    src/cdrom/CDAFReader_FLAC.cpp
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

    # linux
    src/mthreading/MThreading_POSIX.cpp
    src/time/Time_POSIX.cpp
    src/sexyal/drivers/alsa.cpp
    src/sexyal/drivers/oss.cpp
    src/net/Net_POSIX.cpp
)


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
    src/drivers_imgui/Joystick_Linux.cpp
    # wip
    src/drivers_imgui/med_imgui.cpp
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
set(SASPLAY src/sasplay/sasplay.cpp)

set(SYSTEMS ${SATURN} ${SASPLAY} src/demo/demo.cpp src/cdplay/cdplay.cpp)
#set(SYSTEMS )


# add sdl2
pkg_search_module(SDL2 REQUIRED sdl2)
include_directories(${SDL2_INCLUDE_DIRS})
pkg_search_module(FLAC REQUIRED flac)
pkg_search_module(ALSA REQUIRED alsa)
find_package(OpenGL REQUIRED)
find_package(ZLIB REQUIRED)


add_executable(${CMAKE_PROJECT_NAME} ${SRCS} ${DRIVERS} ${SYSTEMS})
target_link_libraries(${CMAKE_PROJECT_NAME} ${SDL2_LIBRARIES} ${OPENGL_LIBRARY} ${FLAC_LIBRARIES} ${ZLIB_LIBRARIES} ${ALSA_LIBRARIES})
target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE -fsigned-char -fwrapv     -fno-fast-math -fno-unsafe-math-optimizations -fno-aggressive-loop-optimizations  -Os -fvisibility=hidden )
target_link_options(${CMAKE_PROJECT_NAME} PRIVATE )


set_property(TARGET ${CMAKE_PROJECT_NAME} PROPERTY POSITION_INDEPENDENT_CODE FALSE)
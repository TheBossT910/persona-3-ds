cmake_minimum_required(VERSION 3.23)

project(persona_3_dual LANGUAGES C CXX)

option(P3D_BUILD_TESTS "Build host-side unit tests" ON)
option(P3D_BUILD_NDS "Build Nintendo DS ROM target (WIP)" OFF)
option(P3D_ENABLE_ASSETS "Enable CMake-driven asset generation target" ON)
option(P3D_ENABLE_SDCARD "Enable CMake-driven sdcard packaging target" ON)
option(P3D_USE_BUNDLED_AEGIS "Use bundled libs/aegis_engine CMake target" ON)
option(P3D_BUILD_AEGIS_TESTS "Build bundled aegis_engine tests from root project" OFF)

set(P3D_BLOCKSDS_ROOT "$ENV{BLOCKSDS}" CACHE PATH "Path to BlocksDS core root")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(P3D_NAME "persona-3-dual")
set(P3D_SPECS_FILE "${P3D_BLOCKSDS_ROOT}/sys/crts/ds_arm9.specs")
set(P3D_ROM_PATH "${CMAKE_SOURCE_DIR}/${P3D_NAME}.nds")

set(P3D_ENGINE_INCLUDE_DIRS
    ${CMAKE_SOURCE_DIR}/source
)

set(P3D_BLOCKSDS_INCLUDE_DIRS
    ${P3D_BLOCKSDS_ROOT}/libs/maxmod/include
    ${P3D_BLOCKSDS_ROOT}/libs/libnds/include
)

set(P3D_BLOCKSDS_LIBRARY_DIRS
    ${P3D_BLOCKSDS_ROOT}/libs/maxmod/lib
    ${P3D_BLOCKSDS_ROOT}/libs/libnds/lib
)

find_package(Python3 COMPONENTS Interpreter REQUIRED)

file(GLOB_RECURSE P3D_GAME_HEADERS CONFIGURE_DEPENDS
    ${CMAKE_SOURCE_DIR}/source/*.h
    ${CMAKE_SOURCE_DIR}/source/*.hpp
)

file(GLOB_RECURSE P3D_GAME_SOURCES CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/source/*.cpp)
set(P3D_MAIN_CPP ${CMAKE_SOURCE_DIR}/source/main.cpp)

if(P3D_BUILD_NDS)
    set(P3D_GAME_LIBRARY_SOURCES ${P3D_GAME_SOURCES})
    list(REMOVE_ITEM P3D_GAME_LIBRARY_SOURCES ${P3D_MAIN_CPP})
else()
    # Keep host tests NDS-free by building only the core logic subset.
    set(P3D_GAME_LIBRARY_SOURCES ${CMAKE_SOURCE_DIR}/source/battleActions/skills/BattleCalcsCore.cpp)
endif()

if(P3D_GAME_LIBRARY_SOURCES STREQUAL "")
    message(FATAL_ERROR "No game sources found for p3d_game")
endif()


if(P3D_USE_BUNDLED_AEGIS)
    if(P3D_BUILD_AEGIS_TESTS)
        set(AEGIS_BUILD_TESTS ON CACHE BOOL "Build aegis_engine tests" FORCE)
    else()
        set(AEGIS_BUILD_TESTS OFF CACHE BOOL "Build aegis_engine tests" FORCE)
    endif()
    add_subdirectory(${CMAKE_SOURCE_DIR}/libs/aegis_engine EXCLUDE_FROM_ALL)
endif()

add_library(p3d_game STATIC ${P3D_GAME_LIBRARY_SOURCES})
set_target_properties(p3d_game PROPERTIES CXX_EXTENSIONS ON)
target_compile_features(p3d_game PUBLIC cxx_std_17)
if(TARGET aegis_engine)
    target_link_libraries(p3d_game PUBLIC aegis_engine)
endif()
target_sources(p3d_game
    PUBLIC
        FILE_SET p3d_game_headers TYPE HEADERS
        BASE_DIRS ${CMAKE_SOURCE_DIR}/source
        FILES ${P3D_GAME_HEADERS}
)
target_include_directories(p3d_game
    PUBLIC
        ${P3D_ENGINE_INCLUDE_DIRS}
)

if(P3D_ENABLE_ASSETS)
    find_program(P3D_FFMPEG_EXECUTABLE ffmpeg)
    if(NOT P3D_FFMPEG_EXECUTABLE)
        message(FATAL_ERROR "ffmpeg not found, required for music conversion. Install ffmpeg or configure with -DP3D_ENABLE_ASSETS=OFF")
    endif()

    if(NOT P3D_BLOCKSDS_ROOT)
        set(P3D_BLOCKSDS_ROOT "/opt/blocksds/core")
    endif()

    find_program(P3D_GRIT_EXECUTABLE grit
        HINTS
            ${P3D_BLOCKSDS_ROOT}/tools/grit
            /opt/blocksds/core/tools/grit
            /opt/wonderful/thirdparty/blocksds/core/tools/grit
    )

    if(NOT P3D_GRIT_EXECUTABLE)
        message(FATAL_ERROR "grit not found, required for graphics conversion. Set BLOCKSDS/P3D_BLOCKSDS_ROOT or configure with -DP3D_ENABLE_ASSETS=OFF")
    endif()

    file(GLOB_RECURSE P3D_ASSET_TOOL_INPUTS CONFIGURE_DEPENDS
        ${CMAKE_SOURCE_DIR}/tools/converters/*
    )

    function(p3d_add_asset_group group_name)
        set(options)
        set(oneValueArgs COMMENT)
        set(multiValueArgs PATTERNS)
        cmake_parse_arguments(P3D_GROUP "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

        if(NOT P3D_GROUP_PATTERNS)
            message(FATAL_ERROR "p3d_add_asset_group(${group_name}) requires PATTERNS")
        endif()

        file(GLOB_RECURSE P3D_GROUP_INPUTS CONFIGURE_DEPENDS ${P3D_GROUP_PATTERNS})
        set(P3D_GROUP_STAMP ${CMAKE_BINARY_DIR}/p3d_assets_${group_name}.stamp)
        set(P3D_GROUP_TARGET p3d_assets_${group_name})

        add_custom_command(
            OUTPUT ${P3D_GROUP_STAMP}
            COMMAND ${CMAKE_COMMAND}
                -DP3D_SOURCE_DIR=${CMAKE_SOURCE_DIR}
                -DP3D_PYTHON_EXECUTABLE=${Python3_EXECUTABLE}
                -DP3D_FFMPEG_EXECUTABLE=${P3D_FFMPEG_EXECUTABLE}
                -DP3D_GRIT_EXECUTABLE=${P3D_GRIT_EXECUTABLE}
                -DP3D_ASSET_GROUP=${group_name}
                -P ${CMAKE_SOURCE_DIR}/cmake/BuildAssets.cmake
            COMMAND ${CMAKE_COMMAND} -E touch ${P3D_GROUP_STAMP}
            DEPENDS
                ${P3D_GROUP_INPUTS}
                ${P3D_ASSET_TOOL_INPUTS}
                ${CMAKE_SOURCE_DIR}/tools/build_asset.py
                ${CMAKE_SOURCE_DIR}/cmake/BuildAssets.cmake
            COMMENT "${P3D_GROUP_COMMENT}"
            VERBATIM
        )

        add_custom_target(${P3D_GROUP_TARGET} DEPENDS ${P3D_GROUP_STAMP})
        set_property(GLOBAL APPEND PROPERTY P3D_ASSET_GROUP_TARGETS ${P3D_GROUP_TARGET})
    endfunction()

    p3d_add_asset_group(dialogue
        COMMENT "Generating dialogue assets"
        PATTERNS
            ${CMAKE_SOURCE_DIR}/assets/dialogue/*.dlg
            ${CMAKE_SOURCE_DIR}/assets/dialogue/*.build.json
    )
    p3d_add_asset_group(music
        COMMENT "Generating music assets"
        PATTERNS
            ${CMAKE_SOURCE_DIR}/assets/music/*.mp3
    )
    p3d_add_asset_group(video
        COMMENT "Generating video assets"
        PATTERNS
            ${CMAKE_SOURCE_DIR}/assets/video/*.mp4
            ${CMAKE_SOURCE_DIR}/assets/video/*.build.json
    )
    p3d_add_asset_group(environments
        COMMENT "Generating environment assets"
        PATTERNS
            ${CMAKE_SOURCE_DIR}/assets/environments/*/*.obj
            ${CMAKE_SOURCE_DIR}/assets/environments/*/*.png
            ${CMAKE_SOURCE_DIR}/assets/environments/*/*.mtl
            ${CMAKE_SOURCE_DIR}/assets/environments/*/*.build.json
            ${CMAKE_SOURCE_DIR}/assets/environments/*.build.json
    )
    p3d_add_asset_group(models
        COMMENT "Generating model assets"
        PATTERNS
            ${CMAKE_SOURCE_DIR}/assets/models/*/*.json
            ${CMAKE_SOURCE_DIR}/assets/models/*/*.png
            ${CMAKE_SOURCE_DIR}/assets/models/*/*.build.json
            ${CMAKE_SOURCE_DIR}/assets/models/*.build.json
    )
    p3d_add_asset_group(maps
        COMMENT "Generating map assets"
        PATTERNS
            ${CMAKE_SOURCE_DIR}/assets/maps/*.jmap
    )
    p3d_add_asset_group(graphics
        COMMENT "Generating graphics assets"
        PATTERNS
            ${CMAKE_SOURCE_DIR}/assets/graphics/*.png
            ${CMAKE_SOURCE_DIR}/assets/graphics/*.grit
            ${CMAKE_SOURCE_DIR}/assets/environments/*/*.png
            ${CMAKE_SOURCE_DIR}/assets/environments/*/*.grit
            ${CMAKE_SOURCE_DIR}/assets/models/*/*.png
            ${CMAKE_SOURCE_DIR}/assets/models/*/*.grit
    )
    p3d_add_asset_group(fonts
        COMMENT "Generating font assets"
        PATTERNS
            ${CMAKE_SOURCE_DIR}/assets/fonts/*.png
            ${CMAKE_SOURCE_DIR}/assets/fonts/*.fnt
            ${CMAKE_SOURCE_DIR}/assets/fonts/*.grit
    )

    add_custom_target(p3d_assets)
    get_property(P3D_ASSET_GROUP_TARGETS GLOBAL PROPERTY P3D_ASSET_GROUP_TARGETS)
    if(P3D_ASSET_GROUP_TARGETS)
        add_dependencies(p3d_assets ${P3D_ASSET_GROUP_TARGETS})
    endif()
endif()

if(P3D_ENABLE_SDCARD)
    find_program(P3D_MFORMAT_EXECUTABLE mformat)
    find_program(P3D_MCOPY_EXECUTABLE mcopy)

    if(NOT P3D_MFORMAT_EXECUTABLE OR NOT P3D_MCOPY_EXECUTABLE)
        message(FATAL_ERROR "mtools not found (mformat/mcopy), required for sdcard image packaging. Install mtools or configure with -DP3D_ENABLE_SDCARD=OFF")
    endif()

    file(GLOB_RECURSE P3D_SDCARD_DATA_FILES CONFIGURE_DEPENDS
        ${CMAKE_SOURCE_DIR}/data/*
    )

    set(P3D_SDCARD_IMAGE_PATH ${CMAKE_SOURCE_DIR}/sdcard.img)

    add_custom_command(
        OUTPUT ${P3D_SDCARD_IMAGE_PATH}
        COMMAND ${CMAKE_COMMAND}
            -DP3D_SOURCE_DIR=${CMAKE_SOURCE_DIR}
            -DP3D_PYTHON_EXECUTABLE=${Python3_EXECUTABLE}
            -DP3D_MFORMAT_EXECUTABLE=${P3D_MFORMAT_EXECUTABLE}
            -DP3D_MCOPY_EXECUTABLE=${P3D_MCOPY_EXECUTABLE}
            -P ${CMAKE_SOURCE_DIR}/cmake/PackageSdcard.cmake
        DEPENDS
            ${P3D_SDCARD_DATA_FILES}
            ${P3D_ROM_PATH}
            ${CMAKE_SOURCE_DIR}/cmake/PackageSdcard.cmake
        COMMENT "Packaging sdcard.img from ROM and data directory"
        VERBATIM
    )

    add_custom_target(p3d_sdcard_image DEPENDS ${P3D_SDCARD_IMAGE_PATH})

    if(TARGET p3d_assets)
        add_dependencies(p3d_sdcard_image p3d_assets)
    endif()
endif()

if(P3D_BUILD_TESTS OR P3D_BUILD_AEGIS_TESTS)
    enable_testing()
endif()

if(P3D_BUILD_TESTS)
    add_subdirectory(tests)
endif()

if(P3D_BUILD_NDS)
    if(P3D_BUILD_TESTS)
        message(WARNING "P3D_BUILD_TESTS is ignored for NDS cross-build. Configure with -DP3D_BUILD_TESTS=OFF for toolchain-only builds.")
    endif()

    if(NOT EXISTS ${P3D_SPECS_FILE})
        message(FATAL_ERROR "Missing ARM9 specs file: ${P3D_SPECS_FILE}. Set BLOCKSDS/P3D_BLOCKSDS_ROOT correctly.")
    endif()

    set(P3D_MAXMOD_GEN_DIR ${CMAKE_SOURCE_DIR}/build/${P3D_NAME}/maxmod)
    set(P3D_MAXMOD_SOUND_OBJ ${P3D_MAXMOD_GEN_DIR}/soundbank.c.o)
    set(P3D_ARM7_MAXMOD_ELF ${P3D_BLOCKSDS_ROOT}/sys/arm7/main_core/arm7_maxmod.elf)
    set(P3D_ICON_BMP ${CMAKE_SOURCE_DIR}/icon.bmp)
    set(P3D_NDS_BANNER "Persona 3 Dual\;Memento Mori.\;Atlus, The P3D Project")

    find_program(P3D_NDSTOOL_EXECUTABLE ndstool
        HINTS
            ${P3D_BLOCKSDS_ROOT}/tools/ndstool
            /opt/wonderful/thirdparty/blocksds/core/tools/ndstool
            /opt/blocksds/core/tools/ndstool
    )
    if(NOT P3D_NDSTOOL_EXECUTABLE)
        message(FATAL_ERROR "ndstool not found. Set BLOCKSDS/P3D_BLOCKSDS_ROOT correctly.")
    endif()
    if(NOT EXISTS ${P3D_MAXMOD_GEN_DIR}/soundbank.h OR NOT EXISTS ${P3D_MAXMOD_GEN_DIR}/soundbank_bin.h)
        message(WARNING "Maxmod generated headers not found at ${P3D_MAXMOD_GEN_DIR}. Run the existing Make build once or add CMake maxmod generation before compiling ARM9.")
    endif()

    set(P3D_LINK_EXTRA_OBJECTS)
    if(EXISTS ${P3D_MAXMOD_SOUND_OBJ})
        list(APPEND P3D_LINK_EXTRA_OBJECTS ${P3D_MAXMOD_SOUND_OBJ})
    else()
        message(WARNING "Maxmod object not found: ${P3D_MAXMOD_SOUND_OBJ}. Audio bank symbols may fail to link.")
    endif()

    if(P3D_GAME_LIBRARY_SOURCES STREQUAL "")
        message(FATAL_ERROR "No game sources found under source/")
    endif()

    target_include_directories(p3d_game
        PUBLIC
            ${P3D_BLOCKSDS_INCLUDE_DIRS}
            ${P3D_MAXMOD_GEN_DIR}
    )

    target_compile_definitions(p3d_game PUBLIC __NDS__ __BLOCKSDS__ ARM9)
    target_compile_options(p3d_game PUBLIC
        -Wall
        -mthumb
        -mcpu=arm946e-s+nofp
        -ffunction-sections
        -fdata-sections
        -fno-exceptions
        -fno-rtti
    )

    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_options(p3d_game PUBLIC -O0 -ggdb -Wno-psabi)
    else()
        target_compile_options(p3d_game PUBLIC -O2)
    endif()

    add_executable(p3d_arm9_elf ${P3D_MAIN_CPP} ${P3D_LINK_EXTRA_OBJECTS})
    target_link_libraries(p3d_arm9_elf PRIVATE p3d_game)
    set_target_properties(p3d_arm9_elf PROPERTIES
        OUTPUT_NAME ${P3D_NAME}
        SUFFIX .elf
        CXX_EXTENSIONS ON
        LINKER_LANGUAGE C
    )

    target_link_directories(p3d_arm9_elf PRIVATE ${P3D_BLOCKSDS_LIBRARY_DIRS})
    target_link_libraries(p3d_arm9_elf PRIVATE
        -Wl,--start-group
        mm9
        nds9
        stdc++
        c
        -Wl,--end-group
    )
    target_link_options(p3d_arm9_elf PRIVATE
        -mthumb
        -mcpu=arm946e-s+nofp
        -Wl,-Map,${CMAKE_BINARY_DIR}/${P3D_NAME}.map
        -specs=${P3D_SPECS_FILE}
    )

    if(TARGET p3d_assets)
        add_dependencies(p3d_arm9_elf p3d_assets)
    endif()

    if(NOT EXISTS ${P3D_ARM7_MAXMOD_ELF})
        message(FATAL_ERROR "ARM7 maxmod ELF not found: ${P3D_ARM7_MAXMOD_ELF}")
    endif()
    if(NOT EXISTS ${P3D_ICON_BMP})
        message(FATAL_ERROR "Game icon not found: ${P3D_ICON_BMP}")
    endif()

    add_custom_command(
        OUTPUT ${P3D_ROM_PATH}
        COMMAND ${P3D_NDSTOOL_EXECUTABLE}
            -c ${P3D_ROM_PATH}
            -7 ${P3D_ARM7_MAXMOD_ELF}
            -9 $<TARGET_FILE:p3d_arm9_elf>
            -b ${P3D_ICON_BMP} ${P3D_NDS_BANNER}
        DEPENDS p3d_arm9_elf
        COMMENT "Assembling ${P3D_NAME}.nds via ndstool"
        VERBATIM
    )

    add_custom_target(p3d_nds_rom DEPENDS ${P3D_ROM_PATH})

    if(TARGET p3d_sdcard_image)
        add_dependencies(p3d_sdcard_image p3d_nds_rom)
    endif()

    message(STATUS "Configured NDS ARM9 ELF target: p3d_arm9_elf")
    message(STATUS "Configured NDS ROM target: p3d_nds_rom")
endif()

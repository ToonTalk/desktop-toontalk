cmake_minimum_required(VERSION 3.20)
project(toontalk-core)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Emscripten-specific settings
if(EMSCRIPTEN)
    message(STATUS "Building for WebAssembly with Emscripten")

    # Emscripten flags
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} \
        -s WASM=1 \
        -s ALLOW_MEMORY_GROWTH=1 \
        -s MODULARIZE=1 \
        -s EXPORT_NAME='createToonTalkCore' \
        -s EXPORTED_RUNTIME_METHODS=['ccall','cwrap'] \
        -s EXPORTED_FUNCTIONS=['_malloc','_free'] \
        --bind \
        -s ENVIRONMENT=web \
        -s FILESYSTEM=0 \
        --no-entry"
    )
endif()

# Source files
set(CORE_SOURCES
    platform/platform.cpp
    stubs/windows_stubs.cpp
    sprite.cpp
    core.cpp
    objects.cpp
    # test_platform.cpp - excluded, uses minimal/number.h which conflicts with objects.cpp
)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/platform
    ${CMAKE_CURRENT_SOURCE_DIR}/stubs
    ${CMAKE_CURRENT_SOURCE_DIR}/minimal
)

# Create WASM library
add_executable(toontalk-core ${CORE_SOURCES})

# Set output name
set_target_properties(toontalk-core PROPERTIES
    OUTPUT_NAME "toontalk-core"
)

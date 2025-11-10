#!/bin/bash

# Build script for ToonTalk WASM core
# Requires Emscripten SDK (emsdk) to be installed and activated

set -e  # Exit on error

echo "ToonTalk WASM Core Build Script"
echo "================================"

# Check if emscripten is available
if ! command -v emcc &> /dev/null; then
    echo "Error: Emscripten (emcc) not found!"
    echo "Please install and activate emsdk:"
    echo "  git clone https://github.com/emscripten-core/emsdk.git"
    echo "  cd emsdk"
    echo "  ./emsdk install latest"
    echo "  ./emsdk activate latest"
    echo "  source ./emsdk_env.sh"
    exit 1
fi

echo "Using Emscripten: $(emcc --version | head -n 1)"

# Create build directory
BUILD_DIR="build"
if [ -d "$BUILD_DIR" ]; then
    echo "Cleaning existing build directory..."
    rm -rf "$BUILD_DIR"
fi

mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

echo "Running CMake..."
python $EMSDK/upstream/emscripten/emcmake.py cmake ..

echo "Building WASM module..."
# Use ninja if available, otherwise make
if command -v ninja &> /dev/null; then
    ninja
else
    python $EMSDK/upstream/emscripten/emmake.py make
fi

echo ""
echo "Build complete! Output files:"
ls -lh toontalk-core.*

echo ""
echo "Files are ready in: core/build/"
echo "The web app will load toontalk-core.js and toontalk-core.wasm"

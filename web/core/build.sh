#!/bin/bash

# Build script for ToonTalk WASM core
# Requires Emscripten SDK (emsdk) to be installed and activated

set -e  # Exit on error

echo "ToonTalk WASM Core Build Script"
echo "================================"

# Check if emscripten is available
if ! command -v emcc &> /dev/null; then
    # Try to auto-source local emsdk if available
    SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
    EMSDK_ENV="$SCRIPT_DIR/emsdk/emsdk_env.sh"

    if [ -f "$EMSDK_ENV" ]; then
        echo "Emscripten not in PATH, sourcing local emsdk..."
        source "$EMSDK_ENV"

        if ! command -v emcc &> /dev/null; then
            echo "Error: Failed to activate Emscripten!"
            echo "Try running: source setup-emsdk.sh"
            exit 1
        fi
        echo "✓ Emscripten activated from local emsdk"
    else
        echo "Error: Emscripten (emcc) not found!"
        echo "Please install and activate emsdk:"
        echo "  git clone https://github.com/emscripten-core/emsdk.git"
        echo "  cd emsdk"
        echo "  python emsdk.py install latest"
        echo "  python emsdk.py activate latest"
        echo "  source ./emsdk_env.sh"
        echo ""
        echo "Or run: source setup-emsdk.sh"
        exit 1
    fi
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
# Use ninja if build.ninja was generated, otherwise use make
if [ -f "build.ninja" ]; then
    ninja
else
    emmake make
fi

echo ""
echo "Build complete! Output files:"
ls -lh toontalk-core.*

echo ""
echo "Copying files to public directory..."
cd ..
mkdir -p ../public/core/build
cp build/toontalk-core.js ../public/core/build/
cp build/toontalk-core.wasm ../public/core/build/

echo ""
echo "✅ Build complete and files copied!"
echo "   Output: public/core/build/toontalk-core.{js,wasm}"
echo "   Reload your browser (Ctrl+Shift+R) to use the new WASM module"

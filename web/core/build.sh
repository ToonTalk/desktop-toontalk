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

        # On Windows (Git Bash/MINGW), create a Python wrapper to bypass Windows Store alias
        if [[ "$OSTYPE" == "msys" || "$OSTYPE" == "win32" || "$OSTYPE" == "cygwin" ]]; then
            # Find Python installation
            PYTHON_EXE=""
            for PYDIR in "/c/Python312" "/c/Python311" "/c/Python310" "/c/Python39"; do
                if [ -f "$PYDIR/python.exe" ]; then
                    echo "Found Python at: $PYDIR"
                    PYTHON_EXE="$PYDIR/python.exe"
                    break
                fi
            done

            if [ -n "$PYTHON_EXE" ]; then
                PYTHON_DIR=$(dirname "$PYTHON_EXE")
                TEMP_BIN="$SCRIPT_DIR/.temp-bin"
                mkdir -p "$TEMP_BIN"

                # Copy python.exe to temp bin as python3.exe
                # The copied executable will find DLLs via PATH
                cp "$PYTHON_EXE" "$TEMP_BIN/python3.exe" 2>/dev/null
                cp "$PYTHON_EXE" "$TEMP_BIN/python.exe" 2>/dev/null

                # Add temp bin FIRST (for python3.exe), then Python dir (for DLLs)
                export PATH="$TEMP_BIN:$PYTHON_DIR:$PATH"

                echo "Created python executables in temp directory"
                echo "Python directory: $PYTHON_DIR"

                # Verify python3 resolves correctly
                if command -v python3 &> /dev/null; then
                    PYTHON3_PATH=$(command -v python3)
                    echo "python3 resolved to: $PYTHON3_PATH"

                    # Test if it actually works
                    if "$PYTHON3_PATH" --version &> /dev/null; then
                        echo "✓ python3 is working"
                    else
                        echo "✗ python3 found but not working"
                    fi
                fi
            fi
        fi

        source "$EMSDK_ENV"

        # If emcc is still not found after sourcing, manually add emscripten to PATH
        if ! command -v emcc &> /dev/null; then
            echo "emcc not found after sourcing emsdk_env.sh, adding paths manually..."

            # Add emscripten directories to PATH
            EMSCRIPTEN_DIR="$SCRIPT_DIR/emsdk/upstream/emscripten"
            if [ -d "$EMSCRIPTEN_DIR" ]; then
                export PATH="$EMSCRIPTEN_DIR:$PATH"
                echo "Added to PATH: $EMSCRIPTEN_DIR"
            else
                echo "Warning: Directory not found: $EMSCRIPTEN_DIR"
            fi

            if [ -d "$SCRIPT_DIR/emsdk/upstream/bin" ]; then
                export PATH="$SCRIPT_DIR/emsdk/upstream/bin:$PATH"
            fi

            # Set EMSDK if not already set
            if [ -z "$EMSDK" ]; then
                export EMSDK="$SCRIPT_DIR/emsdk"
            fi

            # Check if emcc file exists and is executable
            if [ -f "$EMSCRIPTEN_DIR/emcc" ]; then
                echo "emcc file exists at: $EMSCRIPTEN_DIR/emcc"
                if [ -x "$EMSCRIPTEN_DIR/emcc" ]; then
                    echo "emcc is executable"
                else
                    echo "emcc is NOT executable, fixing..."
                    chmod +x "$EMSCRIPTEN_DIR/emcc"
                fi
            fi

            # Verify emcc is now available
            if ! command -v emcc &> /dev/null; then
                echo "Error: Failed to activate Emscripten!"
                echo "Debug: SCRIPT_DIR=$SCRIPT_DIR"
                echo "Debug: First 200 chars of PATH: ${PATH:0:200}"
                echo "Debug: Looking for emcc..."
                ls -la "$EMSCRIPTEN_DIR/emcc" 2>&1 || echo "File not found"
                exit 1
            fi
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

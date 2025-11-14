#!/bin/bash
# Setup script for Emscripten SDK
# Run this with: source setup-emsdk.sh

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
EMSDK_DIR="$SCRIPT_DIR/emsdk"

if [ ! -d "$EMSDK_DIR" ]; then
    echo "Error: emsdk not found at $EMSDK_DIR"
    echo "Please run: git clone https://github.com/emscripten-core/emsdk.git"
    return 1
fi

echo "Setting up Emscripten SDK..."

# On Windows (Git Bash/MINGW), ensure Python is in PATH before sourcing emsdk
if [[ "$OSTYPE" == "msys" || "$OSTYPE" == "win32" || "$OSTYPE" == "cygwin" ]]; then
    # Add common Python installation paths on Windows
    export PATH="/c/Python312:/c/Python311:/c/Python310:/c/Python39:$PATH"

    # Create a shell function to bypass Windows app execution alias
    # This ensures 'python' calls python.exe directly from the correct location
    if [ -f "/c/Python311/python.exe" ]; then
        python() { /c/Python311/python.exe "$@"; }
        export -f python
    elif [ -f "/c/Python312/python.exe" ]; then
        python() { /c/Python312/python.exe "$@"; }
        export -f python
    elif [ -f "/c/Python310/python.exe" ]; then
        python() { /c/Python310/python.exe "$@"; }
        export -f python
    elif [ -f "/c/Python39/python.exe" ]; then
        python() { /c/Python39/python.exe "$@"; }
        export -f python
    fi
fi

source "$EMSDK_DIR/emsdk_env.sh"

if command -v emcc &> /dev/null; then
    echo "✓ Emscripten is now available!"
    echo "  emcc version: $(emcc --version | head -1)"
    echo ""
    echo "You can now run: bash build.sh"
else
    echo "✗ Failed to set up Emscripten"
    return 1
fi

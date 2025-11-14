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
    # Create a bin directory with Python wrapper to bypass Windows Store alias
    PYTHON_BIN_DIR="$SCRIPT_DIR/.python-bin"
    mkdir -p "$PYTHON_BIN_DIR"

    # Copy the wrapper as 'python' in the bin directory
    if [ -f "$SCRIPT_DIR/python-wrapper.sh" ]; then
        cp "$SCRIPT_DIR/python-wrapper.sh" "$PYTHON_BIN_DIR/python"
        chmod +x "$PYTHON_BIN_DIR/python"
    fi

    # Add our bin directory to PATH FIRST so it's found before Windows Store alias
    export PATH="$PYTHON_BIN_DIR:$PATH"
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

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

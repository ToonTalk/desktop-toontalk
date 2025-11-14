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
        # Add Python directory directly to PATH (before Windows Store alias paths)
        PYTHON_DIR=$(dirname "$PYTHON_EXE")
        export PATH="$PYTHON_DIR:$PATH"

        # Create a python3 symlink/copy in Python's own directory if it doesn't exist
        if [ ! -f "$PYTHON_DIR/python3.exe" ]; then
            # Try to create a copy of python.exe as python3.exe in the Python directory
            if cp "$PYTHON_EXE" "$PYTHON_DIR/python3.exe" 2>/dev/null; then
                echo "Created python3.exe in Python directory"
            else
                echo "Note: Could not create python3.exe (may need admin rights)"
            fi
        fi

        echo "Added Python directory to PATH: $PYTHON_DIR"
        echo "Python executable: $PYTHON_EXE"
        # Verify python3 can be found
        if command -v python3 &> /dev/null; then
            echo "python3 found at: $(command -v python3)"
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

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
        # Create a temporary bin directory with a python wrapper script
        TEMP_BIN="$SCRIPT_DIR/.temp-bin"
        mkdir -p "$TEMP_BIN"

        # Create wrapper script that calls python.exe directly
        cat > "$TEMP_BIN/python" << EOF
#!/bin/bash
exec "$PYTHON_EXE" "\$@"
EOF
        chmod +x "$TEMP_BIN/python"

        # Add temp bin to PATH FIRST
        export PATH="$TEMP_BIN:$PATH"

        # Also set PYTHON environment variable that emsdk might use
        export PYTHON="$PYTHON_EXE"
        export EM_PYTHON="$PYTHON_EXE"

        echo "Created Python wrapper in temporary bin directory"
        echo "Set PYTHON=$PYTHON_EXE"
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

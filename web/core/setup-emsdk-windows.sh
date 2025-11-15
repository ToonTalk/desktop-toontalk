#!/bin/bash
# Auto-setup Emscripten on Windows with Python detection

echo "🔍 Detecting Python installation..."

# Try to find real Python (not Windows Store stub)
PYTHON_EXE=""

# Check common locations
for path in \
    "/c/Python3"*"/python.exe" \
    "/c/Python"*"/python.exe" \
    "/c/Program Files/Python3"*"/python.exe" \
    "/c/Program Files/Python"*"/python.exe" \
    "$LOCALAPPDATA/Programs/Python/Python"*"/python.exe" \
    "$APPDATA/Local/Programs/Python/Python"*"/python.exe"
do
    if [ -f "$path" ]; then
        # Test if it's real Python (not MS Store stub)
        if "$path" --version >/dev/null 2>&1; then
            PYTHON_EXE="$path"
            echo "✅ Found Python at: $PYTHON_EXE"
            break
        fi
    fi
done

if [ -z "$PYTHON_EXE" ]; then
    echo "⚠️  Could not find Python installation"
    echo "Please install Python from python.org or use emsdk.bat instead"
    echo ""
    echo "Alternative: Run these commands in PowerShell:"
    echo "  cd \$HOME\\emsdk"
    echo "  .\\emsdk.ps1 install latest"
    echo "  .\\emsdk.ps1 activate latest"
    exit 1
fi

# Set environment variable for emsdk
export EMSDK_PYTHON="$PYTHON_EXE"
echo "🔧 Set EMSDK_PYTHON=$EMSDK_PYTHON"

# Install emsdk
echo ""
echo "📦 Installing Emscripten..."
cd ~/emsdk
./emsdk install latest

echo ""
echo "✅ Activating Emscripten..."
./emsdk activate latest

echo ""
echo "🎉 Done! Now run:"
echo "  source ~/emsdk/emsdk_env.sh"
echo "  export EMSDK_PYTHON=\"$PYTHON_EXE\""
echo ""
echo "Or add to your ~/.bashrc:"
echo "  export EMSDK_PYTHON=\"$PYTHON_EXE\""

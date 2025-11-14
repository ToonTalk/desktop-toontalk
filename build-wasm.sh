#!/bin/bash
# Wrapper script to build WASM from project root
# This can be run from anywhere in the project

# Get the directory where this script is located
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
CORE_DIR="$SCRIPT_DIR/web/core"

# Check if we're in the right directory structure
if [ ! -d "$CORE_DIR" ]; then
    echo "Error: Cannot find web/core directory!"
    echo "This script should be in the desktop-toontalk root directory"
    exit 1
fi

echo "Building ToonTalk WASM from: $CORE_DIR"
echo ""

# Change to core directory and run build
cd "$CORE_DIR"
bash build.sh

echo ""
echo "Done! You can now use the WASM module in your web app."

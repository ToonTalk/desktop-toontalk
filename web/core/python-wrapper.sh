#!/bin/bash
# Python wrapper for Windows - bypasses Microsoft Store app execution alias
# This script is added to PATH before emsdk_env.sh runs, so it intercepts 'python' calls

# Find the real Python installation
if [ -f "/c/Python311/python.exe" ]; then
    exec /c/Python311/python.exe "$@"
elif [ -f "/c/Python312/python.exe" ]; then
    exec /c/Python312/python.exe "$@"
elif [ -f "/c/Python310/python.exe" ]; then
    exec /c/Python310/python.exe "$@"
elif [ -f "/c/Python39/python.exe" ]; then
    exec /c/Python39/python.exe "$@"
else
    echo "Error: Python not found in standard Windows locations" >&2
    echo "Looked in: /c/Python311, /c/Python312, /c/Python310, /c/Python39" >&2
    exit 1
fi

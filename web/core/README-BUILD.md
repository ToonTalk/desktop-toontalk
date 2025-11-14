# ToonTalk WASM Core Build Instructions

This directory contains the C++ code for the ToonTalk web port, which is compiled to WebAssembly using Emscripten.

## Prerequisites

- Emscripten SDK (emsdk)
- CMake 3.10 or higher
- Python 3.x

## Quick Start

### First Time Setup

1. **The emsdk is already cloned** in this directory. Just activate it:

```bash
cd web/core
source setup-emsdk.sh
```

This will:
- Activate the Emscripten SDK
- Add emcc and other tools to your PATH for the current session
- Verify the installation

### Building

Once emsdk is activated, just run:

```bash
bash build.sh
```

The build script will:
- Automatically source emsdk if needed
- Create a build directory
- Compile the C++ code to WebAssembly
- Copy the output files to `web/public/core/build/`

### Output Files

The build produces:
- `toontalk-core.js` - JavaScript glue code
- `toontalk-core.wasm` - WebAssembly binary

These are automatically copied to `web/public/core/build/` for use by the web application.

## For Each New Shell Session

When you open a new Git Bash terminal, you need to reactivate Emscripten:

```bash
source web/core/setup-emsdk.sh
```

**OR** add this to your `~/.bash_profile` or `~/.bashrc` to automatically set it up:

```bash
# Auto-setup Emscripten for ToonTalk
if [ -f "$HOME/Documents/desktop-toontalk/web/core/emsdk/emsdk_env.sh" ]; then
    source "$HOME/Documents/desktop-toontalk/web/core/emsdk/emsdk_env.sh" >/dev/null 2>&1
fi
```

(Adjust the path to match your installation location)

## Manual Emscripten Setup

If you need to reinstall or update Emscripten:

```bash
cd web/core/emsdk
python emsdk.py install latest
python emsdk.py activate latest
source emsdk_env.sh
```

## Troubleshooting

### "emcc not found" error

Run: `source web/core/setup-emsdk.sh`

### "Python not found" error

Make sure Python is in your PATH. On Windows with Git Bash:
- Check: `which python` or `which python3`
- If not found, install Python from python.org or use the full path: `/usr/local/bin/python`

### Build fails

1. Make sure emsdk is activated: `source setup-emsdk.sh`
2. Check emcc version: `emcc --version`
3. Clean and rebuild: `rm -rf build && bash build.sh`

## Development Workflow

1. Edit C++ files in `web/core/`
2. Run `bash build.sh`
3. Reload your browser (Ctrl+Shift+R or Cmd+Shift+R)
4. Test your changes in the web app

## Implementation Status

Current implementations (aligned with ToonTalk documentation):

- **Number** - Operations (+, -, *, /, %, ^, ~, &, |), blank numbers, operations
- **Text** - Fixed/variable width fonts, combining, blank text, insertion point
- **Box** - Multiple holes, labels, blank boxes, joining, breaking (0-9 holes)
- **Scale** - Comparison tool with tilt states (TOTTER, TILT_LEFT, TILT_RIGHT, BALANCED, FROZEN, REMEMBERING)

See the ToonTalk documentation in `ToonTalk-website/English/` for complete behavior specifications.

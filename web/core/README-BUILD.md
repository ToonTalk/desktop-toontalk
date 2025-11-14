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

The build script **automatically sources emsdk** if it's not already in PATH. You can run from:

**Option 1: From project root (recommended)**
```bash
bash build-wasm.sh
```

**Option 2: From web/core directory**
```bash
cd web/core
bash build.sh
```

The build script will:
- Automatically detect and source emsdk if needed (no manual activation required!)
- Create a build directory
- Compile the C++ code to WebAssembly
- Copy the output files to `web/public/core/build/`

### Output Files

The build produces:
- `toontalk-core.js` - JavaScript glue code
- `toontalk-core.wasm` - WebAssembly binary

These are automatically copied to `web/public/core/build/` for use by the web application.

## For Each New Shell Session

**Good news:** The build script automatically sources emsdk, so you can just run:

```bash
# From anywhere in the project
bash build-wasm.sh
```

**Optional:** To have emsdk commands (like `emcc`) always available in your terminal without the build script, add this to your `~/.bash_profile`:

**On Windows (Git Bash):**
```bash
# Auto-setup Emscripten for ToonTalk
if [ -f "$HOME/Documents/desktop-toontalk/web/core/emsdk/emsdk_env.sh" ]; then
    source "$HOME/Documents/desktop-toontalk/web/core/emsdk/emsdk_env.sh" >/dev/null 2>&1
fi
```

**On Linux/Mac:**
```bash
# Auto-setup Emscripten for ToonTalk
if [ -f "$HOME/desktop-toontalk/web/core/emsdk/emsdk_env.sh" ]; then
    source "$HOME/desktop-toontalk/web/core/emsdk/emsdk_env.sh" >/dev/null 2>&1
fi
```

Then reload your profile: `source ~/.bash_profile`

**Note:** Adjust the path to match where you cloned the repository.

## Manual Emscripten Setup

If you need to reinstall or update Emscripten:

```bash
cd web/core/emsdk
python emsdk.py install latest
python emsdk.py activate latest
source emsdk_env.sh
```

## Troubleshooting

### "emcc not found" error when running build script

The build script should automatically detect and source emsdk. If it's still failing:

1. **Make sure you have the emsdk directory:**
   ```bash
   ls -la web/core/emsdk/emsdk_env.sh
   ```
   If this file doesn't exist, run the setup from web/core:
   ```bash
   cd web/core
   git clone https://github.com/emscripten-core/emsdk.git
   cd emsdk
   python emsdk.py install latest
   python emsdk.py activate latest
   ```

2. **Try running from the correct directory:**
   ```bash
   # From project root
   bash build-wasm.sh

   # OR from web/core
   cd web/core && bash build.sh
   ```

### "Python not found" error

Make sure Python is in your PATH. On Windows with Git Bash:
- Check: `which python` or `which python3`
- If not found, install Python from python.org or use Git Bash's built-in: `/usr/local/bin/python`

### Build fails with errors

1. Clean the build directory: `rm -rf web/core/build`
2. Try building again: `bash build-wasm.sh`
3. Check for C++ syntax errors in the output

### Wrong directory error

If you see "Cannot find web/core directory", you're running the build-wasm.sh script from the wrong location. It should be run from the project root where build-wasm.sh is located.

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

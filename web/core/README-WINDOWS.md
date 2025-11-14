# ToonTalk WASM Build - Windows Instructions

## Prerequisites

1. **Git Bash** (you already have this)
2. **CMake** - Download from https://cmake.org/download/
3. **Emscripten SDK** - We'll install this below

## Step-by-Step Setup

### 1. Install Emscripten SDK

Open **Git Bash** and run:

```bash
cd ~
git clone https://github.com/emscripten-core/emsdk.git
cd emsdk
./emsdk install latest
./emsdk activate latest
```

This will take a few minutes to download and install.

### 2. Verify Installation

```bash
source ~/emsdk/emsdk_env.sh
emcc --version
```

You should see something like: `emcc (Emscripten gcc/clang-like replacement + linker emulating GNU ld) 4.0.19`

### 3. Build ToonTalk WASM

**Option A: Using Git Bash**

```bash
cd ~/Documents/desktop-toontalk/web/core
source ~/emsdk/emsdk_env.sh
emcmake cmake -B build -S .
emmake make -C build
```

**Option B: Using Windows Batch File**

Just double-click `build-windows.bat` in File Explorer, or run:

```cmd
cd %USERPROFILE%\Documents\desktop-toontalk\web\core
build-windows.bat
```

### 4. Start Test Server

**Option A: Using Git Bash**

```bash
cd ~/Documents/desktop-toontalk/web/core
./serve.sh
```

**Option B: Using Windows Batch File**

Double-click `serve.bat` or run:

```cmd
cd %USERPROFILE%\Documents\desktop-toontalk\web\core
serve.bat
```

### 5. Open in Browser

Go to: http://localhost:8000/test.html

Click "Test Platform Layer" button. You should see:
- A green rectangle
- Text "Hello ToonTalk!"
- Log messages showing C++ calling JavaScript

## Troubleshooting

### "emcmake: command not found"

Make sure you ran `source ~/emsdk/emsdk_env.sh` first in Git Bash.

### "Python was not found"

The `serve.bat` script will automatically use Node.js (which comes with Emscripten). If that doesn't work:

**Install Python:**
1. Download from https://www.python.org/downloads/
2. During installation, check "Add Python to PATH"
3. Restart Git Bash

**OR install Node.js http-server:**
```bash
npm install -g http-server
```

### "cmake: command not found"

Install CMake from https://cmake.org/download/ and make sure to:
- Check "Add CMake to system PATH" during installation
- Restart Git Bash after installation

### Build fails with errors

Try a clean rebuild:

```bash
cd ~/Documents/desktop-toontalk/web/core
rm -rf build
source ~/emsdk/emsdk_env.sh
emcmake cmake -B build -S .
emmake make -C build
```

### Wrong paths

Windows Git Bash uses Unix-style paths:
- Your home directory: `~/` or `/c/Users/YourName/`
- Documents folder: `~/Documents/` or `/c/Users/YourName/Documents/`
- NOT `/home/user/` (that's Linux)

Use `pwd` to see where you are:
```bash
pwd
# Should show something like: /c/Users/toont/Documents/desktop-toontalk/web/core
```

## File Locations

After building, you'll have:

- `build/toontalk-core.wasm` - The WebAssembly binary
- `build/toontalk-core.js` - JavaScript wrapper
- `test.html` - Test page

## Next Steps

Once the test page works, you're ready to start porting the original ToonTalk C++ source!

See:
- `../../RESTART_PLAN.md` - Complete strategy
- `../../PORTING_GUIDE.md` - Step-by-step porting
- `../../IMPLEMENTATION_EXAMPLE.md` - Concrete example

## Quick Reference

### Every time you open Git Bash

```bash
source ~/emsdk/emsdk_env.sh
cd ~/Documents/desktop-toontalk/web/core
```

### To rebuild after changes

```bash
emmake make -C build
```

### To clean rebuild

```bash
rm -rf build
emcmake cmake -B build -S .
emmake make -C build
```

### To test

```bash
./serve.sh
# Then open http://localhost:8000/test.html
```

## Getting Help

If you're still stuck:

1. Check that Emscripten is activated: `emcc --version`
2. Check you're in the right directory: `pwd`
3. Check files exist: `ls -la`
4. Look at build errors carefully - they usually tell you what's wrong

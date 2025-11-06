# ToonTalk Web - Windows Setup Guide

This guide is specifically for Windows users setting up ToonTalk Web.

## Prerequisites

### Required (for JavaScript/TypeScript development):
- **Node.js** 18+ and **npm**
  - Download from: https://nodejs.org/
  - Verify in PowerShell: `node --version` and `npm --version`

- **Modern web browser**
  - Chrome 90+, Firefox 88+, Edge 90+
  - Must support WebAssembly

### Optional (for WASM development):
- **Emscripten SDK**
- **CMake** 3.20+
- **Visual Studio** (for C++ compiler) or **Git Bash** (includes mingw)

## Quick Start (JavaScript Only)

Open PowerShell and navigate to the web directory:

```powershell
cd web
npm install
npm run dev
```

**Note**: PowerShell uses `;` not `&&` to chain commands:
```powershell
npm install; npm run dev
```

Open http://localhost:3000 in your browser!

## Full Setup (Including WASM)

### Option 1: Using PowerShell

#### 1. Install Emscripten SDK

```powershell
# Clone emsdk (in a directory of your choice, e.g., C:\dev)
cd C:\dev
git clone https://github.com/emscripten-core/emsdk.git
cd emsdk

# Install and activate latest version
.\emsdk install latest
.\emsdk activate latest

# Add to PATH for current session
.\emsdk_env.ps1
```

**Important**: You need to run `.\emsdk_env.ps1` every time you open a new PowerShell window, OR add Emscripten to your system PATH permanently via System Properties.

#### 2. Verify Emscripten Installation

```powershell
emcc --version
# Should show: emcc (Emscripten gcc/clang-like replacement) X.X.X
```

#### 3. Build the WASM Module

```powershell
cd path\to\desktop-toontalk\web\core
.\build.ps1
```

### Option 2: Using Git Bash (Easier)

If you have Git for Windows installed, you can use Git Bash which supports Linux-style commands:

1. Open **Git Bash** (not PowerShell)
2. Follow the Linux instructions:

```bash
cd /c/dev  # or wherever you want emsdk
git clone https://github.com/emscripten-core/emsdk.git
cd emsdk
./emsdk install latest
./emsdk activate latest
source ./emsdk_env.sh

# Then build
cd /c/path/to/desktop-toontalk/web/core
./build.sh
```

### 4. Install JavaScript Dependencies

```powershell
cd path\to\desktop-toontalk\web
npm install
```

### 5. Run Development Server

```powershell
npm run dev
```

## Common PowerShell Commands

| Bash/Linux | PowerShell | Description |
|------------|------------|-------------|
| `ls` | `ls` or `dir` | List files |
| `cd folder` | `cd folder` | Change directory |
| `rm -rf folder` | `Remove-Item -Recurse -Force folder` | Delete folder |
| `cat file.txt` | `Get-Content file.txt` | View file |
| `cmd1 && cmd2` | `cmd1; cmd2` | Run commands sequentially |
| `export VAR=value` | `$env:VAR="value"` | Set environment variable |

## Troubleshooting

### PowerShell Execution Policy Error

If you see: "cannot be loaded because running scripts is disabled"

Run PowerShell as Administrator and execute:
```powershell
Set-ExecutionPolicy -ExecutionPolicy RemoteSigned -Scope CurrentUser
```

### Port 3000 Already in Use

```powershell
# Find process using port 3000
netstat -ano | findstr :3000

# Kill the process (replace <PID> with the number from above)
taskkill /PID <PID> /F
```

### WASM Build Fails - No CMake

Install CMake from: https://cmake.org/download/
Or use chocolatey: `choco install cmake`

### WASM Build Fails - No Compiler

Install Visual Studio 2022 Community Edition with "Desktop development with C++" workload
Or use Git Bash which includes mingw

### Node/npm Not Found

1. Download Node.js from https://nodejs.org/
2. Run the installer
3. Restart PowerShell
4. Verify: `node --version`

## Development Workflow

### Starting Development (PowerShell)

```powershell
cd path\to\desktop-toontalk\web
npm run dev
```

### Building WASM (PowerShell)

```powershell
# First, activate Emscripten (each session)
cd C:\dev\emsdk
.\emsdk_env.ps1

# Then build
cd path\to\desktop-toontalk\web
npm run build:wasm
```

### Building for Production

```powershell
npm run build
```

Output in `dist\` directory.

## Recommended Tools for Windows

- **Visual Studio Code** - Best editor for TypeScript/JavaScript
- **Windows Terminal** - Better than default PowerShell
- **Git for Windows** - Includes Git Bash (easier for Emscripten)
- **Node Version Manager (nvm-windows)** - Manage multiple Node.js versions

## Quick Reference

```powershell
# Start development server
npm run dev

# Build WASM (after activating emsdk)
npm run build:wasm

# Build for production
npm run build

# Clean build artifacts
npm run clean

# Run specific file
node .\script.js
```

## Getting Help

If you encounter issues:
1. Check that Node.js is installed: `node --version`
2. Check that npm is working: `npm --version`
3. Make sure you're in the `web\` directory
4. Try deleting `node_modules` and `package-lock.json`, then `npm install` again

For WASM issues:
1. Check Emscripten: `emcc --version`
2. Make sure you ran `.\emsdk_env.ps1` in current session
3. Try using Git Bash instead of PowerShell

---

**Pro Tip**: Use Git Bash for a better experience with Emscripten tools, as they were designed for Unix-like environments.

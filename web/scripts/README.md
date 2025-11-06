# ToonTalk Web - Quick Start Scripts for Windows

This folder contains convenient scripts for Windows users.

## PowerShell Scripts

### `start-dev.ps1` - Start development server
```powershell
.\start-dev.ps1
```
Installs dependencies (if needed) and starts the Vite dev server.

### `build-wasm.ps1` - Build WASM module
```powershell
.\build-wasm.ps1
```
Builds the C++ core to WebAssembly (requires Emscripten).

### `clean.ps1` - Clean build artifacts
```powershell
.\clean.ps1
```
Removes node_modules, dist, and build directories.

## Usage

1. **First time setup:**
   ```powershell
   .\start-dev.ps1
   ```
   This will install dependencies and start the server.

2. **Daily development:**
   ```powershell
   cd web\scripts
   .\start-dev.ps1
   ```

3. **After changing C++ code:**
   ```powershell
   .\build-wasm.ps1
   ```
   Then refresh your browser.

## Execution Policy

If you get an error about execution policy, run PowerShell as Administrator:
```powershell
Set-ExecutionPolicy -ExecutionPolicy RemoteSigned -Scope CurrentUser
```

## Alternative: Git Bash

You can also use Git Bash and run the shell scripts:
```bash
cd web/core
./build.sh
```

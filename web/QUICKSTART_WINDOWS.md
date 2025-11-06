# 🚀 ToonTalk Web - Windows Quick Start

## Super Quick Start (Just want to see it work!)

1. Open **PowerShell**
2. Navigate to the web folder:
   ```powershell
   cd path\to\desktop-toontalk\web
   ```
3. Run these commands **one at a time**:
   ```powershell
   npm install
   ```
   Then:
   ```powershell
   npm run dev
   ```
4. Open **http://localhost:3000** in your browser!

## What You'll See

- A golden bouncing "bird" sprite
- Hover over it to make it bigger
- Click to rotate it
- FPS counter at the bottom

## If Something Goes Wrong

### "npm not found"
Install Node.js from https://nodejs.org/ (LTS version)

### Port 3000 already in use
Change the port in `vite.config.ts` or kill the process:
```powershell
netstat -ano | findstr :3000
# Note the PID number, then:
taskkill /PID <number> /F
```

### Want to clean everything and start over?
```powershell
cd web
Remove-Item -Recurse -Force node_modules, dist, package-lock.json
npm install
npm run dev
```

## Pro Tips for Windows Users

**Easier command chaining:**
Use `;` instead of `&&`:
```powershell
npm install; npm run dev
```

**Convenient scripts:**
```powershell
cd web\scripts
.\start-dev.ps1    # Starts dev server
.\build-wasm.ps1   # Builds C++ to WebAssembly (advanced)
.\clean.ps1        # Clean build files
```

## Next Steps

- Read `SETUP_WINDOWS.md` for full Windows setup guide
- Read `README.md` for project overview
- Read `MIGRATION_GUIDE.md` to see what's being migrated

## Emscripten (Advanced - Optional for now)

The JavaScript demo works without building the WASM module.

To build C++ to WebAssembly later:
1. Install Emscripten (see `SETUP_WINDOWS.md`)
2. Run:
   ```powershell
   cd web\scripts
   .\build-wasm.ps1
   ```

But **you don't need this** to see the proof-of-concept!

---

**Need help?** Check `SETUP_WINDOWS.md` for detailed troubleshooting.

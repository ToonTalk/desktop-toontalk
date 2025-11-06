# ToonTalk Web - Setup Guide

This guide will help you set up the development environment for ToonTalk Web.

## Prerequisites

### Required (for JavaScript/TypeScript development):
- **Node.js** 18+ and **npm**
  - Download from: https://nodejs.org/
  - Verify: `node --version` and `npm --version`

- **Modern web browser**
  - Chrome 90+, Firefox 88+, Safari 15+, or Edge 90+
  - Must support WebAssembly

### Optional (for WASM development):
- **Emscripten SDK** (for compiling C++ to WebAssembly)
- **CMake** 3.20+
- **C++ compiler** (GCC, Clang, or MSVC)

## Quick Start (JavaScript Only)

If you just want to run the proof-of-concept without building WASM:

```bash
cd web
npm install
npm run dev
```

Open http://localhost:3000 in your browser. You should see the ToonTalk demo with a bouncing sprite!

## Full Setup (Including WASM)

### 1. Install Emscripten SDK

```bash
# Clone emsdk
git clone https://github.com/emscripten-core/emsdk.git
cd emsdk

# Install and activate latest version
./emsdk install latest
./emsdk activate latest

# Add to PATH (do this every session, or add to .bashrc/.zshrc)
source ./emsdk_env.sh
```

Verify installation:
```bash
emcc --version
# Should show: emcc (Emscripten gcc/clang-like replacement) X.X.X
```

### 2. Build the WASM Module

```bash
cd web/core
chmod +x build.sh
./build.sh
```

This will create:
- `core/build/toontalk-core.js` - JavaScript glue code
- `core/build/toontalk-core.wasm` - WebAssembly binary

### 3. Install JavaScript Dependencies

```bash
cd web
npm install
```

### 4. Run Development Server

```bash
npm run dev
```

The app should now be running at http://localhost:3000

## Development Workflow

### Making Changes to JavaScript/TypeScript

1. Edit files in `web/src/`
2. Vite will hot-reload automatically
3. Check browser console for errors

### Making Changes to C++ (WASM)

1. Edit files in `web/core/`
2. Rebuild WASM: `npm run build:wasm`
3. Refresh the browser

## Project Structure

```
web/
├── core/                  # C++ source (compiled to WASM)
│   ├── sprite.cpp        # Sprite system
│   ├── core.cpp          # Core functions
│   ├── CMakeLists.txt    # Build config
│   └── build.sh          # Build script
│
├── src/                   # TypeScript source
│   ├── main.ts           # Entry point
│   ├── renderer/         # PixiJS rendering
│   ├── input/            # Input handling
│   └── types/            # TypeScript definitions
│
├── public/                # Static assets
│   ├── index.html        # HTML entry
│   └── sprites/          # Sprite images
│
└── dist/                  # Build output (generated)
```

## Available Scripts

- `npm run dev` - Start development server
- `npm run build` - Build for production
- `npm run preview` - Preview production build
- `npm run build:wasm` - Build WASM module
- `npm run clean` - Clean build artifacts

## Troubleshooting

### Port 3000 already in use
```bash
# Edit vite.config.ts and change the port
# Or kill the process using port 3000
```

### WASM build fails
- Make sure Emscripten is installed and activated
- Run `source /path/to/emsdk/emsdk_env.sh`
- Check that CMake is available: `cmake --version`

### TypeScript errors
```bash
# Reinstall dependencies
rm -rf node_modules package-lock.json
npm install
```

### PixiJS not loading
- Check browser console for errors
- Ensure you have a modern browser with WebGL support
- Try clearing browser cache

## Next Steps

1. **Add actual ToonTalk sprites**: Convert BMP files from `../source/pics/` to PNG
2. **Port more C++ classes**: Start with basic objects from `../source/`
3. **Implement save/load**: Use IndexedDB for persistence
4. **Add audio**: Integrate Web Audio API with sounds from `../source/sounds/`

## Resources

- [Emscripten Documentation](https://emscripten.org/docs/)
- [PixiJS Documentation](https://pixijs.com/)
- [Vite Documentation](https://vitejs.dev/)
- [TypeScript Handbook](https://www.typescriptlang.org/docs/)

## Getting Help

- Check the main README: `web/README.md`
- Review the original C++ source: `../source/`
- Look at the proof-of-concept code for examples

Happy coding! 🚀

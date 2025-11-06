# ToonTalk Web - Proof of Concept

This is the web version of ToonTalk, a visual programming environment for education. This proof-of-concept demonstrates the hybrid approach of using WebAssembly (compiled from C++) for core game logic and modern web technologies for rendering and UI.

## Architecture

- **Core Engine**: C++ compiled to WebAssembly via Emscripten
- **Rendering**: PixiJS (WebGL-based 2D renderer)
- **UI**: TypeScript + HTML5
- **Build System**: Vite + CMake/Emscripten

## Prerequisites

### For JavaScript/TypeScript development:
- Node.js 18+ and npm
- Modern browser with WebAssembly support

### For WASM development (optional for now):
- Emscripten SDK (emsdk)
- CMake 3.20+
- C++ compiler

## Quick Start

### 1. Install JavaScript dependencies:
```bash
cd web
npm install
```

### 2. Run the development server:
```bash
npm run dev
```

This will start the Vite dev server at http://localhost:3000

### 3. Build WASM module (when ready):
```bash
npm run build:wasm
```

## Project Structure

```
web/
├── core/              # C++ source code (compiled to WASM)
│   ├── sprite.cpp     # Sprite system
│   ├── object.cpp     # Game object base
│   └── CMakeLists.txt # Build configuration
├── src/               # TypeScript/JavaScript source
│   ├── renderer/      # PixiJS rendering layer
│   ├── audio/         # Web Audio API wrapper
│   ├── input/         # Input handling
│   ├── ui/            # UI components
│   ├── types/         # TypeScript type definitions
│   └── main.ts        # Entry point
├── public/            # Static assets
│   ├── sprites/       # Sprite images
│   ├── sounds/        # Audio files
│   └── index.html     # HTML entry point
└── dist/              # Build output
```

## Development Roadmap

### Phase 1: Proof of Concept (Current) ✓
- [x] Project setup
- [ ] Basic WASM module compilation
- [ ] PixiJS renderer initialization
- [ ] Display a single sprite
- [ ] Mouse interaction
- [ ] WASM ↔ JS communication

### Phase 2: Core Engine
- [ ] Port sprite system
- [ ] Port game object hierarchy
- [ ] Event system
- [ ] Basic physics

### Phase 3: Features
- [ ] Audio system
- [ ] Save/load (IndexedDB)
- [ ] UI framework
- [ ] Touch support

## Building for Production

```bash
npm run build
```

Output will be in the `dist/` directory.

## References

- Original C++ source: `../source/`
- [Emscripten Documentation](https://emscripten.org/docs/)
- [PixiJS Documentation](https://pixijs.com/)
- [ToonTalk Original](http://www.toontalk.com/)

## License

BSD 3-Clause License (same as original ToonTalk)

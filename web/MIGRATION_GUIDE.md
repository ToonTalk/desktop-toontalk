# ToonTalk Migration Guide - Desktop to Web

This document outlines the strategy and progress for migrating ToonTalk from a Windows desktop application to a modern web application.

## Migration Strategy: Hybrid Approach (Option C)

We're using a hybrid approach that balances code reuse with modern web best practices:

- **Core Logic → WASM**: Game objects, physics, math (C++ → WebAssembly)
- **Rendering → PixiJS**: Graphics engine (DirectDraw/GDI → WebGL)
- **UI → Web**: Modern HTML/CSS/TypeScript
- **APIs → Web Standards**: Audio, input, storage (DirectX → Web APIs)

## Library Replacement Map

| Original (C++ Desktop) | Replacement (Web) | Status |
|------------------------|-------------------|--------|
| DirectDraw/GDI | PixiJS (WebGL) | ✅ Setup complete |
| DirectInput | Pointer/Keyboard Events | ✅ Setup complete |
| DirectSound | Web Audio API | ⏳ Pending |
| DirectPlay | WebRTC/WebSockets | ⏳ Pending |
| Windows UI | HTML/CSS/React | ⏳ Pending |
| SAPI (TTS) | Web Speech API | ⏳ Pending |
| Windows File I/O | File System Access API | ⏳ Pending |
| GMP (big numbers) | BigInt / bn.js | ⏳ Pending |
| ZIP | JSZip | ⏳ Pending |

## File Migration Status

### Phase 1: Core Objects ✅ In Progress

| C++ Source | Web Equivalent | Status | Priority |
|------------|----------------|--------|----------|
| `sprite.cpp` (74KB) | `core/sprite.cpp` | ✅ Basic version | HIGH |
| `object.cpp` | `core/object.cpp` | ⏳ TODO | HIGH |
| `bird.cpp` (30KB) | `core/bird.cpp` | ✅ Stub | MEDIUM |
| `robot.cpp` (105KB) | `core/robot.cpp` | ⏳ TODO | HIGH |
| `number.cpp` (21KB) | `core/number.cpp` | ⏳ TODO | HIGH |
| `text.cpp` (40KB) | `core/text.cpp` | ⏳ TODO | MEDIUM |

### Phase 2: Rendering System ✅ In Progress

| C++ Source | Web Equivalent | Status |
|------------|----------------|--------|
| `blit.cpp` (17KB) | `src/renderer/` | ✅ PixiJS wrapper |
| `screen.cpp` (27KB) | `src/renderer/` | ✅ Basic |
| `picture.cpp` (66KB) | `src/renderer/sprite-loader.ts` | ⏳ TODO |
| `animate.cpp` (18KB) | PixiJS Ticker | ✅ Using PixiJS |

### Phase 3: Input & Interaction ✅ Basic Complete

| C++ Source | Web Equivalent | Status |
|------------|----------------|--------|
| `mouse.cpp` | `src/input/input.ts` | ✅ Basic |
| `keyboard.txt` | `src/input/input.ts` | ✅ Basic |
| DirectInput code | Pointer Events API | ✅ Basic |

### Phase 4: Audio System ⏳ Not Started

| C++ Source | Web Equivalent | Status |
|------------|----------------|--------|
| `sound.cpp` (33KB) | `src/audio/sound.ts` | ⏳ TODO |
| `wave.cpp` (4KB) | `src/audio/audio-manager.ts` | ⏳ TODO |
| `dsutil.cpp` (20KB) | Web Audio API | ⏳ TODO |
| `speak.cpp` (14KB) | `src/audio/speech.ts` | ⏳ TODO |

### Phase 5: File System ⏳ Not Started

| C++ Source | Web Equivalent | Status |
|------------|----------------|--------|
| `file.cpp` (14KB) | `src/storage/file.ts` | ⏳ TODO |
| `ttfile.cpp` | `src/storage/save-load.ts` | ⏳ TODO |
| `zip.cpp` | JSZip library | ⏳ TODO |
| `xml.cpp` (15KB) | DOMParser / JSON | ⏳ TODO |

### Phase 6: Networking ⏳ Not Started

| C++ Source | Web Equivalent | Status |
|------------|----------------|--------|
| `network.cpp` (23KB) | `src/network/multiplayer.ts` | ⏳ TODO |
| DirectPlay code | WebRTC/WebSockets | ⏳ TODO |
| `ftp.cpp` | Fetch API | ⏳ TODO |

### Phase 7: UI Components ⏳ Not Started

| C++ Source | Web Equivalent | Status |
|------------|----------------|--------|
| Windows dialogs | React components | ⏳ TODO |
| `tools.cpp` (82KB) | `src/ui/tools.tsx` | ⏳ TODO |
| Resource files (.rc) | i18n JSON files | ⏳ TODO |

## Key Challenges & Solutions

### Challenge 1: DirectDraw → PixiJS
**Problem**: ToonTalk uses DirectDraw for 2D sprite rendering with palette management.

**Solution**:
- ✅ Use PixiJS (WebGL-based) for hardware-accelerated rendering
- ✅ Convert palette-based BMPs to RGBA PNGs
- ✅ Sprite batching for performance
- ⏳ Implement sprite loader from original assets

### Challenge 2: Memory Management
**Problem**: C++ manual memory management vs JavaScript garbage collection.

**Solution**:
- ✅ Use Emscripten's memory allocator for WASM
- ✅ Careful handling of WASM ↔ JS boundary
- ⏳ Implement object pooling for performance
- ⏳ Proper cleanup with `.delete()` calls on WASM objects

### Challenge 3: File Format Compatibility
**Problem**: Binary .tt save files need to work on the web.

**Solution**:
- ⏳ Port serialization code to WASM (reuse C++ logic)
- ⏳ Add JSON export/import option
- ⏳ Use IndexedDB for storage
- ⏳ Backward compatibility layer

### Challenge 4: Large Codebase
**Problem**: 175,000 lines of C++ to migrate.

**Solution**:
- ✅ Start with core classes (Sprite, Object hierarchy)
- ✅ Use WASM for complex logic
- ⏳ Progressive migration - web version works alongside desktop
- ⏳ Focus on essential features first

### Challenge 5: Asset Conversion
**Problem**: 500+ BMP files, 100+ WAV files in legacy formats.

**Solution**:
- ⏳ Batch convert BMPs to PNG with ImageMagick
- ⏳ Convert WAVs to MP3/OGG with FFmpeg
- ⏳ Create asset manifest for lazy loading
- ⏳ Maintain original assets in `source/` for reference

## Migration Phases

### ✅ Phase 1: Proof of Concept (COMPLETE)
- [x] Project structure
- [x] Build system (Vite + CMake)
- [x] PixiJS renderer setup
- [x] Basic input handling
- [x] Simple WASM module
- [x] Demo running in browser

**Result**: You can run `npm run dev` and see a demo!

### Phase 2: Core Engine (4-6 weeks)
- [ ] Port Sprite hierarchy
- [ ] Port basic game objects (Bird, Robot, Number, Text)
- [ ] Event system
- [ ] Object serialization
- [ ] Basic physics/collision

### Phase 3: Rendering (2-3 weeks)
- [ ] Asset conversion pipeline
- [ ] Sprite loading system
- [ ] Animation system
- [ ] Palette to RGBA conversion
- [ ] Text rendering

### Phase 4: Audio (1-2 weeks)
- [ ] Web Audio API wrapper
- [ ] Sound effect manager
- [ ] Audio mixing
- [ ] Text-to-speech integration

### Phase 5: Persistence (2 weeks)
- [ ] IndexedDB wrapper
- [ ] Save/load system
- [ ] User profiles
- [ ] Settings management

### Phase 6: UI Framework (3-4 weeks)
- [ ] React components for UI
- [ ] Tools implementation
- [ ] Menus and dialogs
- [ ] Help system

### Phase 7: Advanced Features (4-6 weeks)
- [ ] Networking (multiplayer)
- [ ] Touch support
- [ ] Mobile optimization
- [ ] Progressive Web App

### Phase 8: Polish (2-3 weeks)
- [ ] Performance optimization
- [ ] Browser compatibility
- [ ] Loading screens
- [ ] Error handling
- [ ] Documentation

## Progress Tracking

**Current Phase**: Phase 1 ✅
**Overall Progress**: ~5%
**Lines of Code**:
- C++ (WASM): ~200 / ~40,000 target
- TypeScript: ~400 / ~15,000 target

**Next Milestone**: Complete Phase 2 - Core Engine

## Testing Strategy

1. **Unit Tests**: Test WASM functions independently
2. **Integration Tests**: Test WASM ↔ JS communication
3. **Visual Tests**: Compare rendering with original
4. **Performance Tests**: Ensure smooth 60 FPS
5. **Compatibility Tests**: Test across browsers

## How to Contribute

### Adding a New C++ Class
1. Create `.cpp` file in `web/core/`
2. Add Emscripten bindings
3. Update `CMakeLists.txt`
4. Create TypeScript definitions in `web/src/types/`
5. Rebuild WASM: `npm run build:wasm`

### Adding a New TypeScript Module
1. Create file in appropriate `web/src/` subdirectory
2. Import and use in `main.ts`
3. Hot reload will update automatically

### Converting Assets
1. Put original assets in `source/pics/` or `source/sounds/`
2. Run conversion script (TODO: create this)
3. Output goes to `web/public/sprites/` or `web/public/sounds/`

## References

- Original C++ source: `../source/`
- ToonTalk papers: `../source/Doc/`
- Original documentation: http://www.toontalk.com/

---

Last Updated: 2025-11-06

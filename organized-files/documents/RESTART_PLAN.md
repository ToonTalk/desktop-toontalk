# ToonTalk Web Port - Fresh Start with Original C++ Plan

## What We're Doing Differently

**Previous Approach:**
- Created minimal C++ data structures in `web/core/objects.cpp`
- Re-implemented all behavior in TypeScript
- ❌ Result: No behavior, objects don't work like original ToonTalk

**New Approach:**
- ✅ Compile the **original ToonTalk C++ source** (`source/*.cpp`) to WASM
- ✅ Keep **all 30+ years of behavior logic**
- ✅ Only replace rendering: DirectX → JavaScript callbacks
- ✅ Result: Authentic ToonTalk behavior with web rendering

## What's Been Prepared

### ✅ Platform Abstraction Layer
- **Location:** `web/core/platform/`
- **Purpose:** Bridges C++ rendering calls to JavaScript
- **Files:**
  - `platform.h` - Interface definitions
  - `platform.cpp` - Implementation using EM_ASM

### ✅ JavaScript Rendering Bridge
- **Location:** `web/src/renderer/cpp-renderer-bridge.ts`
- **Purpose:** Receives rendering calls from C++ via EM_ASM
- **Implements:** `window.ToonTalkRenderer` with PixiJS

### ✅ JavaScript Audio Bridge
- **Location:** `web/src/audio/cpp-audio-bridge.ts`
- **Purpose:** Receives sound calls from C++ via EM_ASM
- **Implements:** `window.ToonTalkAudio` with Web Audio API

### ✅ Windows Stubs
- **Location:** `web/core/stubs/windows_stubs.cpp`
- **Purpose:** Empty implementations of Windows APIs we don't need

### ✅ Build Configuration
- **Location:** `web/core/CMakeLists_v2.txt`
- **Purpose:** CMake configuration to compile original source with Emscripten

### ✅ Documentation
- `PORTING_GUIDE.md` - Complete porting strategy
- `IMPLEMENTATION_EXAMPLE.md` - Detailed example for number.cpp

## File Structure

```
desktop-toontalk/
├── source/              # Original ToonTalk C++ (30+ years of code)
│   ├── number.cpp      # Number pad behavior (500KB!)
│   ├── text.cpp        # Text pad behavior
│   ├── cubby.cpp       # Box behavior
│   ├── picture.cpp     # Picture & Scale behavior
│   ├── robot.cpp       # Robot behavior
│   └── ... (60+ more files)
│
└── web/
    ├── core/
    │   ├── platform/           # NEW: Platform abstraction
    │   │   ├── platform.h
    │   │   └── platform.cpp
    │   ├── stubs/             # NEW: Windows stubs
    │   │   └── windows_stubs.cpp
    │   ├── CMakeLists_v2.txt  # NEW: Build config
    │   └── objects.cpp        # OLD: Delete this eventually
    │
    └── src/
        ├── renderer/
        │   └── cpp-renderer-bridge.ts  # NEW: Rendering bridge
        └── audio/
            └── cpp-audio-bridge.ts     # NEW: Audio bridge
```

## Implementation Steps

### Phase 1: Set Up Build System (30 min)

```bash
cd web/core

# Configure with Emscripten
emcmake cmake -B build -S . -f CMakeLists_v2.txt

# Try to build (will have errors initially)
emmake make -C build
```

**Expected:** Compilation errors - that's normal! We'll fix them incrementally.

### Phase 2: Get First File Compiling (2-3 hours)

Start with `source/sprite.cpp` (base class):

1. Open `source/sprite.cpp`
2. Add at top:
   ```cpp
   #ifdef __EMSCRIPTEN__
   #include "platform/platform.h"
   #endif
   ```
3. Find `Sprite::display()` method
4. Wrap Windows/DirectX calls with `#ifdef __EMSCRIPTEN__`
5. Replace with `TTPlatform::` calls
6. Compile again

### Phase 3: Get Number Pads Working (4-6 hours)

Follow `IMPLEMENTATION_EXAMPLE.md` to modify `source/number.cpp`:

1. Add platform header
2. Replace GDI rectangle calls (line ~482)
3. Replace text rendering calls
4. Replace plate display calls
5. Test that numbers appear and respond to input

### Phase 4: Get Other Objects Working (2-3 days)

Repeat the process for:
- `source/text.cpp` - Text pads
- `source/cubby.cpp` - Boxes
- `source/picture.cpp` - Pictures and Scales
- `source/robot.cpp` - Robots

### Phase 5: Hook Up JavaScript Side (1 day)

Update your TypeScript app to:

1. Initialize renderer bridge before loading WASM:
   ```typescript
   const renderer = new CppRendererBridge(app);
   const audio = new CppAudioBridge();
   ```

2. Load WASM module:
   ```typescript
   const ToonTalk = await createToonTalkModule();
   ```

3. Start game loop:
   ```typescript
   function gameLoop(deltaTime) {
       ToonTalk.update(deltaTime);  // C++ handles everything
       requestAnimationFrame(gameLoop);
   }
   ```

## Key Principles

### ✅ DO:
- Keep ALL C++ behavior logic unchanged
- Only modify rendering calls
- Use `#ifdef __EMSCRIPTEN__` to maintain Windows build
- Test incrementally - one file at a time
- Refer to original code for authentic behavior

### ❌ DON'T:
- Don't rewrite behavior in TypeScript
- Don't try to compile all files at once
- Don't change object interaction logic
- Don't skip the platform abstraction layer

## Why This Will Work

1. **Original code is solid** - 30+ years of refinement
2. **Rendering is separate** - DirectX calls are isolated
3. **Emscripten is mature** - Handles most C++ → WASM issues
4. **Incremental approach** - Fix errors one file at a time
5. **Authentic behavior** - Original ToonTalk logic preserved

## Estimated Timeline

- **Week 1:** Build system + sprite.cpp + number.cpp
- **Week 2:** text.cpp + cubby.cpp
- **Week 3:** picture.cpp + robot.cpp
- **Week 4:** Integration + testing + bug fixes

**Total:** ~1 month for working prototype with core objects

## Success Criteria

You'll know it's working when:

✅ Number pads display with correct graphics
✅ You can type numbers on keyboard
✅ You can drag numbers around
✅ Dropping number on number combines them
✅ Dropping operation on number changes it
✅ Text pads work similarly
✅ Boxes accept items in holes
✅ Scales compare neighbors and tilt
✅ Robots can be trained

## Getting Started Right Now

1. **Read** `PORTING_GUIDE.md` (15 min)
2. **Read** `IMPLEMENTATION_EXAMPLE.md` (15 min)
3. **Try building:**
   ```bash
   cd web/core
   emcmake cmake -B build -S . -f CMakeLists_v2.txt
   emmake make -C build 2>&1 | tee build.log
   ```
4. **Review errors** in `build.log`
5. **Start fixing** - one error at a time

## Questions?

Common questions answered:

**Q: Do we keep the old `objects.cpp`?**
A: No, eventually delete it. The original source has everything we need.

**Q: What about the TypeScript we already wrote?**
A: Keep the renderer bridge and UI code. Delete behavior re-implementations.

**Q: Can we test incrementally?**
A: Yes! Compile each file, test it, then move to the next.

**Q: What if we get stuck?**
A: Check `PORTING_GUIDE.md` for troubleshooting, or look at how original code works.

## The Big Picture

```
┌─────────────────────────────────────────────┐
│  Original ToonTalk (1992-2020)              │
│  Windows + DirectX + 30 years of behavior   │
└──────────────────┬──────────────────────────┘
                   │
                   │ Port to WASM
                   ↓
┌─────────────────────────────────────────────┐
│  ToonTalk Web (2025)                        │
│  WASM + PixiJS + Same authentic behavior    │
└─────────────────────────────────────────────┘
```

**Let's bring authentic ToonTalk to the web! 🚀**

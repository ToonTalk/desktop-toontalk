# ToonTalk C++ to WebAssembly Porting Guide

## Overview

This guide explains how to port the original ToonTalk C++ source code to WebAssembly while preserving all the original behavior. The strategy is:

1. **Keep all C++ logic** - Object interactions, game rules, state management
2. **Replace platform-specific code** - DirectX → JavaScript callbacks
3. **Compile to WASM** - Using Emscripten
4. **Render in JavaScript** - Using PixiJS

## Architecture

```
┌────────────────────────────────────────┐
│   Original ToonTalk C++ → WASM         │
│   - All object behavior preserved      │
│   - Game loop and timing               │
│   - Object interactions                │
│   - State management                   │
│                                        │
│   Platform calls replaced with:       │
│   TTPlatform::drawSprite(...)          │
│   TTPlatform::playSound(...)           │
└──────────┬─────────────────────────────┘
           │ EM_ASM JavaScript calls
           ↓
┌────────────────────────────────────────┐
│   JavaScript/TypeScript (PixiJS)       │
│   window.ToonTalkRenderer.drawSprite() │
│   window.ToonTalkAudio.playSound()     │
└────────────────────────────────────────┘
```

## Step-by-Step Porting Process

### 1. Identify Platform-Specific Code

Look for these Windows/DirectX patterns in the original source:

```cpp
// DirectDraw rendering
tt_screen->blit(...)
tt_screen->draw_rectangle(...)
screen->display(...)

// Windows GDI
BEGIN_GDI
    HGDIOBJ old_selection = tt_main_window->select_object(...)
    tt_screen->draw_rectangle(...)
    DeleteObject(...)
END_GDI

// DirectSound audio
play_sound(SOUND_ID, ...)
```

### 2. Replace with Platform Layer Calls

Replace the above with our platform abstraction:

```cpp
#include "platform/platform.h"

// OLD CODE (DirectX):
tt_screen->blit(sprite_image, x, y, width, height);

// NEW CODE (Platform layer):
TTPlatform::drawSprite(NUMBER_PAD_SPRITE, x, y, width, height);

// OLD CODE (Windows GDI):
BEGIN_GDI
    tt_main_window->select_stock_object(NULL_PEN);
    HGDIOBJ old_selection = tt_main_window->select_object(
        tt_main_window->create_solid_brush(background_color));
    tt_screen->draw_rectangle(llx, llx+width, lly, lly+height);
    DeleteObject(tt_main_window->select_object(old_selection));
END_GDI

// NEW CODE (Platform layer):
TTPlatform::drawRect(llx, lly, width, height, background_color);

// OLD CODE (DirectSound):
play_sound(PICK_UP_SOUND);

// NEW CODE (Platform layer):
TTPlatform::playSound("pickup");
```

### 3. Example: Porting number.cpp display() method

**BEFORE (lines 482-488 in source/number.cpp):**

```cpp
if (show_all() && !is_blank()) {
    BEGIN_GDI
        tt_main_window->select_stock_object(NULL_PEN);
        HGDIOBJ old_selection = tt_main_window->select_object(
            tt_main_window->create_solid_brush(background_color));
        tt_screen->draw_rectangle(adjusted_llx, adjusted_llx+adjusted_width,
                                  adjusted_lly, adjusted_lly+adjusted_height);
        if (old_selection != NULL)
            DeleteObject(tt_main_window->select_object(old_selection));
    END_GDI
}
```

**AFTER (using platform layer):**

```cpp
#include "platform/platform.h"  // Add at top of file

if (show_all() && !is_blank()) {
    // Draw background rectangle for number pad
    TTPlatform::drawRect(adjusted_llx, adjusted_lly,
                        adjusted_width, adjusted_height,
                        background_color);
}
```

### 4. Handling Sprites and Images

The original code uses DirectDraw surfaces for sprites. Replace with sprite IDs:

**BEFORE:**
```cpp
// Complex DirectDraw surface management
IDirectDrawSurface *surface = get_digit_surface(digit);
tt_screen->blit_surface(surface, x, y, width, height, ...);
```

**AFTER:**
```cpp
// Simple sprite ID - JavaScript will handle texture lookup
TTPlatform::drawSprite(NUMBER_PAD_SPRITE, x, y, width, height);
```

### 5. Handling Text Rendering

**BEFORE:**
```cpp
// Windows GDI text rendering
HDC hdc = GetDC(hwnd);
SetTextColor(hdc, color);
TextOut(hdc, x, y, text, length);
ReleaseDC(hwnd, hdc);
```

**AFTER:**
```cpp
TTPlatform::drawText(text, x, y, fontSize, color);
```

### 6. Files That Need Modification

Here are the key files and what needs changing:

#### Core Objects (Heavy modification needed):
- `source/number.cpp` - Replace all `tt_screen->` and GDI calls
- `source/text.cpp` - Replace text rendering calls
- `source/cubby.cpp` - Replace sprite blitting
- `source/picture.cpp` - Replace image rendering and collision detection graphics
- `source/robot.cpp` - Replace robot animation rendering
- `source/pad.cpp` - Replace notebook rendering

#### Support Files (Moderate modification):
- `source/sprite.cpp` - Base class display methods
- `source/animate.cpp` - Animation system (keep logic, replace rendering)
- `source/screen.cpp` - **Replace entirely with platform layer**
- `source/globals.cpp` - Remove Windows-specific globals

#### Files to Stub Out (Not needed on web):
- `source/winmain.cpp` - Windows entry point
- `source/mouse.cpp` - Windows mouse handling (use browser events)
- `source/sound.cpp` - DirectSound (use Web Audio)
- `source/city.cpp` - 3D city view (optional feature)

### 7. Build Process

Use the new CMakeLists_v2.txt:

```bash
cd web/core
emcmake cmake -B build -S . -DCMAKE_BUILD_TYPE=Release
emmake make -C build
```

This will produce `toontalk.js` and `toontalk.wasm`.

### 8. JavaScript Integration

In your TypeScript app:

```typescript
import { CppRendererBridge } from './renderer/cpp-renderer-bridge';
import { CppAudioBridge } from './audio/cpp-audio-bridge';
import createToonTalkModule from './core/toontalk';

// Initialize bridges
const app = new PIXI.Application({...});
const renderer = new CppRendererBridge(app);
const audio = new CppAudioBridge();

// Load WASM module
const ToonTalk = await createToonTalkModule();

// Start game loop
function gameLoop() {
    ToonTalk.update(deltaTime);  // C++ game loop
    requestAnimationFrame(gameLoop);
}
gameLoop();
```

## Priority Order for Porting

Start with these files in this order:

1. ✅ **Platform layer** - `web/core/platform/*` (already created)
2. ✅ **Stubs** - `web/core/stubs/*` (already created)
3. **Core infrastructure**:
   - Modify `source/sprite.cpp` - Base display() method
   - Modify `source/sprites.cpp` - Sprite collection
   - Modify `source/globals.cpp` - Remove Windows globals
4. **First object** - `source/number.cpp`:
   - Replace all GDI calls with TTPlatform:: calls
   - Keep all behavior logic intact
   - Test that numbers display and work
5. **Second object** - `source/text.cpp`
6. **Third object** - `source/cubby.cpp`
7. **Fourth object** - `source/picture.cpp`
8. **Fifth object** - `source/robot.cpp`
9. **Interaction system**:
   - Modify drag & drop (source/dragdrop.cpp)
   - Modify input handling (source/input.cpp)

## Testing Strategy

For each file you port:

1. **Compile test**: Does it compile with Emscripten?
2. **Link test**: Does it link with the rest of the code?
3. **Display test**: Does the object appear on screen?
4. **Behavior test**: Does it behave like original ToonTalk?

## Benefits of This Approach

✅ **Preserve all behavior** - No re-implementation needed
✅ **Systematic** - Port file by file, test incrementally
✅ **Maintainable** - Original C++ structure preserved
✅ **Performance** - WASM is fast, only rendering in JS
✅ **Authentic** - Exact same logic as original ToonTalk

## Common Pitfalls

❌ **Don't** try to compile Windows-specific files as-is
❌ **Don't** rewrite behavior logic - just replace rendering
❌ **Don't** forget to add #include "platform/platform.h"
❌ **Don't** use synchronous file I/O (use async in JS)
❌ **Don't** rely on Windows message loop (use ASYNCIFY)

## Next Steps

Ready to start? Begin with:

1. Set up the build system (CMakeLists_v2.txt)
2. Modify source/sprite.cpp display() method
3. Modify source/number.cpp display() method
4. Test that a number pad renders in the browser

See `IMPLEMENTATION_EXAMPLE.md` for a detailed walkthrough of modifying `number.cpp`.

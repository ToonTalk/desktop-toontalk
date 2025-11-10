# ToonTalk WASM Porting Strategy

## UX Improvements Over Original

### Drop Detection: Mouse Position vs Sprite Centers

**Original Desktop ToonTalk** (`source/text.cpp:991-993`):
```cpp
boolean Text::on_right_side(Sprite *item) {
    return(item->current_llx()+item->current_width()/2 > llx+width/2);
};
```
- Uses **sprite centers** to determine left/right
- If dropped sprite's CENTER < target's CENTER → left half
- If dropped sprite's CENTER > target's CENTER → right half
- **Problem**: Unintuitive - where you grab the object matters

**ToonTalk Web** (our improvement):
```typescript
const mouseX = event.global.x;
const targetX = target.getX();
this.dropOnLeftHalf = (mouseX < targetX);
```
- Uses **mouse cursor position** to determine left/right
- If CURSOR < target center → left half
- If CURSOR > target center → right half
- **Benefit**: More intuitive - where you point determines the result

**Why This Matters**: In the original, dropping "Hello" with your mouse on World's left side could still result in a RIGHT drop if Hello's center landed right of World's center. Now, it always works as expected based on where your cursor is.

---

## Analysis of Original Code

### Platform Dependencies Found:
- **windows.h** - Core Windows API
- **dsound.h** - DirectSound (audio)
- **ddraw.h** - DirectDraw (2D graphics)
- **dinput.h** - DirectInput (keyboard/mouse/joystick)
- **dplay.h** - DirectPlay (networking)
- **gdiplus.h** - GDI+ (advanced graphics)

### Core Source Files Structure:
```
source/
├── sprite.cpp/h       - Base sprite class (rendering, positioning)
├── number.cpp/h       - Number objects (arithmetic, display)
├── text.cpp/h         - Text objects (concatenation, display)
├── cubby.cpp/h        - Container objects (nests)
├── robot.cpp/h        - Robot training/execution
├── dragdrop.cpp       - Drag-drop interaction logic ⭐
├── bird.cpp/h         - Bird sprites
├── block.cpp/h        - Block objects
├── box.cpp/h          - Box containers
└── ... (60+ more files)

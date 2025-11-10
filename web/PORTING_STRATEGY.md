# ToonTalk WASM Porting Strategy

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

# Concrete Example: Porting number.cpp

This document shows **exactly** how to modify `source/number.cpp` to work with WebAssembly.

## Step 1: Add Platform Header

At the top of `source/number.cpp` (after line 65), add:

```cpp
#if !defined(__TT_NUMBER_H)
#include "number.h"
#endif

// ADD THIS:
#ifdef __EMSCRIPTEN__
#include "platform/platform.h"
#endif
```

## Step 2: Modify the display() Method

Find the `Number::display()` method at line 347 and make these specific changes:

### Change #1: Replace GDI Rectangle Drawing (lines 482-488)

**FIND THIS CODE:**
```cpp
if (show_all() && !is_blank()) { // new on 040102
BEGIN_GDI
    tt_main_window->select_stock_object(NULL_PEN);
    HGDIOBJ old_selection = tt_main_window->select_object(tt_main_window->create_solid_brush(background_color));
    tt_screen->draw_rectangle(adjusted_llx,adjusted_llx+adjusted_width,
                              adjusted_lly,adjusted_lly+adjusted_height);
    if (old_selection != NULL) DeleteObject(tt_main_window->select_object(old_selection));
END_GDI
};
```

**REPLACE WITH:**
```cpp
if (show_all() && !is_blank()) {
#ifdef __EMSCRIPTEN__
    // Web version: Use platform layer
    TTPlatform::drawRect(adjusted_llx, adjusted_lly,
                        adjusted_width, adjusted_height,
                        background_color);
#else
    // Windows version: Original GDI code
    BEGIN_GDI
        tt_main_window->select_stock_object(NULL_PEN);
        HGDIOBJ old_selection = tt_main_window->select_object(
            tt_main_window->create_solid_brush(background_color));
        tt_screen->draw_rectangle(adjusted_llx,adjusted_llx+adjusted_width,
                                  adjusted_lly,adjusted_lly+adjusted_height);
        if (old_selection != NULL)
            DeleteObject(tt_main_window->select_object(old_selection));
    END_GDI
#endif
}
```

### Change #2: Replace Plate Display (around line 467)

**FIND THIS CODE:**
```cpp
if (is_blank()) {
    // new on 050102
    display_plate(adjusted_llx,adjusted_lly,adjusted_width,adjusted_height,
                  plate_state(),TRUE,permutation);
    return;
}
```

**REPLACE WITH:**
```cpp
if (is_blank()) {
#ifdef __EMSCRIPTEN__
    // Web version: Draw a simple dashed rectangle for blank state
    TTPlatform::drawRect(adjusted_llx, adjusted_lly,
                        adjusted_width, adjusted_height,
                        0xFFFFFF); // White background
    // TODO: Add dashed border in JavaScript
#else
    // Windows version: Original plate display
    display_plate(adjusted_llx,adjusted_lly,adjusted_width,adjusted_height,
                  plate_state(),TRUE,permutation);
#endif
    return;
}
```

### Change #3: Replace Text Display Calls

Search for calls to `display_text()` or `text_out()` functions. These will be throughout the method.

**EXAMPLE - FIND:**
```cpp
text_out(screen_x, screen_y, wide_text, text_length,
         character_width, character_height, text_color, permutation);
```

**REPLACE WITH:**
```cpp
#ifdef __EMSCRIPTEN__
    // Convert wide string to narrow string for JavaScript
    char* narrow_text = copy_narrow_string(wide_text);
    if (narrow_text != NULL) {
        TTPlatform::drawText(narrow_text, screen_x, screen_y,
                           character_height, text_color);
        delete[] narrow_text;
    }
#else
    // Windows version
    text_out(screen_x, screen_y, wide_text, text_length,
             character_width, character_height, text_color, permutation);
#endif
```

## Step 3: Modify Sprite Display Calls

If `number.cpp` uses `display_sprite()` or similar:

**FIND:**
```cpp
screen->display_sprite(sprite_id, x, y, width, height);
```

**REPLACE WITH:**
```cpp
#ifdef __EMSCRIPTEN__
    TTPlatform::drawSprite(sprite_id, x, y, width, height);
#else
    screen->display_sprite(sprite_id, x, y, width, height);
#endif
```

## Step 4: Handle Sound Calls

Search for sound playback:

**FIND:**
```cpp
play_sound(PICK_UP_NUMBER_SOUND);
```

**REPLACE WITH:**
```cpp
#ifdef __EMSCRIPTEN__
    TTPlatform::playSound("pickup_number");
#else
    play_sound(PICK_UP_NUMBER_SOUND);
#endif
```

## Step 5: Remove or Stub Windows-Specific Utility Functions

Some functions in `number.cpp` may use Windows APIs. Wrap them:

**EXAMPLE:**
```cpp
void Number::some_windows_function() {
#ifdef __EMSCRIPTEN__
    // Web version: Do nothing or provide alternative
    return;
#else
    // Original Windows code
    HWND hwnd = GetActiveWindow();
    // ... Windows-specific stuff
#endif
}
```

## Complete Modified Section Example

Here's a complete before/after for a section of the display method:

### BEFORE (Original Windows version):
```cpp
void Number::display(SelectionFeedback selection_feedback,
                     boolean followers_too, TTRegion *region) {
    // ... initialization code ...

    if (show_all() && !is_blank()) {
        BEGIN_GDI
            tt_main_window->select_stock_object(NULL_PEN);
            HGDIOBJ old_selection = tt_main_window->select_object(
                tt_main_window->create_solid_brush(background_color));
            tt_screen->draw_rectangle(adjusted_llx,adjusted_llx+adjusted_width,
                                      adjusted_lly,adjusted_lly+adjusted_height);
            if (old_selection != NULL)
                DeleteObject(tt_main_window->select_object(old_selection));
        END_GDI
    }

    // Display the number text
    text_out(start_x, start_y, wide_text, text_length,
             adjusted_character_width, adjusted_character_height,
             text_color, permutation);

    // ... more display code ...

    Sprite::display(selection_feedback, followers_too, region);
}
```

### AFTER (Cross-platform version):
```cpp
#ifdef __EMSCRIPTEN__
#include "platform/platform.h"
#endif

void Number::display(SelectionFeedback selection_feedback,
                     boolean followers_too, TTRegion *region) {
    // ... initialization code (unchanged) ...

    if (show_all() && !is_blank()) {
#ifdef __EMSCRIPTEN__
        // Web: Simple platform call
        TTPlatform::drawRect(adjusted_llx, adjusted_lly,
                            adjusted_width, adjusted_height,
                            background_color);
#else
        // Windows: Original GDI code
        BEGIN_GDI
            tt_main_window->select_stock_object(NULL_PEN);
            HGDIOBJ old_selection = tt_main_window->select_object(
                tt_main_window->create_solid_brush(background_color));
            tt_screen->draw_rectangle(adjusted_llx,adjusted_llx+adjusted_width,
                                      adjusted_lly,adjusted_lly+adjusted_height);
            if (old_selection != NULL)
                DeleteObject(tt_main_window->select_object(old_selection));
        END_GDI
#endif
    }

    // Display the number text
#ifdef __EMSCRIPTEN__
    char* narrow_text = copy_narrow_string(wide_text);
    if (narrow_text != NULL) {
        TTPlatform::drawText(narrow_text, start_x, start_y,
                           adjusted_character_height, text_color);
        delete[] narrow_text;
    }
#else
    text_out(start_x, start_y, wide_text, text_length,
             adjusted_character_width, adjusted_character_height,
             text_color, permutation);
#endif

    // ... more display code ...

    Sprite::display(selection_feedback, followers_too, region);
}
```

## Testing Your Changes

After modifying `number.cpp`:

1. **Compile:**
   ```bash
   cd web/core
   emcmake cmake -B build -S .
   emmake make -C build
   ```

2. **Check for errors** - Fix any compilation issues

3. **Load in browser** - Test that numbers appear

4. **Test behavior**:
   - Can you click on numbers?
   - Do they respond to keyboard input?
   - Can you drag them?

## Common Issues and Fixes

### Issue: "undefined reference to TTPlatform::drawRect"
**Fix:** Make sure `platform.cpp` is in your CMakeLists.txt sources

### Issue: Text not appearing
**Fix:** Check that `window.ToonTalkRenderer` is initialized before WASM loads

### Issue: Colors look wrong
**Fix:** Make sure color format matches (RGB vs BGR, 0xRRGGBB format)

### Issue: Positioning is off
**Fix:** Check coordinate system - Windows GDI uses top-left origin, web might differ

## Next File to Port

After successfully porting `number.cpp`, move on to:
- `source/text.cpp` - Similar structure to numbers
- `source/cubby.cpp` - Box objects with holes
- `source/picture.cpp` - Animated pictures and scales

## Summary

The pattern for all files is the same:

1. Add `#include "platform/platform.h"` under `#ifdef __EMSCRIPTEN__`
2. Find all Windows/DirectX rendering calls
3. Wrap in `#ifdef __EMSCRIPTEN__` / `#else` / `#endif`
4. Replace with `TTPlatform::` calls in the EMSCRIPTEN branch
5. Keep original code in the else branch
6. Test incrementally

This preserves all behavior while making rendering cross-platform!

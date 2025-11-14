// Platform Abstraction Layer Implementation
// Uses Emscripten EM_ASM to call JavaScript functions

#include "platform.h"
#include <emscripten.h>
#include <cstring>

namespace TTPlatform {

// ============================================================================
// Graphics Implementation
// ============================================================================

void drawSprite(int spriteId, int x, int y, int width, int height,
                int rotation, float alpha) {
    EM_ASM({
        // Call JavaScript rendering function
        if (window.ToonTalkRenderer) {
            window.ToonTalkRenderer.drawSprite($0, $1, $2, $3, $4, $5, $6);
        }
    }, spriteId, x, y, width, height, rotation, alpha);
}

void drawRect(int x, int y, int width, int height, unsigned int color) {
    EM_ASM({
        if (window.ToonTalkRenderer) {
            window.ToonTalkRenderer.drawRect($0, $1, $2, $3, $4);
        }
    }, x, y, width, height, color);
}

void drawCircle(int x, int y, int radius, unsigned int color) {
    EM_ASM({
        if (window.ToonTalkRenderer) {
            window.ToonTalkRenderer.drawCircle($0, $1, $2, $3);
        }
    }, x, y, radius, color);
}

void drawLine(int x1, int y1, int x2, int y2, unsigned int color) {
    EM_ASM({
        if (window.ToonTalkRenderer) {
            window.ToonTalkRenderer.drawLine($0, $1, $2, $3, $4);
        }
    }, x1, y1, x2, y2, color);
}

void drawText(const char* text, int x, int y, int fontSize,
              unsigned int color, const char* fontFamily) {
    EM_ASM({
        if (window.ToonTalkRenderer) {
            const text = UTF8ToString($0);
            const fontFamily = UTF8ToString($5);
            window.ToonTalkRenderer.drawText(text, $1, $2, $3, $4, fontFamily);
        }
    }, text, x, y, fontSize, color, fontFamily);
}

void clearScreen() {
    EM_ASM({
        if (window.ToonTalkRenderer) {
            window.ToonTalkRenderer.clear();
        }
    });
}

void present() {
    EM_ASM({
        if (window.ToonTalkRenderer) {
            window.ToonTalkRenderer.present();
        }
    });
}

void setClipRect(int x, int y, int width, int height) {
    EM_ASM({
        if (window.ToonTalkRenderer) {
            window.ToonTalkRenderer.setClipRect($0, $1, $2, $3);
        }
    }, x, y, width, height);
}

void clearClip() {
    EM_ASM({
        if (window.ToonTalkRenderer) {
            window.ToonTalkRenderer.clearClip();
        }
    });
}

// ============================================================================
// Audio Implementation
// ============================================================================

void playSound(const char* soundName, bool loop) {
    EM_ASM({
        if (window.ToonTalkAudio) {
            const soundName = UTF8ToString($0);
            window.ToonTalkAudio.playSound(soundName, $1);
        }
    }, soundName, loop);
}

void stopSound(const char* soundName) {
    EM_ASM({
        if (window.ToonTalkAudio) {
            const soundName = UTF8ToString($0);
            window.ToonTalkAudio.stopSound(soundName);
        }
    }, soundName);
}

void setSoundVolume(const char* soundName, float volume) {
    EM_ASM({
        if (window.ToonTalkAudio) {
            const soundName = UTF8ToString($0);
            window.ToonTalkAudio.setVolume(soundName, $1);
        }
    }, soundName, volume);
}

// ============================================================================
// Timing Implementation
// ============================================================================

double getTimeMs() {
    return emscripten_get_now();
}

void sleep(int ms) {
    emscripten_sleep(ms);  // Requires ASYNCIFY
}

// ============================================================================
// File I/O Implementation
// ============================================================================

bool fileExists(const char* path) {
    return EM_ASM_INT({
        const path = UTF8ToString($0);
        // Check in Emscripten file system or make async fetch
        try {
            return FS.stat(path) ? 1 : 0;
        } catch (e) {
            return 0;
        }
    }, path);
}

char* readFile(const char* path, int* length) {
    // For now, return nullptr - we'll implement asset loading differently
    *length = 0;
    return nullptr;
}

bool writeFile(const char* path, const char* data, int length) {
    // For now, don't implement - would save to IndexedDB
    return false;
}

// ============================================================================
// Utility Implementation
// ============================================================================

void logMessage(const char* message) {
    EM_ASM({
        console.log('[ToonTalk]', UTF8ToString($0));
    }, message);
}

void logError(const char* error) {
    EM_ASM({
        console.error('[ToonTalk Error]', UTF8ToString($0));
    }, error);
}

// ============================================================================
// Input Handlers (called from JavaScript)
// ============================================================================

// These will be exposed via EMSCRIPTEN_BINDINGS

} // namespace TTPlatform

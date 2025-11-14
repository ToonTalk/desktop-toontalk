// Platform Abstraction Layer for ToonTalk Web Port
// Replaces Windows/DirectX calls with JavaScript callbacks

#ifndef TOONTALK_PLATFORM_H
#define TOONTALK_PLATFORM_H

#include <emscripten.h>
#include <string>

// ============================================================================
// Graphics Platform Layer (replaces DirectDraw)
// ============================================================================

namespace TTPlatform {

// Sprite/texture rendering
void drawSprite(int spriteId, int x, int y, int width, int height,
                int rotation = 0, float alpha = 1.0f);

// Primitive shapes
void drawRect(int x, int y, int width, int height, unsigned int color);
void drawCircle(int x, int y, int radius, unsigned int color);
void drawLine(int x1, int y1, int x2, int y2, unsigned int color);

// Text rendering
void drawText(const char* text, int x, int y, int fontSize,
              unsigned int color, const char* fontFamily = "Arial");

// Screen management
void clearScreen();
void present();  // Swap buffers / render to screen

// Clipping region
void setClipRect(int x, int y, int width, int height);
void clearClip();

// ============================================================================
// Input Platform Layer (replaces Windows input)
// ============================================================================

// These will be called FROM JavaScript TO C++
void handleMouseMove(int x, int y);
void handleMouseDown(int x, int y, int button);
void handleMouseUp(int x, int y, int button);
void handleKeyDown(int keyCode, bool shift, bool ctrl);
void handleKeyUp(int keyCode);

// ============================================================================
// Audio Platform Layer (replaces DirectSound)
// ============================================================================

void playSound(const char* soundName, bool loop = false);
void stopSound(const char* soundName);
void setSoundVolume(const char* soundName, float volume);

// ============================================================================
// Timing Platform Layer (replaces Windows timers)
// ============================================================================

double getTimeMs();
void sleep(int ms);  // Will use Emscripten asyncify

// ============================================================================
// File I/O Platform Layer (replaces Windows file API)
// ============================================================================

bool fileExists(const char* path);
char* readFile(const char* path, int* length);
bool writeFile(const char* path, const char* data, int length);

// ============================================================================
// Utility Functions
// ============================================================================

void logMessage(const char* message);
void logError(const char* error);

} // namespace TTPlatform

#endif // TOONTALK_PLATFORM_H

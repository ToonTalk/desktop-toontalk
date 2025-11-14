// Simple test to verify platform layer compiles
#include <emscripten/bind.h>
#include "platform/platform.h"

// Test function that exercises the platform layer
void testPlatformLayer() {
    TTPlatform::logMessage("Platform layer initialized!");
    TTPlatform::drawRect(100, 100, 200, 150, 0x00FF00);
    TTPlatform::drawText("Hello ToonTalk!", 150, 125, 24, 0xFFFFFF);
}

// Expose to JavaScript
EMSCRIPTEN_BINDINGS(test_platform) {
    emscripten::function("testPlatformLayer", &testPlatformLayer);
}

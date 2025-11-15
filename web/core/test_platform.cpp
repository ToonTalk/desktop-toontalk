// Simple test to verify platform layer compiles
#include <emscripten/bind.h>
#include "platform/platform.h"
#include "minimal/number.h"

// Test function that exercises the platform layer
void testPlatformLayer() {
    TTPlatform::logMessage("Platform layer initialized!");
    TTPlatform::drawRect(100, 100, 200, 150, 0x00FF00);
    TTPlatform::drawText("Hello ToonTalk!", 150, 125, 24, 0xFFFFFF);
}

// Expose to JavaScript
EMSCRIPTEN_BINDINGS(toontalk_core) {
    emscripten::function("testPlatformLayer", &testPlatformLayer);

    // Expose Number class
    emscripten::class_<Number>("Number")
        .constructor<double, int, int, int, int>()
        .function("display", &Number::display)
        .function("setValue", &Number::setValue)
        .function("getValue", &Number::getValue)
        .function("setPosition", &Number::setPosition)
        .function("setSize", &Number::setSize);
}

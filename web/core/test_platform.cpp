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

    // Expose NumberOperation enum
    emscripten::enum_<NumberOperation>("NumberOperation")
        .value("SUBTRACT_FROM", SUBTRACT_FROM)
        .value("MULTIPLY_BY", MULTIPLY_BY)
        .value("INCREASE_BY", INCREASE_BY)
        .value("DIVIDE_BY", DIVIDE_BY)
        .value("MODULUS_BY", MODULUS_BY)
        .value("MAKE_EQUAL", MAKE_EQUAL)
        .value("NO_NUMBER_OPERATION", NO_NUMBER_OPERATION);

    // Expose Number class
    emscripten::class_<Number>("Number")
        .constructor<double, int, int, int, int, NumberOperation>()
        .function("display", &Number::display)
        .function("setValue", &Number::setValue)
        .function("getValue", &Number::getValue)
        .function("setOperation", &Number::setOperation)
        .function("getOperation", &Number::getOperation)
        .function("setPosition", &Number::setPosition)
        .function("setSize", &Number::setSize)
        .function("applyOperation", &Number::applyOperation)
        .function("applyTo", &Number::applyTo);
}

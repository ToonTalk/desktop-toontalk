/**
 * core.cpp - Core ToonTalk functions
 *
 * Utility functions and core game logic that will be called from JavaScript
 */

#include <emscripten/bind.h>
#include <string>

namespace toontalk {

/**
 * Get version information
 */
std::string getVersion() {
    return "ToonTalk Web 0.1.0 (WASM Core)";
}

/**
 * Initialize the core system
 */
bool initialize() {
    // TODO: Initialize game systems
    return true;
}

/**
 * Test function for WASM communication
 */
int add(int a, int b) {
    return a + b;
}

/**
 * Test function returning a string
 */
std::string greet(const std::string& name) {
    return "Hello from WASM, " + name + "!";
}

} // namespace toontalk

// Emscripten bindings
using namespace emscripten;
using namespace toontalk;

EMSCRIPTEN_BINDINGS(toontalk_core_functions) {
    function("getVersion", &getVersion);
    function("initialize", &initialize);
    function("add", &add);
    function("greet", &greet);
}

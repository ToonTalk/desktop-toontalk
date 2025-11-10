/**
 * objects.cpp - Number, Text, and Box classes for ToonTalk Web
 *
 * Simplified port from ../source/number.cpp, ../source/text.cpp, etc.
 * These classes inherit from Sprite (defined in sprite.cpp)
 */

#include "sprite.h"
#include <emscripten/bind.h>
#include <string>
#include <sstream>
#include <iomanip>

namespace toontalk {

/**
 * Number class - Represents a numerical value
 */
class Number : public Sprite {
public:
    Number(float x, float y, double value = 0.0)
        : Sprite(x, y, 80.0f, 60.0f), value_(value) {}

    // Get/set the numerical value
    double getValue() const { return value_; }
    void setValue(double v) { value_ = v; }

    // Arithmetic operations
    void add(double amount) { value_ += amount; }
    void subtract(double amount) { value_ -= amount; }
    void multiply(double amount) { value_ *= amount; }
    void divide(double amount) {
        if (amount != 0.0) {
            value_ /= amount;
        }
    }

    // Get string representation
    std::string toString() const {
        std::ostringstream oss;
        // Format with 2 decimal places if not a whole number
        if (value_ == static_cast<long>(value_)) {
            oss << static_cast<long>(value_);
        } else {
            oss << std::fixed << std::setprecision(2) << value_;
        }
        return oss.str();
    }

    // Override update
    void update(float deltaTime) override {
        Sprite::update(deltaTime);
        // Numbers don't move on their own
    }

private:
    double value_;
};

/**
 * Text class - Represents a text string
 */
class Text : public Sprite {
public:
    Text(float x, float y, const std::string& text = "")
        : Sprite(x, y, 100.0f, 40.0f), text_(text) {}

    // Get/set text
    std::string getText() const { return text_; }
    void setText(const std::string& text) { text_ = text; }

    // Text operations
    void append(const std::string& str) { text_ += str; }
    void clear() { text_ = ""; }
    size_t length() const { return text_.length(); }

    // Override update
    void update(float deltaTime) override {
        Sprite::update(deltaTime);
        // Text doesn't move on its own
    }

private:
    std::string text_;
};

/**
 * Box class - Container that can hold other objects
 */
class Box : public Sprite {
public:
    Box(float x, float y, size_t capacity = 10)
        : Sprite(x, y, 120.0f, 100.0f), capacity_(capacity), count_(0) {}

    // Container operations
    size_t getCapacity() const { return capacity_; }
    size_t getCount() const { return count_; }
    bool isEmpty() const { return count_ == 0; }
    bool isFull() const { return count_ >= capacity_; }

    // Simulate adding/removing items (simplified - no actual sprite storage in WASM)
    bool addItem() {
        if (!isFull()) {
            count_++;
            return true;
        }
        return false;
    }

    bool removeItem() {
        if (!isEmpty()) {
            count_--;
            return true;
        }
        return false;
    }

    void clear() { count_ = 0; }

    // Get visual representation info
    float getFullness() const {
        return capacity_ > 0 ? static_cast<float>(count_) / capacity_ : 0.0f;
    }

    // Override update
    void update(float deltaTime) override {
        Sprite::update(deltaTime);
        // Boxes don't move on their own
    }

private:
    size_t capacity_;
    size_t count_;
};

} // namespace toontalk

// Emscripten bindings - only bind the NEW classes (Sprite is already bound in sprite.cpp)
using namespace emscripten;
using namespace toontalk;

EMSCRIPTEN_BINDINGS(toontalk_objects) {
    class_<Number, base<Sprite>>("Number")
        .constructor<float, float, double>()
        .function("getValue", &Number::getValue)
        .function("setValue", &Number::setValue)
        .function("add", &Number::add)
        .function("subtract", &Number::subtract)
        .function("multiply", &Number::multiply)
        .function("divide", &Number::divide)
        .function("toString", &Number::toString);

    class_<Text, base<Sprite>>("Text")
        .constructor<float, float, std::string>()
        .function("getText", &Text::getText)
        .function("setText", &Text::setText)
        .function("append", &Text::append)
        .function("clear", &Text::clear)
        .function("length", &Text::length);

    class_<Box, base<Sprite>>("Box")
        .constructor<float, float, size_t>()
        .function("getCapacity", &Box::getCapacity)
        .function("getCount", &Box::getCount)
        .function("isEmpty", &Box::isEmpty)
        .function("isFull", &Box::isFull)
        .function("addItem", &Box::addItem)
        .function("removeItem", &Box::removeItem)
        .function("clear", &Box::clear)
        .function("getFullness", &Box::getFullness);
}

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
#include <vector>

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

/**
 * Nest class - Container with holes (slots) for holding items
 * Based on ../source/cubby.cpp - simplified version
 */
class Nest : public Sprite {
public:
    Nest(float x, float y, size_t num_holes = 3)
        : Sprite(x, y, 120.0f, 80.0f), num_holes_(num_holes) {
        // Initialize all holes as empty (nullptr)
        holes_.resize(num_holes, nullptr);
    }

    // Get number of holes
    size_t getNumHoles() const { return num_holes_; }

    // Check if a hole is empty
    bool isHoleEmpty(size_t index) const {
        return index < holes_.size() && holes_[index] == nullptr;
    }

    // Get hole contents (returns pointer - be careful!)
    void* getHoleContents(size_t index) const {
        if (index < holes_.size()) {
            return holes_[index];
        }
        return nullptr;
    }

    // Set hole contents (simplified - just tracking occupied state)
    void setHole(size_t index, bool occupied) {
        if (index < holes_.size()) {
            holes_[index] = occupied ? reinterpret_cast<void*>(1) : nullptr;
        }
    }

    // Clear a specific hole
    void clearHole(size_t index) {
        if (index < holes_.size()) {
            holes_[index] = nullptr;
        }
    }

    // Clear all holes
    void clearAll() {
        for (size_t i = 0; i < holes_.size(); ++i) {
            holes_[i] = nullptr;
        }
    }

    // Count occupied holes
    size_t countOccupied() const {
        size_t count = 0;
        for (const auto& hole : holes_) {
            if (hole != nullptr) count++;
        }
        return count;
    }

    // Check if nest is full
    bool isFull() const {
        return countOccupied() >= num_holes_;
    }

    // Check if nest is empty
    bool isEmpty() const {
        return countOccupied() == 0;
    }

    // Override update
    void update(float deltaTime) override {
        Sprite::update(deltaTime);
        // Nests don't move on their own
    }

private:
    size_t num_holes_;
    std::vector<void*> holes_; // Simplified - just tracking occupied state
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

    class_<Nest, base<Sprite>>("Nest")
        .constructor<float, float, size_t>()
        .function("getNumHoles", &Nest::getNumHoles)
        .function("isHoleEmpty", &Nest::isHoleEmpty)
        .function("setHole", &Nest::setHole)
        .function("clearHole", &Nest::clearHole)
        .function("clearAll", &Nest::clearAll)
        .function("countOccupied", &Nest::countOccupied)
        .function("isFull", &Nest::isFull)
        .function("isEmpty", &Nest::isEmpty);
}

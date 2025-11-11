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

/**
 * Scale class - Copy tool for duplicating objects
 * Based on ../source/scale.cpp - simplified version
 */
class Scale : public Sprite {
public:
    Scale(float x, float y)
        : Sprite(x, y, 60.0f, 60.0f), active_(false) {}

    // Get/set active state (being used to copy)
    bool isActive() const { return active_; }
    void setActive(bool active) { active_ = active; }

    // Override update
    void update(float deltaTime) override {
        Sprite::update(deltaTime);
        // Scales don't move on their own
    }

private:
    bool active_;
};

/**
 * Wand class - Magic wand for creating new objects
 * Based on ../source/wand.cpp - simplified version
 */
class Wand : public Sprite {
public:
    enum WandMode {
        CREATE_NUMBER = 0,
        CREATE_TEXT = 1,
        CREATE_BOX = 2,
        CREATE_NEST = 3,
        CREATE_BIRD = 4
    };

    Wand(float x, float y, WandMode mode = CREATE_NUMBER)
        : Sprite(x, y, 50.0f, 80.0f), mode_(mode), active_(false) {}

    // Get/set wand mode
    WandMode getMode() const { return mode_; }
    void setMode(WandMode mode) { mode_ = mode; }

    // Get mode as integer (for JS binding)
    int getModeInt() const { return static_cast<int>(mode_); }
    void setModeInt(int mode) {
        if (mode >= CREATE_NUMBER && mode <= CREATE_BIRD) {
            mode_ = static_cast<WandMode>(mode);
        }
    }

    // Get/set active state (being waved)
    bool isActive() const { return active_; }
    void setActive(bool active) { active_ = active; }

    // Cycle through modes
    void nextMode() {
        int next = static_cast<int>(mode_) + 1;
        if (next > CREATE_BIRD) {
            mode_ = CREATE_NUMBER;
        } else {
            mode_ = static_cast<WandMode>(next);
        }
    }

    void previousMode() {
        int prev = static_cast<int>(mode_) - 1;
        if (prev < CREATE_NUMBER) {
            mode_ = CREATE_BIRD;
        } else {
            mode_ = static_cast<WandMode>(prev);
        }
    }

    // Override update
    void update(float deltaTime) override {
        Sprite::update(deltaTime);
        // Wands don't move on their own
    }

private:
    WandMode mode_;
    bool active_;
};

/**
 * Robot class - Programmable agent that can perform actions
 * Based on ../source/robot.cpp - simplified version
 */
class Robot : public Sprite {
public:
    enum RobotState {
        IDLE = 0,
        RUNNING = 1,
        PAUSED = 2,
        TRAINING = 3
    };

    Robot(float x, float y)
        : Sprite(x, y, 70.0f, 90.0f), state_(IDLE), instruction_count_(0) {}

    // Get/set robot state
    RobotState getState() const { return state_; }
    void setState(RobotState state) { state_ = state; }

    // Get state as integer (for JS binding)
    int getStateInt() const { return static_cast<int>(state_); }
    void setStateInt(int state) {
        if (state >= IDLE && state <= TRAINING) {
            state_ = static_cast<RobotState>(state);
        }
    }

    // Instruction management (simplified - just count for now)
    size_t getInstructionCount() const { return instruction_count_; }
    void setInstructionCount(size_t count) { instruction_count_ = count; }
    void addInstruction() { instruction_count_++; }
    void clearInstructions() { instruction_count_ = 0; }

    // Start/stop/pause
    void start() { state_ = RUNNING; }
    void stop() { state_ = IDLE; }
    void pause() { state_ = PAUSED; }
    void train() { state_ = TRAINING; }

    // Override update
    void update(float deltaTime) override {
        Sprite::update(deltaTime);
        // Robots execute their program when running
        // (actual execution logic would go here)
    }

private:
    RobotState state_;
    size_t instruction_count_;
};

/**
 * House class - Workspace container where robots operate
 * Based on ../source/house.cpp - simplified version
 */
class House : public Sprite {
public:
    House(float x, float y, size_t num_rooms = 1)
        : Sprite(x, y, 150.0f, 120.0f), num_rooms_(num_rooms), current_room_(0) {
        // Initialize rooms
        room_occupied_.resize(num_rooms, false);
    }

    // Room management
    size_t getNumRooms() const { return num_rooms_; }
    size_t getCurrentRoom() const { return current_room_; }
    void setCurrentRoom(size_t room) {
        if (room < num_rooms_) {
            current_room_ = room;
        }
    }

    // Room occupancy
    bool isRoomOccupied(size_t room) const {
        return room < room_occupied_.size() && room_occupied_[room];
    }

    void setRoomOccupied(size_t room, bool occupied) {
        if (room < room_occupied_.size()) {
            room_occupied_[room] = occupied;
        }
    }

    // Count occupied rooms
    size_t countOccupiedRooms() const {
        size_t count = 0;
        for (bool occupied : room_occupied_) {
            if (occupied) count++;
        }
        return count;
    }

    bool isFull() const { return countOccupiedRooms() >= num_rooms_; }
    bool isEmpty() const { return countOccupiedRooms() == 0; }

    // Override update
    void update(float deltaTime) override {
        Sprite::update(deltaTime);
        // Houses are static workspaces
    }

private:
    size_t num_rooms_;
    size_t current_room_;
    std::vector<bool> room_occupied_;
};

/**
 * Truck class - Delivery/transport vehicle
 * Based on ../source/truck.cpp - simplified version
 */
class Truck : public Sprite {
public:
    enum TruckState {
        EMPTY = 0,
        LOADED = 1,
        DELIVERING = 2
    };

    Truck(float x, float y)
        : Sprite(x, y, 100.0f, 60.0f), state_(EMPTY), has_cargo_(false) {}

    // Get/set truck state
    TruckState getState() const { return state_; }
    void setState(TruckState state) { state_ = state; }

    // Get state as integer (for JS binding)
    int getStateInt() const { return static_cast<int>(state_); }
    void setStateInt(int state) {
        if (state >= EMPTY && state <= DELIVERING) {
            state_ = static_cast<TruckState>(state);
        }
    }

    // Cargo management (simplified - just a flag for now)
    bool hasCargo() const { return has_cargo_; }
    void setCargo(bool has_cargo) {
        has_cargo_ = has_cargo;
        state_ = has_cargo ? LOADED : EMPTY;
    }

    void startDelivery() {
        if (has_cargo_) {
            state_ = DELIVERING;
        }
    }

    void completeDelivery() {
        has_cargo_ = false;
        state_ = EMPTY;
    }

    // Override update
    void update(float deltaTime) override {
        Sprite::update(deltaTime);
        // Trucks can move during delivery
    }

private:
    TruckState state_;
    bool has_cargo_;
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

    class_<Scale, base<Sprite>>("Scale")
        .constructor<float, float>()
        .function("isActive", &Scale::isActive)
        .function("setActive", &Scale::setActive);

    class_<Wand, base<Sprite>>("Wand")
        .constructor<float, float, Wand::WandMode>()
        .function("getModeInt", &Wand::getModeInt)
        .function("setModeInt", &Wand::setModeInt)
        .function("isActive", &Wand::isActive)
        .function("setActive", &Wand::setActive)
        .function("nextMode", &Wand::nextMode)
        .function("previousMode", &Wand::previousMode);

    // Wand mode enum values
    enum_<Wand::WandMode>("WandMode")
        .value("CREATE_NUMBER", Wand::CREATE_NUMBER)
        .value("CREATE_TEXT", Wand::CREATE_TEXT)
        .value("CREATE_BOX", Wand::CREATE_BOX)
        .value("CREATE_NEST", Wand::CREATE_NEST)
        .value("CREATE_BIRD", Wand::CREATE_BIRD);

    class_<Robot, base<Sprite>>("Robot")
        .constructor<float, float>()
        .function("getStateInt", &Robot::getStateInt)
        .function("setStateInt", &Robot::setStateInt)
        .function("getInstructionCount", &Robot::getInstructionCount)
        .function("setInstructionCount", &Robot::setInstructionCount)
        .function("addInstruction", &Robot::addInstruction)
        .function("clearInstructions", &Robot::clearInstructions)
        .function("start", &Robot::start)
        .function("stop", &Robot::stop)
        .function("pause", &Robot::pause)
        .function("train", &Robot::train);

    // Robot state enum values
    enum_<Robot::RobotState>("RobotState")
        .value("IDLE", Robot::IDLE)
        .value("RUNNING", Robot::RUNNING)
        .value("PAUSED", Robot::PAUSED)
        .value("TRAINING", Robot::TRAINING);

    class_<House, base<Sprite>>("House")
        .constructor<float, float, size_t>()
        .function("getNumRooms", &House::getNumRooms)
        .function("getCurrentRoom", &House::getCurrentRoom)
        .function("setCurrentRoom", &House::setCurrentRoom)
        .function("isRoomOccupied", &House::isRoomOccupied)
        .function("setRoomOccupied", &House::setRoomOccupied)
        .function("countOccupiedRooms", &House::countOccupiedRooms)
        .function("isFull", &House::isFull)
        .function("isEmpty", &House::isEmpty);

    class_<Truck, base<Sprite>>("Truck")
        .constructor<float, float>()
        .function("getStateInt", &Truck::getStateInt)
        .function("setStateInt", &Truck::setStateInt)
        .function("hasCargo", &Truck::hasCargo)
        .function("setCargo", &Truck::setCargo)
        .function("startDelivery", &Truck::startDelivery)
        .function("completeDelivery", &Truck::completeDelivery);

    // Truck state enum values
    enum_<Truck::TruckState>("TruckState")
        .value("EMPTY", Truck::EMPTY)
        .value("LOADED", Truck::LOADED)
        .value("DELIVERING", Truck::DELIVERING);
}

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

/**
 * Picture class - Display images and graphics
 * Based on ../source/picture.cpp - simplified version
 */
class Picture : public Sprite {
public:
    Picture(float x, float y, size_t width = 120, size_t height = 100)
        : Sprite(x, y, static_cast<float>(width), static_cast<float>(height)),
          pic_width_(width), pic_height_(height), has_image_(false) {}

    // Image dimensions
    size_t getPictureWidth() const { return pic_width_; }
    size_t getPictureHeight() const { return pic_height_; }

    void setPictureSize(size_t width, size_t height) {
        pic_width_ = width;
        pic_height_ = height;
        setWidth(static_cast<float>(width));
        setHeight(static_cast<float>(height));
    }

    // Image state
    bool hasImage() const { return has_image_; }
    void setHasImage(bool has_image) { has_image_ = has_image; }

    // Get image ID (simplified - just an integer reference)
    int getImageId() const { return image_id_; }
    void setImageId(int id) {
        image_id_ = id;
        has_image_ = (id >= 0);
    }

    // Override update
    void update(float deltaTime) override {
        Sprite::update(deltaTime);
        // Pictures are static displays
    }

private:
    size_t pic_width_;
    size_t pic_height_;
    bool has_image_;
    int image_id_ = -1;
};

/**
 * Sensor class - Input and sensor functionality
 * Based on ../source/sensor.cpp - simplified version
 */
class Sensor : public Sprite {
public:
    enum SensorType {
        MOUSE = 0,
        KEYBOARD = 1,
        TIME = 2,
        COLLISION = 3
    };

    Sensor(float x, float y, SensorType type = MOUSE)
        : Sprite(x, y, 60.0f, 60.0f), type_(type), active_(false), value_(0.0) {}

    // Get/set sensor type
    SensorType getType() const { return type_; }
    void setType(SensorType type) { type_ = type; }

    // Get type as integer (for JS binding)
    int getTypeInt() const { return static_cast<int>(type_); }
    void setTypeInt(int type) {
        if (type >= MOUSE && type <= COLLISION) {
            type_ = static_cast<SensorType>(type);
        }
    }

    // Active state
    bool isActive() const { return active_; }
    void setActive(bool active) { active_ = active; }

    // Sensor value (generic numeric value)
    double getValue() const { return value_; }
    void setValue(double value) { value_ = value; }

    // Override update
    void update(float deltaTime) override {
        Sprite::update(deltaTime);
        // Sensors read their input source
    }

private:
    SensorType type_;
    bool active_;
    double value_;
};

/**
 * Notebook class - For notes and documentation
 * Based on ../source/notebook.cpp - simplified version
 */
class Notebook : public Sprite {
public:
    Notebook(float x, float y, size_t num_pages = 5)
        : Sprite(x, y, 120.0f, 100.0f), num_pages_(num_pages), current_page_(0) {
        // Initialize page content (simplified - just track if pages have content)
        page_has_content_.resize(num_pages, false);
    }

    // Page management
    size_t getNumPages() const { return num_pages_; }
    size_t getCurrentPage() const { return current_page_; }

    void setCurrentPage(size_t page) {
        if (page < num_pages_) {
            current_page_ = page;
        }
    }

    void nextPage() {
        if (current_page_ < num_pages_ - 1) {
            current_page_++;
        }
    }

    void previousPage() {
        if (current_page_ > 0) {
            current_page_--;
        }
    }

    // Page content (simplified - just a flag)
    bool pageHasContent(size_t page) const {
        return page < page_has_content_.size() && page_has_content_[page];
    }

    void setPageContent(size_t page, bool has_content) {
        if (page < page_has_content_.size()) {
            page_has_content_[page] = has_content;
        }
    }

    size_t countPagesWithContent() const {
        size_t count = 0;
        for (bool has_content : page_has_content_) {
            if (has_content) count++;
        }
        return count;
    }

    // Override update
    void update(float deltaTime) override {
        Sprite::update(deltaTime);
        // Notebooks are static
    }

private:
    size_t num_pages_;
    size_t current_page_;
    std::vector<bool> page_has_content_;
};

/**
 * Bomb class - Explosive tool for destroying/transforming objects
 * Based on ../source/bomb.h - simplified version
 */
class Bomb : public Sprite {
public:
    enum BombState {
        INACTIVE = 0,
        ARMED = 1,
        EXPLODING = 2,
        EXPLODED = 3
    };

    Bomb(float x, float y)
        : Sprite(x, y, 50.0f, 70.0f), state_(INACTIVE), fuse_time_(3.0f), timer_(0.0f) {}

    // Get/set bomb state
    BombState getState() const { return state_; }
    void setState(BombState state) { state_ = state; }

    // Get state as integer (for JS binding)
    int getStateInt() const { return static_cast<int>(state_); }
    void setStateInt(int state) {
        if (state >= INACTIVE && state <= EXPLODED) {
            state_ = static_cast<BombState>(state);
        }
    }

    // Fuse management
    float getFuseTime() const { return fuse_time_; }
    void setFuseTime(float time) { fuse_time_ = time; }

    float getTimer() const { return timer_; }
    void setTimer(float time) { timer_ = time; }

    // Actions
    void arm() {
        state_ = ARMED;
        timer_ = 0.0f;
    }

    void explode() {
        state_ = EXPLODING;
    }

    void reset() {
        state_ = INACTIVE;
        timer_ = 0.0f;
    }

    // Override update
    void update(float deltaTime) override {
        Sprite::update(deltaTime);

        // Count down when armed
        if (state_ == ARMED) {
            timer_ += deltaTime / 1000.0f; // deltaTime is in ms
            if (timer_ >= fuse_time_) {
                state_ = EXPLODING;
            }
        }
    }

private:
    BombState state_;
    float fuse_time_;  // Seconds until explosion
    float timer_;      // Current countdown timer
};

/**
 * Thought_Bubble class - Container for Robot's program
 * Based on ../source/thought.h - simplified version
 */
class Thought_Bubble : public Sprite {
public:
    Thought_Bubble(float x, float y)
        : Sprite(x, y, 100.0f, 80.0f), has_cubby_(false), cubby_id_(-1) {}

    // Cubby management (simplified - just track if it has one)
    bool hasCubby() const { return has_cubby_; }
    void setHasCubby(bool has_cubby) { has_cubby_ = has_cubby; }

    // Cubby ID (reference to what the robot should do)
    int getCubbyId() const { return cubby_id_; }
    void setCubbyId(int id) {
        cubby_id_ = id;
        has_cubby_ = (id >= 0);
    }

    void clearCubby() {
        has_cubby_ = false;
        cubby_id_ = -1;
    }

    // Override update
    void update(float deltaTime) override {
        Sprite::update(deltaTime);
        // Thought bubbles are static displays
    }

private:
    bool has_cubby_;
    int cubby_id_;  // Simplified reference to program
};

/**
 * Mouse class - Animated operator that performs math operations
 * Based on ../source/mouse.h - simplified version
 */
class Mouse : public Sprite {
public:
    enum MouseState {
        IDLE = 0,
        WORKING = 1,
        SMASHING = 2
    };

    enum Operation {
        ADD = 0,
        SUBTRACT = 1,
        MULTIPLY = 2,
        DIVIDE = 3,
        NONE = 4
    };

    Mouse(float x, float y)
        : Sprite(x, y, 60.0f, 60.0f), state_(IDLE), operation_(NONE),
          operand1_(0.0), operand2_(0.0), result_(0.0) {}

    // Get/set mouse state
    MouseState getState() const { return state_; }
    void setState(MouseState state) { state_ = state; }

    // Get state as integer (for JS binding)
    int getStateInt() const { return static_cast<int>(state_); }
    void setStateInt(int state) {
        if (state >= IDLE && state <= SMASHING) {
            state_ = static_cast<MouseState>(state);
        }
    }

    // Get/set operation
    Operation getOperation() const { return operation_; }
    void setOperation(Operation op) { operation_ = op; }

    // Get operation as integer (for JS binding)
    int getOperationInt() const { return static_cast<int>(operation_); }
    void setOperationInt(int op) {
        if (op >= ADD && op <= NONE) {
            operation_ = static_cast<Operation>(op);
        }
    }

    // Operands and result
    double getOperand1() const { return operand1_; }
    void setOperand1(double value) { operand1_ = value; }

    double getOperand2() const { return operand2_; }
    void setOperand2(double value) { operand2_ = value; }

    double getResult() const { return result_; }

    // Perform operation
    void doOperation() {
        state_ = WORKING;

        switch (operation_) {
            case ADD:
                result_ = operand1_ + operand2_;
                break;
            case SUBTRACT:
                result_ = operand1_ - operand2_;
                break;
            case MULTIPLY:
                result_ = operand1_ * operand2_;
                break;
            case DIVIDE:
                result_ = (operand2_ != 0.0) ? (operand1_ / operand2_) : 0.0;
                break;
            case NONE:
                result_ = 0.0;
                break;
        }

        // After operation, go to smashing state
        state_ = SMASHING;
    }

    void finishOperation() {
        state_ = IDLE;
        operation_ = NONE;
    }

    // Override update
    void update(float deltaTime) override {
        Sprite::update(deltaTime);
        // Mouse animates during work/smashing
    }

private:
    MouseState state_;
    Operation operation_;
    double operand1_;
    double operand2_;
    double result_;
};

// ===== Vacuum Class =====
// Simplified vacuum that can suck up, blank, and restore items
class Vacuum : public Sprite {
public:
    enum VacuumState {
        SUCK = 0,      // Ready to suck up items
        SPIT = 1,      // Ready to restore items
        BLANK = 2      // Ready to blank items
    };

    Vacuum(float x, float y)
        : Sprite(x, y, 60.0f, 80.0f), state_(SUCK), sucked_count_(0) {}

    VacuumState getState() const { return state_; }
    int getStateInt() const { return static_cast<int>(state_); }

    void setState(VacuumState state) { state_ = state; }
    void setStateInt(int state) {
        if (state >= 0 && state <= 2) {
            state_ = static_cast<VacuumState>(state);
        }
    }

    int getSuckedCount() const { return sucked_count_; }
    void setSuckedCount(int count) { sucked_count_ = count; }

    void suckUp() {
        if (state_ == SUCK) {
            sucked_count_++;
        }
    }

    void restore() {
        if (state_ == SPIT && sucked_count_ > 0) {
            sucked_count_--;
        }
    }

    void makeBlank() {
        if (state_ == BLANK) {
            // Blank operation
        }
    }

    void update(float deltaTime) override {
        Sprite::update(deltaTime);
        // Vacuum animations could go here
    }

private:
    VacuumState state_;
    int sucked_count_;  // Number of items sucked up
};

// ===== Martian Class =====
// Simplified help character (Marty) with talk balloon
class Martian : public Sprite {
public:
    enum MartianState {
        IDLE = 0,           // Waiting
        TELEPORTING_IN = 1,  // Arriving
        TELEPORTING_OUT = 2, // Leaving
        TALKING = 3          // Speaking to user
    };

    Martian(float x, float y)
        : Sprite(x, y, 70.0f, 90.0f),
          state_(IDLE),
          has_balloon_(false),
          message_id_(0),
          on_floor_(false) {}

    MartianState getState() const { return state_; }
    int getStateInt() const { return static_cast<int>(state_); }

    void setState(MartianState state) { state_ = state; }
    void setStateInt(int state) {
        if (state >= 0 && state <= 3) {
            state_ = static_cast<MartianState>(state);
        }
    }

    bool hasBalloon() const { return has_balloon_; }
    void setHasBalloon(bool has) { has_balloon_ = has; }

    int getMessageId() const { return message_id_; }
    void setMessageId(int id) { message_id_ = id; }

    bool isOnFloor() const { return on_floor_; }
    void setOnFloor(bool on) { on_floor_ = on; }

    void teleportIn() {
        state_ = TELEPORTING_IN;
    }

    void teleportOut() {
        state_ = TELEPORTING_OUT;
    }

    void say(int messageId) {
        state_ = TALKING;
        has_balloon_ = true;
        message_id_ = messageId;
    }

    void stopTalking() {
        state_ = IDLE;
        has_balloon_ = false;
    }

    void update(float deltaTime) override {
        Sprite::update(deltaTime);
        // Martian animations could go here
    }

private:
    MartianState state_;
    bool has_balloon_;  // Has talk balloon visible
    int message_id_;    // Current message ID
    bool on_floor_;     // Is standing on floor
};

// ===== Toolbox Class =====
// Simplified toolbox containing stacks of objects
class Toolbox : public Sprite {
public:
    enum ToolboxState {
        CLOSED = 0,
        OPENING = 1,
        OPEN = 2,
        CLOSING = 3
    };

    enum ToolboxStack {
        NUMBERS = 0,
        TEXTS = 1,
        CUBBIES = 2,
        NESTS = 3,
        SCALES = 4,
        ROBOTS = 5,
        TRUCKS = 6,
        BOMBS = 7,
        STACK_COUNT = 8
    };

    Toolbox(float x, float y)
        : Sprite(x, y, 200.0f, 150.0f),
          state_(CLOSED),
          selected_stack_(NUMBERS) {
        // Initialize stack counts
        for (int i = 0; i < STACK_COUNT; i++) {
            stack_counts_[i] = 10;  // Each stack starts with 10 items
        }
    }

    ToolboxState getState() const { return state_; }
    int getStateInt() const { return static_cast<int>(state_); }

    void setState(ToolboxState state) { state_ = state; }
    void setStateInt(int state) {
        if (state >= 0 && state <= 3) {
            state_ = static_cast<ToolboxState>(state);
        }
    }

    int getSelectedStack() const { return static_cast<int>(selected_stack_); }
    void setSelectedStack(int stack) {
        if (stack >= 0 && stack < STACK_COUNT) {
            selected_stack_ = static_cast<ToolboxStack>(stack);
        }
    }

    int getStackCount(int stack) const {
        if (stack >= 0 && stack < STACK_COUNT) {
            return stack_counts_[stack];
        }
        return 0;
    }

    void setStackCount(int stack, int count) {
        if (stack >= 0 && stack < STACK_COUNT) {
            stack_counts_[stack] = count;
        }
    }

    void open() {
        if (state_ == CLOSED) {
            state_ = OPENING;
        }
    }

    void close() {
        if (state_ == OPEN) {
            state_ = CLOSING;
        }
    }

    void finishOpening() {
        if (state_ == OPENING) {
            state_ = OPEN;
        }
    }

    void finishClosing() {
        if (state_ == CLOSING) {
            state_ = CLOSED;
        }
    }

    void update(float deltaTime) override {
        Sprite::update(deltaTime);
        // Auto-complete opening/closing animations
    }

private:
    ToolboxState state_;
    ToolboxStack selected_stack_;
    int stack_counts_[STACK_COUNT];  // Count of items in each stack
};

// ===== Expander Class =====
// Simplified expander/bike pump for resizing objects
class Expander : public Sprite {
public:
    enum ExpanderState {
        BIGGER = 0,      // Grow in all dimensions
        TALLER = 1,      // Grow height only
        WIDER = 2,       // Grow width only
        SMALLER = 3,     // Shrink in all dimensions
        SHORTER = 4,     // Shrink height only
        NARROWER = 5,    // Shrink width only
        GOOD_SIZE = 6    // Perfect size
    };

    Expander(float x, float y)
        : Sprite(x, y, 60.0f, 80.0f),
          state_(BIGGER),
          scale_factor_(1.0f),
          target_scale_(1.0f) {}

    ExpanderState getState() const { return state_; }
    int getStateInt() const { return static_cast<int>(state_); }

    void setState(ExpanderState state) { state_ = state; }
    void setStateInt(int state) {
        if (state >= 0 && state <= 6) {
            state_ = static_cast<ExpanderState>(state);
        }
    }

    float getScaleFactor() const { return scale_factor_; }
    void setScaleFactor(float scale) { scale_factor_ = scale; }

    float getTargetScale() const { return target_scale_; }
    void setTargetScale(float scale) { target_scale_ = scale; }

    void expand() {
        if (state_ == BIGGER || state_ == TALLER || state_ == WIDER) {
            target_scale_ += 0.2f;
        }
    }

    void shrink() {
        if (state_ == SMALLER || state_ == SHORTER || state_ == NARROWER) {
            target_scale_ -= 0.2f;
            if (target_scale_ < 0.2f) target_scale_ = 0.2f;
        }
    }

    void setGoodSize() {
        state_ = GOOD_SIZE;
        target_scale_ = 1.0f;
    }

    void update(float deltaTime) override {
        Sprite::update(deltaTime);
        // Smoothly interpolate to target scale
        if (scale_factor_ != target_scale_) {
            float diff = target_scale_ - scale_factor_;
            scale_factor_ += diff * 0.1f;
        }
    }

private:
    ExpanderState state_;
    float scale_factor_;   // Current scale
    float target_scale_;   // Target scale
};

// ===== Copier Class =====
// Simplified magic wand for copying objects
class Copier : public Sprite {
public:
    enum CopierState {
        NORMAL = 0,    // Normal copy mode
        ORIGINAL = 1,  // Keep original
        SELF = 2       // Copy self
    };

    Copier(float x, float y)
        : Sprite(x, y, 60.0f, 70.0f),
          state_(NORMAL),
          copy_count_(0),
          max_copies_(-1),  // -1 = infinite
          has_attached_(false) {}

    CopierState getState() const { return state_; }
    int getStateInt() const { return static_cast<int>(state_); }

    void setState(CopierState state) { state_ = state; }
    void setStateInt(int state) {
        if (state >= 0 && state <= 2) {
            state_ = static_cast<CopierState>(state);
        }
    }

    int getCopyCount() const { return copy_count_; }
    void setCopyCount(int count) { copy_count_ = count; }

    int getMaxCopies() const { return max_copies_; }
    void setMaxCopies(int max) { max_copies_ = max; }

    bool hasAttached() const { return has_attached_; }
    void setHasAttached(bool has) { has_attached_ = has; }

    void makeCopy() {
        if (max_copies_ < 0 || copy_count_ < max_copies_) {
            copy_count_++;
        }
    }

    void reset() {
        copy_count_ = 0;
        has_attached_ = false;
    }

    void update(float deltaTime) override {
        Sprite::update(deltaTime);
        // Copier animations could go here
    }

private:
    CopierState state_;
    int copy_count_;       // Number of copies made
    int max_copies_;       // Maximum copies (-1 = infinite)
    bool has_attached_;    // Has attached object to copy
};

// ===== Eraser Class =====
// Simplified eraser for deleting objects
class Eraser : public Sprite {
public:
    enum EraserState {
        READY = 0,      // Ready to erase
        ERASING = 1,    // Currently erasing
        DONE = 2        // Erasing complete
    };

    Eraser(float x, float y)
        : Sprite(x, y, 50.0f, 60.0f),
          state_(READY),
          erased_count_(0),
          erase_progress_(0.0f) {}

    EraserState getState() const { return state_; }
    int getStateInt() const { return static_cast<int>(state_); }

    void setState(EraserState state) { state_ = state; }
    void setStateInt(int state) {
        if (state >= 0 && state <= 2) {
            state_ = static_cast<EraserState>(state);
        }
    }

    int getErasedCount() const { return erased_count_; }
    void setErasedCount(int count) { erased_count_ = count; }

    float getEraseProgress() const { return erase_progress_; }
    void setEraseProgress(float progress) { erase_progress_ = progress; }

    void startErasing() {
        state_ = ERASING;
        erase_progress_ = 0.0f;
    }

    void erase() {
        if (state_ == READY || state_ == ERASING) {
            erased_count_++;
            state_ = DONE;
            erase_progress_ = 1.0f;
        }
    }

    void reset() {
        state_ = READY;
        erase_progress_ = 0.0f;
    }

    void update(float deltaTime) override {
        Sprite::update(deltaTime);
        // Update erase animation progress
        if (state_ == ERASING && erase_progress_ < 1.0f) {
            erase_progress_ += deltaTime / 500.0f;  // 500ms to erase
            if (erase_progress_ >= 1.0f) {
                state_ = DONE;
            }
        }
    }

private:
    EraserState state_;
    int erased_count_;      // Count of erased objects
    float erase_progress_;  // Animation progress (0-1)
};

// ===== Cubby Class =====
// Multi-hole container with labels (like a structured box)
class Cubby : public Sprite {
public:
    Cubby(float x, float y, int numHoles = 4)
        : Sprite(x, y, 80.0f + (numHoles * 15.0f), 80.0f),
          num_holes_(numHoles),
          filled_count_(0) {
        // Initialize holes as empty
        for (int i = 0; i < num_holes_ && i < MAX_HOLES; i++) {
            hole_filled_[i] = false;
            hole_labels_[i] = '\0';
        }
    }

    int getNumHoles() const { return num_holes_; }

    int getFilledCount() const { return filled_count_; }

    bool isHoleFilled(int index) const {
        if (index >= 0 && index < num_holes_) {
            return hole_filled_[index];
        }
        return false;
    }

    void setHoleFilled(int index, bool filled) {
        if (index >= 0 && index < num_holes_) {
            bool was_filled = hole_filled_[index];
            hole_filled_[index] = filled;

            // Update count
            if (filled && !was_filled) {
                filled_count_++;
            } else if (!filled && was_filled) {
                filled_count_--;
            }
        }
    }

    char getHoleLabel(int index) const {
        if (index >= 0 && index < num_holes_) {
            return hole_labels_[index];
        }
        return '\0';
    }

    void setHoleLabel(int index, char label) {
        if (index >= 0 && index < num_holes_) {
            hole_labels_[index] = label;
        }
    }

    bool isFull() const {
        return filled_count_ >= num_holes_;
    }

    bool isEmpty() const {
        return filled_count_ == 0;
    }

    void clear() {
        for (int i = 0; i < num_holes_; i++) {
            hole_filled_[i] = false;
            hole_labels_[i] = '\0';
        }
        filled_count_ = 0;
    }

    void update(float deltaTime) override {
        Sprite::update(deltaTime);
    }

private:
    static const int MAX_HOLES = 10;
    int num_holes_;
    int filled_count_;
    bool hole_filled_[MAX_HOLES];
    char hole_labels_[MAX_HOLES];
};

// ===== Button Class =====
// UI control element with keyboard binding
class Button : public Sprite {
public:
    enum ButtonState {
        NORMAL = 0,
        HOVERED = 1,
        PRESSED = 2,
        DISABLED = 3
    };

    Button(float x, float y, char key = '\0')
        : Sprite(x, y, 60.0f, 40.0f),
          state_(NORMAL),
          key_binding_(key),
          press_count_(0),
          enabled_(true) {}

    ButtonState getState() const { return state_; }
    int getStateInt() const { return static_cast<int>(state_); }

    void setState(ButtonState state) { state_ = state; }
    void setStateInt(int state) {
        if (state >= 0 && state <= 3) {
            state_ = static_cast<ButtonState>(state);
        }
    }

    char getKeyBinding() const { return key_binding_; }
    void setKeyBinding(char key) { key_binding_ = key; }

    int getPressCount() const { return press_count_; }
    void setPressCount(int count) { press_count_ = count; }

    bool isEnabled() const { return enabled_; }
    void setEnabled(bool enabled) {
        enabled_ = enabled;
        if (!enabled) {
            state_ = DISABLED;
        } else if (state_ == DISABLED) {
            state_ = NORMAL;
        }
    }

    void press() {
        if (enabled_ && state_ != PRESSED) {
            state_ = PRESSED;
            press_count_++;
        }
    }

    void release() {
        if (state_ == PRESSED) {
            state_ = NORMAL;
        }
    }

    void hover() {
        if (enabled_ && state_ == NORMAL) {
            state_ = HOVERED;
        }
    }

    void unhover() {
        if (state_ == HOVERED) {
            state_ = NORMAL;
        }
    }

    void update(float deltaTime) override {
        Sprite::update(deltaTime);
    }

private:
    ButtonState state_;
    char key_binding_;
    int press_count_;
    bool enabled_;
};

// ===== Stack Class =====
// Formal stack data structure (LIFO)
class Stack : public Sprite {
public:
    Stack(float x, float y, int capacity = 10)
        : Sprite(x, y, 70.0f, 90.0f),
          capacity_(capacity),
          count_(0) {}

    int getCapacity() const { return capacity_; }

    int getCount() const { return count_; }

    bool isFull() const { return count_ >= capacity_; }

    bool isEmpty() const { return count_ == 0; }

    float getFullness() const {
        return capacity_ > 0 ? static_cast<float>(count_) / capacity_ : 0.0f;
    }

    bool push() {
        if (count_ < capacity_) {
            count_++;
            return true;
        }
        return false;
    }

    bool pop() {
        if (count_ > 0) {
            count_--;
            return true;
        }
        return false;
    }

    int peek() const {
        return count_;  // Return top index
    }

    void clear() {
        count_ = 0;
    }

    void setCapacity(int cap) {
        capacity_ = cap;
        if (count_ > capacity_) {
            count_ = capacity_;
        }
    }

    void update(float deltaTime) override {
        Sprite::update(deltaTime);
    }

private:
    int capacity_;
    int count_;
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

    class_<Picture, base<Sprite>>("Picture")
        .constructor<float, float, size_t, size_t>()
        .function("getPictureWidth", &Picture::getPictureWidth)
        .function("getPictureHeight", &Picture::getPictureHeight)
        .function("setPictureSize", &Picture::setPictureSize)
        .function("hasImage", &Picture::hasImage)
        .function("setHasImage", &Picture::setHasImage)
        .function("getImageId", &Picture::getImageId)
        .function("setImageId", &Picture::setImageId);

    class_<Sensor, base<Sprite>>("Sensor")
        .constructor<float, float, Sensor::SensorType>()
        .function("getTypeInt", &Sensor::getTypeInt)
        .function("setTypeInt", &Sensor::setTypeInt)
        .function("isActive", &Sensor::isActive)
        .function("setActive", &Sensor::setActive)
        .function("getValue", &Sensor::getValue)
        .function("setValue", &Sensor::setValue);

    // Sensor type enum values
    enum_<Sensor::SensorType>("SensorType")
        .value("MOUSE", Sensor::MOUSE)
        .value("KEYBOARD", Sensor::KEYBOARD)
        .value("TIME", Sensor::TIME)
        .value("COLLISION", Sensor::COLLISION);

    class_<Notebook, base<Sprite>>("Notebook")
        .constructor<float, float, size_t>()
        .function("getNumPages", &Notebook::getNumPages)
        .function("getCurrentPage", &Notebook::getCurrentPage)
        .function("setCurrentPage", &Notebook::setCurrentPage)
        .function("nextPage", &Notebook::nextPage)
        .function("previousPage", &Notebook::previousPage)
        .function("pageHasContent", &Notebook::pageHasContent)
        .function("setPageContent", &Notebook::setPageContent)
        .function("countPagesWithContent", &Notebook::countPagesWithContent);

    class_<Bomb, base<Sprite>>("Bomb")
        .constructor<float, float>()
        .function("getStateInt", &Bomb::getStateInt)
        .function("setStateInt", &Bomb::setStateInt)
        .function("getFuseTime", &Bomb::getFuseTime)
        .function("setFuseTime", &Bomb::setFuseTime)
        .function("getTimer", &Bomb::getTimer)
        .function("setTimer", &Bomb::setTimer)
        .function("arm", &Bomb::arm)
        .function("explode", &Bomb::explode)
        .function("reset", &Bomb::reset);

    // Bomb state enum values
    enum_<Bomb::BombState>("BombState")
        .value("INACTIVE", Bomb::INACTIVE)
        .value("ARMED", Bomb::ARMED)
        .value("EXPLODING", Bomb::EXPLODING)
        .value("EXPLODED", Bomb::EXPLODED);

    class_<Thought_Bubble, base<Sprite>>("Thought_Bubble")
        .constructor<float, float>()
        .function("hasCubby", &Thought_Bubble::hasCubby)
        .function("setHasCubby", &Thought_Bubble::setHasCubby)
        .function("getCubbyId", &Thought_Bubble::getCubbyId)
        .function("setCubbyId", &Thought_Bubble::setCubbyId)
        .function("clearCubby", &Thought_Bubble::clearCubby);

    class_<Mouse, base<Sprite>>("Mouse")
        .constructor<float, float>()
        .function("getStateInt", &Mouse::getStateInt)
        .function("setStateInt", &Mouse::setStateInt)
        .function("getOperationInt", &Mouse::getOperationInt)
        .function("setOperationInt", &Mouse::setOperationInt)
        .function("getOperand1", &Mouse::getOperand1)
        .function("setOperand1", &Mouse::setOperand1)
        .function("getOperand2", &Mouse::getOperand2)
        .function("setOperand2", &Mouse::setOperand2)
        .function("getResult", &Mouse::getResult)
        .function("doOperation", &Mouse::doOperation)
        .function("finishOperation", &Mouse::finishOperation);

    // Mouse state enum values
    enum_<Mouse::MouseState>("MouseState")
        .value("IDLE", Mouse::IDLE)
        .value("WORKING", Mouse::WORKING)
        .value("SMASHING", Mouse::SMASHING);

    // Mouse operation enum values
    enum_<Mouse::Operation>("MouseOperation")
        .value("ADD", Mouse::ADD)
        .value("SUBTRACT", Mouse::SUBTRACT)
        .value("MULTIPLY", Mouse::MULTIPLY)
        .value("DIVIDE", Mouse::DIVIDE)
        .value("NONE", Mouse::NONE);

    // Vacuum class
    class_<Vacuum, base<Sprite>>("Vacuum")
        .constructor<float, float>()
        .function("getStateInt", &Vacuum::getStateInt)
        .function("setStateInt", &Vacuum::setStateInt)
        .function("getSuckedCount", &Vacuum::getSuckedCount)
        .function("setSuckedCount", &Vacuum::setSuckedCount)
        .function("suckUp", &Vacuum::suckUp)
        .function("restore", &Vacuum::restore)
        .function("makeBlank", &Vacuum::makeBlank);

    // Vacuum state enum values
    enum_<Vacuum::VacuumState>("VacuumState")
        .value("SUCK", Vacuum::SUCK)
        .value("SPIT", Vacuum::SPIT)
        .value("BLANK", Vacuum::BLANK);

    // Martian class
    class_<Martian, base<Sprite>>("Martian")
        .constructor<float, float>()
        .function("getStateInt", &Martian::getStateInt)
        .function("setStateInt", &Martian::setStateInt)
        .function("hasBalloon", &Martian::hasBalloon)
        .function("setHasBalloon", &Martian::setHasBalloon)
        .function("getMessageId", &Martian::getMessageId)
        .function("setMessageId", &Martian::setMessageId)
        .function("isOnFloor", &Martian::isOnFloor)
        .function("setOnFloor", &Martian::setOnFloor)
        .function("teleportIn", &Martian::teleportIn)
        .function("teleportOut", &Martian::teleportOut)
        .function("say", &Martian::say)
        .function("stopTalking", &Martian::stopTalking);

    // Martian state enum values
    enum_<Martian::MartianState>("MartianState")
        .value("IDLE", Martian::IDLE)
        .value("TELEPORTING_IN", Martian::TELEPORTING_IN)
        .value("TELEPORTING_OUT", Martian::TELEPORTING_OUT)
        .value("TALKING", Martian::TALKING);

    // Toolbox class
    class_<Toolbox, base<Sprite>>("Toolbox")
        .constructor<float, float>()
        .function("getStateInt", &Toolbox::getStateInt)
        .function("setStateInt", &Toolbox::setStateInt)
        .function("getSelectedStack", &Toolbox::getSelectedStack)
        .function("setSelectedStack", &Toolbox::setSelectedStack)
        .function("getStackCount", &Toolbox::getStackCount)
        .function("setStackCount", &Toolbox::setStackCount)
        .function("open", &Toolbox::open)
        .function("close", &Toolbox::close)
        .function("finishOpening", &Toolbox::finishOpening)
        .function("finishClosing", &Toolbox::finishClosing);

    // Toolbox state enum values
    enum_<Toolbox::ToolboxState>("ToolboxState")
        .value("CLOSED", Toolbox::CLOSED)
        .value("OPENING", Toolbox::OPENING)
        .value("OPEN", Toolbox::OPEN)
        .value("CLOSING", Toolbox::CLOSING);

    // Toolbox stack enum values
    enum_<Toolbox::ToolboxStack>("ToolboxStack")
        .value("NUMBERS", Toolbox::NUMBERS)
        .value("TEXTS", Toolbox::TEXTS)
        .value("CUBBIES", Toolbox::CUBBIES)
        .value("NESTS", Toolbox::NESTS)
        .value("SCALES", Toolbox::SCALES)
        .value("ROBOTS", Toolbox::ROBOTS)
        .value("TRUCKS", Toolbox::TRUCKS)
        .value("BOMBS", Toolbox::BOMBS)
        .value("STACK_COUNT", Toolbox::STACK_COUNT);

    // Expander class
    class_<Expander, base<Sprite>>("Expander")
        .constructor<float, float>()
        .function("getStateInt", &Expander::getStateInt)
        .function("setStateInt", &Expander::setStateInt)
        .function("getScaleFactor", &Expander::getScaleFactor)
        .function("setScaleFactor", &Expander::setScaleFactor)
        .function("getTargetScale", &Expander::getTargetScale)
        .function("setTargetScale", &Expander::setTargetScale)
        .function("expand", &Expander::expand)
        .function("shrink", &Expander::shrink)
        .function("setGoodSize", &Expander::setGoodSize);

    // Expander state enum values
    enum_<Expander::ExpanderState>("ExpanderState")
        .value("BIGGER", Expander::BIGGER)
        .value("TALLER", Expander::TALLER)
        .value("WIDER", Expander::WIDER)
        .value("SMALLER", Expander::SMALLER)
        .value("SHORTER", Expander::SHORTER)
        .value("NARROWER", Expander::NARROWER)
        .value("GOOD_SIZE", Expander::GOOD_SIZE);

    // Copier class
    class_<Copier, base<Sprite>>("Copier")
        .constructor<float, float>()
        .function("getStateInt", &Copier::getStateInt)
        .function("setStateInt", &Copier::setStateInt)
        .function("getCopyCount", &Copier::getCopyCount)
        .function("setCopyCount", &Copier::setCopyCount)
        .function("getMaxCopies", &Copier::getMaxCopies)
        .function("setMaxCopies", &Copier::setMaxCopies)
        .function("hasAttached", &Copier::hasAttached)
        .function("setHasAttached", &Copier::setHasAttached)
        .function("makeCopy", &Copier::makeCopy)
        .function("reset", &Copier::reset);

    // Copier state enum values
    enum_<Copier::CopierState>("CopierState")
        .value("NORMAL", Copier::NORMAL)
        .value("ORIGINAL", Copier::ORIGINAL)
        .value("SELF", Copier::SELF);

    // Eraser class
    class_<Eraser, base<Sprite>>("Eraser")
        .constructor<float, float>()
        .function("getStateInt", &Eraser::getStateInt)
        .function("setStateInt", &Eraser::setStateInt)
        .function("getErasedCount", &Eraser::getErasedCount)
        .function("setErasedCount", &Eraser::setErasedCount)
        .function("getEraseProgress", &Eraser::getEraseProgress)
        .function("setEraseProgress", &Eraser::setEraseProgress)
        .function("startErasing", &Eraser::startErasing)
        .function("erase", &Eraser::erase)
        .function("reset", &Eraser::reset);

    // Eraser state enum values
    enum_<Eraser::EraserState>("EraserState")
        .value("READY", Eraser::READY)
        .value("ERASING", Eraser::ERASING)
        .value("DONE", Eraser::DONE);

    // Cubby class
    class_<Cubby, base<Sprite>>("Cubby")
        .constructor<float, float, int>()
        .function("getNumHoles", &Cubby::getNumHoles)
        .function("getFilledCount", &Cubby::getFilledCount)
        .function("isHoleFilled", &Cubby::isHoleFilled)
        .function("setHoleFilled", &Cubby::setHoleFilled)
        .function("getHoleLabel", &Cubby::getHoleLabel)
        .function("setHoleLabel", &Cubby::setHoleLabel)
        .function("isFull", &Cubby::isFull)
        .function("isEmpty", &Cubby::isEmpty)
        .function("clear", &Cubby::clear);

    // Button class
    class_<Button, base<Sprite>>("Button")
        .constructor<float, float, char>()
        .function("getStateInt", &Button::getStateInt)
        .function("setStateInt", &Button::setStateInt)
        .function("getKeyBinding", &Button::getKeyBinding)
        .function("setKeyBinding", &Button::setKeyBinding)
        .function("getPressCount", &Button::getPressCount)
        .function("setPressCount", &Button::setPressCount)
        .function("isEnabled", &Button::isEnabled)
        .function("setEnabled", &Button::setEnabled)
        .function("press", &Button::press)
        .function("release", &Button::release)
        .function("hover", &Button::hover)
        .function("unhover", &Button::unhover);

    // Button state enum values
    enum_<Button::ButtonState>("ButtonState")
        .value("NORMAL", Button::NORMAL)
        .value("HOVERED", Button::HOVERED)
        .value("PRESSED", Button::PRESSED)
        .value("DISABLED", Button::DISABLED);

    // Stack class
    class_<Stack, base<Sprite>>("Stack")
        .constructor<float, float, int>()
        .function("getCapacity", &Stack::getCapacity)
        .function("getCount", &Stack::getCount)
        .function("isFull", &Stack::isFull)
        .function("isEmpty", &Stack::isEmpty)
        .function("getFullness", &Stack::getFullness)
        .function("push", &Stack::push)
        .function("pop", &Stack::pop)
        .function("peek", &Stack::peek)
        .function("clear", &Stack::clear)
        .function("setCapacity", &Stack::setCapacity);
}

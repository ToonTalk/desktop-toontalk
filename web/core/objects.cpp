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
 * Box class - Container with multiple holes, like original ToonTalk Cubby
 * Based on ../source/cubby.h and ../source/cubby.cpp
 */
class Box : public Sprite {
public:
    Box(float x, float y, size_t num_holes = 1)
        : Sprite(x, y, calculateWidth(num_holes), 100.0f),
          num_holes_(num_holes),
          holes_filled_(num_holes, false),
          labels_(num_holes, "") {
        updateDimensions();
    }

    // Number of holes
    size_t getNumHoles() const { return num_holes_; }

    void setNumHoles(size_t num) {
        num_holes_ = num;
        holes_filled_.resize(num, false);
        labels_.resize(num, "");
        updateDimensions();
    }

    // Hole-specific operations
    bool isHoleFilled(size_t index) const {
        return index < holes_filled_.size() && holes_filled_[index];
    }

    void setHoleFilled(size_t index, bool filled) {
        if (index < holes_filled_.size()) {
            holes_filled_[index] = filled;
        }
    }

    // Get hole label
    std::string getHoleLabel(size_t index) const {
        if (index < labels_.size()) {
            return labels_[index];
        }
        return "";
    }

    // Set hole label
    void setHoleLabel(size_t index, const std::string& label) {
        if (index < labels_.size()) {
            labels_[index] = label;
        }
    }

    // Container-level operations
    size_t getCount() const {
        size_t count = 0;
        for (bool filled : holes_filled_) {
            if (filled) count++;
        }
        return count;
    }

    bool isEmpty() const { return getCount() == 0; }
    bool isFull() const { return getCount() >= num_holes_; }

    void clear() {
        for (size_t i = 0; i < holes_filled_.size(); ++i) {
            holes_filled_[i] = false;
        }
    }

    // Legacy methods for backward compatibility
    bool addItem() {
        // Fill first empty hole
        for (size_t i = 0; i < holes_filled_.size(); ++i) {
            if (!holes_filled_[i]) {
                holes_filled_[i] = true;
                return true;
            }
        }
        return false;
    }

    bool removeItem() {
        // Empty last filled hole
        for (int i = holes_filled_.size() - 1; i >= 0; --i) {
            if (holes_filled_[i]) {
                holes_filled_[i] = false;
                return true;
            }
        }
        return false;
    }

    // Get visual representation info
    float getFullness() const {
        return num_holes_ > 0 ? static_cast<float>(getCount()) / num_holes_ : 0.0f;
    }

    // Override update
    void update(float deltaTime) override {
        Sprite::update(deltaTime);
        // Boxes don't move on their own
    }

private:
    size_t num_holes_;
    std::vector<bool> holes_filled_;
    std::vector<std::string> labels_;

    // Calculate width based on number of holes (like original ToonTalk)
    // Original: width = (5*number_of_holes+1)*peg_width
    // Using peg_width ≈ 20 pixels as base unit
    static float calculateWidth(size_t num_holes) {
        const float PEG_WIDTH = 20.0f;
        return (5.0f * num_holes + 1.0f) * PEG_WIDTH;
    }

    void updateDimensions() {
        setWidth(calculateWidth(num_holes_));
        // Height stays constant at 4 peg units
        setHeight(4.0f * 20.0f); // 80 pixels
    }
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

/**
 * Flipper - Tool for flipping and rotating objects
 * Can flip horizontally, vertically, or rotate in either direction
 */
class Flipper : public Sprite {
public:
    enum FlipperMode {
        HORIZONTAL = 0,
        VERTICAL = 1,
        ROTATE_CW = 2,
        ROTATE_CCW = 3
    };

    Flipper(float x, float y)
        : Sprite(x, y, 70.0f, 50.0f),
          mode_(HORIZONTAL),
          flip_count_(0),
          rotation_angle_(0.0f),
          has_attached_(false) {}

    // Mode management
    int getModeInt() const { return static_cast<int>(mode_); }
    void setModeInt(int mode) {
        if (mode >= 0 && mode <= 3) {
            mode_ = static_cast<FlipperMode>(mode);
        }
    }

    void nextMode() {
        mode_ = static_cast<FlipperMode>((static_cast<int>(mode_) + 1) % 4);
    }

    // Flip operations
    void doFlip() {
        flip_count_++;
        if (mode_ == ROTATE_CW) {
            rotation_angle_ += 90.0f;
        } else if (mode_ == ROTATE_CCW) {
            rotation_angle_ -= 90.0f;
        }
        // Normalize angle to 0-360
        while (rotation_angle_ >= 360.0f) rotation_angle_ -= 360.0f;
        while (rotation_angle_ < 0.0f) rotation_angle_ += 360.0f;
    }

    void reset() {
        rotation_angle_ = 0.0f;
        flip_count_ = 0;
    }

    // Getters
    int getFlipCount() const { return flip_count_; }
    void setFlipCount(int count) { flip_count_ = count; }

    float getRotationAngle() const { return rotation_angle_; }
    void setRotationAngle(float angle) { rotation_angle_ = angle; }

    bool hasAttached() const { return has_attached_; }
    void setAttached(bool attached) { has_attached_ = attached; }

    void update(float deltaTime) override {
        Sprite::update(deltaTime);
    }

private:
    FlipperMode mode_;
    int flip_count_;
    float rotation_angle_;
    bool has_attached_;
};

/**
 * Meter - Visual gauge/indicator for monitoring values
 * Shows current value relative to min/max with warning/critical thresholds
 */
class Meter : public Sprite {
public:
    enum MeterState {
        NORMAL = 0,
        WARNING = 1,
        CRITICAL = 2,
        MAXED = 3
    };

    Meter(float x, float y, float minValue = 0.0f, float maxValue = 100.0f)
        : Sprite(x, y, 60.0f, 80.0f),
          state_(NORMAL),
          current_value_(0.0f),
          min_value_(minValue),
          max_value_(maxValue),
          warning_threshold_(70.0f),
          critical_threshold_(90.0f) {
        updateState();
    }

    // State management
    int getStateInt() const { return static_cast<int>(state_); }
    void setStateInt(int state) {
        if (state >= 0 && state <= 3) {
            state_ = static_cast<MeterState>(state);
        }
    }

    // Value management
    float getValue() const { return current_value_; }
    void setValue(float value) {
        current_value_ = value;
        if (current_value_ < min_value_) current_value_ = min_value_;
        if (current_value_ > max_value_) current_value_ = max_value_;
        updateState();
    }

    float getMinValue() const { return min_value_; }
    void setMinValue(float value) { min_value_ = value; }

    float getMaxValue() const { return max_value_; }
    void setMaxValue(float value) { max_value_ = value; }

    float getWarningThreshold() const { return warning_threshold_; }
    void setWarningThreshold(float value) { warning_threshold_ = value; }

    float getCriticalThreshold() const { return critical_threshold_; }
    void setCriticalThreshold(float value) { critical_threshold_ = value; }

    // Computed properties
    float getPercentage() const {
        if (max_value_ <= min_value_) return 0.0f;
        return ((current_value_ - min_value_) / (max_value_ - min_value_)) * 100.0f;
    }

    bool isWarning() const { return state_ == WARNING; }
    bool isCritical() const { return state_ == CRITICAL; }
    bool isMaxed() const { return state_ == MAXED; }

    void increment(float amount = 1.0f) {
        setValue(current_value_ + amount);
    }

    void decrement(float amount = 1.0f) {
        setValue(current_value_ - amount);
    }

    void update(float deltaTime) override {
        Sprite::update(deltaTime);
    }

private:
    void updateState() {
        float percentage = getPercentage();
        if (percentage >= 100.0f) {
            state_ = MAXED;
        } else if (percentage >= critical_threshold_) {
            state_ = CRITICAL;
        } else if (percentage >= warning_threshold_) {
            state_ = WARNING;
        } else {
            state_ = NORMAL;
        }
    }

    MeterState state_;
    float current_value_;
    float min_value_;
    float max_value_;
    float warning_threshold_;
    float critical_threshold_;
};

/**
 * Beeper - Sound/alert tool for triggering audio feedback
 * Can beep once or continuously with configurable frequency
 */
class Beeper : public Sprite {
public:
    enum BeeperState {
        SILENT = 0,
        BEEPING = 1,
        CONTINUOUS = 2
    };

    Beeper(float x, float y)
        : Sprite(x, y, 50.0f, 50.0f),
          state_(SILENT),
          beep_count_(0),
          frequency_(440.0f),  // A4 note in Hz
          duration_(0.2f),
          timer_(0.0f) {}

    // State management
    int getStateInt() const { return static_cast<int>(state_); }
    void setStateInt(int state) {
        if (state >= 0 && state <= 2) {
            state_ = static_cast<BeeperState>(state);
        }
    }

    // Beep operations
    void beep() {
        if (state_ == SILENT || state_ == BEEPING) {
            state_ = BEEPING;
            beep_count_++;
            timer_ = duration_;
        }
    }

    void startContinuous() {
        state_ = CONTINUOUS;
    }

    void stop() {
        state_ = SILENT;
        timer_ = 0.0f;
    }

    // Properties
    int getBeepCount() const { return beep_count_; }
    void setBeepCount(int count) { beep_count_ = count; }

    float getFrequency() const { return frequency_; }
    void setFrequency(float freq) {
        if (freq > 0.0f) frequency_ = freq;
    }

    float getDuration() const { return duration_; }
    void setDuration(float dur) {
        if (dur > 0.0f) duration_ = dur;
    }

    float getTimer() const { return timer_; }

    bool isBeeping() const { return state_ != SILENT; }

    void reset() {
        beep_count_ = 0;
        stop();
    }

    void update(float deltaTime) override {
        if (state_ == BEEPING) {
            timer_ -= deltaTime;
            if (timer_ <= 0.0f) {
                state_ = SILENT;
                timer_ = 0.0f;
            }
        }
        Sprite::update(deltaTime);
    }

private:
    BeeperState state_;
    int beep_count_;
    float frequency_;
    float duration_;
    float timer_;
};

/**
 * Connector - Tool for linking objects together
 * Creates connections between objects for data flow or interaction
 */
class Connector : public Sprite {
public:
    enum ConnectorState {
        DISCONNECTED = 0,
        CONNECTING = 1,
        CONNECTED = 2,
        ERROR = 3
    };

    Connector(float x, float y)
        : Sprite(x, y, 60.0f, 40.0f),
          state_(DISCONNECTED),
          connection_count_(0),
          max_connections_(2),
          source_id_(0),
          target_id_(0) {}

    // State management
    int getStateInt() const { return static_cast<int>(state_); }
    void setStateInt(int state) {
        if (state >= 0 && state <= 3) {
            state_ = static_cast<ConnectorState>(state);
        }
    }

    // Connection management
    void connect(int sourceId, int targetId) {
        if (state_ != CONNECTED && connection_count_ < max_connections_) {
            source_id_ = sourceId;
            target_id_ = targetId;
            state_ = CONNECTED;
            connection_count_++;
        } else if (connection_count_ >= max_connections_) {
            state_ = ERROR;
        }
    }

    void disconnect() {
        state_ = DISCONNECTED;
        source_id_ = 0;
        target_id_ = 0;
        if (connection_count_ > 0) {
            connection_count_--;
        }
    }

    void startConnecting() {
        state_ = CONNECTING;
    }

    // Getters/Setters
    int getConnectionCount() const { return connection_count_; }
    void setConnectionCount(int count) { connection_count_ = count; }

    int getMaxConnections() const { return max_connections_; }
    void setMaxConnections(int max) { max_connections_ = max; }

    int getSourceId() const { return source_id_; }
    void setSourceId(int id) { source_id_ = id; }

    int getTargetId() const { return target_id_; }
    void setTargetId(int id) { target_id_ = id; }

    bool isConnected() const { return state_ == CONNECTED; }
    bool hasError() const { return state_ == ERROR; }

    void update(float deltaTime) override {
        Sprite::update(deltaTime);
    }

private:
    ConnectorState state_;
    int connection_count_;
    int max_connections_;
    int source_id_;
    int target_id_;
};

/**
 * Timer - Time-based trigger tool
 * Counts down and triggers events at specified intervals
 */
class Timer : public Sprite {
public:
    enum TimerState {
        STOPPED = 0,
        RUNNING = 1,
        PAUSED = 2,
        FINISHED = 3
    };

    Timer(float x, float y, float duration = 10.0f)
        : Sprite(x, y, 70.0f, 70.0f),
          state_(STOPPED),
          duration_(duration),
          elapsed_(0.0f),
          remaining_(duration),
          loop_(false),
          trigger_count_(0) {}

    // State management
    int getStateInt() const { return static_cast<int>(state_); }
    void setStateInt(int state) {
        if (state >= 0 && state <= 3) {
            state_ = static_cast<TimerState>(state);
        }
    }

    // Timer controls
    void start() {
        if (state_ == STOPPED || state_ == FINISHED) {
            state_ = RUNNING;
            elapsed_ = 0.0f;
            remaining_ = duration_;
        }
    }

    void pause() {
        if (state_ == RUNNING) {
            state_ = PAUSED;
        }
    }

    void resume() {
        if (state_ == PAUSED) {
            state_ = RUNNING;
        }
    }

    void stop() {
        state_ = STOPPED;
        elapsed_ = 0.0f;
        remaining_ = duration_;
    }

    void reset() {
        stop();
        trigger_count_ = 0;
    }

    // Properties
    float getDuration() const { return duration_; }
    void setDuration(float dur) {
        if (dur > 0.0f) {
            duration_ = dur;
            remaining_ = dur - elapsed_;
        }
    }

    float getElapsed() const { return elapsed_; }
    void setElapsed(float elapsed) { elapsed_ = elapsed; }

    float getRemaining() const { return remaining_; }

    float getProgress() const {
        if (duration_ <= 0.0f) return 0.0f;
        return elapsed_ / duration_;
    }

    bool isLoop() const { return loop_; }
    void setLoop(bool loop) { loop_ = loop; }

    int getTriggerCount() const { return trigger_count_; }
    void setTriggerCount(int count) { trigger_count_ = count; }

    bool isRunning() const { return state_ == RUNNING; }
    bool isFinished() const { return state_ == FINISHED; }

    void update(float deltaTime) override {
        if (state_ == RUNNING) {
            elapsed_ += deltaTime;
            remaining_ = duration_ - elapsed_;

            if (remaining_ <= 0.0f) {
                trigger_count_++;
                if (loop_) {
                    elapsed_ = 0.0f;
                    remaining_ = duration_;
                } else {
                    state_ = FINISHED;
                    remaining_ = 0.0f;
                }
            }
        }
        Sprite::update(deltaTime);
    }

private:
    TimerState state_;
    float duration_;
    float elapsed_;
    float remaining_;
    bool loop_;
    int trigger_count_;
};

/**
 * Counter - Counting tool with increment/decrement
 * Tracks counts with optional min/max limits
 */
class Counter : public Sprite {
public:
    enum CounterState {
        NORMAL = 0,
        AT_MIN = 1,
        AT_MAX = 2,
        OVERFLOW = 3
    };

    Counter(float x, float y, int initialValue = 0)
        : Sprite(x, y, 60.0f, 60.0f),
          state_(NORMAL),
          value_(initialValue),
          min_value_(0),
          max_value_(999),
          step_(1),
          has_min_limit_(false),
          has_max_limit_(false) {
        updateState();
    }

    // State management
    int getStateInt() const { return static_cast<int>(state_); }
    void setStateInt(int state) {
        if (state >= 0 && state <= 3) {
            state_ = static_cast<CounterState>(state);
        }
    }

    // Counting operations
    void increment() {
        int oldValue = value_;
        value_ += step_;

        if (has_max_limit_ && value_ > max_value_) {
            value_ = max_value_;
            state_ = AT_MAX;
        } else if (!has_max_limit_ && value_ > max_value_) {
            state_ = OVERFLOW;
        } else {
            updateState();
        }
    }

    void decrement() {
        int oldValue = value_;
        value_ -= step_;

        if (has_min_limit_ && value_ < min_value_) {
            value_ = min_value_;
            state_ = AT_MIN;
        } else if (!has_min_limit_ && value_ < min_value_) {
            value_ = min_value_; // Still clamp but mark as underflow
            state_ = AT_MIN;
        } else {
            updateState();
        }
    }

    void reset() {
        value_ = 0;
        updateState();
    }

    // Value management
    int getValue() const { return value_; }
    void setValue(int value) {
        value_ = value;
        updateState();
    }

    int getMinValue() const { return min_value_; }
    void setMinValue(int value) { min_value_ = value; }

    int getMaxValue() const { return max_value_; }
    void setMaxValue(int value) { max_value_ = value; }

    int getStep() const { return step_; }
    void setStep(int step) {
        if (step > 0) step_ = step;
    }

    bool hasMinLimit() const { return has_min_limit_; }
    void setMinLimit(bool limit) { has_min_limit_ = limit; }

    bool hasMaxLimit() const { return has_max_limit_; }
    void setMaxLimit(bool limit) { has_max_limit_ = limit; }

    bool isAtMin() const { return state_ == AT_MIN; }
    bool isAtMax() const { return state_ == AT_MAX; }
    bool isOverflow() const { return state_ == OVERFLOW; }

    void update(float deltaTime) override {
        Sprite::update(deltaTime);
    }

private:
    void updateState() {
        if (has_min_limit_ && value_ <= min_value_) {
            state_ = AT_MIN;
        } else if (has_max_limit_ && value_ >= max_value_) {
            state_ = AT_MAX;
        } else if (value_ > max_value_) {
            state_ = OVERFLOW;
        } else {
            state_ = NORMAL;
        }
    }

    CounterState state_;
    int value_;
    int min_value_;
    int max_value_;
    int step_;
    bool has_min_limit_;
    bool has_max_limit_;
};

/**
 * Sampler - Tool for sampling/recording values at intervals
 * Records data points over time with configurable sample rate
 */
class Sampler : public Sprite {
public:
    enum SamplerState {
        IDLE = 0,
        SAMPLING = 1,
        PAUSED = 2,
        FULL = 3
    };

    Sampler(float x, float y, int capacity = 100)
        : Sprite(x, y, 70.0f, 60.0f),
          state_(IDLE),
          sample_count_(0),
          capacity_(capacity),
          sample_rate_(1.0f),
          last_sample_time_(0.0f),
          current_value_(0.0f),
          min_value_(0.0f),
          max_value_(0.0f) {}

    // State management
    int getStateInt() const { return static_cast<int>(state_); }
    void setStateInt(int state) {
        if (state >= 0 && state <= 3) {
            state_ = static_cast<SamplerState>(state);
        }
    }

    // Sampling controls
    void startSampling() {
        if (state_ != FULL) {
            state_ = SAMPLING;
            last_sample_time_ = 0.0f;
        }
    }

    void pauseSampling() {
        if (state_ == SAMPLING) {
            state_ = PAUSED;
        }
    }

    void resumeSampling() {
        if (state_ == PAUSED && state_ != FULL) {
            state_ = SAMPLING;
        }
    }

    void stopSampling() {
        state_ = IDLE;
    }

    void clear() {
        sample_count_ = 0;
        min_value_ = 0.0f;
        max_value_ = 0.0f;
        state_ = IDLE;
    }

    void recordSample(float value) {
        if (sample_count_ < capacity_) {
            current_value_ = value;
            if (sample_count_ == 0) {
                min_value_ = max_value_ = value;
            } else {
                if (value < min_value_) min_value_ = value;
                if (value > max_value_) max_value_ = value;
            }
            sample_count_++;
            if (sample_count_ >= capacity_) {
                state_ = FULL;
            }
        }
    }

    // Properties
    int getSampleCount() const { return sample_count_; }
    int getCapacity() const { return capacity_; }
    void setCapacity(int capacity) { capacity_ = capacity; }

    float getSampleRate() const { return sample_rate_; }
    void setSampleRate(float rate) {
        if (rate > 0.0f) sample_rate_ = rate;
    }

    float getCurrentValue() const { return current_value_; }
    float getMinValue() const { return min_value_; }
    float getMaxValue() const { return max_value_; }

    bool isSampling() const { return state_ == SAMPLING; }
    bool isFull() const { return state_ == FULL; }

    float getFullness() const {
        if (capacity_ <= 0) return 0.0f;
        return static_cast<float>(sample_count_) / static_cast<float>(capacity_);
    }

    void update(float deltaTime) override {
        if (state_ == SAMPLING) {
            last_sample_time_ += deltaTime;
            if (last_sample_time_ >= (1.0f / sample_rate_)) {
                // Time to take a sample (in real implementation, would sample from attached object)
                last_sample_time_ = 0.0f;
            }
        }
        Sprite::update(deltaTime);
    }

private:
    SamplerState state_;
    int sample_count_;
    int capacity_;
    float sample_rate_;
    float last_sample_time_;
    float current_value_;
    float min_value_;
    float max_value_;
};

/**
 * Comparator - Tool for comparing two values
 * Outputs comparison results with configurable tolerance
 */
class Comparator : public Sprite {
public:
    enum ComparisonResult {
        EQUAL = 0,
        LESS_THAN = 1,
        GREATER_THAN = 2,
        NOT_EQUAL = 3
    };

    Comparator(float x, float y)
        : Sprite(x, y, 80.0f, 60.0f),
          value_a_(0.0f),
          value_b_(0.0f),
          result_(EQUAL),
          tolerance_(0.001f),
          comparison_count_(0) {
        updateComparison();
    }

    // Value management
    float getValueA() const { return value_a_; }
    void setValueA(float value) {
        value_a_ = value;
        updateComparison();
        comparison_count_++;
    }

    float getValueB() const { return value_b_; }
    void setValueB(float value) {
        value_b_ = value;
        updateComparison();
        comparison_count_++;
    }

    void setValues(float a, float b) {
        value_a_ = a;
        value_b_ = b;
        updateComparison();
        comparison_count_++;
    }

    // Result
    int getResultInt() const { return static_cast<int>(result_); }

    bool isEqual() const { return result_ == EQUAL; }
    bool isLessThan() const { return result_ == LESS_THAN; }
    bool isGreaterThan() const { return result_ == GREATER_THAN; }
    bool isNotEqual() const { return result_ == NOT_EQUAL; }

    // Properties
    float getTolerance() const { return tolerance_; }
    void setTolerance(float tolerance) {
        if (tolerance >= 0.0f) {
            tolerance_ = tolerance;
            updateComparison();
        }
    }

    float getDifference() const { return value_a_ - value_b_; }
    int getComparisonCount() const { return comparison_count_; }

    void reset() {
        value_a_ = 0.0f;
        value_b_ = 0.0f;
        comparison_count_ = 0;
        updateComparison();
    }

    void update(float deltaTime) override {
        Sprite::update(deltaTime);
    }

private:
    void updateComparison() {
        float diff = value_a_ - value_b_;
        if (std::abs(diff) <= tolerance_) {
            result_ = EQUAL;
        } else if (diff < 0.0f) {
            result_ = LESS_THAN;
        } else {
            result_ = GREATER_THAN;
        }

        if (std::abs(diff) > tolerance_) {
            result_ = NOT_EQUAL;
        } else {
            result_ = EQUAL;
        }
    }

    float value_a_;
    float value_b_;
    ComparisonResult result_;
    float tolerance_;
    int comparison_count_;
};

/**
 * Randomizer - Random value generator
 * Generates random numbers with configurable range and distribution
 */
class Randomizer : public Sprite {
public:
    enum RandomizerMode {
        UNIFORM = 0,
        INTEGER = 1,
        BOOLEAN = 2,
        DICE = 3
    };

    Randomizer(float x, float y)
        : Sprite(x, y, 60.0f, 60.0f),
          mode_(UNIFORM),
          min_value_(0.0f),
          max_value_(1.0f),
          current_value_(0.0f),
          generation_count_(0),
          seed_(12345) {}

    // Mode management
    int getModeInt() const { return static_cast<int>(mode_); }
    void setModeInt(int mode) {
        if (mode >= 0 && mode <= 3) {
            mode_ = static_cast<RandomizerMode>(mode);
        }
    }

    // Generation
    void generate() {
        generation_count_++;

        // Simple linear congruential generator for deterministic randomness
        seed_ = (seed_ * 1103515245 + 12345) & 0x7fffffff;
        float random_0_1 = static_cast<float>(seed_) / static_cast<float>(0x7fffffff);

        switch (mode_) {
            case UNIFORM:
                current_value_ = min_value_ + random_0_1 * (max_value_ - min_value_);
                break;
            case INTEGER:
                current_value_ = min_value_ + std::floor(random_0_1 * (max_value_ - min_value_ + 1));
                break;
            case BOOLEAN:
                current_value_ = random_0_1 < 0.5f ? 0.0f : 1.0f;
                break;
            case DICE:
                current_value_ = 1.0f + std::floor(random_0_1 * 6.0f);
                break;
        }
    }

    // Properties
    float getCurrentValue() const { return current_value_; }

    float getMinValue() const { return min_value_; }
    void setMinValue(float value) { min_value_ = value; }

    float getMaxValue() const { return max_value_; }
    void setMaxValue(float value) { max_value_ = value; }

    int getGenerationCount() const { return generation_count_; }
    void setGenerationCount(int count) { generation_count_ = count; }

    int getSeed() const { return seed_; }
    void setSeed(int seed) { seed_ = seed; }

    void reset() {
        current_value_ = 0.0f;
        generation_count_ = 0;
    }

    void update(float deltaTime) override {
        Sprite::update(deltaTime);
    }

private:
    RandomizerMode mode_;
    float min_value_;
    float max_value_;
    float current_value_;
    int generation_count_;
    int seed_;
};

/**
 * Logger - Tool for recording and displaying log entries
 * Records timestamped messages with severity levels
 */
class Logger : public Sprite {
public:
    enum LogLevel {
        DEBUG = 0,
        INFO = 1,
        WARNING = 2,
        ERROR = 3
    };

    enum LoggerState {
        IDLE = 0,
        LOGGING = 1,
        PAUSED = 2,
        FULL = 3
    };

    Logger(float x, float y, int max_entries = 100)
        : Sprite(x, y, 80.0f, 70.0f),
          state_(IDLE),
          entry_count_(0),
          max_entries_(max_entries),
          min_level_(DEBUG),
          current_level_(INFO),
          debug_count_(0),
          info_count_(0),
          warning_count_(0),
          error_count_(0) {}

    int getStateInt() const { return static_cast<int>(state_); }
    void setStateInt(int state) { state_ = static_cast<LoggerState>(state); }

    void startLogging() {
        if (state_ != FULL) {
            state_ = LOGGING;
        }
    }

    void pauseLogging() {
        if (state_ == LOGGING) {
            state_ = PAUSED;
        }
    }

    void resumeLogging() {
        if (state_ == PAUSED) {
            state_ = LOGGING;
        }
    }

    void stopLogging() {
        state_ = IDLE;
    }

    void clear() {
        entry_count_ = 0;
        debug_count_ = 0;
        info_count_ = 0;
        warning_count_ = 0;
        error_count_ = 0;
        if (state_ == FULL) {
            state_ = IDLE;
        }
    }

    void logEntry(int level) {
        if (state_ == LOGGING && entry_count_ < max_entries_) {
            current_level_ = static_cast<LogLevel>(level);
            if (current_level_ >= min_level_) {
                entry_count_++;
                switch (current_level_) {
                    case DEBUG: debug_count_++; break;
                    case INFO: info_count_++; break;
                    case WARNING: warning_count_++; break;
                    case ERROR: error_count_++; break;
                }
                if (entry_count_ >= max_entries_) {
                    state_ = FULL;
                }
            }
        }
    }

    int getEntryCount() const { return entry_count_; }
    int getMaxEntries() const { return max_entries_; }
    void setMaxEntries(int max) { max_entries_ = max; }

    int getMinLevelInt() const { return static_cast<int>(min_level_); }
    void setMinLevelInt(int level) { min_level_ = static_cast<LogLevel>(level); }

    int getCurrentLevelInt() const { return static_cast<int>(current_level_); }

    int getDebugCount() const { return debug_count_; }
    int getInfoCount() const { return info_count_; }
    int getWarningCount() const { return warning_count_; }
    int getErrorCount() const { return error_count_; }

    bool isLogging() const { return state_ == LOGGING; }
    bool isFull() const { return state_ == FULL; }

    float getFullness() const {
        if (max_entries_ <= 0) return 0.0f;
        return static_cast<float>(entry_count_) / static_cast<float>(max_entries_);
    }

private:
    LoggerState state_;
    int entry_count_;
    int max_entries_;
    LogLevel min_level_;
    LogLevel current_level_;
    int debug_count_;
    int info_count_;
    int warning_count_;
    int error_count_;
};

/**
 * Filter - Tool for filtering and transforming values
 * Applies configurable rules to accept/reject/transform input
 */
class Filter : public Sprite {
public:
    enum FilterMode {
        PASS_ALL = 0,
        PASS_RANGE = 1,
        BLOCK_RANGE = 2,
        TRANSFORM = 3
    };

    enum FilterState {
        IDLE = 0,
        ACTIVE = 1,
        BYPASSED = 2
    };

    Filter(float x, float y)
        : Sprite(x, y, 70.0f, 60.0f),
          mode_(PASS_ALL),
          state_(IDLE),
          min_threshold_(0.0f),
          max_threshold_(100.0f),
          transform_scale_(1.0f),
          transform_offset_(0.0f),
          input_value_(0.0f),
          output_value_(0.0f),
          passed_count_(0),
          blocked_count_(0) {}

    int getModeInt() const { return static_cast<int>(mode_); }
    void setModeInt(int mode) { mode_ = static_cast<FilterMode>(mode); }

    int getStateInt() const { return static_cast<int>(state_); }
    void setStateInt(int state) { state_ = static_cast<FilterState>(state); }

    void activate() { state_ = ACTIVE; }
    void deactivate() { state_ = IDLE; }
    void bypass() { state_ = BYPASSED; }

    bool processValue(float value) {
        input_value_ = value;

        if (state_ == BYPASSED) {
            output_value_ = value;
            passed_count_++;
            return true;
        }

        if (state_ != ACTIVE) {
            return false;
        }

        bool passes = false;
        switch (mode_) {
            case PASS_ALL:
                passes = true;
                output_value_ = value;
                break;
            case PASS_RANGE:
                passes = (value >= min_threshold_ && value <= max_threshold_);
                output_value_ = value;
                break;
            case BLOCK_RANGE:
                passes = (value < min_threshold_ || value > max_threshold_);
                output_value_ = value;
                break;
            case TRANSFORM:
                passes = true;
                output_value_ = value * transform_scale_ + transform_offset_;
                break;
        }

        if (passes) {
            passed_count_++;
        } else {
            blocked_count_++;
        }

        return passes;
    }

    float getInputValue() const { return input_value_; }
    float getOutputValue() const { return output_value_; }

    float getMinThreshold() const { return min_threshold_; }
    void setMinThreshold(float value) { min_threshold_ = value; }

    float getMaxThreshold() const { return max_threshold_; }
    void setMaxThreshold(float value) { max_threshold_ = value; }

    void setThresholds(float min_val, float max_val) {
        min_threshold_ = min_val;
        max_threshold_ = max_val;
    }

    float getTransformScale() const { return transform_scale_; }
    void setTransformScale(float scale) { transform_scale_ = scale; }

    float getTransformOffset() const { return transform_offset_; }
    void setTransformOffset(float offset) { transform_offset_ = offset; }

    void setTransform(float scale, float offset) {
        transform_scale_ = scale;
        transform_offset_ = offset;
    }

    int getPassedCount() const { return passed_count_; }
    int getBlockedCount() const { return blocked_count_; }

    void reset() {
        passed_count_ = 0;
        blocked_count_ = 0;
        input_value_ = 0.0f;
        output_value_ = 0.0f;
    }

    bool isActive() const { return state_ == ACTIVE; }
    bool isBypassed() const { return state_ == BYPASSED; }

private:
    FilterMode mode_;
    FilterState state_;
    float min_threshold_;
    float max_threshold_;
    float transform_scale_;
    float transform_offset_;
    float input_value_;
    float output_value_;
    int passed_count_;
    int blocked_count_;
};

/**
 * Accumulator - Tool for accumulating and aggregating values
 * Computes running statistics (sum, average, min, max)
 */
class Accumulator : public Sprite {
public:
    enum AccumulatorMode {
        SUM = 0,
        AVERAGE = 1,
        MIN = 2,
        MAX = 3,
        COUNT = 4
    };

    enum AccumulatorState {
        IDLE = 0,
        ACCUMULATING = 1,
        PAUSED = 2
    };

    Accumulator(float x, float y)
        : Sprite(x, y, 70.0f, 60.0f),
          mode_(SUM),
          state_(IDLE),
          sum_(0.0f),
          count_(0),
          min_value_(0.0f),
          max_value_(0.0f),
          current_value_(0.0f),
          decay_rate_(0.0f) {}

    int getModeInt() const { return static_cast<int>(mode_); }
    void setModeInt(int mode) { mode_ = static_cast<AccumulatorMode>(mode); }

    int getStateInt() const { return static_cast<int>(state_); }
    void setStateInt(int state) { state_ = static_cast<AccumulatorState>(state); }

    void start() { state_ = ACCUMULATING; }
    void pause() { if (state_ == ACCUMULATING) state_ = PAUSED; }
    void resume() { if (state_ == PAUSED) state_ = ACCUMULATING; }
    void stop() { state_ = IDLE; }

    void accumulate(float value) {
        if (state_ != ACCUMULATING) return;

        if (count_ == 0) {
            sum_ = value;
            min_value_ = value;
            max_value_ = value;
        } else {
            // Apply decay if configured
            if (decay_rate_ > 0.0f) {
                sum_ *= (1.0f - decay_rate_);
            }

            sum_ += value;
            if (value < min_value_) min_value_ = value;
            if (value > max_value_) max_value_ = value;
        }

        count_++;
        updateCurrentValue();
    }

    void clear() {
        sum_ = 0.0f;
        count_ = 0;
        min_value_ = 0.0f;
        max_value_ = 0.0f;
        current_value_ = 0.0f;
    }

    void reset() {
        clear();
        state_ = IDLE;
    }

    float getCurrentValue() const { return current_value_; }
    float getSum() const { return sum_; }
    int getCount() const { return count_; }
    float getMinValue() const { return min_value_; }
    float getMaxValue() const { return max_value_; }

    float getAverage() const {
        if (count_ == 0) return 0.0f;
        return sum_ / static_cast<float>(count_);
    }

    float getDecayRate() const { return decay_rate_; }
    void setDecayRate(float rate) {
        decay_rate_ = (rate < 0.0f) ? 0.0f : (rate > 1.0f) ? 1.0f : rate;
    }

    bool isAccumulating() const { return state_ == ACCUMULATING; }

private:
    void updateCurrentValue() {
        switch (mode_) {
            case SUM:
                current_value_ = sum_;
                break;
            case AVERAGE:
                current_value_ = getAverage();
                break;
            case MIN:
                current_value_ = min_value_;
                break;
            case MAX:
                current_value_ = max_value_;
                break;
            case COUNT:
                current_value_ = static_cast<float>(count_);
                break;
        }
    }

    AccumulatorMode mode_;
    AccumulatorState state_;
    float sum_;
    int count_;
    float min_value_;
    float max_value_;
    float current_value_;
    float decay_rate_;
};

/**
 * Sequencer - Tool for sequencing operations in order
 * Manages step-by-step execution with configurable flow control
 */
class Sequencer : public Sprite {
public:
    enum SequencerMode {
        LINEAR = 0,      // Execute steps in order
        LOOP = 1,        // Loop back to start after end
        PINGPONG = 2,    // Reverse direction at ends
        RANDOM = 3       // Random step selection
    };

    enum SequencerState {
        IDLE = 0,
        RUNNING = 1,
        PAUSED = 2,
        COMPLETED = 3
    };

    Sequencer(float x, float y, int num_steps = 10)
        : Sprite(x, y, 70.0f, 60.0f),
          mode_(LINEAR),
          state_(IDLE),
          current_step_(0),
          num_steps_(num_steps),
          direction_(1),
          loop_count_(0),
          execution_count_(0),
          auto_advance_(false) {}

    int getModeInt() const { return static_cast<int>(mode_); }
    void setModeInt(int mode) { mode_ = static_cast<SequencerMode>(mode); }

    int getStateInt() const { return static_cast<int>(state_); }
    void setStateInt(int state) { state_ = static_cast<SequencerState>(state); }

    void start() {
        if (state_ != RUNNING) {
            state_ = RUNNING;
            current_step_ = 0;
            execution_count_ = 0;
        }
    }

    void pause() {
        if (state_ == RUNNING) {
            state_ = PAUSED;
        }
    }

    void resume() {
        if (state_ == PAUSED) {
            state_ = RUNNING;
        }
    }

    void stop() {
        state_ = IDLE;
        current_step_ = 0;
    }

    void reset() {
        stop();
        loop_count_ = 0;
        execution_count_ = 0;
    }

    void nextStep() {
        if (state_ != RUNNING) return;

        execution_count_++;

        switch (mode_) {
            case LINEAR:
                current_step_++;
                if (current_step_ >= num_steps_) {
                    state_ = COMPLETED;
                }
                break;
            case LOOP:
                current_step_ = (current_step_ + 1) % num_steps_;
                if (current_step_ == 0) {
                    loop_count_++;
                }
                break;
            case PINGPONG:
                current_step_ += direction_;
                if (current_step_ >= num_steps_ - 1) {
                    direction_ = -1;
                    loop_count_++;
                } else if (current_step_ <= 0) {
                    direction_ = 1;
                }
                break;
            case RANDOM:
                // Simple pseudo-random step selection
                current_step_ = (current_step_ * 1103515245 + 12345) % num_steps_;
                break;
        }
    }

    void previousStep() {
        if (state_ != RUNNING || current_step_ <= 0) return;
        current_step_--;
        execution_count_++;
    }

    void jumpToStep(int step) {
        if (step >= 0 && step < num_steps_) {
            current_step_ = step;
        }
    }

    int getCurrentStep() const { return current_step_; }
    int getNumSteps() const { return num_steps_; }
    void setNumSteps(int steps) {
        if (steps > 0) {
            num_steps_ = steps;
            if (current_step_ >= num_steps_) {
                current_step_ = num_steps_ - 1;
            }
        }
    }

    int getLoopCount() const { return loop_count_; }
    int getExecutionCount() const { return execution_count_; }

    bool getAutoAdvance() const { return auto_advance_; }
    void setAutoAdvance(bool enabled) { auto_advance_ = enabled; }

    float getProgress() const {
        if (num_steps_ <= 0) return 0.0f;
        return static_cast<float>(current_step_) / static_cast<float>(num_steps_ - 1);
    }

    bool isRunning() const { return state_ == RUNNING; }
    bool isCompleted() const { return state_ == COMPLETED; }

private:
    SequencerMode mode_;
    SequencerState state_;
    int current_step_;
    int num_steps_;
    int direction_;
    int loop_count_;
    int execution_count_;
    bool auto_advance_;
};

/**
 * Trigger - Event-based trigger with configurable conditions
 * Fires when conditions are met, with debouncing and latching
 */
class Trigger : public Sprite {
public:
    enum TriggerMode {
        EDGE_RISING = 0,   // Trigger on value going from low to high
        EDGE_FALLING = 1,  // Trigger on value going from high to low
        EDGE_BOTH = 2,     // Trigger on any edge
        LEVEL_HIGH = 3,    // Trigger while value is high
        LEVEL_LOW = 4      // Trigger while value is low
    };

    enum TriggerState {
        IDLE = 0,
        ARMED = 1,
        TRIGGERED = 2,
        LATCHED = 3
    };

    Trigger(float x, float y)
        : Sprite(x, y, 60.0f, 60.0f),
          mode_(EDGE_RISING),
          state_(IDLE),
          threshold_(0.5f),
          current_value_(0.0f),
          previous_value_(0.0f),
          trigger_count_(0),
          debounce_count_(0),
          debounce_threshold_(0),
          is_latched_(false) {}

    int getModeInt() const { return static_cast<int>(mode_); }
    void setModeInt(int mode) { mode_ = static_cast<TriggerMode>(mode); }

    int getStateInt() const { return static_cast<int>(state_); }
    void setStateInt(int state) { state_ = static_cast<TriggerState>(state); }

    void arm() {
        if (state_ == IDLE) {
            state_ = ARMED;
        }
    }

    void disarm() {
        state_ = IDLE;
    }

    void reset() {
        state_ = IDLE;
        is_latched_ = false;
        trigger_count_ = 0;
        debounce_count_ = 0;
    }

    void unlatch() {
        if (state_ == LATCHED) {
            is_latched_ = false;
            state_ = ARMED;
        }
    }

    bool checkTrigger(float value) {
        if (state_ == LATCHED) {
            return false;
        }

        previous_value_ = current_value_;
        current_value_ = value;

        if (state_ != ARMED) {
            return false;
        }

        bool should_trigger = false;
        bool is_high = current_value_ >= threshold_;
        bool was_high = previous_value_ >= threshold_;

        switch (mode_) {
            case EDGE_RISING:
                should_trigger = !was_high && is_high;
                break;
            case EDGE_FALLING:
                should_trigger = was_high && !is_high;
                break;
            case EDGE_BOTH:
                should_trigger = (was_high != is_high);
                break;
            case LEVEL_HIGH:
                should_trigger = is_high;
                break;
            case LEVEL_LOW:
                should_trigger = !is_high;
                break;
        }

        // Debouncing
        if (should_trigger) {
            debounce_count_++;
            if (debounce_count_ >= debounce_threshold_) {
                state_ = TRIGGERED;
                trigger_count_++;
                debounce_count_ = 0;
                return true;
            }
        } else {
            debounce_count_ = 0;
        }

        return false;
    }

    float getThreshold() const { return threshold_; }
    void setThreshold(float value) { threshold_ = value; }

    float getCurrentValue() const { return current_value_; }
    int getTriggerCount() const { return trigger_count_; }

    int getDebounceThreshold() const { return debounce_threshold_; }
    void setDebounceThreshold(int threshold) { debounce_threshold_ = threshold; }

    void latch() {
        if (state_ == TRIGGERED) {
            state_ = LATCHED;
            is_latched_ = true;
        }
    }

    bool isArmed() const { return state_ == ARMED; }
    bool isTriggered() const { return state_ == TRIGGERED; }
    bool isLatched() const { return state_ == LATCHED; }

private:
    TriggerMode mode_;
    TriggerState state_;
    float threshold_;
    float current_value_;
    float previous_value_;
    int trigger_count_;
    int debounce_count_;
    int debounce_threshold_;
    bool is_latched_;
};

/**
 * Scheduler - Time-based task scheduler
 * Schedules and manages tasks with delays and intervals
 */
class Scheduler : public Sprite {
public:
    enum SchedulerMode {
        ONCE = 0,        // Execute once after delay
        INTERVAL = 1,    // Execute repeatedly at interval
        CRON = 2         // Execute at specific times (simplified)
    };

    enum SchedulerState {
        IDLE = 0,
        WAITING = 1,
        READY = 2,
        EXECUTING = 3
    };

    Scheduler(float x, float y)
        : Sprite(x, y, 70.0f, 60.0f),
          mode_(ONCE),
          state_(IDLE),
          delay_(1.0f),
          interval_(1.0f),
          elapsed_time_(0.0f),
          last_execution_time_(0.0f),
          execution_count_(0),
          max_executions_(-1) {}

    int getModeInt() const { return static_cast<int>(mode_); }
    void setModeInt(int mode) { mode_ = static_cast<SchedulerMode>(mode); }

    int getStateInt() const { return static_cast<int>(state_); }
    void setStateInt(int state) { state_ = static_cast<SchedulerState>(state); }

    void start() {
        if (state_ == IDLE) {
            state_ = WAITING;
            elapsed_time_ = 0.0f;
            execution_count_ = 0;
        }
    }

    void stop() {
        state_ = IDLE;
        elapsed_time_ = 0.0f;
    }

    void pause() {
        if (state_ == WAITING) {
            state_ = IDLE;
        }
    }

    void resume() {
        if (state_ == IDLE && elapsed_time_ > 0.0f) {
            state_ = WAITING;
        }
    }

    void update(float delta_time) {
        if (state_ != WAITING) return;

        elapsed_time_ += delta_time;

        bool should_execute = false;

        switch (mode_) {
            case ONCE:
                if (elapsed_time_ >= delay_) {
                    should_execute = true;
                }
                break;
            case INTERVAL:
                if (elapsed_time_ >= delay_ + (execution_count_ * interval_)) {
                    should_execute = true;
                }
                break;
            case CRON:
                // Simplified cron-like behavior (uses interval as period)
                if (elapsed_time_ >= delay_ &&
                    (elapsed_time_ - delay_) >= (execution_count_ * interval_)) {
                    should_execute = true;
                }
                break;
        }

        if (should_execute) {
            state_ = READY;
            last_execution_time_ = elapsed_time_;
            execution_count_++;

            // Check if we've reached max executions
            if (max_executions_ > 0 && execution_count_ >= max_executions_) {
                state_ = IDLE;
            } else if (mode_ == ONCE) {
                state_ = IDLE;
            } else {
                // Continue waiting for next execution
                state_ = WAITING;
            }
        }
    }

    void execute() {
        if (state_ == READY) {
            state_ = EXECUTING;
        }
    }

    void finishExecution() {
        if (state_ == EXECUTING) {
            if (mode_ == ONCE ||
                (max_executions_ > 0 && execution_count_ >= max_executions_)) {
                state_ = IDLE;
            } else {
                state_ = WAITING;
            }
        }
    }

    float getDelay() const { return delay_; }
    void setDelay(float delay) { delay_ = (delay < 0.0f) ? 0.0f : delay; }

    float getInterval() const { return interval_; }
    void setInterval(float interval) {
        interval_ = (interval < 0.0f) ? 0.0f : interval;
    }

    float getElapsedTime() const { return elapsed_time_; }
    float getLastExecutionTime() const { return last_execution_time_; }
    int getExecutionCount() const { return execution_count_; }

    int getMaxExecutions() const { return max_executions_; }
    void setMaxExecutions(int max) { max_executions_ = max; }

    float getProgress() const {
        if (mode_ == ONCE) {
            return (delay_ <= 0.0f) ? 1.0f : (elapsed_time_ / delay_);
        } else if (mode_ == INTERVAL || mode_ == CRON) {
            float target_time = delay_ + (execution_count_ * interval_);
            float since_last = elapsed_time_ - last_execution_time_;
            return (interval_ <= 0.0f) ? 1.0f : (since_last / interval_);
        }
        return 0.0f;
    }

    bool isWaiting() const { return state_ == WAITING; }
    bool isReady() const { return state_ == READY; }
    bool isExecuting() const { return state_ == EXECUTING; }

private:
    SchedulerMode mode_;
    SchedulerState state_;
    float delay_;
    float interval_;
    float elapsed_time_;
    float last_execution_time_;
    int execution_count_;
    int max_executions_;
};

/**
 * CollisionManager - Spatial partitioning and collision detection
 */
class CollisionManager {
public:
    struct AABB {
        float x, y, width, height;

        bool intersects(const AABB& other) const {
            return !(x + width < other.x || other.x + other.width < x ||
                     y + height < other.y || other.y + other.height < y);
        }

        bool contains(float px, float py) const {
            return px >= x && px <= x + width && py >= y && py <= y + height;
        }
    };

    CollisionManager(float worldWidth, float worldHeight, int maxDepth = 5)
        : world_width_(worldWidth), world_height_(worldHeight),
          max_depth_(maxDepth), object_count_(0) {}

    // Add object with bounding box (returns object ID)
    int addObject(float x, float y, float width, float height) {
        int id = object_count_++;
        AABB box = {x, y, width, height};
        objects_[id] = box;
        return id;
    }

    // Update object position
    void updateObject(int id, float x, float y, float width, float height) {
        AABB box = {x, y, width, height};
        objects_[id] = box;
    }

    // Remove object
    void removeObject(int id) {
        objects_.erase(id);
    }

    // Check collision between two objects
    bool checkCollision(int id1, int id2) const {
        auto it1 = objects_.find(id1);
        auto it2 = objects_.find(id2);
        if (it1 == objects_.end() || it2 == objects_.end()) {
            return false;
        }
        return it1->second.intersects(it2->second);
    }

    // Find all objects intersecting with a point
    std::vector<int> queryPoint(float x, float y) const {
        std::vector<int> result;
        for (const auto& pair : objects_) {
            if (pair.second.contains(x, y)) {
                result.push_back(pair.first);
            }
        }
        return result;
    }

    // Find all objects intersecting with a region
    std::vector<int> queryRegion(float x, float y, float width, float height) const {
        std::vector<int> result;
        AABB query = {x, y, width, height};
        for (const auto& pair : objects_) {
            if (pair.second.intersects(query)) {
                result.push_back(pair.first);
            }
        }
        return result;
    }

    // Ray cast - find first object hit by ray
    int rayCast(float startX, float startY, float dirX, float dirY, float maxDist) const {
        float minDist = maxDist;
        int hitId = -1;

        for (const auto& pair : objects_) {
            const AABB& box = pair.second;
            float t = rayBoxIntersection(startX, startY, dirX, dirY, box);
            if (t >= 0.0f && t < minDist) {
                minDist = t;
                hitId = pair.first;
            }
        }
        return hitId;
    }

    void clear() {
        objects_.clear();
        object_count_ = 0;
    }

    int getObjectCount() const { return objects_.size(); }

private:
    float rayBoxIntersection(float ox, float oy, float dx, float dy, const AABB& box) const {
        float tmin = -std::numeric_limits<float>::infinity();
        float tmax = std::numeric_limits<float>::infinity();

        if (std::abs(dx) > 1e-6f) {
            float tx1 = (box.x - ox) / dx;
            float tx2 = (box.x + box.width - ox) / dx;
            tmin = std::max(tmin, std::min(tx1, tx2));
            tmax = std::min(tmax, std::max(tx1, tx2));
        }

        if (std::abs(dy) > 1e-6f) {
            float ty1 = (box.y - oy) / dy;
            float ty2 = (box.y + box.height - oy) / dy;
            tmin = std::max(tmin, std::min(ty1, ty2));
            tmax = std::min(tmax, std::max(ty1, ty2));
        }

        return (tmax >= tmin && tmin >= 0.0f) ? tmin : -1.0f;
    }

    std::map<int, AABB> objects_;
    float world_width_;
    float world_height_;
    int max_depth_;
    int object_count_;
};

/**
 * AnimationController - Tweening and easing system
 */
class AnimationController {
public:
    enum EasingType {
        LINEAR = 0,
        EASE_IN_QUAD,
        EASE_OUT_QUAD,
        EASE_IN_OUT_QUAD,
        EASE_IN_CUBIC,
        EASE_OUT_CUBIC,
        EASE_IN_OUT_CUBIC,
        EASE_IN_ELASTIC,
        EASE_OUT_ELASTIC,
        EASE_IN_BOUNCE,
        EASE_OUT_BOUNCE
    };

    AnimationController() : current_time_(0.0f) {}

    // Start a new animation (returns animation ID)
    int animate(float startValue, float endValue, float duration, EasingType easing = LINEAR) {
        int id = animations_.size();
        Animation anim = {startValue, endValue, duration, easing, 0.0f, true};
        animations_[id] = anim;
        return id;
    }

    // Update animations
    void update(float deltaTime) {
        current_time_ += deltaTime;

        std::vector<int> toRemove;
        for (auto& pair : animations_) {
            Animation& anim = pair.second;
            if (anim.active) {
                anim.elapsed += deltaTime;
                if (anim.elapsed >= anim.duration) {
                    toRemove.push_back(pair.first);
                }
            }
        }

        for (int id : toRemove) {
            animations_.erase(id);
        }
    }

    // Get current value of animation
    float getValue(int id) const {
        auto it = animations_.find(id);
        if (it == animations_.end() || !it->second.active) {
            return 0.0f;
        }

        const Animation& anim = it->second;
        float t = anim.elapsed / anim.duration;
        if (t >= 1.0f) t = 1.0f;

        float easedT = applyEasing(t, anim.easing);
        return anim.startValue + (anim.endValue - anim.startValue) * easedT;
    }

    // Check if animation is still running
    bool isActive(int id) const {
        auto it = animations_.find(id);
        return it != animations_.end() && it->second.active;
    }

    // Stop an animation
    void stop(int id) {
        animations_.erase(id);
    }

    void clear() {
        animations_.clear();
    }

    int getActiveCount() const { return animations_.size(); }

private:
    struct Animation {
        float startValue;
        float endValue;
        float duration;
        EasingType easing;
        float elapsed;
        bool active;
    };

    float applyEasing(float t, EasingType easing) const {
        switch (easing) {
            case LINEAR:
                return t;
            case EASE_IN_QUAD:
                return t * t;
            case EASE_OUT_QUAD:
                return t * (2.0f - t);
            case EASE_IN_OUT_QUAD:
                return t < 0.5f ? 2.0f * t * t : -1.0f + (4.0f - 2.0f * t) * t;
            case EASE_IN_CUBIC:
                return t * t * t;
            case EASE_OUT_CUBIC:
                return (--t) * t * t + 1.0f;
            case EASE_IN_OUT_CUBIC:
                return t < 0.5f ? 4.0f * t * t * t : (t - 1.0f) * (2.0f * t - 2.0f) * (2.0f * t - 2.0f) + 1.0f;
            case EASE_OUT_ELASTIC: {
                const float c4 = (2.0f * 3.14159265f) / 3.0f;
                return t == 0.0f ? 0.0f : t == 1.0f ? 1.0f :
                       std::pow(2.0f, -10.0f * t) * std::sin((t * 10.0f - 0.75f) * c4) + 1.0f;
            }
            case EASE_OUT_BOUNCE: {
                const float n1 = 7.5625f;
                const float d1 = 2.75f;
                if (t < 1.0f / d1) {
                    return n1 * t * t;
                } else if (t < 2.0f / d1) {
                    return n1 * (t -= 1.5f / d1) * t + 0.75f;
                } else if (t < 2.5f / d1) {
                    return n1 * (t -= 2.25f / d1) * t + 0.9375f;
                } else {
                    return n1 * (t -= 2.625f / d1) * t + 0.984375f;
                }
            }
            default:
                return t;
        }
    }

    std::map<int, Animation> animations_;
    float current_time_;
};

/**
 * SceneManager - Layer and camera management
 */
class SceneManager {
public:
    enum LayerType {
        BACKGROUND = 0,
        GAME = 1,
        UI = 2,
        OVERLAY = 3
    };

    SceneManager(float viewportWidth, float viewportHeight)
        : viewport_width_(viewportWidth), viewport_height_(viewportHeight),
          camera_x_(0.0f), camera_y_(0.0f), camera_zoom_(1.0f) {}

    // Camera controls
    void setCameraPosition(float x, float y) {
        camera_x_ = x;
        camera_y_ = y;
    }

    float getCameraX() const { return camera_x_; }
    float getCameraY() const { return camera_y_; }

    void setCameraZoom(float zoom) {
        camera_zoom_ = (zoom > 0.0f) ? zoom : 1.0f;
    }

    float getCameraZoom() const { return camera_zoom_; }

    void moveCamera(float dx, float dy) {
        camera_x_ += dx;
        camera_y_ += dy;
    }

    // Transform world coordinates to screen coordinates
    void worldToScreen(float worldX, float worldY, float& screenX, float& screenY) const {
        screenX = (worldX - camera_x_) * camera_zoom_ + viewport_width_ * 0.5f;
        screenY = (worldY - camera_y_) * camera_zoom_ + viewport_height_ * 0.5f;
    }

    // Transform screen coordinates to world coordinates
    void screenToWorld(float screenX, float screenY, float& worldX, float& worldY) const {
        worldX = (screenX - viewport_width_ * 0.5f) / camera_zoom_ + camera_x_;
        worldY = (screenY - viewport_height_ * 0.5f) / camera_zoom_ + camera_y_;
    }

    // Check if a world-space object is visible
    bool isVisible(float worldX, float worldY, float width, float height) const {
        float halfW = (viewport_width_ / camera_zoom_) * 0.5f;
        float halfH = (viewport_height_ / camera_zoom_) * 0.5f;

        return !(worldX + width < camera_x_ - halfW ||
                 worldX > camera_x_ + halfW ||
                 worldY + height < camera_y_ - halfH ||
                 worldY > camera_y_ + halfH);
    }

    // Viewport management
    void setViewportSize(float width, float height) {
        viewport_width_ = width;
        viewport_height_ = height;
    }

    float getViewportWidth() const { return viewport_width_; }
    float getViewportHeight() const { return viewport_height_; }

    // Get visible bounds in world space
    void getVisibleBounds(float& minX, float& minY, float& maxX, float& maxY) const {
        float halfW = (viewport_width_ / camera_zoom_) * 0.5f;
        float halfH = (viewport_height_ / camera_zoom_) * 0.5f;
        minX = camera_x_ - halfW;
        minY = camera_y_ - halfH;
        maxX = camera_x_ + halfW;
        maxY = camera_y_ + halfH;
    }

private:
    float viewport_width_;
    float viewport_height_;
    float camera_x_;
    float camera_y_;
    float camera_zoom_;
};

/**
 * EntityManager - Simple entity component system
 */
class EntityManager {
public:
    EntityManager() : next_entity_id_(0) {}

    // Create a new entity (returns entity ID)
    int createEntity(const std::string& type) {
        int id = next_entity_id_++;
        entities_[id] = type;
        active_[id] = true;
        return id;
    }

    // Destroy an entity
    void destroyEntity(int id) {
        entities_.erase(id);
        active_.erase(id);
        components_.erase(id);
    }

    // Check if entity exists
    bool hasEntity(int id) const {
        return entities_.find(id) != entities_.end();
    }

    // Get entity type
    std::string getEntityType(int id) const {
        auto it = entities_.find(id);
        return (it != entities_.end()) ? it->second : "";
    }

    // Active state
    void setActive(int id, bool active) {
        active_[id] = active;
    }

    bool isActive(int id) const {
        auto it = active_.find(id);
        return (it != active_.end()) ? it->second : false;
    }

    // Component storage (simple key-value per entity)
    void setComponent(int id, const std::string& key, float value) {
        components_[id][key] = value;
    }

    float getComponent(int id, const std::string& key) const {
        auto entityIt = components_.find(id);
        if (entityIt == components_.end()) return 0.0f;

        auto compIt = entityIt->second.find(key);
        return (compIt != entityIt->second.end()) ? compIt->second : 0.0f;
    }

    bool hasComponent(int id, const std::string& key) const {
        auto entityIt = components_.find(id);
        if (entityIt == components_.end()) return false;
        return entityIt->second.find(key) != entityIt->second.end();
    }

    // Query entities by type
    std::vector<int> getEntitiesByType(const std::string& type) const {
        std::vector<int> result;
        for (const auto& pair : entities_) {
            if (pair.second == type && isActive(pair.first)) {
                result.push_back(pair.first);
            }
        }
        return result;
    }

    int getEntityCount() const { return entities_.size(); }

    void clear() {
        entities_.clear();
        active_.clear();
        components_.clear();
        next_entity_id_ = 0;
    }

private:
    std::map<int, std::string> entities_;
    std::map<int, bool> active_;
    std::map<int, std::map<std::string, float>> components_;
    int next_entity_id_;
};

/**
 * InputManager - Abstract input handling and gesture recognition
 */
class InputManager {
public:
    enum GestureType {
        NONE = 0,
        TAP,
        DOUBLE_TAP,
        SWIPE_LEFT,
        SWIPE_RIGHT,
        SWIPE_UP,
        SWIPE_DOWN,
        PINCH_IN,
        PINCH_OUT
    };

    InputManager() : last_tap_time_(0.0f), tap_count_(0),
                     gesture_start_x_(0.0f), gesture_start_y_(0.0f),
                     is_tracking_(false), current_time_(0.0f) {}

    void update(float deltaTime) {
        current_time_ += deltaTime;

        // Reset tap count if too much time has passed
        if (current_time_ - last_tap_time_ > 0.5f) {
            tap_count_ = 0;
        }
    }

    // Record input events
    void onPointerDown(float x, float y) {
        gesture_start_x_ = x;
        gesture_start_y_ = y;
        is_tracking_ = true;

        // Track taps
        if (current_time_ - last_tap_time_ < 0.5f) {
            tap_count_++;
        } else {
            tap_count_ = 1;
        }
        last_tap_time_ = current_time_;
    }

    void onPointerUp(float x, float y) {
        if (!is_tracking_) return;

        float dx = x - gesture_start_x_;
        float dy = y - gesture_start_y_;
        float dist = std::sqrt(dx * dx + dy * dy);

        // Detect gesture
        if (dist < 10.0f) {
            // Tap or double tap
            last_gesture_ = (tap_count_ >= 2) ? DOUBLE_TAP : TAP;
        } else if (std::abs(dx) > std::abs(dy)) {
            // Horizontal swipe
            last_gesture_ = (dx > 0) ? SWIPE_RIGHT : SWIPE_LEFT;
        } else {
            // Vertical swipe
            last_gesture_ = (dy > 0) ? SWIPE_DOWN : SWIPE_UP;
        }

        is_tracking_ = false;
    }

    GestureType getLastGesture() const { return last_gesture_; }

    void clearGesture() { last_gesture_ = NONE; }

    // Command buffer for input recording
    void recordCommand(const std::string& command) {
        command_buffer_.push_back(command);
    }

    std::vector<std::string> getCommands() const {
        return command_buffer_;
    }

    void clearCommands() {
        command_buffer_.clear();
    }

    bool hasCommands() const {
        return !command_buffer_.empty();
    }

    int getCommandCount() const {
        return command_buffer_.size();
    }

private:
    float last_tap_time_;
    int tap_count_;
    float gesture_start_x_;
    float gesture_start_y_;
    bool is_tracking_;
    float current_time_;
    GestureType last_gesture_;
    std::vector<std::string> command_buffer_;
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
        .function("getNumHoles", &Box::getNumHoles)
        .function("setNumHoles", &Box::setNumHoles)
        .function("isHoleFilled", &Box::isHoleFilled)
        .function("setHoleFilled", &Box::setHoleFilled)
        .function("getHoleLabel", &Box::getHoleLabel)
        .function("setHoleLabel", &Box::setHoleLabel)
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

    // Flipper class
    class_<Flipper, base<Sprite>>("Flipper")
        .constructor<float, float>()
        .function("getModeInt", &Flipper::getModeInt)
        .function("setModeInt", &Flipper::setModeInt)
        .function("nextMode", &Flipper::nextMode)
        .function("doFlip", &Flipper::doFlip)
        .function("reset", &Flipper::reset)
        .function("getFlipCount", &Flipper::getFlipCount)
        .function("setFlipCount", &Flipper::setFlipCount)
        .function("getRotationAngle", &Flipper::getRotationAngle)
        .function("setRotationAngle", &Flipper::setRotationAngle)
        .function("hasAttached", &Flipper::hasAttached)
        .function("setAttached", &Flipper::setAttached);

    // Flipper mode enum values
    enum_<Flipper::FlipperMode>("FlipperMode")
        .value("HORIZONTAL", Flipper::HORIZONTAL)
        .value("VERTICAL", Flipper::VERTICAL)
        .value("ROTATE_CW", Flipper::ROTATE_CW)
        .value("ROTATE_CCW", Flipper::ROTATE_CCW);

    // Meter class
    class_<Meter, base<Sprite>>("Meter")
        .constructor<float, float, float, float>()
        .function("getStateInt", &Meter::getStateInt)
        .function("setStateInt", &Meter::setStateInt)
        .function("getValue", &Meter::getValue)
        .function("setValue", &Meter::setValue)
        .function("getMinValue", &Meter::getMinValue)
        .function("setMinValue", &Meter::setMinValue)
        .function("getMaxValue", &Meter::getMaxValue)
        .function("setMaxValue", &Meter::setMaxValue)
        .function("getWarningThreshold", &Meter::getWarningThreshold)
        .function("setWarningThreshold", &Meter::setWarningThreshold)
        .function("getCriticalThreshold", &Meter::getCriticalThreshold)
        .function("setCriticalThreshold", &Meter::setCriticalThreshold)
        .function("getPercentage", &Meter::getPercentage)
        .function("isWarning", &Meter::isWarning)
        .function("isCritical", &Meter::isCritical)
        .function("isMaxed", &Meter::isMaxed)
        .function("increment", &Meter::increment)
        .function("decrement", &Meter::decrement);

    // Meter state enum values
    enum_<Meter::MeterState>("MeterState")
        .value("NORMAL", Meter::NORMAL)
        .value("WARNING", Meter::WARNING)
        .value("CRITICAL", Meter::CRITICAL)
        .value("MAXED", Meter::MAXED);

    // Beeper class
    class_<Beeper, base<Sprite>>("Beeper")
        .constructor<float, float>()
        .function("getStateInt", &Beeper::getStateInt)
        .function("setStateInt", &Beeper::setStateInt)
        .function("beep", &Beeper::beep)
        .function("startContinuous", &Beeper::startContinuous)
        .function("stop", &Beeper::stop)
        .function("getBeepCount", &Beeper::getBeepCount)
        .function("setBeepCount", &Beeper::setBeepCount)
        .function("getFrequency", &Beeper::getFrequency)
        .function("setFrequency", &Beeper::setFrequency)
        .function("getDuration", &Beeper::getDuration)
        .function("setDuration", &Beeper::setDuration)
        .function("getTimer", &Beeper::getTimer)
        .function("isBeeping", &Beeper::isBeeping)
        .function("reset", &Beeper::reset);

    // Beeper state enum values
    enum_<Beeper::BeeperState>("BeeperState")
        .value("SILENT", Beeper::SILENT)
        .value("BEEPING", Beeper::BEEPING)
        .value("CONTINUOUS", Beeper::CONTINUOUS);

    // Connector class
    class_<Connector, base<Sprite>>("Connector")
        .constructor<float, float>()
        .function("getStateInt", &Connector::getStateInt)
        .function("setStateInt", &Connector::setStateInt)
        .function("connect", &Connector::connect)
        .function("disconnect", &Connector::disconnect)
        .function("startConnecting", &Connector::startConnecting)
        .function("getConnectionCount", &Connector::getConnectionCount)
        .function("setConnectionCount", &Connector::setConnectionCount)
        .function("getMaxConnections", &Connector::getMaxConnections)
        .function("setMaxConnections", &Connector::setMaxConnections)
        .function("getSourceId", &Connector::getSourceId)
        .function("setSourceId", &Connector::setSourceId)
        .function("getTargetId", &Connector::getTargetId)
        .function("setTargetId", &Connector::setTargetId)
        .function("isConnected", &Connector::isConnected)
        .function("hasError", &Connector::hasError);

    // Connector state enum values
    enum_<Connector::ConnectorState>("ConnectorState")
        .value("DISCONNECTED", Connector::DISCONNECTED)
        .value("CONNECTING", Connector::CONNECTING)
        .value("CONNECTED", Connector::CONNECTED)
        .value("ERROR", Connector::ERROR);

    // Timer class
    class_<Timer, base<Sprite>>("Timer")
        .constructor<float, float, float>()
        .function("getStateInt", &Timer::getStateInt)
        .function("setStateInt", &Timer::setStateInt)
        .function("start", &Timer::start)
        .function("pause", &Timer::pause)
        .function("resume", &Timer::resume)
        .function("stop", &Timer::stop)
        .function("reset", &Timer::reset)
        .function("getDuration", &Timer::getDuration)
        .function("setDuration", &Timer::setDuration)
        .function("getElapsed", &Timer::getElapsed)
        .function("setElapsed", &Timer::setElapsed)
        .function("getRemaining", &Timer::getRemaining)
        .function("getProgress", &Timer::getProgress)
        .function("isLoop", &Timer::isLoop)
        .function("setLoop", &Timer::setLoop)
        .function("getTriggerCount", &Timer::getTriggerCount)
        .function("setTriggerCount", &Timer::setTriggerCount)
        .function("isRunning", &Timer::isRunning)
        .function("isFinished", &Timer::isFinished);

    // Timer state enum values
    enum_<Timer::TimerState>("TimerState")
        .value("STOPPED", Timer::STOPPED)
        .value("RUNNING", Timer::RUNNING)
        .value("PAUSED", Timer::PAUSED)
        .value("FINISHED", Timer::FINISHED);

    // Counter class
    class_<Counter, base<Sprite>>("Counter")
        .constructor<float, float, int>()
        .function("getStateInt", &Counter::getStateInt)
        .function("setStateInt", &Counter::setStateInt)
        .function("increment", &Counter::increment)
        .function("decrement", &Counter::decrement)
        .function("reset", &Counter::reset)
        .function("getValue", &Counter::getValue)
        .function("setValue", &Counter::setValue)
        .function("getMinValue", &Counter::getMinValue)
        .function("setMinValue", &Counter::setMinValue)
        .function("getMaxValue", &Counter::getMaxValue)
        .function("setMaxValue", &Counter::setMaxValue)
        .function("getStep", &Counter::getStep)
        .function("setStep", &Counter::setStep)
        .function("hasMinLimit", &Counter::hasMinLimit)
        .function("setMinLimit", &Counter::setMinLimit)
        .function("hasMaxLimit", &Counter::hasMaxLimit)
        .function("setMaxLimit", &Counter::setMaxLimit)
        .function("isAtMin", &Counter::isAtMin)
        .function("isAtMax", &Counter::isAtMax)
        .function("isOverflow", &Counter::isOverflow);

    // Counter state enum values
    enum_<Counter::CounterState>("CounterState")
        .value("NORMAL", Counter::NORMAL)
        .value("AT_MIN", Counter::AT_MIN)
        .value("AT_MAX", Counter::AT_MAX)
        .value("OVERFLOW", Counter::OVERFLOW);

    // Sampler class
    class_<Sampler, base<Sprite>>("Sampler")
        .constructor<float, float, int>()
        .function("getStateInt", &Sampler::getStateInt)
        .function("setStateInt", &Sampler::setStateInt)
        .function("startSampling", &Sampler::startSampling)
        .function("pauseSampling", &Sampler::pauseSampling)
        .function("resumeSampling", &Sampler::resumeSampling)
        .function("stopSampling", &Sampler::stopSampling)
        .function("clear", &Sampler::clear)
        .function("recordSample", &Sampler::recordSample)
        .function("getSampleCount", &Sampler::getSampleCount)
        .function("getCapacity", &Sampler::getCapacity)
        .function("setCapacity", &Sampler::setCapacity)
        .function("getSampleRate", &Sampler::getSampleRate)
        .function("setSampleRate", &Sampler::setSampleRate)
        .function("getCurrentValue", &Sampler::getCurrentValue)
        .function("getMinValue", &Sampler::getMinValue)
        .function("getMaxValue", &Sampler::getMaxValue)
        .function("isSampling", &Sampler::isSampling)
        .function("isFull", &Sampler::isFull)
        .function("getFullness", &Sampler::getFullness);

    // Sampler state enum values
    enum_<Sampler::SamplerState>("SamplerState")
        .value("IDLE", Sampler::IDLE)
        .value("SAMPLING", Sampler::SAMPLING)
        .value("PAUSED", Sampler::PAUSED)
        .value("FULL", Sampler::FULL);

    // Comparator class
    class_<Comparator, base<Sprite>>("Comparator")
        .constructor<float, float>()
        .function("getValueA", &Comparator::getValueA)
        .function("setValueA", &Comparator::setValueA)
        .function("getValueB", &Comparator::getValueB)
        .function("setValueB", &Comparator::setValueB)
        .function("setValues", &Comparator::setValues)
        .function("getResultInt", &Comparator::getResultInt)
        .function("isEqual", &Comparator::isEqual)
        .function("isLessThan", &Comparator::isLessThan)
        .function("isGreaterThan", &Comparator::isGreaterThan)
        .function("isNotEqual", &Comparator::isNotEqual)
        .function("getTolerance", &Comparator::getTolerance)
        .function("setTolerance", &Comparator::setTolerance)
        .function("getDifference", &Comparator::getDifference)
        .function("getComparisonCount", &Comparator::getComparisonCount)
        .function("reset", &Comparator::reset);

    // Comparison result enum values
    enum_<Comparator::ComparisonResult>("ComparisonResult")
        .value("EQUAL", Comparator::EQUAL)
        .value("LESS_THAN", Comparator::LESS_THAN)
        .value("GREATER_THAN", Comparator::GREATER_THAN)
        .value("NOT_EQUAL", Comparator::NOT_EQUAL);

    // Randomizer class
    class_<Randomizer, base<Sprite>>("Randomizer")
        .constructor<float, float>()
        .function("getModeInt", &Randomizer::getModeInt)
        .function("setModeInt", &Randomizer::setModeInt)
        .function("generate", &Randomizer::generate)
        .function("getCurrentValue", &Randomizer::getCurrentValue)
        .function("getMinValue", &Randomizer::getMinValue)
        .function("setMinValue", &Randomizer::setMinValue)
        .function("getMaxValue", &Randomizer::getMaxValue)
        .function("setMaxValue", &Randomizer::setMaxValue)
        .function("getGenerationCount", &Randomizer::getGenerationCount)
        .function("setGenerationCount", &Randomizer::setGenerationCount)
        .function("getSeed", &Randomizer::getSeed)
        .function("setSeed", &Randomizer::setSeed)
        .function("reset", &Randomizer::reset);

    // Randomizer mode enum values
    enum_<Randomizer::RandomizerMode>("RandomizerMode")
        .value("UNIFORM", Randomizer::UNIFORM)
        .value("INTEGER", Randomizer::INTEGER)
        .value("BOOLEAN", Randomizer::BOOLEAN)
        .value("DICE", Randomizer::DICE);

    // Logger class
    class_<Logger, base<Sprite>>("Logger")
        .constructor<float, float, int>()
        .function("getStateInt", &Logger::getStateInt)
        .function("setStateInt", &Logger::setStateInt)
        .function("startLogging", &Logger::startLogging)
        .function("pauseLogging", &Logger::pauseLogging)
        .function("resumeLogging", &Logger::resumeLogging)
        .function("stopLogging", &Logger::stopLogging)
        .function("clear", &Logger::clear)
        .function("logEntry", &Logger::logEntry)
        .function("getEntryCount", &Logger::getEntryCount)
        .function("getMaxEntries", &Logger::getMaxEntries)
        .function("setMaxEntries", &Logger::setMaxEntries)
        .function("getMinLevelInt", &Logger::getMinLevelInt)
        .function("setMinLevelInt", &Logger::setMinLevelInt)
        .function("getCurrentLevelInt", &Logger::getCurrentLevelInt)
        .function("getDebugCount", &Logger::getDebugCount)
        .function("getInfoCount", &Logger::getInfoCount)
        .function("getWarningCount", &Logger::getWarningCount)
        .function("getErrorCount", &Logger::getErrorCount)
        .function("isLogging", &Logger::isLogging)
        .function("isFull", &Logger::isFull)
        .function("getFullness", &Logger::getFullness);

    // Logger state enum values
    enum_<Logger::LoggerState>("LoggerState")
        .value("IDLE", Logger::IDLE)
        .value("LOGGING", Logger::LOGGING)
        .value("PAUSED", Logger::PAUSED)
        .value("FULL", Logger::FULL);

    // Logger level enum values
    enum_<Logger::LogLevel>("LogLevel")
        .value("DEBUG", Logger::DEBUG)
        .value("INFO", Logger::INFO)
        .value("WARNING", Logger::WARNING)
        .value("ERROR", Logger::ERROR);

    // Filter class
    class_<Filter, base<Sprite>>("Filter")
        .constructor<float, float>()
        .function("getModeInt", &Filter::getModeInt)
        .function("setModeInt", &Filter::setModeInt)
        .function("getStateInt", &Filter::getStateInt)
        .function("setStateInt", &Filter::setStateInt)
        .function("activate", &Filter::activate)
        .function("deactivate", &Filter::deactivate)
        .function("bypass", &Filter::bypass)
        .function("processValue", &Filter::processValue)
        .function("getInputValue", &Filter::getInputValue)
        .function("getOutputValue", &Filter::getOutputValue)
        .function("getMinThreshold", &Filter::getMinThreshold)
        .function("setMinThreshold", &Filter::setMinThreshold)
        .function("getMaxThreshold", &Filter::getMaxThreshold)
        .function("setMaxThreshold", &Filter::setMaxThreshold)
        .function("setThresholds", &Filter::setThresholds)
        .function("getTransformScale", &Filter::getTransformScale)
        .function("setTransformScale", &Filter::setTransformScale)
        .function("getTransformOffset", &Filter::getTransformOffset)
        .function("setTransformOffset", &Filter::setTransformOffset)
        .function("setTransform", &Filter::setTransform)
        .function("getPassedCount", &Filter::getPassedCount)
        .function("getBlockedCount", &Filter::getBlockedCount)
        .function("reset", &Filter::reset)
        .function("isActive", &Filter::isActive)
        .function("isBypassed", &Filter::isBypassed);

    // Filter mode enum values
    enum_<Filter::FilterMode>("FilterMode")
        .value("PASS_ALL", Filter::PASS_ALL)
        .value("PASS_RANGE", Filter::PASS_RANGE)
        .value("BLOCK_RANGE", Filter::BLOCK_RANGE)
        .value("TRANSFORM", Filter::TRANSFORM);

    // Filter state enum values
    enum_<Filter::FilterState>("FilterState")
        .value("IDLE", Filter::IDLE)
        .value("ACTIVE", Filter::ACTIVE)
        .value("BYPASSED", Filter::BYPASSED);

    // Accumulator class
    class_<Accumulator, base<Sprite>>("Accumulator")
        .constructor<float, float>()
        .function("getModeInt", &Accumulator::getModeInt)
        .function("setModeInt", &Accumulator::setModeInt)
        .function("getStateInt", &Accumulator::getStateInt)
        .function("setStateInt", &Accumulator::setStateInt)
        .function("start", &Accumulator::start)
        .function("pause", &Accumulator::pause)
        .function("resume", &Accumulator::resume)
        .function("stop", &Accumulator::stop)
        .function("accumulate", &Accumulator::accumulate)
        .function("clear", &Accumulator::clear)
        .function("reset", &Accumulator::reset)
        .function("getCurrentValue", &Accumulator::getCurrentValue)
        .function("getSum", &Accumulator::getSum)
        .function("getCount", &Accumulator::getCount)
        .function("getMinValue", &Accumulator::getMinValue)
        .function("getMaxValue", &Accumulator::getMaxValue)
        .function("getAverage", &Accumulator::getAverage)
        .function("getDecayRate", &Accumulator::getDecayRate)
        .function("setDecayRate", &Accumulator::setDecayRate)
        .function("isAccumulating", &Accumulator::isAccumulating);

    // Accumulator mode enum values
    enum_<Accumulator::AccumulatorMode>("AccumulatorMode")
        .value("SUM", Accumulator::SUM)
        .value("AVERAGE", Accumulator::AVERAGE)
        .value("MIN", Accumulator::MIN)
        .value("MAX", Accumulator::MAX)
        .value("COUNT", Accumulator::COUNT);

    // Accumulator state enum values
    enum_<Accumulator::AccumulatorState>("AccumulatorState")
        .value("IDLE", Accumulator::IDLE)
        .value("ACCUMULATING", Accumulator::ACCUMULATING)
        .value("PAUSED", Accumulator::PAUSED);

    // Sequencer class
    class_<Sequencer, base<Sprite>>("Sequencer")
        .constructor<float, float, int>()
        .function("getModeInt", &Sequencer::getModeInt)
        .function("setModeInt", &Sequencer::setModeInt)
        .function("getStateInt", &Sequencer::getStateInt)
        .function("setStateInt", &Sequencer::setStateInt)
        .function("start", &Sequencer::start)
        .function("pause", &Sequencer::pause)
        .function("resume", &Sequencer::resume)
        .function("stop", &Sequencer::stop)
        .function("reset", &Sequencer::reset)
        .function("nextStep", &Sequencer::nextStep)
        .function("previousStep", &Sequencer::previousStep)
        .function("jumpToStep", &Sequencer::jumpToStep)
        .function("getCurrentStep", &Sequencer::getCurrentStep)
        .function("getNumSteps", &Sequencer::getNumSteps)
        .function("setNumSteps", &Sequencer::setNumSteps)
        .function("getLoopCount", &Sequencer::getLoopCount)
        .function("getExecutionCount", &Sequencer::getExecutionCount)
        .function("getAutoAdvance", &Sequencer::getAutoAdvance)
        .function("setAutoAdvance", &Sequencer::setAutoAdvance)
        .function("getProgress", &Sequencer::getProgress)
        .function("isRunning", &Sequencer::isRunning)
        .function("isCompleted", &Sequencer::isCompleted);

    // Sequencer mode enum values
    enum_<Sequencer::SequencerMode>("SequencerMode")
        .value("LINEAR", Sequencer::LINEAR)
        .value("LOOP", Sequencer::LOOP)
        .value("PINGPONG", Sequencer::PINGPONG)
        .value("RANDOM", Sequencer::RANDOM);

    // Sequencer state enum values
    enum_<Sequencer::SequencerState>("SequencerState")
        .value("IDLE", Sequencer::IDLE)
        .value("RUNNING", Sequencer::RUNNING)
        .value("PAUSED", Sequencer::PAUSED)
        .value("COMPLETED", Sequencer::COMPLETED);

    // Trigger class
    class_<Trigger, base<Sprite>>("Trigger")
        .constructor<float, float>()
        .function("getModeInt", &Trigger::getModeInt)
        .function("setModeInt", &Trigger::setModeInt)
        .function("getStateInt", &Trigger::getStateInt)
        .function("setStateInt", &Trigger::setStateInt)
        .function("arm", &Trigger::arm)
        .function("disarm", &Trigger::disarm)
        .function("reset", &Trigger::reset)
        .function("unlatch", &Trigger::unlatch)
        .function("checkTrigger", &Trigger::checkTrigger)
        .function("getThreshold", &Trigger::getThreshold)
        .function("setThreshold", &Trigger::setThreshold)
        .function("getCurrentValue", &Trigger::getCurrentValue)
        .function("getTriggerCount", &Trigger::getTriggerCount)
        .function("getDebounceThreshold", &Trigger::getDebounceThreshold)
        .function("setDebounceThreshold", &Trigger::setDebounceThreshold)
        .function("latch", &Trigger::latch)
        .function("isArmed", &Trigger::isArmed)
        .function("isTriggered", &Trigger::isTriggered)
        .function("isLatched", &Trigger::isLatched);

    // Trigger mode enum values
    enum_<Trigger::TriggerMode>("TriggerMode")
        .value("EDGE_RISING", Trigger::EDGE_RISING)
        .value("EDGE_FALLING", Trigger::EDGE_FALLING)
        .value("EDGE_BOTH", Trigger::EDGE_BOTH)
        .value("LEVEL_HIGH", Trigger::LEVEL_HIGH)
        .value("LEVEL_LOW", Trigger::LEVEL_LOW);

    // Trigger state enum values
    enum_<Trigger::TriggerState>("TriggerState")
        .value("IDLE", Trigger::IDLE)
        .value("ARMED", Trigger::ARMED)
        .value("TRIGGERED", Trigger::TRIGGERED)
        .value("LATCHED", Trigger::LATCHED);

    // Scheduler class
    class_<Scheduler, base<Sprite>>("Scheduler")
        .constructor<float, float>()
        .function("getModeInt", &Scheduler::getModeInt)
        .function("setModeInt", &Scheduler::setModeInt)
        .function("getStateInt", &Scheduler::getStateInt)
        .function("setStateInt", &Scheduler::setStateInt)
        .function("start", &Scheduler::start)
        .function("stop", &Scheduler::stop)
        .function("pause", &Scheduler::pause)
        .function("resume", &Scheduler::resume)
        .function("update", &Scheduler::update)
        .function("execute", &Scheduler::execute)
        .function("finishExecution", &Scheduler::finishExecution)
        .function("getDelay", &Scheduler::getDelay)
        .function("setDelay", &Scheduler::setDelay)
        .function("getInterval", &Scheduler::getInterval)
        .function("setInterval", &Scheduler::setInterval)
        .function("getElapsedTime", &Scheduler::getElapsedTime)
        .function("getLastExecutionTime", &Scheduler::getLastExecutionTime)
        .function("getExecutionCount", &Scheduler::getExecutionCount)
        .function("getMaxExecutions", &Scheduler::getMaxExecutions)
        .function("setMaxExecutions", &Scheduler::setMaxExecutions)
        .function("getProgress", &Scheduler::getProgress)
        .function("isWaiting", &Scheduler::isWaiting)
        .function("isReady", &Scheduler::isReady)
        .function("isExecuting", &Scheduler::isExecuting);

    // Scheduler mode enum values
    enum_<Scheduler::SchedulerMode>("SchedulerMode")
        .value("ONCE", Scheduler::ONCE)
        .value("INTERVAL", Scheduler::INTERVAL)
        .value("CRON", Scheduler::CRON);

    // Scheduler state enum values
    enum_<Scheduler::SchedulerState>("SchedulerState")
        .value("IDLE", Scheduler::IDLE)
        .value("WAITING", Scheduler::WAITING)
        .value("READY", Scheduler::READY)
        .value("EXECUTING", Scheduler::EXECUTING);

    // CollisionManager class
    class_<CollisionManager>("CollisionManager")
        .constructor<float, float, int>()
        .function("addObject", &CollisionManager::addObject)
        .function("updateObject", &CollisionManager::updateObject)
        .function("removeObject", &CollisionManager::removeObject)
        .function("checkCollision", &CollisionManager::checkCollision)
        .function("queryPoint", &CollisionManager::queryPoint)
        .function("queryRegion", &CollisionManager::queryRegion)
        .function("rayCast", &CollisionManager::rayCast)
        .function("clear", &CollisionManager::clear)
        .function("getObjectCount", &CollisionManager::getObjectCount);

    // Register std::vector<int> for CollisionManager return types
    register_vector<int>("VectorInt");

    // AnimationController class
    class_<AnimationController>("AnimationController")
        .constructor<>()
        .function("animate", &AnimationController::animate)
        .function("update", &AnimationController::update)
        .function("getValue", &AnimationController::getValue)
        .function("isActive", &AnimationController::isActive)
        .function("stop", &AnimationController::stop)
        .function("clear", &AnimationController::clear)
        .function("getActiveCount", &AnimationController::getActiveCount);

    // AnimationController easing enum values
    enum_<AnimationController::EasingType>("EasingType")
        .value("LINEAR", AnimationController::LINEAR)
        .value("EASE_IN_QUAD", AnimationController::EASE_IN_QUAD)
        .value("EASE_OUT_QUAD", AnimationController::EASE_OUT_QUAD)
        .value("EASE_IN_OUT_QUAD", AnimationController::EASE_IN_OUT_QUAD)
        .value("EASE_IN_CUBIC", AnimationController::EASE_IN_CUBIC)
        .value("EASE_OUT_CUBIC", AnimationController::EASE_OUT_CUBIC)
        .value("EASE_IN_OUT_CUBIC", AnimationController::EASE_IN_OUT_CUBIC)
        .value("EASE_IN_ELASTIC", AnimationController::EASE_IN_ELASTIC)
        .value("EASE_OUT_ELASTIC", AnimationController::EASE_OUT_ELASTIC)
        .value("EASE_IN_BOUNCE", AnimationController::EASE_IN_BOUNCE)
        .value("EASE_OUT_BOUNCE", AnimationController::EASE_OUT_BOUNCE);

    // SceneManager class
    class_<SceneManager>("SceneManager")
        .constructor<float, float>()
        .function("setCameraPosition", &SceneManager::setCameraPosition)
        .function("getCameraX", &SceneManager::getCameraX)
        .function("getCameraY", &SceneManager::getCameraY)
        .function("setCameraZoom", &SceneManager::setCameraZoom)
        .function("getCameraZoom", &SceneManager::getCameraZoom)
        .function("moveCamera", &SceneManager::moveCamera)
        .function("isVisible", &SceneManager::isVisible)
        .function("setViewportSize", &SceneManager::setViewportSize)
        .function("getViewportWidth", &SceneManager::getViewportWidth)
        .function("getViewportHeight", &SceneManager::getViewportHeight);

    // SceneManager layer enum values
    enum_<SceneManager::LayerType>("LayerType")
        .value("BACKGROUND", SceneManager::BACKGROUND)
        .value("GAME", SceneManager::GAME)
        .value("UI", SceneManager::UI)
        .value("OVERLAY", SceneManager::OVERLAY);

    // EntityManager class
    class_<EntityManager>("EntityManager")
        .constructor<>()
        .function("createEntity", &EntityManager::createEntity)
        .function("destroyEntity", &EntityManager::destroyEntity)
        .function("hasEntity", &EntityManager::hasEntity)
        .function("getEntityType", &EntityManager::getEntityType)
        .function("setActive", &EntityManager::setActive)
        .function("isActive", &EntityManager::isActive)
        .function("setComponent", &EntityManager::setComponent)
        .function("getComponent", &EntityManager::getComponent)
        .function("hasComponent", &EntityManager::hasComponent)
        .function("getEntitiesByType", &EntityManager::getEntitiesByType)
        .function("getEntityCount", &EntityManager::getEntityCount)
        .function("clear", &EntityManager::clear);

    // InputManager class
    class_<InputManager>("InputManager")
        .constructor<>()
        .function("update", &InputManager::update)
        .function("onPointerDown", &InputManager::onPointerDown)
        .function("onPointerUp", &InputManager::onPointerUp)
        .function("getLastGesture", &InputManager::getLastGesture)
        .function("clearGesture", &InputManager::clearGesture)
        .function("recordCommand", &InputManager::recordCommand)
        .function("getCommands", &InputManager::getCommands)
        .function("clearCommands", &InputManager::clearCommands)
        .function("hasCommands", &InputManager::hasCommands)
        .function("getCommandCount", &InputManager::getCommandCount);

    // Register std::vector<std::string> for InputManager
    register_vector<std::string>("VectorString");

    // InputManager gesture enum values
    enum_<InputManager::GestureType>("GestureType")
        .value("NONE", InputManager::NONE)
        .value("TAP", InputManager::TAP)
        .value("DOUBLE_TAP", InputManager::DOUBLE_TAP)
        .value("SWIPE_LEFT", InputManager::SWIPE_LEFT)
        .value("SWIPE_RIGHT", InputManager::SWIPE_RIGHT)
        .value("SWIPE_UP", InputManager::SWIPE_UP)
        .value("SWIPE_DOWN", InputManager::SWIPE_DOWN)
        .value("PINCH_IN", InputManager::PINCH_IN)
        .value("PINCH_OUT", InputManager::PINCH_OUT);
}

/**
 * sprite.cpp - Basic sprite class for ToonTalk Web
 *
 * This is a simplified version ported from ../source/sprite.cpp
 * Eventually this will be expanded to match the full ToonTalk sprite system
 */

#include "sprite.h"
#include <emscripten/bind.h>
#include <string>
#include <cmath>

namespace toontalk {

// Sprite implementation
Sprite::Sprite(float x, float y, float width, float height)
    : x_(x), y_(y), width_(width), height_(height),
      rotation_(0.0f), visible_(true),
      dragging_(false), dragOffsetX_(0.0f), dragOffsetY_(0.0f) {}

// Position
float Sprite::getX() const { return x_; }
float Sprite::getY() const { return y_; }
void Sprite::setX(float x) { x_ = x; }
void Sprite::setY(float y) { y_ = y; }
void Sprite::setPosition(float x, float y) {
    x_ = x;
    y_ = y;
}

// Size
float Sprite::getWidth() const { return width_; }
float Sprite::getHeight() const { return height_; }
void Sprite::setWidth(float w) { width_ = w; }
void Sprite::setHeight(float h) { height_ = h; }

// Rotation
float Sprite::getRotation() const { return rotation_; }
void Sprite::setRotation(float r) { rotation_ = r; }
void Sprite::rotate(float dr) { rotation_ += dr; }

// Visibility
bool Sprite::isVisible() const { return visible_; }
void Sprite::setVisible(bool v) { visible_ = v; }

// Update method (called each frame)
void Sprite::update(float deltaTime) {
    // Override in subclasses
}

// Hit test
bool Sprite::containsPoint(float px, float py) const {
    return px >= x_ && px <= x_ + width_ &&
           py >= y_ && py <= y_ + height_;
}

// Drag methods
void Sprite::startDrag(float mouseX, float mouseY) {
    dragging_ = true;
    dragOffsetX_ = mouseX - x_;
    dragOffsetY_ = mouseY - y_;
}

void Sprite::updateDrag(float mouseX, float mouseY) {
    if (dragging_) {
        x_ = mouseX - dragOffsetX_;
        y_ = mouseY - dragOffsetY_;
    }
}

void Sprite::endDrag() {
    dragging_ = false;
}

bool Sprite::isDragging() const {
    return dragging_;
}

/**
 * Bird class - Example ToonTalk entity
 * In the full version, this will be ported from ../source/bird.cpp
 */
class Bird : public Sprite {
public:
    Bird(float x, float y)
        : Sprite(x, y, 60.0f, 60.0f), vx_(0.0f), vy_(0.0f) {}

    void setVelocity(float vx, float vy) {
        vx_ = vx;
        vy_ = vy;
    }

    void update(float deltaTime) override {
        // Simple physics
        x_ += vx_ * deltaTime;
        y_ += vy_ * deltaTime;

        // Simple animation - flap wings
        rotation_ = std::sin(x_ / 10.0f) * 0.1f;
    }

private:
    float vx_, vy_;  // Velocity
};

} // namespace toontalk

// Emscripten bindings - exposes C++ classes to JavaScript
using namespace emscripten;
using namespace toontalk;

EMSCRIPTEN_BINDINGS(toontalk_core) {
    class_<Sprite>("Sprite")
        .constructor<float, float, float, float>()
        .function("getX", &Sprite::getX)
        .function("getY", &Sprite::getY)
        .function("setX", &Sprite::setX)
        .function("setY", &Sprite::setY)
        .function("setPosition", &Sprite::setPosition)
        .function("getWidth", &Sprite::getWidth)
        .function("getHeight", &Sprite::getHeight)
        .function("setWidth", &Sprite::setWidth)
        .function("setHeight", &Sprite::setHeight)
        .function("getRotation", &Sprite::getRotation)
        .function("setRotation", &Sprite::setRotation)
        .function("rotate", &Sprite::rotate)
        .function("isVisible", &Sprite::isVisible)
        .function("setVisible", &Sprite::setVisible)
        .function("update", &Sprite::update)
        .function("containsPoint", &Sprite::containsPoint)
        .function("startDrag", &Sprite::startDrag)
        .function("updateDrag", &Sprite::updateDrag)
        .function("endDrag", &Sprite::endDrag)
        .function("isDragging", &Sprite::isDragging);

    class_<Bird, base<Sprite>>("Bird")
        .constructor<float, float>()
        .function("setVelocity", &Bird::setVelocity);
}

/**
 * sprite.h - Sprite class declarations for ToonTalk Web
 */

#ifndef TOONTALK_SPRITE_H
#define TOONTALK_SPRITE_H

namespace toontalk {

/**
 * Basic Sprite class
 * Represents a 2D object with position, size, and behavior
 */
class Sprite {
public:
    Sprite(float x, float y, float width, float height);
    virtual ~Sprite() {}

    // Unique ID
    int getId() const;

    // Position
    float getX() const;
    float getY() const;
    void setX(float x);
    void setY(float y);
    void setPosition(float x, float y);

    // Size
    float getWidth() const;
    float getHeight() const;
    void setWidth(float w);
    void setHeight(float h);

    // Rotation (in radians)
    float getRotation() const;
    void setRotation(float r);
    void rotate(float dr);

    // Visibility
    bool isVisible() const;
    void setVisible(bool v);

    // Update method (called each frame)
    virtual void update(float deltaTime);

    // Collision detection
    bool containsPoint(float px, float py) const;

    // Mouse interaction / dragging
    void startDrag(float mouseX, float mouseY);
    void updateDrag(float mouseX, float mouseY);
    void endDrag();
    bool isDragging() const;

protected:
    int id_;
    float x_, y_;
    float width_, height_;
    float rotation_;
    bool visible_;

    // Drag state
    bool dragging_;
    float dragOffsetX_, dragOffsetY_;

    // Static ID counter
    static int nextId_;
};

} // namespace toontalk

#endif // TOONTALK_SPRITE_H

// Minimal Number class implementation
// Based on original ToonTalk number.cpp but simplified for initial porting

#include "number.h"
#include "../platform/platform.h"
#include <string>
#include <sstream>
#include <iomanip>

// Original ToonTalk colors
const unsigned int NUMBER_BACKGROUND_COLOR = 0xB9FAC6;  // RGB(185,250,198) - light green
const unsigned int OPERATION_BACKGROUND_COLOR = 0xC3DC6C; // RGB(195,220,108) - yellowish green

Number::Number(double value, int x, int y, int width, int height, NumberOperation op)
    : value(value), operation(op), x(x), y(y), width(width), height(height),
      isDragging(false), dragOffsetX(0), dragOffsetY(0) {
}

void Number::display() {
    // Draw background plate using appropriate color
    unsigned int bg_color = getBackgroundColor();
    TTPlatform::drawRect(x, y, width, height, bg_color);

    // Draw border
    TTPlatform::drawRect(x, y, width, 4, 0x000000);                    // Top
    TTPlatform::drawRect(x, y + height - 4, width, 4, 0x000000);       // Bottom
    TTPlatform::drawRect(x, y, 4, height, 0x000000);                   // Left
    TTPlatform::drawRect(x + width - 4, y, 4, height, 0x000000);       // Right

    // Build text to display
    std::ostringstream oss;

    // If it's an operation, show the operation symbol
    if (operation != NO_NUMBER_OPERATION) {
        oss << getOperationSymbol();
        // If there's also a value, show it after the operation
        if (value != 0.0) {
            oss << " ";
        }
    }

    // Add the value (if non-zero or if it's a regular number)
    if (value != 0.0 || operation == NO_NUMBER_OPERATION) {
        // Format the number nicely
        if (value == (long)value) {
            // It's an integer, display without decimal point
            oss << (long)value;
        } else {
            // It's a decimal, show up to 6 decimal places
            std::ostringstream temp;
            temp << std::fixed << std::setprecision(6) << value;

            // Remove trailing zeros
            std::string str = temp.str();
            str.erase(str.find_last_not_of('0') + 1, std::string::npos);
            if (str.back() == '.') str.pop_back();
            oss << str;
        }
    }

    std::string text = oss.str();

    // Calculate font size to fit the text
    int padding = 20;
    int text_width = width - 2 * padding;
    int text_height = height - 2 * padding;

    // Estimate character width (rough approximation)
    int char_count = text.length();
    int font_size = text_height;

    // Adjust font size if text is too wide
    if (char_count > 0) {
        int estimated_width = font_size * char_count * 0.6;  // Rough approximation
        if (estimated_width > text_width) {
            font_size = text_width / (char_count * 0.6);
        }
    }

    // Don't make font too small
    if (font_size < 12) font_size = 12;

    // Calculate text position (centered)
    int text_x = x + padding;
    int text_y = y + (height / 2) + (font_size / 3);  // Vertically centered

    // Draw the text
    TTPlatform::drawText(text.c_str(), text_x, text_y, font_size, 0x000000);  // Black text
}

void Number::setValue(double new_value) {
    value = new_value;
}

double Number::getValue() const {
    return value;
}

void Number::setOperation(NumberOperation op) {
    operation = op;
}

NumberOperation Number::getOperation() const {
    return operation;
}

void Number::setPosition(int new_x, int new_y) {
    x = new_x;
    y = new_y;
}

void Number::setSize(int new_width, int new_height) {
    width = new_width;
    height = new_height;
}

const char* Number::getOperationSymbol() const {
    switch (operation) {
        case SUBTRACT_FROM:  return "-";
        case MULTIPLY_BY:    return "*";
        case INCREASE_BY:    return "+";
        case DIVIDE_BY:      return "/";
        case MODULUS_BY:     return "%";
        case MAKE_EQUAL:     return "=";
        default:             return "";
    }
}

unsigned int Number::getBackgroundColor() const {
    return (operation == NO_NUMBER_OPERATION) ? NUMBER_BACKGROUND_COLOR : OPERATION_BACKGROUND_COLOR;
}

void Number::applyOperation(NumberOperation op, double operand) {
    switch (op) {
        case INCREASE_BY:
            value += operand;
            break;
        case SUBTRACT_FROM:
            value -= operand;
            break;
        case MULTIPLY_BY:
            value *= operand;
            break;
        case DIVIDE_BY:
            if (operand != 0.0) {
                value /= operand;
            }
            // Note: in original ToonTalk, division by zero is handled specially
            break;
        case MODULUS_BY:
            if (operand != 0.0) {
                value = (long)value % (long)operand;
            }
            break;
        case MAKE_EQUAL:
            value = operand;
            break;
        case NO_NUMBER_OPERATION:
            // No operation to apply
            break;
    }
}

double Number::applyTo(double target) const {
    // If this number is an operation, apply it to the target
    if (operation == NO_NUMBER_OPERATION) {
        return target;  // Not an operation, return target unchanged
    }

    switch (operation) {
        case INCREASE_BY:
            return target + value;
        case SUBTRACT_FROM:
            return target - value;
        case MULTIPLY_BY:
            return target * value;
        case DIVIDE_BY:
            return (value != 0.0) ? target / value : target;
        case MODULUS_BY:
            return (value != 0.0) ? (long)target % (long)value : target;
        case MAKE_EQUAL:
            return value;
        default:
            return target;
    }
}

void Number::startDrag(int mouseX, int mouseY) {
    isDragging = true;
    // Store offset from the object's top-left corner to where the mouse clicked
    dragOffsetX = mouseX - x;
    dragOffsetY = mouseY - y;
}

void Number::updateDrag(int mouseX, int mouseY) {
    if (isDragging) {
        // Update position, accounting for the drag offset
        x = mouseX - dragOffsetX;
        y = mouseY - dragOffsetY;
    }
}

void Number::endDrag() {
    isDragging = false;
}

bool Number::getDragging() const {
    return isDragging;
}

bool Number::containsPoint(int pointX, int pointY) const {
    return (pointX >= x && pointX <= x + width &&
            pointY >= y && pointY <= y + height);
}

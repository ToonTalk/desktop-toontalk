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
    : value(value), operation(op), x(x), y(y), width(width), height(height) {
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

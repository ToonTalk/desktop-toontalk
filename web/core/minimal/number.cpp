// Minimal Number class implementation
// Based on original ToonTalk number.cpp but simplified for initial porting

#include "number.h"
#include "../platform/platform.h"
#include <string>
#include <sstream>
#include <iomanip>

Number::Number(double value, int x, int y, int width, int height)
    : value(value), x(x), y(y), width(width), height(height) {
}

void Number::display() {
    // Draw background plate (yellow for numbers in original ToonTalk)
    TTPlatform::drawRect(x, y, width, height, 0xFFFFCC);  // Light yellow

    // Draw border
    TTPlatform::drawRect(x, y, width, 4, 0x000000);                    // Top
    TTPlatform::drawRect(x, y + height - 4, width, 4, 0x000000);       // Bottom
    TTPlatform::drawRect(x, y, 4, height, 0x000000);                   // Left
    TTPlatform::drawRect(x + width - 4, y, 4, height, 0x000000);       // Right

    // Convert value to string
    std::ostringstream oss;

    // Format the number nicely
    if (value == (long)value) {
        // It's an integer, display without decimal point
        oss << (long)value;
    } else {
        // It's a decimal, show up to 6 decimal places
        oss << std::fixed << std::setprecision(6) << value;

        // Remove trailing zeros
        std::string str = oss.str();
        str.erase(str.find_last_not_of('0') + 1, std::string::npos);
        if (str.back() == '.') str.pop_back();
        oss.str(str);
        oss.clear();
        oss << str;
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

void Number::setPosition(int new_x, int new_y) {
    x = new_x;
    y = new_y;
}

void Number::setSize(int new_width, int new_height) {
    width = new_width;
    height = new_height;
}

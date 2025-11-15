// Minimal Number class for web port
// Based on original ToonTalk number.cpp but simplified for initial porting

#ifndef __MINIMAL_NUMBER_H
#define __MINIMAL_NUMBER_H

class Number {
public:
    // Constructor
    Number(double value = 0.0, int x = 0, int y = 0, int width = 200, int height = 100);

    // Display the number
    void display();

    // Set/get value
    void setValue(double new_value);
    double getValue() const;

    // Set position and size
    void setPosition(int new_x, int new_y);
    void setSize(int new_width, int new_height);

private:
    double value;
    int x, y;           // Position
    int width, height;  // Size
};

#endif // __MINIMAL_NUMBER_H

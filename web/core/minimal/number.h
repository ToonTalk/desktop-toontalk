// Minimal Number class for web port
// Based on original ToonTalk number.cpp but simplified for initial porting

#ifndef __MINIMAL_NUMBER_H
#define __MINIMAL_NUMBER_H

// NumberOperation enum from original ToonTalk (simplified subset)
enum NumberOperation {
    SUBTRACT_FROM = 0,    // -
    MULTIPLY_BY,          // *
    INCREASE_BY,          // +
    DIVIDE_BY,            // /
    MODULUS_BY,           // %
    MAKE_EQUAL,           // =
    NO_NUMBER_OPERATION   // Regular number (not an operation)
};

class Number {
public:
    // Constructor
    Number(double value = 0.0, int x = 0, int y = 0, int width = 200, int height = 100,
           NumberOperation op = NO_NUMBER_OPERATION);

    // Display the number
    void display();

    // Set/get value
    void setValue(double new_value);
    double getValue() const;

    // Set/get operation
    void setOperation(NumberOperation op);
    NumberOperation getOperation() const;

    // Set position and size
    void setPosition(int new_x, int new_y);
    void setSize(int new_width, int new_height);

private:
    double value;
    NumberOperation operation;
    int x, y;           // Position
    int width, height;  // Size

    // Helper to get operation symbol
    const char* getOperationSymbol() const;

    // Helper to get appropriate background color
    unsigned int getBackgroundColor() const;
};

#endif // __MINIMAL_NUMBER_H

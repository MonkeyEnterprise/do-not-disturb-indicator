# DigitalInput Library

A robust, debounced digital input class for Arduino with edge detection, invert logic, active duration measurement, and millis() overflow protection.

## Features

- Debounces digital inputs with configurable debounce time
- Detects rising and falling edges
- Handles millis() overflow safely
- Supports internal pull-up resistors
- Optional invert logic (active LOW or HIGH)
- Measures how long the input has been active

## API

- `DigitalInput(const int pin, const int debounceMs = 30, const bool pullup = true, const bool invert = false)`
- `void update()`
- `bool read() const`
- `bool rose() const`
- `bool fell() const`
- `unsigned long activeDuration() const`

# DigitalOutput Library

A simple, robust digital output class for Arduino with toggle, PWM, and non-blocking blink functionality.

## Features

- Simple HIGH/LOW output control
- PWM output support
- Toggle output state
- Non-blocking blink with configurable interval
- Easy to use in any Arduino sketch

## API

- `DigitalOutput(const int pin, const bool initialState = LOW)`
- `void write(const bool state)`
- `void write(const int pwm)`
- `bool read() const`
- `void toggle()`
- `void enableBlink(const unsigned long intervalMs)`
- `void disableBlink()`
- `void update()`

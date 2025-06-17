/**
 * @file DigitalOutput.h
 * @brief Header file for the DigitalOutput class, providing non-blocking digital output control with PWM and blinking functionality.
 *
 * @copyright (c) 2025 Lorenzo Pouw, PouwerTronics/MonkeyEnterprise.
 * Licensed under the BSD license. This notice must remain in all redistributions.
 */

#ifndef __DIGITAL_OUTPUT_H__
#define __DIGITAL_OUTPUT_H__

#include <Arduino.h>

/**
 * @class DigitalOutput
 * @brief Simple digital output control with optional toggle and blink functionality.
 */
class DigitalOutput
{
public:
    /**
     * @brief Construct a DigitalOutput object.
     * @param pin The Arduino digital pin number.
     * @param initialState Initial output state (HIGH or LOW).
     */
    DigitalOutput(const int pin, const bool initialState = LOW);

    /**
     * @brief Set the output state.
     * @param state true for HIGH, false for LOW.
     */
    void write(const bool state);

    /**
     * @brief Set the output state using PWM value (0-255).
     * @param pwm PWM value (0 for LOW, 255 for HIGH).
     */
    void write(const int pwm);

    /**
     * @brief Get the current output state.
     * @return true if HIGH, false if LOW.
     */
    bool read() const;

    /**
     * @brief Toggle the output state.
     */
    void toggle();

    /**
     * @brief Enable blinking functionality.
     * @param intervalMs Blink interval in milliseconds.
     */
    void enableBlink(const unsigned long intervalMs);

    /**
     * @brief Disable blinking functionality.
     */
    void disableBlink();

    /**
     * @brief Call this in loop() to update blink/toggle state.
     */
    void update();

private:
    uint8_t _pin;              ///< The Arduino digital pin number.
    bool _state;               ///< Current output state (HIGH or LOW).
    bool _blinking;            ///< Whether the output is blinking.
    unsigned long _interval;   ///< Blink interval in milliseconds.
    unsigned long _lastToggle; ///< Last time the output was toggled.
};

#endif // __DIGITAL_OUTPUT_H__

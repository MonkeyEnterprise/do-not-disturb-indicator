/**
 ******************************************************************************
 * @file    DigitalInput.h
 * @version 1.1.0
 * @date    2025-05-29
 * @brief   Header file for the DigitalInput class, providing debounced digital input handling,
 *          edge detection, invert logic, and active duration measurement.
 ******************************************************************************
 */

#pragma once
#ifndef __DIGITALINPUT_H__
#define __DIGITALINPUT_H__

#include <Arduino.h>

/**
 * @class DigitalInput
 * @brief Debounced digital input with edge detection, invert logic, and millis() overflow protection.
 */
class DigitalInput
{
public:
    /**
     * @brief Construct a DigitalInput object.
     * @param pin The Arduino digital pin number.
     * @param debounceMs Debounce time in milliseconds.
     * @param pullup Enable internal pull-up resistor if true.
     * @param invert Invert logic if true (active LOW).
     */
    DigitalInput(uint8_t pin, uint16_t debounceMs = 30, bool pullup = true, bool invert = false);

    /**
     * @brief Update the input state. Call this frequently (e.g. every loop).
     */
    void update();

    /**
     * @brief Get the current debounced state (active/inactive).
     * @return true if active, false if inactive.
     */
    bool read() const;

    /**
     * @brief Returns true if the input transitioned from inactive to active.
     * @return true on rising edge, false otherwise.
     */
    bool rose() const;

    /**
     * @brief Returns true if the input transitioned from active to inactive.
     * @return true on falling edge, false otherwise.
     */
    bool fell() const;

    /**
     * @brief Get the duration (in ms) the input has been active (HIGH or LOW, depending on invert).
     *        Returns 0 if not currently active.
     *        Handles millis() overflow safely.
     * @return Duration in milliseconds.
     */
    unsigned long activeDuration() const;

private:
    uint8_t _pin;
    uint16_t _debounceMs;
    bool _pullup;
    bool _invert;
    bool _state;
    bool _lastState;
    bool _rose;
    bool _fell;
    uint32_t _lastChangeTime;
    bool _rawLast;
    uint32_t _activeStart;
};

#endif // __DIGITALINPUT_H__

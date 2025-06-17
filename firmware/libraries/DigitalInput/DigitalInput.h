/**
 * @file DigitalInput.h
 * @brief Header file for debounced digital input handling.
 *
 * @copyright (c) 2025 Lorenzo Pouw, PouwerTronics/MonkeyEnterprise.
 * Licensed under the BSD license. This notice must remain in all redistributions.
 */

#ifndef __DIGITAL_INPUT_H__
#define __DIGITAL_INPUT_H__

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
    DigitalInput(const int pin, const unsigned long debounceMs = 30, const bool pullup = true, const bool invert = false);

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
    int _pin;                      ///< The Arduino digital pin number.
    unsigned long _debounceMs;               ///< Debounce time in milliseconds.
    bool _pullup;                  ///< Enable internal pull-up resistor if true.
    bool _invert;                  ///< Invert logic if true (active LOW).
    bool _state;                   ///< Current debounced state (active/inactive).
    bool _lastState;               ///< Last debounced state (active/inactive).
    bool _rose;                    ///< True if the input transitioned from inactive to active.
    bool _fell;                    ///< True if the input transitioned from active to inactive.
    unsigned long _lastChangeTime; ///< Last time the input state changed (in milliseconds).
    bool _rawLast;                 ///< Last raw state read from the pin.
    unsigned long _activeStart;    ///< Time when the input became active (in milliseconds).
};

#endif // __DIGITAL_INPUT_H__

/**
 ******************************************************************************
 * @file    DigitalInput.cpp
 * @date    22-02-2025
 * @version 1.0.0
 * @brief   Library for reading digital inputs with optional pull-up and inversion functionality.
 ******************************************************************************
 */

#include "DigitalInput.h"

/**
 * @brief Construct a DigitalInput object.
 * @param pin The Arduino digital pin number.
 * @param debounceMs Debounce time in milliseconds (default is 30 ms).
 * @param pullup Enable internal pull-up resistor if true (default is true).
 * @param invert Invert the input signal if true (default is false).
 */
DigitalInput::DigitalInput(uint8_t pin, uint16_t debounceMs, bool pullup, bool invert)
    : _pin(pin), _debounceMs(debounceMs), _pullup(pullup), _invert(invert),
      _state(false), _lastState(false), _rose(false), _fell(false),
      _lastChangeTime(0), _rawLast(false), _activeStart(0)
{
    if (_pullup)
        pinMode(_pin, INPUT_PULLUP);
    else
        pinMode(_pin, INPUT);

    bool raw = digitalRead(_pin);
    if (_invert)
        raw = !raw;
    _state = raw;
    _lastState = _state;
    _rawLast = raw;
    if (_state)
        _activeStart = millis();
}

/**
 * @brief Update the input state. Call this frequently.
 */
void DigitalInput::update()
{
    bool raw = digitalRead(_pin);
    if (_invert)
        raw = !raw;
    uint32_t now = millis();

    _rose = false;
    _fell = false;

    if (raw != _rawLast)
    {
        _lastChangeTime = now;
        _rawLast = raw;
    }

    if ((uint32_t)(now - _lastChangeTime) >= _debounceMs)
    {
        if (raw != _state)
        {
            _lastState = _state;
            _state = raw;
            if (_state && !_lastState)
            {
                _rose = true;
                _activeStart = now;
            }
            if (!_state && _lastState)
            {
                _fell = true;
            }
        }
    }
}

/**
 * @brief Get the current debounced state.
 */
bool DigitalInput::read() const
{
    return _state;
}

/**
 * @brief Returns true if the input transitioned from LOW to HIGH.
 */
bool DigitalInput::rose() const
{
    return _rose;
}

/**
 * @brief Returns true if the input transitioned from HIGH to LOW.
 */
bool DigitalInput::fell() const
{
    return _fell;
}

/**
 * @brief Returns the duration for which the input has been active.
 */
unsigned long DigitalInput::activeDuration() const
{
    if (_state)
    {
        return (uint32_t)(millis() - _activeStart);
    }
    return 0;
}

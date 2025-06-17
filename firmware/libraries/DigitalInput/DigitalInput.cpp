/**
 * @file DigitalInput.cpp
 * @brief Source file for debounced digital input handling.
 *
 * @copyright (c) 2025 Lorenzo Pouw, PouwerTronics/MonkeyEnterprise.
 * Licensed under the BSD license. This notice must remain in all redistributions.
 */

#include "DigitalInput.h"

DigitalInput::DigitalInput(const int pin, const unsigned long debounceMs, const bool pullup, const bool invert)
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

void DigitalInput::update()
{
    bool raw = digitalRead(_pin);
    if (_invert)
        raw = !raw;
    unsigned long now = millis();

    _rose = false;
    _fell = false;

    if (raw != _rawLast)
    {
        _lastChangeTime = now;
        _rawLast = raw;
    }

    if ((now - _lastChangeTime) >= _debounceMs)
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

bool DigitalInput::read() const { return _state; }
bool DigitalInput::rose() const { return _rose; }
bool DigitalInput::fell() const { return _fell; }

unsigned long DigitalInput::activeDuration() const
{
    if (_state)
    {
        return millis() - _activeStart;
    }
    return 0;
}

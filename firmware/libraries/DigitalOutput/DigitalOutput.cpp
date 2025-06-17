/**
 * @file DigitalOutput.cpp
 * @brief Main source file for the DigitalOutput class, providing non-blocking digital output control with PWM and blinking functionality.
 *
 * @copyright (c) 2025 Lorenzo Pouw, PouwerTronics/MonkeyEnterprise.
 * Licensed under the BSD license. This notice must remain in all redistributions.
 */

#include "DigitalOutput.h"

DigitalOutput::DigitalOutput(const int pin, const bool initialState)
    : _pin(pin), _state(initialState), _blinking(false), _interval(0), _lastToggle(0)
{
    pinMode(_pin, OUTPUT);
    digitalWrite(_pin, _state ? HIGH : LOW);
}

void DigitalOutput::write(const bool state)
{
    _state = state;
    digitalWrite(_pin, _state ? HIGH : LOW);
    _blinking = false;
}

void DigitalOutput::write(const int pwm)
{
    const uint8_t value = constrain(pwm, 0, 255);
    analogWrite(_pin, value);
    _state = (pwm > 0);
}

bool DigitalOutput::read() const { return _state; }
void DigitalOutput::toggle()
{
    _state = !_state;
    digitalWrite(_pin, _state ? HIGH : LOW);
}

void DigitalOutput::enableBlink(const unsigned long intervalMs)
{
    _blinking = true;
    _interval = intervalMs;
    _lastToggle = millis();
}

void DigitalOutput::disableBlink() { _blinking = false; };

void DigitalOutput::update()
{
    if (_blinking)
    {
        unsigned long now = millis();
        if ((unsigned long)(now - _lastToggle) >= _interval)
        {
            this->toggle();
            _lastToggle = now;
        }
    }
}

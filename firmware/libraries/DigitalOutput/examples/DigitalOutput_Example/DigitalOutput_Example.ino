/**
 * @file DigitalOutput_Example.ino
 * @brief Example sketch for using the DigitalOutput library.
 *
 * @copyright (c) 2025 Lorenzo Pouw, PouwerTronics/MonkeyEnterprise.
 * Licensed under the BSD license. This notice must remain in all redistributions.
 */

#include <DigitalOutput.h>

#define LED_PIN LED_BUILTIN           // Use the built-in LED pin
#define LED_INITIAL_STATE LOW         // Initial state of the LED (LOW means off)
#define LED_BLINK_INTERVAL 1000       // Blink interval in milliseconds
#define ANALOG_OUT_PIN 9              // Pin for PWM output
#define ANALOG_OUT_INITIAL_STATE HIGH // Initial state for PWM output

DigitalOutput Led(LED_PIN, LED_INITIAL_STATE);
DigitalOutput AnalogOut(ANALOG_OUT_PIN, ANALOG_OUT_INITIAL_STATE);

/**
 * @brief Setup function.
 * This function initializes the serial communication and starts the LED blinking.
 */
void setup()
{
    Led.enableBlink(LED_BLINK_INTERVAL);
}

/**
 * @brief Main loop function.
 * This function updates the LED state and gradually increases and decreases the PWM value.
 */
void loop()
{

    for (int i = 0; i <= 255; i += 5)
    {
        AnalogOut.write(i);
        delay(50);
    }
    for (int i = 255; i >= 0; i -= 5)
    {
        AnalogOut.write(i);
        delay(50);
    }

    Led.update();
}
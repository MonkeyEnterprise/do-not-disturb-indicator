/**
 * @file DigitalInput_Example.ino
 * @brief Example sketch for using the DigitalInput library.
 *
 * @copyright (c) 2025 Lorenzo Pouw, PouwerTronics/MonkeyEnterprise.
 * Licensed under the BSD license. This notice must remain in all redistributions.
 */

#include <DigitalInput.h>

#define INPUT_PIN 2       // Pin number for the digital input
#define DEBOUNCE_MS 50    // Debounce time in milliseconds
#define PULLUP true       // Set to true to enable internal pull-up resistor
#define INVERT_INPUT true // Set to true to invert logic (active LOW)
#define TIMER_TASK 1000 // Timer task interval in milliseconds

DigitalInput input(INPUT_PIN, DEBOUNCE_MS, PULLUP, INVERT_INPUT);

/**
 * @brief Setup function to initialize the serial communication and input.
 */
void setup()
{
  Serial.begin(115200);
  Serial.println("Digital Input Example");
}

/**
 * @brief Main loop function that continuously checks the input state and prints it.
 */
void loop()
{
  input.update();

  Serial.print("Input State: ");
  Serial.println(input.read() ? "ACTIVE" : "INACTIVE");

  if (input.rose())
    Serial.println("Input became ACTIVE!");
  if (input.fell())
  {
    Serial.print("Input became INACTIVE! Active duration: ");
    Serial.print(input.activeDuration());
    Serial.println(" ms");
  }

  delay(TIMER_TASK);
}
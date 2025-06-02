/**
 ******************************************************************************
 * @file    DigitalInput_Example.ino
 * @date    22-02-2025
 * @version 1.1.0
 * @brief   Example sketch demonstrating the use of the DigitalInput library,
 *          including invert logic and active duration measurement.
 ******************************************************************************
 */

#include <DigitalInput.h>

#define INPUT_PIN 2
#define DEBOUNCE_MS 50
#define INVERT_INPUT true // Set to true to invert logic (active LOW)

DigitalInput input(INPUT_PIN, DEBOUNCE_MS, true, INVERT_INPUT);

void setup()
{
  Serial.begin(115200);
  Serial.println("Digital Input Example");
}

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

  delay(1000); // Adjust as needed
}
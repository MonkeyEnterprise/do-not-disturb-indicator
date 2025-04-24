#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

// Define the pin and number of LEDs
#define LED_PIN 6
#define LED_COUNT 8

// Define the start and stop bytes for the command protocol
#define START_BYTE 0xFF
#define STOP_BYTE 0xFF

// Define the response codes for the command protocol
#define RESPONSE_OK 0xAA
#define RESPONSE_ERR_START 0xBB
#define RESPONSE_ERR_STOP 0xCC
#define RESPONSE_ERR_CHECKSUM 0xDD

// Define other constants
#define BAUD_RATE 115200
#define RAINBOW_DELAY 250 // Delay for rainbow effect in microseconds
#define CMD_LENGTH 7 // Start + R + G + B + Brightness + Checksum + Stop

// Create an instance of the NeoPixel strip
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

/**
 * @brief Calculate the checksum for the command.
 * @param r Red value (0-255)
 * @param g Green value (0-255)
 * @param b Blue value (0-255)
 * @param brightness Brightness value (0-255)
 * @return The calculated checksum byte.
 */
uint8_t calculateChecksum(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t brightness)
{
  uint16_t sum = r + g + b + brightness;
  return sum % 256;
}

/**
 * @brief Validate the checksum of the received data.
 * @param data Pointer to the data array.
 * @return true if checksum is valid, false otherwise.
 */
bool validateChecksum(const uint8_t *data)
{
  uint16_t sum = 0;
  for (int i = 1; i < 5; ++i) // Sum R, G, B, Brightness (index 1 to 4)
    sum += data[i];
  return (sum % 256) == data[5];
}

/**
 * @brief Flush the serial buffer.
 */
void flushSerial()
{
  while (Serial.available() > 0)
    Serial.read();
}

/**
 * @brief Fill the strip with a color.
 * @param color The color to fill the strip with (RGB format).
 * @param wait Delay time between each pixel (in microseconds).
 */
void rainbow(int wait)
{
  for (long firstPixelHue = 0; firstPixelHue < 5 * 65536; firstPixelHue += 256)
  {
    strip.rainbow(firstPixelHue);
    strip.show();
    delayMicroseconds(wait);
  }
  strip.clear(); // Clear the strip after the rainbow effect
  strip.show();  // Update strip to clear it
}

/**
 * @brief Go to a new color.
 * @param red Red value (0-255)
 * @param green Green value (0-255)
 * @param blue Blue value (0-255)
 * @param brightness Brightness value (0-255)
 */
void setColor(int red, int green, int blue, int brightness)
{
  strip.clear();
  strip.setBrightness(brightness);
  strip.fill(strip.Color(red, green, blue), 0, LED_COUNT);
  strip.show();
}

/**
 * @brief Setup function to initialize the serial port and LED strip.
 */
void setup()
{
  Serial.begin(BAUD_RATE);
  strip.begin();
  rainbow(RAINBOW_DELAY);
}

/**
 * @brief Main loop to read commands from the serial port and set LED colors.
 */
void loop()
{
  if (Serial.available() >= CMD_LENGTH)
  {
    uint8_t buffer[CMD_LENGTH];
    Serial.readBytes(buffer, CMD_LENGTH);

    const int startByte = buffer[0];
    const int red = buffer[1];
    const int green = buffer[2];
    const int blue = buffer[3];
    const int brightness = buffer[4];
    const int stopByte = buffer[CMD_LENGTH - 1];

    // Check Start Byte
    if (startByte != START_BYTE)
    {
      Serial.write(RESPONSE_ERR_START);
      flushSerial();
      return;
    }

    // Check Stop Byte
    if (stopByte != STOP_BYTE)
    {
      Serial.write(RESPONSE_ERR_STOP);
      flushSerial();
      return;
    }

    // Validate Checksum
    if (!validateChecksum(buffer))
    {
      Serial.write(RESPONSE_ERR_CHECKSUM);
      flushSerial();
      return;
    }
    // Fade to the new color
    setColor(red, green, blue, brightness);

    // Send OK response
    Serial.write(RESPONSE_OK);
  }
}

#include <Arduino.h>
#include <SoftwareSerial.h>
#include <Adafruit_NeoPixel.h>

// Define the pins for SoftwareSerial communication
#define RX_PIN PB0 // Pin for receiving data
#define TX_PIN PB1 // Pin for transmitting data

// Define the pin and number of LEDs
#define LED_PIN PB2 // Pin connected to the NeoPixel data input
#define LED_COUNT 1 // Number of LEDs in the NeoPixel strip

// Define the start and stop bytes for the command protocol
#define START_BYTE 0xFF // Start byte for the command protocol
#define STOP_BYTE 0xFF  // Stop byte for the command protocol

// Define the response codes for the command protocol
#define RESPONSE_OK 0xAA           // OK response
#define RESPONSE_ERR_START 0xBB    // Start byte error response
#define RESPONSE_ERR_STOP 0xCC     // Stop byte error response
#define RESPONSE_ERR_CHECKSUM 0xDD // Checksum error response

// Define other constants
#define BAUD_RATE 9600    // Baud rate for SoftwareSerial
#define RAINBOW_DELAY 800 // Delay for rainbow effect in microseconds
#define CMD_LENGTH 7      // Start + R + G + B + Brightness + Checksum + Stop

// Create an instance of the NeoPixel Strip
Adafruit_NeoPixel Strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

// Create an instance of SoftwareSerial for communication
SoftwareSerial SoftSerial(RX_PIN, TX_PIN);

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
  while (SoftSerial.available() > 0)
    SoftSerial.read();
}

/**
 * @brief Fill the Strip with a color.
 * @param color The color to fill the Strip with (RGB format).
 * @param wait Delay time between each pixel (in microseconds).
 */
void rainbow(int wait)
{
  for (long firstPixelHue = 0; firstPixelHue < 5 * 65536; firstPixelHue += 256)
  {
    Strip.rainbow(firstPixelHue);
    Strip.show();
    delayMicroseconds(wait);
  }
  Strip.clear(); // Clear the Strip after the rainbow effect
  Strip.show();  // Update Strip to clear it
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
  Strip.clear();
  Strip.setBrightness(brightness);
  Strip.fill(Strip.Color(red, green, blue), 0, LED_COUNT);
  Strip.show();
}

/**
 * @brief Setup function to initialize the serial port and LED Strip.
 */
void setup()
{
  SoftSerial.begin(BAUD_RATE);
  Strip.begin();
  rainbow(RAINBOW_DELAY);
}

/**
 * @brief Main loop to read commands from the serial port and set LED colors.
 */
void loop()
{
  if (SoftSerial.available() >= CMD_LENGTH)
  {
    uint8_t buffer[CMD_LENGTH];
    SoftSerial.readBytes(buffer, CMD_LENGTH);

    const int startByte = buffer[0];
    const int red = buffer[1];
    const int green = buffer[2];
    const int blue = buffer[3];
    const int brightness = buffer[4];
    const int stopByte = buffer[CMD_LENGTH - 1];

    // Check Start Byte
    if (startByte != START_BYTE)
    {
      SoftSerial.write(RESPONSE_ERR_START);
      flushSerial();
      return;
    }

    // Check Stop Byte
    if (stopByte != STOP_BYTE)
    {
      SoftSerial.write(RESPONSE_ERR_STOP);
      flushSerial();
      return;
    }

    // Validate Checksum
    if (!validateChecksum(buffer))
    {
      SoftSerial.write(RESPONSE_ERR_CHECKSUM);
      flushSerial();
      return;
    }
    // Fade to the new color
    setColor(red, green, blue, brightness);

    // Send OK response
    SoftSerial.write(RESPONSE_OK);
  }
}

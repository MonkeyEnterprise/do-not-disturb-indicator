#include <Arduino.h>
#include <Registers.h>
#include <SoftwareSerial.h>
#include <Adafruit_NeoPixel.h>

#include "Configuration.h"

// Create an instance of the NeoPixel pixels
Adafruit_NeoPixel pixels(LED_COUNT, LED_PIN, LED_TYPE);

// Create an instance of SoftwareSerial for communication
SoftwareSerial softSerial(RX_PIN, TX_PIN); // RX, TX pins for SoftwareSerial

// Register Instances for Mode Data (each mode has its color and brightness)
Register regCurrentMode(0);                                                            // Register to store current mode (0-2)
Register regFadeTime(1);                                                               // Register to store fade time
Register regRedMode1(10), regGreenMode1(11), regBlueMode1(12), regBrightnessMode1(13); // Registers for mode 1 color and brightness
Register regRedMode2(20), regGreenMode2(21), regBlueMode2(22), regBrightnessMode2(23); // Registers for mode 2 color and brightness
Register regRedMode3(30), regGreenMode3(31), regBlueMode3(32), regBrightnessMode3(33); // Registers for mode 3 color and brightness

// Global variables for mode state
uint8_t currentMode = 0;
uint8_t currentRed = 0;
uint8_t currentGreen = 0;
uint8_t currentBlue = 0;
uint8_t currentBrightness = 0;
uint16_t currentFadeTime = 0;

/**
 * @brief Store the current mode data to the registers.
 */
void storeModeData()
{
  regCurrentMode.save<uint8_t>(currentMode);
  regFadeTime.save<uint16_t>(currentFadeTime);

  switch (currentMode)
  {
  case 1:
    regRedMode1.save<uint8_t>(currentRed);
    regGreenMode1.save<uint8_t>(currentGreen);
    regBlueMode1.save<uint8_t>(currentBlue);
    regBrightnessMode1.save<uint8_t>(currentBrightness);
    break;

  case 2:
    regRedMode2.save<uint8_t>(currentRed);
    regGreenMode2.save<uint8_t>(currentGreen);
    regBlueMode2.save<uint8_t>(currentBlue);
    regBrightnessMode2.save<uint8_t>(currentBrightness);
    break;

  case 3:
    regRedMode3.save<uint8_t>(currentRed);
    regGreenMode3.save<uint8_t>(currentGreen);
    regBlueMode3.save<uint8_t>(currentBlue);
    regBrightnessMode3.save<uint8_t>(currentBrightness);
    break;

  default:
    // Do nothing for mode 0, as it is the default state
    break;
  }
}

/**
 * @brief Change to a new mode based on currentMode value.
 */
void loadModeData()
{
  currentMode = regCurrentMode.load<uint8_t>();
  currentFadeTime = regFadeTime.load<uint16_t>();

  switch (currentMode)
  {
  case 1:
    currentRed = regRedMode1.load<uint8_t>();
    currentGreen = regGreenMode1.load<uint8_t>();
    currentBlue = regBlueMode1.load<uint8_t>();
    currentBrightness = regBrightnessMode1.load<uint8_t>();
    break;

  case 2:
    currentRed = regRedMode2.load<uint8_t>();
    currentGreen = regGreenMode2.load<uint8_t>();
    currentBlue = regBlueMode2.load<uint8_t>();
    currentBrightness = regBrightnessMode2.load<uint8_t>();
    break;

  case 3:
    currentRed = regRedMode3.load<uint8_t>();
    currentGreen = regGreenMode3.load<uint8_t>();
    currentBlue = regBlueMode3.load<uint8_t>();
    currentBrightness = regBrightnessMode3.load<uint8_t>();
    break;

  default:
    // Nothing to do for mode 0, as it is the default state
    break;
  }
}

/**
 * @brief Smoothly fade to a new color.
 * @param targetRed Target red value (0-255)
 * @param targetGreen Target green value (0-255)
 * @param targetBlue Target blue value (0-255)
 * @param targetBrightness Target brightness value (0-255)
 * @param fadeTime The total fade time in milliseconds
 */
void fadeToColor(uint8_t targetRed, uint8_t targetGreen, uint8_t targetBlue, uint8_t targetBrightness, uint16_t fadeTime)
{

  // Calculate the number of steps based on fadeTime and a fixed delay per step
  const int steps = fadeTime / 10; // 10 milliseconds per step
  for (int i = 0; i <= steps; ++i)
  {
    float t = (float)i / steps;

    uint8_t r = currentRed + (targetRed - currentRed) * t;
    uint8_t g = currentGreen + (targetGreen - currentGreen) * t;
    uint8_t b = currentBlue + (targetBlue - currentBlue) * t;
    uint8_t brightness = currentBrightness + (targetBrightness - currentBrightness) * t;

    pixels.clear();
    pixels.setBrightness(brightness);
    pixels.fill(pixels.Color(r, g, b), 0, LED_COUNT);
    pixels.show();

    delay(10); // Delay between each step
  }

  // Save the new state to global variables
  currentMode = (currentMode == 0) ? 1 : currentMode; // Ensure mode is not 0 after fade
  currentRed = targetRed;
  currentGreen = targetGreen;
  currentBlue = targetBlue;
  currentBrightness = targetBrightness;
  currentFadeTime = fadeTime;
}

/**
 * @brief Validate the checksum of the received data.
 * @param data Pointer to the data array.
 * @return true if checksum is valid, false otherwise.
 */
bool validateChecksum(const uint8_t *data, uint16_t checksum)
{
  uint16_t sum = 0;
  for (int i = IDX_RED_BYTE; i <= IDX_BRIGHTNESS_BYTE; ++i) // Calculate checksum from red, green, blue, brightness bytes
    sum += data[i];

  return (sum % 256) == checksum;
}

void flushSerial()
{
  // Flush the serial buffer to ensure no old data is left
  while (softSerial.available())
  {
    softSerial.read();
  }
}

/**
 * @brief Setup function to initialize the serial port, LED pixels, and button.
 */
void setup()
{
  softSerial.begin(BAUD_RATE);
  pixels.begin();

  pinMode(BUTTON_PIN, INPUT_PULLUP); // Initialize button pin

  // Load the current mode from registers
  loadModeData();
  fadeToColor(currentRed, currentGreen, currentBlue, currentBrightness, currentFadeTime);
}

/**
 * @brief Main loop to read commands from the serial port and set LED colors.
 */
void loop()
{
  static uint32_t lastButtonPressTime = 0;

  // Check button press (change mode on button press)
  // Check for timer overflow to avoid long press issues
  if (digitalRead(BUTTON_PIN) == LOW && millis() - lastButtonPressTime > 500)
  {
    lastButtonPressTime = millis();
    currentMode = (currentMode + 1) % 3;
    storeModeData();
    fadeToColor(currentRed, currentGreen, currentBlue, currentBrightness, currentFadeTime);
  }

  // Read serial commands
  if (softSerial.available() >= CMD_LENGTH)
  {
    // Read the command bytes into a buffer
    uint8_t buffer[CMD_LENGTH];
    softSerial.readBytes(buffer, CMD_LENGTH);

    // Get the command bytes from the buffer
    const uint8_t startByte = buffer[IDX_START_BYTE];
    const uint8_t red = buffer[IDX_RED_BYTE];
    const uint8_t green = buffer[IDX_GREEN_BYTE];
    const uint8_t blue = buffer[IDX_BLUE_BYTE];
    const uint8_t brightness = buffer[IDX_BRIGHTNESS_BYTE];
    const uint16_t fadeTime = (buffer[IDX_FADE_TIME_BYTE] << 8) | buffer[IDX_FADE_TIME_BYTE + 1];
    const uint16_t checksum = (buffer[IDX_CHECKSUM_BYTE] << 8) | buffer[IDX_CHECKSUM_BYTE + 1];
    const uint8_t stopByte = buffer[IDX_STOP_BYTE];

    // Check for stop byte (0x1)
    if (stopByte != 0x1)
    {
      softSerial.write(RESPONSE_ERR_STOP);
      flushSerial();
    }

    if (!validateChecksum(buffer, checksum))
    {
      softSerial.write(RESPONSE_ERR_CHECKSUM);
      flushSerial();
    }

    switch (startByte)
    {
    case START_BYTE_CHANGE_MODE:
      currentMode = (currentMode + 1) % 3;
      loadModeData();
      fadeToColor(currentRed, currentGreen, currentBlue, currentBrightness, currentFadeTime);
      softSerial.write(RESPONSE_OK);
      break;

    case START_BYTE_SET_COLOR:
      fadeToColor(red, green, blue, brightness, fadeTime);
      storeModeData();
      softSerial.write(RESPONSE_OK);
      break;

    default:
      softSerial.write(RESPONSE_ERR_START);
      return;
    };
  }
}

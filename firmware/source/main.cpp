#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <SoftwareSerial.h>
#include "Registers.h"
#include "Commands.h"
#include "Configuration.h"

// Define the pin numbers and LED strip configuration
Adafruit_NeoPixel pixels(LED_COUNT, LED_PIN, LED_TYPE);

// Define the Serial communication parameters
SoftwareSerial tinySerial(RX_PIN, TX_PIN);

// Define the default configuration
Register cfgReg = Register::create<GlobalVar>();
GlobalVar globalVar;
uint8_t buffer[MAX_BUFFER_SIZE];

/**
 * @brief Set the color of the NeoPixel strip.
 * @param red The red component of the color (0-255).
 * @param green The green component of the color (0-255).
 * @param blue The blue component of the color (0-255).
 * @param brightness The brightness level (0-255).
 */
void setColor(const uint8_t red, const uint8_t green, const uint8_t blue, const uint8_t brightness)
{
    pixels.fill(pixels.Color(red, green, blue), 0, LED_COUNT);
    pixels.setBrightness(brightness);
    pixels.show();
}

/**
 * @brief Set the operating mode for the NeoPixel strip, applying the corresponding color.
 * @param mode The mode to set (0, 1, or 2).
 */
void setMode(uint8_t mode)
{
    switch (mode)
    {
    case 0:
        setColor(globalVar.red_mode0, globalVar.green_mode0, globalVar.blue_mode0, globalVar.brightness_mode0);
        break;
    case 1:
        setColor(globalVar.red_mode1, globalVar.green_mode1, globalVar.blue_mode1, globalVar.brightness_mode1);
        break;
    case 2:
        setColor(globalVar.red_mode2, globalVar.green_mode2, globalVar.blue_mode2, globalVar.brightness_mode2);
        break;
    default:
        tinySerial.write(RESPONSE_ERR_MODE);
        break;
    }
    globalVar.mode = mode;  // Update the current mode
    cfgReg.save(globalVar); // Save the current configuration to EEPROM
}

void serialInterrupt()
{
    // Process commands
    switch (buffer[COMMAND_BYTE_IDX])
    {
    case COMMAND_GET_FIRMWARE_VERSION:
        // No additional data bytes expected for this command.
        tinySerial.write(globalVar.firmware_mjr);
        tinySerial.write(globalVar.firmware_mnr);
        tinySerial.write(globalVar.firmware_patch);
        break;

    case COMMAND_GET_SERIAL_NUMBER:
        // No additional data bytes expected for this command.
        tinySerial.write(globalVar.serial_nr_mjr);
        tinySerial.write(globalVar.serial_nr_mnr);
        tinySerial.write(globalVar.serial_nr_patch);
        break;

    case COMMAND_SET_COLOR:
    {
        // Extract color and brightness from the buffer
        uint8_t red = buffer[COMMAND_SET_RED_BYTE_IDX];
        uint8_t green = buffer[COMMAND_SET_GREEN_BYTE_IDX];
        uint8_t blue = buffer[COMMAND_SET_BLUE_BYTE_IDX];
        uint8_t brightness = buffer[COMMAND_SET_BRIGHTNESS_BYTE_IDX];

        switch (globalVar.mode)
        {
        case 0:
            globalVar.red_mode0 = red;
            globalVar.green_mode0 = green;
            globalVar.blue_mode0 = blue;
            globalVar.brightness_mode0 = brightness;
            break;
        case 1:
            globalVar.red_mode1 = red;
            globalVar.green_mode1 = green;
            globalVar.blue_mode1 = blue;
            globalVar.brightness_mode1 = brightness;
            break;
        case 2:
            globalVar.red_mode2 = red;
            globalVar.green_mode2 = green;
            globalVar.blue_mode2 = blue;
            globalVar.brightness_mode2 = brightness;
            break;
        default:
            tinySerial.write(RESPONSE_ERR_MODE);
            return;
        }
        cfgReg.save(globalVar); // Save the current configuration to EEPROM
        tinySerial.write(RESPONSE_OK);
        break;
    }

    case COMMAND_SET_MODE:
        uint8_t mode = buffer[COMMAND_SET_MODE_BYTE_IDX];

        if (globalVar.mode != mode)
            setMode(mode);
        else
            tinySerial.write(RESPONSE_OK);
        break;

    default:
        // If the command is not recognized, respond with an error.
        tinySerial.write(RESPONSE_ERR_COMMAND);
        break;
    }
}

/**
 * @brief Setup function that initializes the communication,
 * loads the configuration from EEPROM, and sets default values if necessary.
 */
void setup()
{
    // Initialize tinySerial (SoftwaretinySerial on ATtiny85 or hardware tinySerial on ATmega328)
    tinySerial.begin(BAUD_RATE);

    // Initialize NeoPixel strip
    pixels.begin();
    pixels.show(); // Clears the strip initially (all LEDs off)

    // Load or reset configuration
    GlobalVar temp = cfgReg.loadOrDefault(defaultCfg);
    if (temp.config_version != defaultCfg.config_version)
    {
        globalVar = defaultCfg;
        cfgReg.save(globalVar); // This save is intentionally left as per user's request
    }
    else
        globalVar = temp;

    setMode(globalVar.mode);
}

/**
 * @brief Main loop function that continuously checks for incoming data
 */
void loop()
{
    int byteCount = tinySerial.available();

    if (byteCount > MAX_BUFFER_SIZE)
        byteCount = MAX_BUFFER_SIZE;

    int bytesRead = tinySerial.readBytes(buffer, byteCount);
    if (bytesRead > 0)
        serialInterrupt();
}
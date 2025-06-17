/**
 * @file Main.cpp
 * @brief Main source file for the device firmware.
 *
 * @copyright (c) 2025 Lorenzo Pouw, PouwerTronics/MonkeyEnterprise.
 * Licensed under the BSD license. This notice must remain in all redistributions.
 */

#include "Main.h"
#include "DigitalInput.h"

/**
 * @brief Get the RGB color and brightness values based on the current mode
 * @param red Reference to store the red component (0-255)
 * @param green Reference to store the green component (0-255)
 * @param blue Reference to store the blue component (0-255)
 * @param brightness Reference to store the brightness (0-255)
 */
void getColor(uint8_t &red, uint8_t &green, uint8_t &blue, uint8_t &brightness)
{
    // Define a structure to hold the color reference based on the current mode
    struct ColorRef
    {
        uint8_t &r, &g, &b, &br;
    };

    // Lambda function to get the current color reference based on the mode
    auto getColorRef = [&]() -> ColorRef
    {
        switch (globalVar.mode)
        {
        case 0:
            return {globalVar.red_mode0, globalVar.green_mode0, globalVar.blue_mode0, globalVar.brightness_mode0};
        case 1:
            return {globalVar.red_mode1, globalVar.green_mode1, globalVar.blue_mode1, globalVar.brightness_mode1};
        case 2:
            return {globalVar.red_mode2, globalVar.green_mode2, globalVar.blue_mode2, globalVar.brightness_mode2};
        default:
            return {globalVar.red_mode0, globalVar.green_mode0, globalVar.blue_mode0, globalVar.brightness_mode0};
        }
    };

    // Get the current color based on the mode
    ColorRef color = getColorRef();

    red = color.r;
    green = color.g;
    blue = color.b;
    brightness = color.br;
}

/**
 * @brief Store the RGB color and brightness values in the global variable based on the current mode
 * @param red The red component of the color (0-255)
 * @param green The green component of the color (0-255)
 * @param blue The blue component of the color (0-255)
 * @param brightness The brightness of the color (0-255)
 */
void storeColor(uint8_t red, uint8_t green, uint8_t blue, uint8_t brightness)
{
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
        break;
    };

    cfgReg.save(globalVar);
}

/**
 * @brief Set the color of the NeoPixel LEDs based on the current mode
 */
void setColor()
{
    uint8_t red, green, blue, brightness;
    getColor(red, green, blue, brightness);
    pixel.setBrightness(brightness);
    pixel.fill(pixel.Color(red, green, blue));
    pixel.show();
}

/**
 * @brief Send a response back to the serial interface, including a CRC for error checking
 * @param responseCode The response code to be sent.
 * @param comc The command value (comc) to be sent (0-255), optional, default is 0x0.
 * @param red The red component of the color to be sent (0-255), optional, default is 0x0.
 * @param green The green component of the color to be sent (0-255), optional, default is 0x0.
 * @param blue The blue component of the color to be sent (0-255), optional, default is 0x0.
 * @param brightness The brightness of the color to be sent (0-255), optional, default is 0x0.
 */
void sendResponse(const uint8_t responseCode,
                  const uint8_t comc = 0x0,
                  const uint8_t red = 0x0,
                  const uint8_t green = 0x0,
                  const uint8_t blue = 0x0,
                  const uint8_t brightness = 0x0)
{
    uint8_t crc = (responseCode + comc + red + green + blue + brightness) % 256;
    tinySerial.write(0x01);         // Start of Frame
    tinySerial.write(responseCode); // Response code
    tinySerial.write(comc);         // Command value (comc)
    tinySerial.write(red);          // Red component
    tinySerial.write(green);        // Green component
    tinySerial.write(blue);         // Blue component
    tinySerial.write(brightness);   // Brightness
    tinySerial.write(crc);          // CRC
    tinySerial.write(0x01);         // End of Frame
    tinySerial.flush();             // Ensure all data is sent
}

/**
 * @brief Handle the command received from the serial interface
 * @param data Pointer to the command data
 * @return Response code indicating success or error
 *
 * @note The error codes are as follows:
 * 0x0 - Success
 * 0x1 - Start of Frame (SOF) error
 * 0x2 - Command (coma) error
 * 0x3 - Command value (comc) error
 * 0x4 - CRC error
 * 0x5 - End of Frame (EOF) error
 */
uint8_t handleCommand(const uint8_t *data)
{
    // Check if the start of frame (SOF) is correct and validate the command structure
    const uint8_t sof = data[0];
    if (sof != 0x1)
        return 0x1;

    // Validate the command (coma), which should be between 1 and 5
    const uint8_t coma = data[1];
    if (coma > 5)
        return 0x2;

    // Validate the command value (comc), which should be between 0 and 2
    const uint8_t comc = data[2];
    if (comc > 2)
        return 0x3;

    // Validate the RGB color components and brightness
    const uint8_t red = data[3];
    const uint8_t green = data[4];
    const uint8_t blue = data[5];
    const uint8_t brightness = data[6];

    // validate the CRC calculation
    const uint8_t crc = data[7];
    const uint8_t crcCalc = (coma + comc + red + green + blue + brightness) % 256;
    if (crc != crcCalc)
        return 0x4;

    // Check if the end of frame (EOF) is correct
    const uint8_t eof = data[8];
    if (eof != 0x1)
        return 0x5;

    // If all checks passed, return success
    return 0x0;
}

/**
 * @brief Setup function to initialize the serial communication, NeoPixel strip, and global variables
 */
void setup()
{
    // Initialize the serial communication with the specified baud rate
    tinySerial.begin(BAUD_RATE);

    // Initialize the global variable with default configuration
    GlobalVar temp = cfgReg.load(defaultCfg);
    if (temp.config_version != defaultCfg.config_version)
    {
        globalVar = defaultCfg;
        cfgReg.wipeEEPROM();
        cfgReg.save(globalVar);
    }
    else
        globalVar = temp;

    // Initialize the pixel strip
    pixel.begin();
    setColor();
    cpuLed.enableBlink(1000);
}

/**
 * @brief Main loop function that continuously checks for button presses and serial commands
 */
void loop()
{
    // Update the cycle button state and check if it was pressed
    cycleButton.update();
    if (cycleButton.fell())
    {
        globalVar.mode = (globalVar.mode + 1) % 3;
        cfgReg.save(globalVar);
        setColor();
    }

    // Check if there are enough bytes available in the serial buffer to read a command
    // The command should be 9 bytes long: SOF (1 byte) + coma (1 byte) + comc (1 byte) + RGB (3 bytes) + brightness (1 byte) + CRC (1 byte) + EOF (1 byte)
    // If there are enough bytes, read the command and handle it
    if (tinySerial.available() >= 9)
    {
        uint8_t data[9];
        tinySerial.readBytes(data, 9);

        const uint8_t responseCode = handleCommand(data);

        if (responseCode == 0x0)
        {
            const uint8_t coma = data[1];
            const uint8_t comc = data[2];
            const uint8_t red = data[3];
            const uint8_t green = data[4];
            const uint8_t blue = data[5];
            const uint8_t brightness = data[6];

            switch (coma)
            {
            // Set color command
            case 0x01:
                setColor();
                sendResponse(0x0); // Success response
                break;
            // Clear the pixel strip
            case 0x02:
                pixel.clear();
                pixel.show();
                sendResponse(0x0); // Success response
                break;
            // Set mode command
            case 0x03:
                globalVar.mode = comc;
                cfgReg.save(globalVar);
                setColor();
                sendResponse(0x0); // Success response
                break;
            // Store color command
            case 0x04:
                storeColor(red, green, blue, brightness);
                sendResponse(0x0); // Success response
                break;
            // Get color command
            case 0x05:
                uint8_t cureRed, currentGreen, currentBlue, currentBrightness;
                getColor(cureRed, currentGreen, currentBlue, currentBrightness);
                sendResponse(0x0, globalVar.mode, cureRed, currentGreen, currentBlue, currentBrightness);
                break;
            // Invalid command, this should not happen, just in case send an error response
            default:
                sendResponse(0x2); // Invalid command
                break;
            }
        }
    }
    cpuLed.update();
}
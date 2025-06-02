#include "Main.h"

/**
 * @brief Initialize the global variable with default configuration
 *        If the stored configuration version does not match the default,
 */
void initGlobalVar()
{
    GlobalVar temp = cfgReg.load(defaultCfg);
    if (temp.config_version != defaultCfg.config_version)
    {
        globalVar = defaultCfg;
        cfgReg.save(globalVar);
    }
    else
    {
        globalVar = temp;
    }
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
 * 0x3 - Command value (comv) error
 * 0x4 - CRC error
 * 0x5 - End of Frame (EOF) error
 */
uint8_t handleCommand(const uint8_t *data)
{
    const uint8_t sof = data[0];
    if (sof != 0x1)
        return 0x1;

    const uint8_t coma = data[1];
    if (coma > 4)
        return 0x2;

    const uint8_t comv = data[2];
    if (comv > 2)
        return 0x3;

    const uint8_t red = data[3];
    const uint8_t green = data[4];
    const uint8_t blue = data[5];
    const uint8_t brightness = data[6];

    const uint8_t crc = data[7];
    const uint8_t crcCalc = (coma + comv + red + green + blue + brightness) % 256;
    if (crc != crcCalc)
        return 0x4;

    const uint8_t eof = data[8];
    if (eof != 0x1)
        return 0x5;

    return 0x0;
}

/**
 * @brief Set the color of the NeoPixel LEDs based on the current mode
 */
void setColor()
{
    struct ColorRef
    {
        uint8_t r, g, b, br;
    };

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
            return {0, 0, 0, 0};
        }
    };

    ColorRef color = getColorRef();
    pixel.setBrightness(color.br);
    pixel.fill(pixel.Color(color.r, color.g, color.b), 0, LED_COUNT);
    pixel.show();
}

/**
 * @brief Store the current mode in the global variable and save it to EEPROM
 * @param mode The mode to be stored (0, 1, or 2)
 */
void storeMode(const uint8_t mode)
{
    globalVar.mode = mode;
    cfgReg.save(globalVar);
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
    struct ColorRef
    {
        uint8_t &r, &g, &b, &br;
    };

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

    ColorRef color = getColorRef();
    color.r = red;
    color.g = green;
    color.b = blue;
    color.br = brightness;

    cfgReg.save(globalVar);
}

/**
 * @brief Setup function to initialize the serial communication, NeoPixel strip, and global variables
 */
void setup()
{
    tinySerial.begin(BAUD_RATE);
    initGlobalVar();
    pixel.begin();
    setColor();
}

/**
 * @brief Main loop function that continuously checks for button presses and serial commands
 */
void loop()
{
    cycleButton.update();
    if (cycleButton.fell())
    {
        globalVar.mode = (globalVar.mode + 1) % 3;
        cfgReg.save(globalVar);
        setColor();
    }

    if (tinySerial.available() >= 9)
    {
        uint8_t data[9];
        tinySerial.readBytes(data, 9);

        const uint8_t responseCode = handleCommand(data);

        if (responseCode == 0x0)
        {
            const uint8_t coma = data[1];
            const uint8_t comv = data[2];
            const uint8_t red = data[3];
            const uint8_t green = data[4];
            const uint8_t blue = data[5];
            const uint8_t brightness = data[6];

            switch (coma)
            {
            case 0x01:
                setColor();
                break;
            case 0x02:
                pixel.clear();
                pixel.show();
                break;
            case 0x03:
                storeMode(comv);
                setColor();
                break;
            case 0x04:
                storeColor(red, green, blue, brightness);
                break;
            default:
                break;
            }
        }

        tinySerial.write(0x1);
        tinySerial.write(responseCode);
        for (int i = 0; i < 7; ++i)
            tinySerial.write((uint8_t)0x0);
        tinySerial.write(0x1);
    }
}

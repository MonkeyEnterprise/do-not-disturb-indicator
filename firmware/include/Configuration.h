#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

// ------- Hardware Configuration -------
// #if defined(__AVR_ATtiny85__)
#define TX_PIN 6
#define RX_PIN 5
#define LED_PIN 7
#define BUTTON_PIN 8
// #else
// #error "Unsupported board configuration. Please define TX_PIN, RX_PIN, LED_PIN, and BUTTON_PIN for your board."
// #endif

// ------- Software Configuration -------
#define BAUD_RATE 9600
#define LED_COUNT 8
#define LED_TYPE NEO_GRB + NEO_KHZ800

// ------- Global Variables -------
struct GlobalVar
{
    uint8_t config_version;
    uint8_t mode;
    uint8_t red_mode0, green_mode0, blue_mode0, brightness_mode0;
    uint8_t red_mode1, green_mode1, blue_mode1, brightness_mode1;
    uint8_t red_mode2, green_mode2, blue_mode2, brightness_mode2;
};

const GlobalVar defaultCfg = {
    1,              // config_version
    0,              // mode
    255, 0, 0, 255, // mode 0: red, green, blue, brightness
    0, 255, 0, 255, // mode 1: red, green, blue, brightness
    255, 255, 0, 255  // mode 2: red, green, blue, brightness
};

#endif // CONFIGURATION_H
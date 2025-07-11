/**
 * @file Configuration.h
 * @brief Configuration header file for the device firmware.
 *
 * @copyright (c) 2025 Lorenzo Pouw, PouwerTronics/MonkeyEnterprise.
 * Licensed under the BSD license. This notice must remain in all redistributions.
 */

#ifndef __CONFIGURATION_H__
#define __CONFIGURATION_H__

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

// Define the CPU frequency for the AVR microcontroller
// ------- Hardware Configuration -------
#define TX_PIN PB0
#define RX_PIN PB1
#define LED_PIN PB2
#define CPU_PIN PB4
#define BUTTON_PIN PB3

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
    1,               // config_version
    0,               // mode
    255, 0, 0, 255,  // mode 0: red, green, blue, brightness
    0, 255, 0, 255,  // mode 1: red, green, blue, brightness
    255, 255, 0, 255 // mode 2: red, green, blue, brightness
};

#endif // __CONFIGURATION_H__
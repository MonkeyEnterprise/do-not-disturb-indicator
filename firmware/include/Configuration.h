/**
 ******************************************************************************
 * @file    Configuration.h
 * @version 1.1.0
 * @date    2025-05-29
 * @brief   Header file for the configuration.
 ******************************************************************************
 */

#ifndef __CONFIGURATION_H__
#define __CONFIGURATION_H__

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

// Define the CPU frequency for the AVR microcontroller
// ------- Hardware Configuration -------
#define TX_PIN PB4
#define RX_PIN PB3
#define LED_PIN PB1
#define BUTTON_PIN PB2

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
    2,               // config_version
    1,               // mode
    255, 0, 0, 127,  // mode 0: red, green, blue, brightness
    0, 255, 0, 127,  // mode 1: red, green, blue, brightness
    255, 255, 0, 127 // mode 2: red, green, blue, brightness
};

#endif // __CONFIGURATION_H__
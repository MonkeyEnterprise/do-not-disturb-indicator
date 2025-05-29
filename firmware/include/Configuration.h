#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

// ------- Hardware Configuration -------
#if defined(__AVR_ATtiny85__)
#define TX_PIN PB0
#define RX_PIN PB1
#define LED_PIN PB2
#define BUTTON_PIN PB3
#else
#error "Unsupported board configuration. Please define TX_PIN, RX_PIN, LED_PIN, and BUTTON_PIN for your board."
#endif

// ------- Software Configuration -------
#define BAUD_RATE 9600
#define LED_COUNT 8
#define LED_TYPE NEO_GRB + NEO_KHZ800
#define MAX_BUFFER_SIZE 64

// ------- Global Variables -------
struct GlobalVar
{
    uint16_t config_version;

    uint8_t mode;

    uint8_t red_mode0, green_mode0, blue_mode0, brightness_mode0;
    uint8_t red_mode1, green_mode1, blue_mode1, brightness_mode1;
    uint8_t red_mode2, green_mode2, blue_mode2, brightness_mode2;

    uint8_t firmware_mjr, firmware_mnr, firmware_patch;
    uint8_t serial_nr_mjr, serial_nr_mnr, serial_nr_patch;
};

const GlobalVar defaultCfg = {
    .config_version = 2,

    .mode = 0,

    .red_mode0 = 255,
    .green_mode0 = 0,
    .blue_mode0 = 0,
    .brightness_mode0 = 255,

    .red_mode1 = 0,
    .green_mode1 = 255,
    .blue_mode1 = 0,
    .brightness_mode1 = 255,

    .red_mode2 = 0,
    .green_mode2 = 0,
    .blue_mode2 = 255,
    .brightness_mode2 = 255,

    .firmware_mjr = 1,
    .firmware_mnr = 0,
    .firmware_patch = 1,

    .serial_nr_mjr = 1,
    .serial_nr_mnr = 0,
    .serial_nr_patch = 1};

#endif // CONFIGURATION_H
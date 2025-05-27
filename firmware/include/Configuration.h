#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

// Define the pins for SoftwareSerial communication
#define RX_PIN PB0 // Pin for receiving data
#define TX_PIN PB1 // Pin for transmitting data

// Define the pin and number of LEDs
#define LED_PIN PB2                   // Pin connected to the NeoPixel data input
#define LED_COUNT 1                   // Number of LEDs in the WS2812 LED strip
#define LED_TYPE NEO_RGB + NEO_KHZ800 // Type of WS2812 LED strip

// Define the pin for the button
#define BUTTON_PIN PB3 // Pin for the button

// Define the start and stop bytes for the command protocol
#define START_BYTE_CHANGE_MODE 0xA0 // Start byte for changing mode command
#define START_BYTE_SET_COLOR 0xA1   // Start byte for setting color command

// Define the command length and indices for the command buffer
#define IDX_START_BYTE 0      // Index of the start byte in the command buffer
#define IDX_RED_BYTE 1        // Index of the red byte in the command buffer
#define IDX_GREEN_BYTE 2      // Index of the green byte in the command buffer
#define IDX_BLUE_BYTE 3       // Index of the blue byte in the command buffer
#define IDX_BRIGHTNESS_BYTE 4 // Index of the brightness byte in the command buffer
#define IDX_FADE_TIME_BYTE 5  // Index of the fade time byte in the command buffer (2 bytes total)
#define IDX_CHECKSUM_BYTE 7   // Index of the checksum byte in the command buffer (2 bytes total)
#define IDX_STOP_BYTE 9       // Index of the stop byte in the command buffer
#define CMD_LENGTH 10         // Total length of the command buffer

// Define the response codes for the command protocol
#define RESPONSE_OK 0xA0           // OK response
#define RESPONSE_ERR_START 0xE1    // Start byte error response
#define RESPONSE_ERR_STOP 0xE2     // Stop byte error response
#define RESPONSE_ERR_CHECKSUM 0xE3 // Checksum error response

// Define other constants
#define BAUD_RATE 115200 // Baud rate for SoftwareSerial

#endif // CONFIGURATION_H
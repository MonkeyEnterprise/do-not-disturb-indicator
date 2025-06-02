#ifndef __MAIN_H__
#define __MAIN_H__

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <SoftwareSerial.h>
#include <Register.h>
#include <DigitalInput.h>
#include "Configuration.h"


// Define the global register for configuration
Register cfgReg = Register::create<GlobalVar>();
GlobalVar globalVar;

// Define the pin numbers for the LED strip and serial communication
DigitalInput cycleButton(BUTTON_PIN, 30, true, false); // Pin 2 for cycle button with debounce of 30ms

// Define the pin numbers and LED strip configuration
Adafruit_NeoPixel pixel(LED_COUNT, LED_PIN, LED_TYPE);

// Define the Serial communication parameters
SoftwareSerial tinySerial(RX_PIN, TX_PIN);

#endif // __MAIN_H__
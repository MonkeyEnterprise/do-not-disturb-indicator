/**
 * @file Hardware.h
 * @brief Hardware configuration and initialization for the device firmware.
 *
 * @copyright (c) 2025 Lorenzo Pouw, PouwerTronics/MonkeyEnterprise.
 * Licensed under the BSD license. This notice must remain in all redistributions.
 */


#ifndef __HARDWARE_H__
#define __HARDWARE_H__

#include <Adafruit_NeoPixel.h>
#include <SoftwareSerial.h>
#include <Register.h>
#include <DigitalInput.h>
#include <DigitalOutput.h>

#include "Configuration.h"

// Define the global register for configuration
Register cfgReg = Register::create<GlobalVar>();
GlobalVar globalVar;

// Define the pin numbers for the LED strip and serial communication
DigitalInput cycleButton(BUTTON_PIN, 30, true, false);

DigitalOutput cpuLed(CPU_PIN, LOW);

// Define the pin numbers and LED strip configuration
Adafruit_NeoPixel pixel(LED_COUNT, LED_PIN, LED_TYPE);

// Define the Serial communication parameters
SoftwareSerial tinySerial(RX_PIN, TX_PIN);

#endif // __HARDWARE_H__
/**
 ******************************************************************************
 * @file    Register.cpp
 * @version 1.0.0
 * @date    29-05-2025
 * @brief   Implementation of the Register class for EEPROM-backed storage.
 ******************************************************************************
 */

#include "Register.h"
#include <EEPROM.h>

uint8_t Register::computeCRC8(const uint8_t *data, size_t len) const
{
    uint8_t crc = 0x00;
    while (len--)
    {
        crc ^= *data++;
        for (uint8_t i = 0; i < 8; ++i)
            crc = (crc & 0x80) ? (crc << 1) ^ 0x07 : (crc << 1);
    }
    return crc;
}

size_t Register::getAddress() const
{
    return _address;
}
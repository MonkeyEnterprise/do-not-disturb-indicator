/**
 ******************************************************************************
 * @file    Register.h
 * @version 1.0.0
 * @date    29-05-2025
 * @brief   Implementation of the Register class for EEPROM-backed storage.
 ******************************************************************************
 */

#pragma once
#ifndef __REGISTER_H__
#define __REGISTER_H__

#include <EEPROM.h>
#include "SlotAllocator.h"

/**
 * @class Register
 * @brief EEPROM-backed register with CRC8 integrity check.
 */
class Register
{
public:
    /**
     * @brief Construct a Register at a specific EEPROM address.
     * @param eeprom_addr The EEPROM address for this register.
     */
    explicit Register(size_t eeprom_addr)
        : _address(eeprom_addr) {}

    /**
     * @brief Compute the CRC8 checksum for a given data array.
     * @param data Pointer to the data array.
     * @param len Length of the data array.
     * @return The computed CRC8 value.
     */
    uint8_t computeCRC8(const uint8_t *data, size_t len) const;

    /**
     * @brief Create a new Register for type T.
     * @tparam T The type to be stored.
     * @return A Register with a unique EEPROM address.
     */
    template <typename T>
    static Register create()
    {
        size_t size = sizeof(T) + 1; // +1 for CRC
        return Register(SlotAllocator::allocate(size));
    }

    /**
     * @brief Get the EEPROM address of this Register.
     * @return The EEPROM address.
     */
    size_t getAddress() const;

    /**
     * @brief Save a value to EEPROM if it differs from the current value.
     *        Also updates the CRC.
     * @tparam T The type of the value.
     * @param value The value to store.
     * @return true if EEPROM was written, false if not needed.
     */
    template <typename T>
    bool save(const T &value)
    {
        T current;
        EEPROM.get(_address, current);
        if (memcmp(&current, &value, sizeof(T)) != 0) {
            const uint8_t *ptr = reinterpret_cast<const uint8_t *>(&value);
            uint8_t crc = computeCRC8(ptr, sizeof(T));
            EEPROM.put(_address, value);
            EEPROM.write(_address + sizeof(T), crc);
            return true;
        }
        // Check if CRC is still valid, otherwise update only CRC
        uint8_t crc = computeCRC8(reinterpret_cast<const uint8_t *>(&value), sizeof(T));
        if (EEPROM.read(_address + sizeof(T)) != crc) {
            EEPROM.write(_address + sizeof(T), crc);
            return true;
        }
        return false;
    }

    /**
     * @brief Load a value from EEPROM and check CRC.
     * @tparam T The type of the value.
     * @param out The loaded value.
     * @param default_value The default value if CRC fails.
     * @return true if loading and CRC were successful, false otherwise.
     */
    template <typename T>
    bool load(T &out, const T &default_value) const
    {
        EEPROM.get(_address, out);
        uint8_t crcStored = EEPROM.read(_address + sizeof(T));
        uint8_t crcCalc = computeCRC8(reinterpret_cast<const uint8_t *>(&out), sizeof(T));
        if (crcStored == crcCalc) {
            return true;
        } else {
            out = default_value;
            return false;
        }
    }

    /**
     * @brief Load a value from EEPROM, return default if CRC fails.
     * @tparam T The type of the value.
     * @param default_value The default value if CRC fails.
     * @return The loaded value or the default.
     */
    template <typename T>
    T load(const T &default_value) const
    {
        T out;
        if (load(out, default_value)) {
            return out;
        }
        return default_value;
    }

private:
    size_t _address;
};

#endif // __REGISTERS_H__
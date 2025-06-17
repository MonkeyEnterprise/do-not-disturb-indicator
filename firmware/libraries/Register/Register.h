
/**
 * @file Register.h
 * @brief A library for managing EEPROM-backed registers with CRC8 validation.
 *
 * @copyright (c) 2025 Lorenzo Pouw, PouwerTronics/MonkeyEnterprise.
 * Licensed under the BSD license. This notice must remain in all redistributions.
 */

#include <Arduino.h>
#include <EEPROM.h>

/**
 * @class SlotAllocator
 * @brief Allocates unique, non-overlapping memory regions in EEPROM for Register instances.
 */
class SlotAllocator
{
public:
    /**
     * @brief Allocate a slot of a specific size in EEPROM memory.
     * @param size Number of bytes to allocate.
     * @return The starting EEPROM address for the allocated slot.
     */
    static size_t allocate(size_t size)
    {
        size_t addr = _next;
        _next += size;
        return addr;
    }

private:
    static size_t _next; ///< Tracks the next available EEPROM address.
};

size_t SlotAllocator::_next = 0;

/**
 * @class Register
 * @brief A typed EEPROM-backed memory register with CRC8 validation.
 */
class Register
{
public:
    /**
     * @brief Constructs a Register bound to a specific EEPROM address.
     * @param eeprom_addr The EEPROM address to associate with this register.
     */
    explicit Register(size_t eeprom_addr)
        : _address(eeprom_addr) {}

    /**
     * @brief Creates a Register instance for a specified data type.
     * @tparam T The type of the data to store.
     * @return A new Register with a unique EEPROM slot.
     */
    template <typename T>
    static Register create()
    {
        size_t size = sizeof(T) + 1; // +1 for CRC
        return Register(SlotAllocator::allocate(size));
    }

    /**
     * @brief Get the EEPROM address associated with this register.
     * @return EEPROM memory address.
     */
    size_t getAddress() const
    {
        return _address;
    }

    /**
     * @brief Save a value to EEPROM if it differs from the stored value.
     *        Also updates the CRC byte.
     * @tparam T The data type to store.
     * @param value The new value to store in EEPROM.
     * @return True if EEPROM was updated, false if no change was needed.
     */
    template <typename T>
    bool save(const T &value)
    {
        T current;
        EEPROM.get(_address, current);

        if (memcmp(&current, &value, sizeof(T)) != 0)
        {
            const uint8_t *ptr = reinterpret_cast<const uint8_t *>(&value);
            uint8_t crc = computeCRC8(ptr, sizeof(T));

            EEPROM.put(_address, value);
            EEPROM.write(_address + sizeof(T), crc);
            return true;
        }

        // Check and update CRC only
        uint8_t crc = computeCRC8(reinterpret_cast<const uint8_t *>(&value), sizeof(T));
        if (EEPROM.read(_address + sizeof(T)) != crc)
        {
            EEPROM.write(_address + sizeof(T), crc);
            return true;
        }

        return false;
    }

    /**
     * @brief Load a value from EEPROM with CRC8 validation.
     * @tparam T The data type to load.
     * @param out Reference to output variable.
     * @param default_value Value to use if CRC fails.
     * @return True if loaded value passed CRC check, false if fallback was used.
     */
    template <typename T>
    bool load(T &out, const T &default_value) const
    {
        EEPROM.get(_address, out);
        uint8_t crcStored = EEPROM.read(_address + sizeof(T));
        uint8_t crcCalc = this->computeCRC8(reinterpret_cast<const uint8_t *>(&out), sizeof(T));

        if (crcStored == crcCalc)
            return true;

        out = default_value;
        return false;
    }

    /**
     * @brief Load a value directly with a fallback default value.
     * @tparam T The data type to load.
     * @param default_value Value to return if CRC check fails.
     * @return The stored value or the default if validation failed.
     */
    template <typename T>
    T load(const T &default_value) const
    {
        T out;
        return this->load(out, default_value) ? out : default_value;
    }

    /**
     * @brief Overwrite the entire EEPROM memory with a given byte value.
     * @param value Byte value to write to all EEPROM addresses (default: 0x00).
     */
    void wipeEEPROM(const uint8_t value = 0x00)
    {
        for (uint16_t i = 0; i < EEPROM.length(); ++i)
        {
            EEPROM.put(i, value);
        }
    }

private:
    size_t _address; ///< EEPROM address associated with this register.

    /**
     * @brief Compute a CRC8 checksum for a block of data.
     * @param data Pointer to the data block.
     * @param len Length of the data in bytes.
     * @return The calculated CRC8 value.
     */
    uint8_t computeCRC8(const uint8_t *data, size_t len) const
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
};
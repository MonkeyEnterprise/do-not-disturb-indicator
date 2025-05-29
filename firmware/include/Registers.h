#ifndef REGISTER_H
#define REGISTER_H

#include <EEPROM.h>

/**
 * @brief Compute CRC8 checksum for a given data buffer
 *
 * @param data Pointer to the data buffer
 * @param len Length of the data buffer
 * @return Computed CRC8 value
 */
uint8_t computeCRC8(const uint8_t *data, size_t len)
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

/**
 * @brief Simple slot allocator for EEPROM addresses
 *
 * This class provides a static method to allocate memory slots in EEPROM.
 * It keeps track of the next available address and increments it as needed.
 */
class SlotAllocator
{
public:
    static int allocate(int size)
    {
        int addr = _next;
        _next += size;
        return addr;
    }

private:
    static int _next;
};

int SlotAllocator::_next = 0;

/**
 * @brief Register class for managing EEPROM storage
 */
class Register
{
public:
    /**
     * @brief Constructor for Register class
     * @param eeprom_addr The EEPROM address where the register will be stored
     */
    explicit Register(int eeprom_addr)
        : _address(eeprom_addr) {}

    /**
     *  @brief Create a new Register instance for a specific type
     *  @tparam T The type of data to be stored in the register
     *  @return A new Register instance with allocated EEPROM space
     */
    template <typename T>
    static Register create()
    {
        int size = sizeof(T) + 1; // +1 for CRC
        return Register(SlotAllocator::allocate(size));
    }

    /**
     * @brief Save a value to the EEPROM with CRC validation
     * @tparam T The type of value to save
     * @param value The value to save
     */
    template <typename T>
    void save(const T &value)
    {
        const uint8_t *ptr = (const uint8_t *)(&value);
        uint8_t crc = computeCRC8(ptr, sizeof(T));

        EEPROM.put(_address, value);
        EEPROM.write(_address + sizeof(T), crc);
    }

    /**
     * @brief Load a value from the EEPROM with CRC validation
     * @tparam T The type of value to load
     * @return The loaded value if CRC is valid, otherwise returns default_value
     */
    template <typename T>
    T loadOrDefault(const T &default_value) const
    {
        T value;
        EEPROM.get(_address, value);

        uint8_t crcStored = EEPROM.read(_address + sizeof(T));
        uint8_t crcCalc = computeCRC8((uint8_t *)&value, sizeof(T));

        if (crcStored == crcCalc)
        {
            return value;
        }
        else
        {
            return default_value;
        }
    }

private:
    int _address;
};

#endif // REGISTER_H

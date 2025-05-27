#ifndef REGISTERS_H
#define REGISTERS_H

#include <EEPROM.h>

/**
 * @brief A class to manage registers in EEPROM.
 *        This class allows saving and loading of values to and from EEPROM
 *        using a specified slot. Each slot corresponds to a specific type
 *        of data, and the size of the data is determined by the type.
 */
class Register
{
public:
    /**
     * @brief Constructs a Register object for a specific slot.
     *
     * @param slot The slot number in EEPROM where the data will be stored.
     */
    explicit Register(int slot) : _slot(slot) {}

    /**
     * @brief Saves a value of type T to the specified EEPROM slot.
     * @param value The value to be saved.
     *
     */
    template <typename T>
    void save(const T &value)
    {
        int eeprom_address = _slot * sizeof(T);
        if (isAddressValid<T>(eeprom_address))
        {
            EEPROM.put(eeprom_address, value);
        }
    }

    /**
     * @brief Loads a value of type T from the specified EEPROM slot.
     * @return The loaded value of type T.
     *
     */
    template <typename T>
    T load() const
    {
        T value{};
        int eeprom_address = _slot * sizeof(T);
        if (isAddressValid<T>(eeprom_address))
        {
            EEPROM.get(eeprom_address, value);
        }
        return value;
    }

private:
    int _slot;

    /**
     * @brief Checks if the address is valid for the given type T.
     * @param address The starting address in EEPROM.
     * @return True if the address is valid, false otherwise.
     *
     */
    template <typename T>
    bool isAddressValid(int address) const
    {
        return address >= 0 && (address + sizeof(T)) <= EEPROM.length();
    }
};

#endif // REGISTERS_H

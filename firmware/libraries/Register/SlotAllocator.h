#pragma once
#ifndef __SLOTALLOCATOR_H__
#define __SLOTALLOCATOR_H__

#include <stddef.h>

/**
 * @class SlotAllocator
 * @brief Allocates unique EEPROM address slots for Register objects.
 */
class SlotAllocator
{
public:
    /**
     * @brief Allocate a slot of given size in EEPROM.
     * @param size Number of bytes to allocate.
     * @return Start address of allocated slot.
     */
    static size_t allocate(size_t size)
    {
        size_t addr = _next;
        _next += size;
        return addr;
    }

private:
    static size_t _next;
};

#endif // __SLOTALLOCATOR_H__

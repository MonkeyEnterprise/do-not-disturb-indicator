# Registers Library

Efficient and reliable storage of variables and structs in Arduino EEPROM, with CRC8 integrity checking and automatic address allocation.

## Features

- **Store any variable or struct** in EEPROM with a single line of code
- **Automatic address allocation**: no manual EEPROM address management
- **CRC8 integrity check**: detects data corruption and restores defaults if needed
- **Minimizes EEPROM wear**: only writes when data changes
- **Simple API**: easy to use in any Arduino sketch

## API

- `Register(size_t eeprom_addr)`
- `int computeCRC8(const int *data, size_t len)`
- `Register create()`
- `size_t getAddress() const`
- `bool save(const T &value)`
- `bool load(T &out, const T &default_value) const`
- `T load(const T &default_value) const`
- `static size_t allocate(size_t size)`

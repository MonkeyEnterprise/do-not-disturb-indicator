# Registers Arduino Library

Efficient and reliable storage of variables and structs in Arduino EEPROM, with CRC8 integrity checking and automatic address allocation.

## Features

- **Store any variable or struct** in EEPROM with a single line of code
- **Automatic address allocation**: no manual EEPROM address management
- **CRC8 integrity check**: detects data corruption and restores defaults if needed
- **Minimizes EEPROM wear**: only writes when data changes
- **Simple API**: easy to use in any Arduino sketch

## Installation

1. Copy the `Registers` folder to your Arduino `libraries` directory.
2. Restart the Arduino IDE.

## Usage

### 1. Include the Library

```cpp
#include <Registers.h>
```

### 2. Define Your Data Structure

```cpp
struct Config {
    uint8_t version;
    int threshold;
    float gain;
};
```

### 3. Create a Register

```cpp
Register configReg = Register::create<Config>();
```

### 4. Load and Save Data

```cpp
const Config defaultConfig = {1, 100, 1.0f};

// Load from EEPROM, or use default if CRC fails
Config cfg = configReg.load(defaultConfig);

// Modify and save if needed
cfg.threshold += 10;
if (configReg.save(cfg)) {
    Serial.println(F("Config updated in EEPROM."));
} else {
    Serial.println(F("Config unchanged, EEPROM not written."));
}
```

## Example

See [`examples/GlobalVar_Example/GlobalVar_Example.ino`](examples/GlobalVar_Example/GlobalVar_Example.ino):

```cpp
#include <Arduino.h>
#include <Registers.h>

struct Config {
    uint8_t version;
    int threshold;
    float gain;
};

const Config defaultConfig = {1, 100, 1.0f};
Register configReg = Register::create<Config>();

void setup() {
    Serial.begin(115200);
    Config cfg = configReg.load(defaultConfig);

    Serial.println(F("Loaded config:"));
    Serial.print(F("Version: ")); Serial.println(cfg.version);
    Serial.print(F("Threshold: ")); Serial.println(cfg.threshold);
    Serial.print(F("Gain: ")); Serial.println(cfg.gain);

    cfg.threshold += 10;
    if (configReg.save(cfg)) {
        Serial.println(F("Config updated in EEPROM."));
    } else {
        Serial.println(F("Config unchanged, EEPROM not written."));
    }
}

void loop() {}
```

## API Reference

### Register

- `Register::create<T>()`  
  Create a new register for type `T`. Allocates EEPROM space automatically.

- `bool save(const T &value)`  
  Save a value to EEPROM if it differs from the current value. Returns `true` if EEPROM was written.

- `bool load(T &out, const T &default_value) const`  
  Load a value from EEPROM, or set to `default_value` if CRC fails. Returns `true` if successful.

- `T load(const T &default_value) const`  
  Load a value from EEPROM, or return `default_value` if CRC fails.

- `size_t getAddress() const`  
  Get the EEPROM address of this register.

### SlotAllocator

- `static size_t allocate(size_t size)`  
  Allocate a unique EEPROM address slot of the given size.

## Notes

- **EEPROM size**: Make sure your total allocated size does not exceed your board's EEPROM.
- **Data integrity**: CRC8 ensures that corrupted data is detected and defaults are restored.
- **Minimize writes**: Only changed data is written to EEPROM, extending its lifespan.

## License

MIT License

## Author

Lorenzo Pouw / PouwerTronics  
[https://pouwertronics.nl](https://pouwertronics.nl)

# DigitalInput Arduino Library

A robust, debounced digital input class for Arduino with edge detection, invert logic, active duration measurement, and millis() overflow protection.

## Features

- Debounces digital inputs with configurable debounce time
- Detects rising and falling edges
- Handles millis() overflow safely
- Supports internal pull-up resistors
- Optional invert logic (active LOW or HIGH)
- Measures how long the input has been active

## Usage

```cpp
#include <DigitalInput.h>

#define INPUT_PIN 2
#define DEBOUNCE_MS 50
#define INVERT_INPUT true  // Set to true to invert logic (active LOW)

DigitalInput input(INPUT_PIN, DEBOUNCE_MS, true, INVERT_INPUT);

void setup() {
    Serial.begin(115200);
    Serial.println("Digital Input Example");
}

void loop() {
    input.update();

    Serial.print("Input State: ");
    Serial.println(input.read() ? "ACTIVE" : "INACTIVE");

    if (input.rose())
        Serial.println("Input became ACTIVE!");
    if (input.fell()) {
        Serial.print("Input became INACTIVE! Active duration: ");
        Serial.print(input.activeDuration());
        Serial.println(" ms");
    }

    delay(100); // Adjust as needed
}
```

## API

- `DigitalInput(pin, debounceMs = 30, pullup = true, invert = false)`
- `void update()`
- `bool read() const`
- `bool rose() const`
- `bool fell() const`
- `unsigned long activeDuration() const`

## License

MIT License

## Author

Lorenzo Pouw / PouwerTronics  
[https://pouwertronics.nl](https://pouwertronics.nl)

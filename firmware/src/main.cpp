#include <Adafruit_NeoPixel.h>

#define LED_PIN    6
#define LED_COUNT  4

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  strip.begin();
  strip.show();
  strip.setBrightness(50);
  Serial.begin(115200);
}

void loop() {
  if (Serial.available() >= 6) {
    uint8_t cmd    = Serial.read();
    uint8_t red    = Serial.read();
    uint8_t green  = Serial.read();
    uint8_t blue   = Serial.read();
    uint8_t bright = Serial.read();
    uint8_t stop   = Serial.read();

    if (cmd == 0x01 && stop == 0x00) {
      strip.setBrightness(bright);
      for (int i = 0; i < LED_COUNT; i++) {
        strip.setPixelColor(i, strip.Color(red, green, blue));
      }
      strip.show();

      Serial.write(0xAA);
    } else {
      Serial.write(0xFF);
      flushSerial();
    }
  }
}

void flushSerial() {
  while (Serial.available() > 0) {
    Serial.read();
  }
}

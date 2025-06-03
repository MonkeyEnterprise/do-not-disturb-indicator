/**
 * @file    Config_Example.ino
 * @brief   Example usage of the Registers library for EEPROM-backed variables.
 */

#include <Arduino.h>
#include <Register.h>

/**
 * @brief Example struct to store in EEPROM.
 */
struct Config
{
  uint8_t version;
  int threshold;
  float gain;
};

/**
 * @brief Default configuration.
 */
const Config defaultConfig = {
    .version = 2,
    .threshold = 100,
    .gain = 1.0f};

/**
 * @brief Create a Register for Config type.
 */
Register configReg = Register::create<Config>();

/**
 * @brief Setup function to initialize the example.
 */
void setup()
{
  Serial.begin(9600);

  Config cfg;
  bool loaded = configReg.load(cfg, defaultConfig);

  if (!loaded || cfg.version != defaultConfig.version)
  {
    Serial.println(F("EEPROM invalid or version mismatch, writing defaults..."));
    cfg = defaultConfig;
    configReg.save(cfg);
  }

  Serial.print(F("Loaded config in addres:"));
  Serial.println(configReg.getAddress());
  Serial.println(F("Current configuration:"));
  Serial.print(F("Version: "));
  Serial.println(cfg.version);
  Serial.print(F("Threshold: "));
  Serial.println(cfg.threshold);
  Serial.print(F("Gain: "));
  Serial.println(cfg.gain);

  // Change and save config if needed
  cfg.threshold += 10;
  cfg.gain *= 1.1f;
  if (configReg.save(cfg))
    Serial.println(F("Config updated in EEPROM."));
  else
    Serial.println(F("Config unchanged, EEPROM not written."));

  Serial.println();
}

/**
 * @brief Empty loop function, nothing to do here.
 */
void loop()
{
  // Nothing to do here
}
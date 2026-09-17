#ifndef LDR_H
#define LDR_H

#include <Arduino.h>

class LDR {
public:
  // pin: ADC-capable pin (e.g., GPIO36)
  // vref: reference voltage used for conversion (typ. 3.3V)
  // adcMax: max raw value (ESP32 ADC is 12-bit → 4095)
  explicit LDR(uint8_t pin, float vref = 3.3f, uint16_t adcMax = 4095);

  // Call in setup(); optionally set attenuation for better range on ESP32
  void begin(bool setAttenuation = true);

  // Read raw ADC value
  int readRaw() const;

  // Read voltage (derived from raw)
  float readVoltage() const;

  // Adjust reference voltage if you use a different Vref
  void setVref(float vref);

  // Getters
  uint8_t pin() const { return _pin; }
  float vref()  const { return _vref; }
  uint16_t adcMax() const { return _adcMax; }

private:
  uint8_t  _pin;
  float    _vref;
  uint16_t _adcMax;
};

#endif // LDR_H

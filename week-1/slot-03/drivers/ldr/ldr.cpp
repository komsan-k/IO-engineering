#include "ldr.h"

#if defined(ARDUINO_ARCH_ESP32)
  // For analogSetPinAttenuation and ADC atten levels
  #include <esp32-hal-adc.h>
#endif

LDR::LDR(uint8_t pin, float vref, uint16_t adcMax)
: _pin(pin), _vref(vref), _adcMax(adcMax) {}

void LDR::begin(bool setAttenuation) {
  // ESP32 notes:
  // - analogReadResolution(12) is default in most cores, but we set it explicitly.
  // - Attenuation widens the measurable voltage range at the cost of resolution.
#if defined(ARDUINO_ARCH_ESP32)
  analogReadResolution(12);
  if (setAttenuation) {
    // 11dB ≈ full-scale ~3.3V (varies by chip)
    analogSetPinAttenuation(_pin, ADC_11db);
  }
#endif
  pinMode(_pin, INPUT);
}

int LDR::readRaw() const {
  return analogRead(_pin);
}

float LDR::readVoltage() const {
  // Simple linear mapping: V = raw * (vref / adcMax)
  // NOTE: ESP32 ADC is non-linear; for higher accuracy consider calibration/characterization.
  int raw = readRaw();
  return raw * (_vref / static_cast<float>(_adcMax));
}

void LDR::setVref(float vref) {
  _vref = vref;
}

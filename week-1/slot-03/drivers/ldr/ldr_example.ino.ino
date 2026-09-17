#include "ldr.h"

// GPIO36 (VP) is ADC1_CH0 on many ESP32 boards
const int LDR_PIN = 36;
LDR ldr(LDR_PIN, 3.3f, 4095);

void setup() {
  Serial.begin(115200);
  delay(500);
  ldr.begin(true); // set ESP32 attenuation to 11dB for ~0–3.3V range
  Serial.println("ESP32 LDR example started.");
}

void loop() {
  int raw = ldr.readRaw();
  float v  = ldr.readVoltage();
  Serial.printf("LDR ADC: %d, Voltage: %.2f V\n", raw, v);
  delay(1000);
}

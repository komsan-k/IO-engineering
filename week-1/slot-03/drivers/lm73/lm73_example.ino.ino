#include <Wire.h>
#include "lm73.h"

// ESP32 custom I2C pins (adjust if needed)
#define SDA1_PIN 4
#define SCL1_PIN 5

// LM73 at default address 0x4D
LM73 lm73(0x4D, Wire);

void setup() {
  Serial.begin(115200);
  delay(200);

  // Initialize I2C on custom pins and probe the sensor
  if (!lm73.begin(SDA1_PIN, SCL1_PIN, /*initWire=*/true)) {
    Serial.println("LM73 init failed (check wiring/address).");
    while (true) { delay(1000); }
  }

  // Optional: enable smoothing with SMA window of 10 samples
  lm73.enableSMA(10);

  Serial.println("LM73 initialized successfully.");
}

void loop() {
  float rawC = lm73.readCelsius();
  float smC  = lm73.readCelsiusSmoothed(); // same as raw if SMA disabled

  if (!isnan(rawC)) {
    Serial.print("Raw Temperature: ");
    Serial.print(rawC, 3);
    Serial.print(" °C   |   Smoothed (SMA): ");
    Serial.print(smC, 3);
    Serial.println(" °C");
  } else {
    Serial.println("Failed to read temperature.");
  }

  delay(1000);
}

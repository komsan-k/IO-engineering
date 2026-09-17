#include <Wire.h>
#include "mpuRPY.h"

// ESP32 I2C pins (adjust if needed)
#define SDA_PIN 4
#define SCL_PIN 5

// MPU6050 at 0x68 by default (AD0=LOW). Use 0x69 if AD0=HIGH.
MpuRPY imu(MpuRPY::MPU6050_ADDR_LOW, Wire);

void setup() {
  Serial.begin(115200);
  delay(200);

  if (!imu.begin(SDA_PIN, SCL_PIN, /*initWire=*/true)) {
    Serial.println("MPU6050 init failed (check wiring/address).");
    while (true) { delay(1000); }
  }

  // Optional: set ranges (and automatically update scales)
  // imu.configureRanges(2, 250); // ±2g, ±250 dps (defaults)

  // Optional: adjust complementary filter alpha
  imu.setComplementaryAlpha(0.98f);

  Serial.println("MPU6050 initialized.");
}

void loop() {
  if (!imu.update()) {
    Serial.println("MPU read failed");
    delay(10);
    return;
  }

  Serial.print("Roll: ");  Serial.print(imu.roll(), 2);
  Serial.print("°, Pitch: "); Serial.print(imu.pitch(), 2);
  Serial.print("°, Yaw*: ");  Serial.print(imu.yaw(), 2);
  Serial.print("°   |  dt: "); Serial.print(imu.lastDt(), 4); Serial.println(" s");

  delay(10); // ~100 Hz print; sensor updates are faster internally
}

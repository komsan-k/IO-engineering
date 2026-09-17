#include <Wire.h>

#define SDA_PIN 4
#define SCL_PIN 5

void setup() {
  Serial.begin(115200);

  Wire.begin(SDA_PIN, SCL_PIN);

  Serial.println("I2C Scanner");
}

void loop() {

  byte error;
  byte address;
  int devices = 0;

  Serial.println("Scanning...");

  for (address = 1; address < 127; address++) {

    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0) {
      Serial.print("I2C device found at address 0x");

      if (address < 16)
        Serial.print("0");

      Serial.println(address, HEX);
      devices++;
    }
  }

  if (devices == 0)
    Serial.println("No I2C devices found");
  else
    Serial.println("Scan complete");

  delay(5000);
}

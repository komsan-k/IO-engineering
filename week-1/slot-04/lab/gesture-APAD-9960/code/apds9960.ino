
#include <Wire.h>
#include <SparkFun_APDS9960.h>

// ==================================================
// APDS-9960 Configuration
// ==================================================
#define APDS9960_ADDRESS 0x39

#define SDA_PIN 4
#define SCL_PIN 5

// Create APDS-9960 object
SparkFun_APDS9960 apds;


// ==================================================
// I2C Scanner
// ==================================================
void scanI2C() {

  Serial.println("Scanning I2C bus...");

  int deviceCount = 0;

  for (uint8_t address = 1; address < 127; address++) {

    Wire.beginTransmission(address);
    uint8_t error = Wire.endTransmission();

    if (error == 0) {

      Serial.print("I2C device found at address 0x");

      if (address < 16) {
        Serial.print("0");
      }

      Serial.println(address, HEX);
      deviceCount++;
    }
  }

  if (deviceCount == 0) {
    Serial.println("No I2C devices found.");
  }

  Serial.println();
}


// ==================================================
// Setup
// ==================================================
void setup() {

  Serial.begin(115200);
  delay(1000);

  Serial.println();
  Serial.println("====================================");
  Serial.println("      APDS-9960 Gesture Sensor");
  Serial.println("====================================");

  // ------------------------------------------------
  // Initialize I2C
  // SDA = GPIO 4
  // SCL = GPIO 5
  // ------------------------------------------------
  Wire.begin(SDA_PIN, SCL_PIN);

  Serial.print("SDA Pin      : GPIO ");
  Serial.println(SDA_PIN);

  Serial.print("SCL Pin      : GPIO ");
  Serial.println(SCL_PIN);

  Serial.print("Sensor Addr. : 0x");
  Serial.println(APDS9960_ADDRESS, HEX);

  Serial.println();

  // ------------------------------------------------
  // Scan I2C bus
  // ------------------------------------------------
  scanI2C();

  // ------------------------------------------------
  // Check APDS-9960 address
  // ------------------------------------------------
  Wire.beginTransmission(APDS9960_ADDRESS);

  if (Wire.endTransmission() != 0) {

    Serial.println("ERROR: APDS-9960 not detected at 0x39.");
    Serial.println("Check SDA, SCL, VCC, and GND.");

    while (1) {
      delay(1000);
    }
  }

  Serial.println("APDS-9960 detected at address 0x39.");

  // ------------------------------------------------
  // Initialize APDS-9960
  // ------------------------------------------------
  if (apds.init()) {

    Serial.println("APDS-9960 initialization complete.");

  } else {

    Serial.println("ERROR: APDS-9960 initialization failed.");

    while (1) {
      delay(1000);
    }
  }

  // ------------------------------------------------
  // Enable gesture sensor
  // ------------------------------------------------
  if (apds.enableGestureSensor(true)) {

    Serial.println("Gesture sensor enabled.");

  } else {

    Serial.println("ERROR: Could not enable gesture sensor.");

    while (1) {
      delay(1000);
    }
  }

  Serial.println();
  Serial.println("------------------------------------");
  Serial.println("Move your hand over the APDS-9960");
  Serial.println("------------------------------------");
  Serial.println("Supported gestures:");
  Serial.println("  UP");
  Serial.println("  DOWN");
  Serial.println("  LEFT");
  Serial.println("  RIGHT");
  Serial.println("------------------------------------");
  Serial.println();
}


// ==================================================
// Main Loop
// ==================================================
void loop() {

  // Check whether a gesture is available
  if (apds.isGestureAvailable()) {

    // Read detected gesture
    int gesture = apds.readGesture();

    switch (gesture) {

      case DIR_UP:
        Serial.println("Gesture: UP");
        break;

      case DIR_DOWN:
        Serial.println("Gesture: DOWN");
        break;

      case DIR_LEFT:
        Serial.println("Gesture: LEFT");
        break;

      case DIR_RIGHT:
        Serial.println("Gesture: RIGHT");
        break;

      case DIR_NEAR:
        Serial.println("Gesture: NEAR");
        break;

      case DIR_FAR:
        Serial.println("Gesture: FAR");
        break;

      default:
        Serial.println("Gesture: UNKNOWN");
        break;
    }
  }

  delay(50);
}


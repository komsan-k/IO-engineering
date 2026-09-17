# Device Interfacing for LDR, LM73, and MPU6050

## 1. Objective
The main objectives of this lab are:
1. To interface and acquire data from multiple sensors:  
   - **LDR** (Light Dependent Resistor) for light intensity measurement  
   - **LM73** digital temperature sensor  
   - **MPU6050** accelerometer + gyroscope IMU
2. To implement I²C communication for digital sensors (LM73, MPU6050).
3. To process and display the acquired sensor data via the Serial Monitor.
4. To gain experience in handling mixed-signal sensor interfacing in an embedded system.

---

## 2. Background
Sensor interfacing is a fundamental skill in embedded programming, requiring knowledge of:
- **Analog Sensors**: e.g., LDRs provide a resistance that varies with light intensity. This is measured using an ADC (Analog-to-Digital Converter).
- **Digital Sensors (I²C)**:  
  - **LM73**: High-accuracy temperature sensor using I²C, with configurable resolution.  
  - **MPU6050**: Combines a 3-axis accelerometer and 3-axis gyroscope; communicates over I²C.

LDR Measurement Principle:
`Vout = Vcc × (R_LDR / (R_fixed + R_LDR))`  
where `R_LDR` is the resistance of the light-dependent resistor.



**I²C Communication Basics:**
- Master-slave protocol over SDA (data) and SCL (clock) lines.
- Each device has a unique 7-bit address.
- Supports multiple devices on the same bus.

---

## 3. Hardware Requirements
- **ESP32 Development Board**
- **LDR** + 10kΩ fixed resistor (voltage divider)
- **LM73** temperature sensor module
- **MPU6050** IMU sensor module
- Breadboard and jumper wires
- USB cable

---

## 4. Pin Connections

| Sensor      | Signal | ESP32 Pin | Notes                      |
|-------------|--------|-----------|----------------------------|
| **LDR**     | ADC    | GPIO 36   | Analog input only           |
| **LM73**    | SDA    | GPIO 4   | I²C data                    |
|             | SCL    | GPIO 5   | I²C clock                   |
| **MPU6050** | SDA    | GPIO 4   | Shared I²C data line        |
|             | SCL    | GPIO 5   | Shared I²C clock line       |
| GND         | GND    | GND       | Common ground               |
| VCC         | 3.3V   | 3.3V      | Power supply                |

---

## 5. Software Requirements
- Arduino IDE with ESP32 board support
- Libraries:
  - `Wire.h` (I²C)
  - `Adafruit_MPU6050` or direct register handling for MPU6050 (optional)
  - No extra library needed for LM73 basic read

---

## 6. Code Implementation

### 6.1 LDR Measurement
```cpp
const int LDR_PIN = 36;

void readLDR() {
  int adcValue = analogRead(LDR_PIN);
  float voltage = adcValue * (3.3 / 4095.0);
  Serial.printf("LDR ADC: %d, Voltage: %.2f V\n", adcValue, voltage);
}
```

### 6.2 LM73 I²C Read
```cpp
#include <Wire.h>
   // Define the I2C address of the LM73 sensor
#define LM73_ADDRESS 0x4D // Default I2C address, check the datasheet if different

// I2C pins for custom I2C setup
#define SDA1_PIN 4   // SDA1 connected to GPIO 4
#define SCL1_PIN 5   // SCL1 connected to GPIO 5


void setup() {
   // Initialize the serial communication for debugging
   Serial.begin(115200);
   // Initialize I2C communication
   Wire.begin(SDA1_PIN, SCL1_PIN);
   // Wait for sensor stabilization
   delay(100);
}
void loop() {
   // Read temperature from the LM73 sensor
   float temperature = readTemperature();
   if (temperature != -1) {
   // Print temperature to the serial monitor
   Serial.print("Temperature: ");
   Serial.print(temperature);
   Serial.println(" °C");
   } else {
      Serial.println("Failed to read temperature.");
   }
   delay(1000); // Delay before the next reading
}
// Function to read temperature from LM73 sensor
float readTemperature() {
   Wire.beginTransmission(LM73_ADDRESS); // Begin I2C communication with LM73
   Wire.write(0x00); // Point to the temperature register
   if (Wire.endTransmission() != 0) {return -1; // Error in communication
   }
   Wire.requestFrom(LM73_ADDRESS, 2); // Request 2 bytes from the sensor

   if (Wire.available() == 2) {
   // Read 2 bytes of temperature data
   byte msb = Wire.read();
   byte lsb = Wire.read();
   // Combine the two bytes into a 16-bit value
   int16_t tempRaw = (msb << 8) | lsb;
   // Shift right to remove the least significant bit, which is unused
   tempRaw >>= 2 ;
   // Convert the raw value to temperature in Celsius (0.03125°C per bit)
   float temperatureC = tempRaw * 0.03125;
   return temperatureC;
   }
   return -1; // Return error if no data available
}
```

### 6.3 MPU6050 Read

```cpp
#include <Wire.h>
#define MPU6050_ADDR 0x68

int16_t ax, ay, az;
int16_t gx, gy, gz;

void setup() {
  Serial.begin(115200);
  Wire.begin(4, 5);   // SDA and SCL (ESP32: GPIO4 & GPIO5)
  
  // Wake up MPU6050
  Wire.beginTransmission(MPU6050_ADDR);
  Wire.write(0x6B); // Power Management 1 register
  Wire.write(0);    // Set to 0 to wake up
  Wire.endTransmission(true);
}

void loop() {
  Wire.beginTransmission(MPU6050_ADDR);
  Wire.write(0x3B); // Start reading from register 0x3B (Accel X High Byte)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU6050_ADDR, 14, true); // 14 bytes: accel(6) + temp(2) + gyro(6)

  ax = Wire.read() << 8 | Wire.read();
  ay = Wire.read() << 8 | Wire.read();
  az = Wire.read() << 8 | Wire.read();
  int16_t temp = Wire.read() << 8 | Wire.read();
  gx = Wire.read() << 8 | Wire.read();
  gy = Wire.read() << 8 | Wire.read();
  gz = Wire.read() << 8 | Wire.read();

  Serial.println("MPU6050 Readings:");
  Serial.print("Accel: ");
  Serial.print(ax); Serial.print(" ");
  Serial.print(ay); Serial.print(" ");
  Serial.println(az);

  Serial.print("Gyro: ");
  Serial.print(gx); Serial.print(" ");
  Serial.print(gy); Serial.print(" ");
  Serial.println(gz);

  Serial.println("--------------------");
  delay(500);
}
```
---
MPU6050 with Device Driver Libary 
---
```cpp
#include <Wire.h>
#include <Adafruit_MPU6050.h>


// Create an instance of the MPU6050 sensor
Adafruit_MPU6050 mpu;

// I2C pins for custom I2C setup
#define SDA1_PIN 4   // SDA1 connected to GPIO 4
#define SCL1_PIN 5   // SCL1 connected to GPIO 5

void setup() {
  Serial.begin(115200);

  // Start I2C on custom pins (SDA1 = GPIO 4, SCL1 = GPIO 5)
  Wire.begin(SDA1_PIN, SCL1_PIN);

  // Try to initialize the MPU6050 sensor
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip. Check wiring.");
    while (1);
  }

  // Configure sensor settings
  mpu.setAccelerometerRange(MPU6050_RANGE_2_G);  // Set accelerometer range to +/- 2g
  mpu.setGyroRange(MPU6050_RANGE_250_DEG);       // Set gyroscope range to +/- 250 deg/s
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);    // Set filter bandwidth to 21 Hz

  Serial.println("MPU6050 Initialized.");
  delay(1000);
}

void loop() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp); // Read accelerometer, gyroscope, and temperature data

  // Print accelerometer data
  Serial.print("Accel X: "); Serial.print(a.acceleration.x); Serial.print(" m/s^2, ");
  Serial.print("Accel Y: "); Serial.print(a.acceleration.y); Serial.print(" m/s^2, ");
  Serial.print("Accel Z: "); Serial.print(a.acceleration.z); Serial.println(" m/s^2");

  // Print gyroscope data
  Serial.print("Gyro X: "); Serial.print(g.gyro.x); Serial.print(" deg/s, ");
  Serial.print("Gyro Y: "); Serial.print(g.gyro.y); Serial.print(" deg/s, ");
  Serial.print("Gyro Z: "); Serial.print(g.gyro.z); Serial.println(" deg/s");

  // Print temperature data
  Serial.print("Temperature: "); Serial.print(temp.temperature); Serial.println(" °C");

  // Wait 500 ms before next reading
  delay(500);
}

```

### 6.4 Main Program
```cpp
void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22);
  mpu6050Init();
}

void loop() {
  readLDR();
  float temp = lm73ReadC();
  if (!isnan(temp)) Serial.printf("LM73 Temp: %.2f °C\n", temp);
  mpu6050Read();
  delay(1000);
}
```

---

## 7. Exercises
1. Calibrate the MPU6050 to remove offset errors.
2. Convert LDR voltage to lux using a calibration curve.
3. Implement an averaging filter for LM73 readings.

---

## 8. Conclusion
This lab demonstrated how to interface analog and digital sensors with an ESP32, covering ADC readings for LDR, I²C communication for LM73 and MPU6050, and basic data acquisition loops. The knowledge from this lab is foundational for building multi-sensor IoT systems.

